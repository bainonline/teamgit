/*
	Copyright 2007 2008 Abhijit Bhopatkar
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 2 of the License.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program, in file COPYING
	If not, see <http://www.devslashzero.com/teamgit/license>.
*/


#include <QMetaObject>
#include <QTimer>
#include <QSettings>
#include <QStringList>
#include <QStringListIterator>
#include <QVariant>
#include <QMetaType>
#include <QMessageBox> 
#include <QInputDialog>
#include <QFileDialog>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QMap>

#include "defs.h"
#include "mainwindowimpl.h"
#include "gitthread.h"
#include "settingsimpl.h"
#include "gsettings.h"



MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f)
	: QMainWindow(parent, f)
{
	setupUi(this);
	
	delete commit_diff1;
	commit_diff = new DiffViewer(this);
	hboxLayout5->addWidget(commit_diff);
	setWindowIcon(QIcon(":/main/icon.png"));
	gt = new GitThread();
	gt->start();
	sd = new SettingsImpl(this);
	npd = new NewProjectImpl(this);
	opd = new OutputDialogImpl(this);
	cmd = new CommitDialogImpl(this);
	rsd = new ResetDialogImpl(this);
	QAction *separator= new QAction((QObject *)this);
	separator->setSeparator(true);
	separator->setText("Recent Repositories");
	menuFile->addAction(separator);
	for(int i=0;i < MAX_RECENT;i++){
		recentRepos[i] = new QAction(this);
		recentRepos[i]->setVisible(false);
		menuFile->addAction(recentRepos[i]);
		connect(recentRepos[i],SIGNAL(triggered()),this,SLOT(openRecent()));
	}
	
	searchClear = new QAction(QIcon(":/main/locationbar_erase.png"),"Reset Search",this);
	searchToolBar->addAction(searchClear);
	searchText = new QLineEdit(this);
	searchText->setMaximumWidth(500);
	searchToolBar->addWidget(searchText);
	searchNext = new QAction(QIcon(":/main/forward.png"),"Next",this);
	searchPrevious = new QAction(QIcon(":/main/back.png"),"Previous",this);
	searchToolBar->addAction(searchPrevious);
	searchToolBar->addAction(searchNext);
	
	searchAuthor = new QAction("Author",this);
	searchDate = new QAction("Date",this);
	searchCommit = new QAction("Commit",this);
	searchLog = new QAction("Log",this);
	searchLog->setCheckable(true);
	searchLog->setChecked(true);
	searchAuthor->setCheckable(true);
	searchDate->setCheckable(true);
	searchCommit->setCheckable(true);
	searchOptionGroup = new QActionGroup(this);
	searchOptionGroup->addAction(searchLog);
	searchOptionGroup->addAction(searchAuthor);
	searchOptionGroup->addAction(searchDate);
	searchOptionGroup->addAction(searchCommit);
	searchToolBar->addActions(searchOptionGroup->actions());
	currentSearch=0;
	
	annotatedFileBox = new QTextEdit(0);
	annotatedFileBox->setReadOnly(true);
	connect(annotatedFileBox,SIGNAL(cursorPositionChanged()),this,SLOT(annotatedFileClicked()));
	QTimer::singleShot(0,this,SLOT(initSlot()));
	readSettings();
	setupConnections();
	
	populateProjects();
	
	projectsModel = NULL;
	stagedModel = NULL;
	unstagedModel = NULL;
	untrackedModel = NULL;
	logModel = NULL;
	tagsModel=NULL;
	branchModel=NULL;
	remoteBranchesModel=NULL;
	
	hideLogReset();
	hideStaged();
	hideUnstaged();
	hideUntracked();
	
	QStringList args = QCoreApplication::arguments ();
	if(args.size() > 1) {
		//Check the first argument and see if its a git directory
		checkAndSetWorkingDir(args[1]);
	} else {
		//Check if working directory is a git repo
		checkAndSetWorkingDir(QDir::currentPath());
	} //else the last path is already loaded
	if(gSettings->teamGitWorkingDir.isEmpty()){
		QTimer::singleShot(0,this,SLOT(openRepo()));
	}
	updateRecentlyOpened();
	fileAnnotationTabIndex=0;
}

void MainWindowImpl::checkAndSetWorkingDir(QString dir)
{
	QString gitPath = dir + "/.git";
	QDir gitDir(gitPath);
	if(gitDir.exists()) {
		gSettings->teamGitWorkingDir = dir;
		addRecentlyOpened(dir);
	}
}

void MainWindowImpl::hideShowUntracked()
{
	if(actionShow_Untracked->isChecked())
		showUntracked();
	else
		hideUntracked();
}

void MainWindowImpl::hideStaged()
{
	stagedFilesView->hide();
}

void MainWindowImpl::hideUnstaged()
{
	unstagedFilesView->hide();
}

void MainWindowImpl::hideUntracked()
{
	untrackedFilesView->hide();
}

void MainWindowImpl::showStaged()
{
	stagedFilesView->show();
	stagedFilesView->expandAll();
}

void MainWindowImpl::showUnstaged()
{
	unstagedFilesView->show();
	unstagedFilesView->expandAll();
}

void MainWindowImpl::showUntracked()
{
	if(actionShow_Untracked->isChecked()) {
		untrackedFilesView->show();
		untrackedFilesView->expandAll();
	}
}

void MainWindowImpl::populateProjects()
{
	projectsComboBox->setVisible(false);
}

void MainWindowImpl::hideLogReset()
{
	searchItemsFoundList.clear();
	LogMessage->hide();
	ResetLogButton->hide();	
	if(fileAnnotationTabIndex) {
		commitLogTabs->removeTab(fileAnnotationTabIndex);
		fileAnnotationTabIndex=0;
	}
}


void MainWindowImpl::showLogReset()
{
	searchItemsFoundList.clear();
	LogMessage->show();
	ResetLogButton->show();
}

void MainWindowImpl::setupConnections()
{
	connect(searchText,SIGNAL(returnPressed()),searchNext,SIGNAL(triggered()));
	connect(searchText,SIGNAL(textChanged(const QString &)),this,SLOT(textSearch(const QString &))); 
	connect(searchNext,SIGNAL(triggered()),this,SLOT(nextSearch()));
	connect(searchPrevious,SIGNAL(triggered()),this,SLOT(prevSearch())); 
	connect(searchClear,SIGNAL(triggered()),searchText,SLOT(clear())); 
	
	connect(actionAbout,SIGNAL(triggered()),this,SLOT(about()));
	connect(action_Options,SIGNAL(triggered()),this,SLOT(settingsDialog()));
	connect(action_New,SIGNAL(triggered()),this,SLOT(newProjectDialog()));
	connect(action_Pull,SIGNAL(triggered()),this,SLOT(pullDialog()));
	connect(action_Refresh,SIGNAL(triggered()),this,SLOT(refresh()));
	connect(actionNew_Tag,SIGNAL(triggered()),this,SLOT(newTag()));
	connect(action_CherryPick,SIGNAL(triggered()),this,SLOT(cherryPickSelectedCommit()));
	connect(action_Commit,SIGNAL(triggered()),this,SLOT(commitSlot()));
	connect(actionCheck_Out,SIGNAL(triggered()),this,SLOT(checkoutSlot()));
	connect(action_Open,SIGNAL(triggered()),this,SLOT(openRepo()));
	connect(action_Push,SIGNAL(triggered()),this,SLOT(pushSlot()));
	connect(action_New_Branch,SIGNAL(triggered()),this,SLOT(newBranchSlot()));
	connect(action_Delete_Branch,SIGNAL(triggered()),this,SLOT(deleteBranchSlot()));
	connect(action_New_Remote_branch,SIGNAL(triggered()),this,SLOT(newRemoteBranchSlot()));
	connect(action_Fetch_Remote_Branch,SIGNAL(triggered()),this,SLOT(fetchRemoteBranchSlot()));
	connect(action_Reset,SIGNAL(triggered()),this,SLOT(resetSlot()));
	connect(actionShow_Untracked,SIGNAL(triggered()),this,SLOT(hideShowUntracked()));
	
	connect(gt->git,SIGNAL(notify(const QString &)),this->statusBar(),SLOT(showMessage(const QString &)));
	connect(gt->git,SIGNAL(progress(int)),this,SLOT(progress(int)));
	connect(gt->git,SIGNAL(logReceived(QString)),this,SLOT(logReceived(QString)));
	connect(gt->git,SIGNAL(namedLogReceived(QString,QString)),this,SLOT(namedLogReceived(QString,QString)));
	connect(gt->git,SIGNAL(projectFiles(QString)),this,SLOT(filesReceived(QString)));
	connect(gt->git,SIGNAL(commitDetails(QStringList)),this,SLOT(commitDetails(QStringList)));
	connect(gt->git,SIGNAL(userSettings(QString, QString)),this,SLOT(userSettings(QString, QString)));
	connect(gt->git,SIGNAL(cloneComplete(QString)),this,SLOT(cloneComplete(QString)));
	connect(gt->git,SIGNAL(filesStatus(QString)),this,SLOT(filesStatusReceived(QString)));
	connect(gt->git,SIGNAL(commitDone()),this,SLOT(refresh()));
	connect(gt->git,SIGNAL(fileDiff(QString,int)),this,SLOT(fileDiffReceived(QString,int)));
	connect(gt->git,SIGNAL(refresh()),this,SLOT(refresh()));
	connect(gt->git,SIGNAL(patchApplied()),this,SLOT(patchApplied()));

	connect(gt->git,SIGNAL(initOutputDialog()),this,SLOT(initOutputDialog()));
	connect(gt->git,SIGNAL(notifyOutputDialog(const QString &)),this,SLOT(notifyOutputDialog(const QString &)));
	connect(gt->git,SIGNAL(doneOutputDialog()),this,SLOT(doneOutputDialog()));
	
	connect(gt->git,SIGNAL(tagList(QString)),this,SLOT(tagsListReceived(QString)));
	connect(gt->git,SIGNAL(branchList(QString)),this,SLOT(branchListReceived(QString)));
	connect(gt->git,SIGNAL(remoteBranchesList(QString)),this,SLOT(remoteBranchListReceived(QString)));
	
	connect(gt->git,SIGNAL(annotatedFile(QString)),this,SLOT(gotAnnotatedFile(QString)));
	
	connect(logView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(logClicked(const QModelIndex &)));
	connect(projectFilesView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(projectFilesViewClicked(const QModelIndex &)));
	connect(projectsComboBox,SIGNAL(activated(int)),this,SLOT(projectsComboBoxActivated(int)));
	connect(ResetLogButton,SIGNAL(clicked()),this,SLOT(resetLog()));
	connect(unstagedFilesView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(unstagedDoubleClicked(const QModelIndex &)));
	connect(stagedFilesView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(stagedDoubleClicked(const QModelIndex &)));
	connect(unstagedFilesView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(unstagedClicked(const QModelIndex &)));
	connect(stagedFilesView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(stagedClicked(const QModelIndex &)));
	connect(branchesView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(branchesViewClicked(const QModelIndex &)));
	connect(tagsView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(tagsViewClicked(const QModelIndex &)));
	connect(remoteBranchesView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(remoteBranchesViewClicked(const QModelIndex &)));
	connect(commit_diff,SIGNAL(doubleClicked()),this,SLOT(diffDoubleClicked()));
	connect(untrackedFilesView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(untrackedDoubleClicked(const QModelIndex &)));
	
	//UI tweaks
	connect(logView,SIGNAL(clicked(const QModelIndex &)),branchesView,SLOT(clearSelection()));
	connect(logView,SIGNAL(clicked(const QModelIndex &)),remoteBranchesView,SLOT(clearSelection()));
	connect(logView,SIGNAL(clicked(const QModelIndex &)),tagsView,SLOT(clearSelection()));
	
	connect(tagsView,SIGNAL(clicked(const QModelIndex &)),branchesView,SLOT(clearSelection()));
	connect(tagsView,SIGNAL(clicked(const QModelIndex &)),remoteBranchesView,SLOT(clearSelection()));
	connect(tagsView,SIGNAL(clicked(const QModelIndex &)),logView,SLOT(clearSelection()));

	connect(branchesView,SIGNAL(clicked(const QModelIndex &)),logView,SLOT(clearSelection()));
	connect(branchesView,SIGNAL(clicked(const QModelIndex &)),remoteBranchesView,SLOT(clearSelection()));
	connect(branchesView,SIGNAL(clicked(const QModelIndex &)),tagsView,SLOT(clearSelection()));
	
	connect(remoteBranchesView,SIGNAL(clicked(const QModelIndex &)),branchesView,SLOT(clearSelection()));
	connect(remoteBranchesView,SIGNAL(clicked(const QModelIndex &)),logView,SLOT(clearSelection()));
	connect(remoteBranchesView,SIGNAL(clicked(const QModelIndex &)),tagsView,SLOT(clearSelection()));

}

MainWindowImpl::~MainWindowImpl()
{
	writeSettings();
}

void MainWindowImpl::about()
{
	QMessageBox::about(this, tr("About Teamgit"),
						"<br> Copyright 2007-2008 Abhijit Bhopatkar - <i>BAIN</i> </b>"
						"<br> bain@devslashzero.com "
						"<br> <a href=http://www.devslashzero.com/teamgit>www.devslashzero.com/teamgit</a>"
						"<br> This program is free software; you can redistribute it and/or modify it under the terms of the  <a href=http://www.devslashzero.com/teamgit/license>GNU General Public License Version 2.</a>"
						"<br> You can get copy of the license by clicking above or at http://www.devslashzero.com/teamgit/license");
}

void MainWindowImpl::openRepo(QString path)
{
	QString startDir=gSettings->teamGitWorkingDir;
	QString dir;
	if(startDir == "notset")
		startDir = "/home";
	if(path.isEmpty()) {
		dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
													startDir,
													QFileDialog::ShowDirsOnly
													| QFileDialog::DontResolveSymlinks);
	} else {
		dir = path;
	}
	if(!dir.isNull())
		gSettings->teamGitWorkingDir= dir;
	addRecentlyOpened(dir);
	refresh();
}

void MainWindowImpl::openRecent()
{
	QAction *action = qobject_cast<QAction *>(sender());
	gSettings->teamGitWorkingDir = action->data().toString();
	refresh();
}

void MainWindowImpl::addRecentlyOpened(QString dir)
{
	if(dir.isEmpty())
		return;
	for(int i=0;i < gSettings->recentlyOpened.size();i++) {
		if(gSettings->recentlyOpened[i] == dir)
			return;
	}
	gSettings->recentlyOpened << dir;
	if(gSettings->recentlyOpened.size() > 5) {
		gSettings->recentlyOpened.removeFirst ();
	}
	updateRecentlyOpened();
}

void MainWindowImpl::updateRecentlyOpened()
{
	for(int i=0;i < MAX_RECENT;i++) {
		if(i < gSettings->recentlyOpened.size()) {
			recentRepos[i]->setText(gSettings->recentlyOpened[i]);
			recentRepos[i]->setData(gSettings->recentlyOpened[i]);
			recentRepos[i]->setVisible(true);
		} else {
			recentRepos[i]->setVisible(false);
		}
	}
}


void MainWindowImpl::writeSettings()
{
	QSettings settings(COMPANY, "Teamgit");
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("horizontalSplitter1", horizontalSplitter1->saveState());
	settings.setValue("verticalSplitter1", verticalSplitter1->saveState());
	settings.endGroup();
	settings.beginGroup("Git");
	settings.setValue("gitbinary",gt->git->getGitBinaryPath());
	settings.endGroup();
	
	settings.beginGroup("TeamGit");
	settings.setValue("workspace",gSettings->teamGitWorkingDir);
	settings.setValue("current_project",gSettings->currProjectPath);
	settings.setValue("RecentlyOpened",gSettings->recentlyOpened);
	settings.setValue("autosignoff",gSettings->autosignoff);
	settings.endGroup();
}

void MainWindowImpl::readSettings()
{
	 QSettings settings(COMPANY, "Teamgit");

     settings.beginGroup("MainWindow");
     resize(settings.value("size", QSize(800, 600)).toSize());
     move(settings.value("pos", QPoint(200,200)).toPoint());
     horizontalSplitter1->restoreState(settings.value("horizontalSplitter1").toByteArray());
     verticalSplitter1->restoreState(settings.value("verticalSplitter1").toByteArray());
     settings.endGroup();
     settings.beginGroup("Git");
     gt->git->setGitBinaryPath(settings.value("gitbinary",QString("/usr/bin/git")).toString());
     settings.endGroup();
     
	settings.beginGroup("TeamGit");
	gSettings->teamGitWorkingDir = settings.value("workspace",QString("notset")).toString();
	gSettings->currProjectPath = QString();
	gSettings->recentlyOpened = settings.value("RecentlyOpened",QStringList()).toStringList();
	gSettings->autosignoff = settings.value("autosignoff",bool()).toBool();
	settings.endGroup();	
	
	GIT_INVOKE("getUserSettings");
}

void MainWindowImpl::initSettings()
{

}

void MainWindowImpl::initSlot()
{
	initSettings();
	if(gSettings->currProjectPath == "notset")
		gSettings->currProjectPath=projectsComboBox->itemText(0);
	else 
		projectsComboBox->setCurrentIndex(projectsComboBox->findText(gSettings->currProjectPath));
	
	refresh();
}

void MainWindowImpl::refresh()
{
	gt->git->setWorkingDir(gSettings->teamGitWorkingDir + gSettings->currProjectPath);
	setWindowTitle(gSettings->teamGitWorkingDir + " - TeamGit");
	commit_author->clear();
	commit_date->clear();
	commit_log->clear();
	commit_diff->clear();
	hideLogReset();
	GIT_INVOKE("getFiles");
	GIT_INVOKE("getLog");
	GIT_INVOKE("getStatus");
	GIT_INVOKE("getTags");
	GIT_INVOKE("getBranches");
	GIT_INVOKE("getRemoteBranches");

}

void MainWindowImpl::textSearch(const QString &text)
{
	int column=0;
	Qt::MatchFlags matchMode=Qt::MatchContains;
	if(searchLog->isChecked()) {
		column=0;
	} else if(searchAuthor->isChecked()){
		column=1;
	} else if(searchDate->isChecked()){
		column=2;
	} else if(searchCommit->isChecked()){
		column=3;
		matchMode=Qt::MatchStartsWith;
	}
	
	searchItemsFoundList.clear();
	searchItemsFoundList << ((QStandardItemModel *)logView->model())->findItems(text,matchMode,column);
	currentSearch=-1;
	nextSearch();
}

void MainWindowImpl::nextSearch()
{
	if(currentSearch < searchItemsFoundList.size()) {
		currentSearch++;
	} else {
		currentSearch=0;
	}
	if(searchItemsFoundList.size() > currentSearch) {
		QStandardItem *item = ((QStandardItemModel *)logView->model())->item(searchItemsFoundList[currentSearch]->row(),0);
		logView->scrollTo((((QStandardItemModel *)logView->model())->indexFromItem(item)));
		logView->setCurrentIndex((((QStandardItemModel *)logView->model())->indexFromItem(item)));
	}
}

void MainWindowImpl::prevSearch()
{
	if(currentSearch > 0) {
		currentSearch--;
	} else {
		currentSearch=searchItemsFoundList.size()-1;
	}
	if(searchItemsFoundList.size() > currentSearch && currentSearch >= 0) {
		logView->scrollTo((((QStandardItemModel *)logView->model())->indexFromItem(searchItemsFoundList[currentSearch])));
		logView->setCurrentIndex((((QStandardItemModel *)logView->model())->indexFromItem(searchItemsFoundList[currentSearch])));
		
	}
}

void MainWindowImpl::checkWorkingDiff()
{
	
}

void MainWindowImpl::newProjectDialog()
{
	int ret = npd->exec();
	if( ret == QDialog::Accepted) {
		QMetaObject::invokeMethod(gt->git,"clone",Qt::QueuedConnection,
							Q_ARG(QString, npd->getRepository()),                       
							Q_ARG(QString, npd->getTarget()),
							Q_ARG(QString, npd->getRefRepo()),
							Q_ARG(QString, npd->getDir()));
	openRepo(npd->getDir() + "/" + npd->getTarget());
	}
}


void MainWindowImpl::checkoutSlot()
{
	QModelIndex index = tagsView->selectionModel()->currentIndex();
	if(index.isValid()) {
		QMetaObject::invokeMethod(gt->git,"checkout",Qt::QueuedConnection,
                           Q_ARG(QString,tagsModel->itemFromIndex(index)->text().trimmed()));
	} else {
		index = branchesView->selectionModel()->currentIndex();
		if(index.isValid()){
			QMetaObject::invokeMethod(gt->git,"checkout",Qt::QueuedConnection,
									Q_ARG(QString,branchModel->itemFromIndex(index)->text().trimmed()));
		} else  {
			index = remoteBranchesView->selectionModel()->currentIndex();
			if(index.isValid()) {
				QString branch;
				branch = remoteBranchesModel->filepath(index);
				branch = branch.trimmed();
				QMetaObject::invokeMethod(gt->git,"checkout",Qt::QueuedConnection,
											Q_ARG(QString,branch));
			}
		}
	}
}

void MainWindowImpl::commitSlot()
{
	if(!stagedFilesView->isVisible())
		return;
	cmd->init(gSettings->userName,gSettings->userEmail,gSettings->autosignoff);
	int ret = cmd->exec();
	if( ret == QDialog::Accepted) {
		QMetaObject::invokeMethod(gt->git,"commit",Qt::QueuedConnection,
						Q_ARG(QString, cmd->getCommitMessage()),
						Q_ARG(QString, cmd->getAuthorName()),
						Q_ARG(QString, cmd->getAuthorEmail()),
						Q_ARG(bool,cmd->signoff()));
	}
}

void MainWindowImpl::pullDialog()
{
	GIT_INVOKE("pull");
	refresh();
	//int ret = pullDialog->exec();
	//if( ret == QDialog::Accepted) {
		//}
}	

void MainWindowImpl::settingsDialog()
{
	sd->setGitBinaryPath(gt->git->getGitBinaryPath());
	sd->refreshUi();
	int ret = sd->exec();
	if( ret == QDialog::Accepted) {
		gt->git->setGitBinaryPath(sd->getGitBinaryPath());
		GIT_INVOKE("setUserSettings");
	}
}


void MainWindowImpl::initOutputDialog()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	opd->initOutputDialog(DEFAULT,true,true);
}

void MainWindowImpl::notifyOutputDialog(const QString &msg)
{
	opd->notifyOutputDialog(msg);
}


void MainWindowImpl::cloneComplete(QString dir)
{
	QString msg("Cloning repository ");
	msg.append(dir);
	msg.append(" complete.");
	statusBar()->showMessage(msg,10);
	refresh();
}

void MainWindowImpl::doneOutputDialog()
{
	opd->doneOutputDialog();
	QApplication::restoreOverrideCursor();
}

void MainWindowImpl::logReceived(QString log)
{
	QStandardItemModel *prevModel;
	prevModel = (QStandardItemModel *)logView->model();
	if(prevModel != logModel && logModel!=NULL)
		delete logModel;
	logModel = parseLog2Model(log);
	logView->setModel(logModel);
	if(prevModel)
		delete prevModel;
	logView->setColumnWidth(0,450);
	logView->setColumnWidth(1,200);

}

void MainWindowImpl::namedLogReceived(QString ref,QString log)
{	
	QStandardItemModel *prevModel;
	prevModel = (QStandardItemModel *)logView->model();
	
	QStandardItemModel *model = parseLog2Model(log);
	logView->setModel(model);
	if(prevModel != logModel)
		delete prevModel;
	this->statusBar()->showMessage("Ready");
	LogMessage->setText("Showing log for : " + ref);
	showLogReset();;
	progress(100);
}

void MainWindowImpl::filesStatusReceived(QString status)
{
	QStringList lines=status.split("\n");
	QString unstagedChanged,stagedChanged,untrackedChanged;
	QString *curList=NULL;
	for(int i=0;i<lines.size();i++) {
		lines[i]=lines[i].simplified();
		if(lines[i].startsWith("# Changes to be committed:")) {
			curList=&stagedChanged;
		} else if(lines[i].startsWith("# Changed but not updated:")) {
			curList=&unstagedChanged;
		} else if(lines[i].startsWith("# Untracked files:")) {
			curList=&untrackedChanged;
		} else if(lines[i].startsWith("# modified: ")) {
			lines[i].remove(0,sizeof("# modified:"));
			curList->append(lines[i]+"\n");
		} else if(lines[i].startsWith("# new file: ")) {
			lines[i].remove(0,sizeof("# new file:"));
			curList->append(lines[i]+"\n");
		} else {
			if(curList == &untrackedChanged) {
				if(lines[i].startsWith("# ") && 
				   !lines[i].startsWith("# (use ")) {
					lines[i].remove(0,sizeof("#"));
					curList->append(lines[i]+"\n");
				}
			}
		}
	}
	
	if(stagedChanged.size()) {
		if(stagedModel)
			delete stagedModel;
		stagedModel = new ProjectsModel(stagedChanged,0,"Staged Modified Files");
		stagedFilesView->setModel(stagedModel);
		showStaged();
	} else {
		hideStaged();
	}
	if(unstagedChanged.size()) {
		if(unstagedModel)
			delete unstagedModel;
		unstagedModel = new ProjectsModel(unstagedChanged,0,"Unstaged Modified Files");
		unstagedFilesView->setModel(unstagedModel);
		showUnstaged();
	} else {
		hideUnstaged();
	}
	
	if(untrackedChanged.size()) {
		if(untrackedModel)
			delete untrackedModel;
		untrackedModel = new ProjectsModel(untrackedChanged,0,"Untracked Files");
		untrackedFilesView->setModel(untrackedModel);
		showUntracked();
	} else {
		hideUntracked();
	}
	//Crazy stuff i could not find a quick way to do this cleanly
	 QTimer::singleShot(200, this, SLOT(expandStagedUnstagedSlot()));
}

void MainWindowImpl::filesReceived(QString files)
{
	if(projectsModel)
		delete projectsModel;
	projectsModel = new ProjectsModel(files);
	projectFilesView->setModel(projectsModel);
	
}

void MainWindowImpl::branchListReceived(QString branch)
{
	if(branchModel)
		delete branchModel;
	branchModel = new QStandardItemModel(0,1);
	QStandardItem *it = new QStandardItem(QString("Branches"));
	branchModel->setHorizontalHeaderItem(0,it);
	QStringList branchList = branch.split("\n");
	for(int i=0;i < branchList.size()-1;i++) {
		QStandardItem *item1 = new QStandardItem(branchList[i]);
		item1->setEditable(false);
		branchModel->appendRow(item1);
	}
	branchesView->setModel(branchModel);
}

void MainWindowImpl::remoteBranchListReceived(QString branches)
{
	if(remoteBranchesModel)
		delete remoteBranchesModel;
	remoteBranchesModel = new ProjectsModel(branches,0,"Remote brnaches");
	remoteBranchesView->setModel(remoteBranchesModel);
}

void MainWindowImpl::tagsListReceived(QString tags)
{
	if(tagsModel)
		delete tagsModel;
	tagsModel = new QStandardItemModel(0,1);
	QStandardItem *it = new QStandardItem(QString("Tags"));
	tagsModel->setHorizontalHeaderItem(0,it);
	QStringList tagsList = tags.split("\n");
	for(int i=0;i < tagsList.size()-1;i++) {
		QStandardItem *item1 = new QStandardItem(tagsList[i]);
		item1->setEditable(false);
		tagsModel->appendRow(item1);
	}
	tagsView->setModel(tagsModel);
}

QStandardItemModel *MainWindowImpl::parseLog2Model(QString log)
{
	QStandardItemModel *model = new QStandardItemModel(0,4);	
	QStandardItem *it = new QStandardItem(QString("Log"));
	QStandardItem *it1 = new QStandardItem(QString("Author"));
	QStandardItem *it2 = new QStandardItem(QString("Date"));
	QStandardItem *it3 = new QStandardItem(QString("Commit"));
	model->setHorizontalHeaderItem(0,it);
	model->setHorizontalHeaderItem(1,it1);
	model->setHorizontalHeaderItem(2,it2);
	model->setHorizontalHeaderItem(3,it3);
	
	QString delimit("TEAMGITFIELDEND");
	QString delimit2("TEAMGITFIELDMARKER");
	QStringList logList=log.split(delimit);
	int numLogs=logList.count();
	int parsed=0;
	QStringListIterator iterator(logList);
	while (iterator.hasNext()) {
		parsed++;
		int percent=50 + ((float)((float)parsed/(float)numLogs)*(float)100)/2;
		if(percent && !(percent % 10)) {
			progress(percent);
		}
		QString singleLog = iterator.next();
		if(!singleLog.contains(delimit2))
			continue;
		QStringList logFields = singleLog.split(delimit2);
		QStringListIterator it2(logFields);
		QList<QStandardItem*> itemlist;
		QString oneLiner = it2.next();
		if(oneLiner.startsWith("\n"))
			oneLiner = oneLiner.remove(0,1);
		QStandardItem *item1 = new QStandardItem(oneLiner);
		item1->setEditable(false);
		itemlist.append(item1);
		
		while(it2.hasNext()) {
			QStandardItem *item1 = new QStandardItem(it2.next());
			item1->setEditable(false);	
			itemlist.append(item1);
		}
		model->appendRow(itemlist);
	}
	return model;
}


void MainWindowImpl::commitDetails(QStringList cd)
{
	commit_author->setText(cd[1].remove(0,8));
	commit_date->setText(cd[2].remove(0,6));
	commit_log->setText(cd[3]);
	QString diff=cd.at(4);
	commit_diff->setDiffText(diff);
	commit_diff->setDiffType(commitDiff);
}

void MainWindowImpl::fileDiffReceived(QString diff,int type)
{
	commit_author->clear();
	commit_date->clear();
	commit_log->clear();
	commit_diff->setDiffText(diff);
	commitLogTabs->setCurrentIndex(1);
	commit_diff->setDiffType(type);
}

void MainWindowImpl::diffDoubleClicked()
{
	if(!commit_diff->getDiffType()) {
		return;
	}
	QString patch;
	patch.append(commit_diff->document()->findBlockByNumber(0).text());
	patch.append("\n");
	patch.append(commit_diff->document()->findBlockByNumber(1).text());
	patch.append("\n");
	patch.append(commit_diff->document()->findBlockByNumber(2).text());
	patch.append("\n");
	patch.append(commit_diff->document()->findBlockByNumber(3).text());
	patch.append("\n");
	patch.append(commit_diff->textCursor().selection().toPlainText());
	if(commit_diff->getDiffType()==stagedDiff) {
			QMetaObject::invokeMethod(gt->git,"unStageHunk",Qt::QueuedConnection,
                           Q_ARG(QString,patch));
	} else {
			QMetaObject::invokeMethod(gt->git,"stageHunk",Qt::QueuedConnection,
                           Q_ARG(QString,patch));
	}
}

void MainWindowImpl::progress(int i)
{
	if(i == 0) {
		progressBar = new QProgressBar(this);
		statusBar()->addPermanentWidget(progressBar); 
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		
	} else if(i == 100) {
		if(progressBar) {
			statusBar()->removeWidget(progressBar);
			delete progressBar;
			progressBar = NULL;
			QApplication::restoreOverrideCursor();
		}
	} else {
		progressBar->setValue(i);
	}
}

void MainWindowImpl::userSettings(QString name, QString email)
{
	gSettings->userName = name;
	gSettings->userEmail = email;

}


void MainWindowImpl::untrackedDoubleClicked(const QModelIndex &index)
{
	QStringList files;
	QModelIndexList indexes = untrackedFilesView->selectionModel()->selectedIndexes();

	for(int i=0;i < indexes.size();i++) {	
		if(!untrackedModel->rowCount(indexes[i]))
			files << untrackedModel->filepath(indexes[i]);
	}
	files << untrackedModel->filepath(index);
	
	QMetaObject::invokeMethod(gt->git,"stageFiles",Qt::QueuedConnection,
							Q_ARG(QStringList,files));
}

void MainWindowImpl::stagedDoubleClicked(const QModelIndex &index)
{
	
	QStringList files;
	QModelIndexList indexes = stagedFilesView->selectionModel()->selectedIndexes();

	for(int i=0;i < indexes.size();i++) {	
		if(!stagedModel->rowCount(indexes[i]))
			files << stagedModel->filepath(indexes[i]);
	}
	files << stagedModel->filepath(index);
	
	QMetaObject::invokeMethod(gt->git,"unstageFiles",Qt::QueuedConnection,
							Q_ARG(QStringList,files));
}

void MainWindowImpl::unstagedDoubleClicked(const QModelIndex &index)
{
	QStringList files;
	QModelIndexList indexes = unstagedFilesView->selectionModel()->selectedIndexes();

	for(int i=0;i < indexes.size();i++) {	
		if(!unstagedModel->rowCount(indexes[i]))
			files << unstagedModel->filepath(indexes[i]);
	}
	files << unstagedModel->filepath(index);
	
	QMetaObject::invokeMethod(gt->git,"stageFiles",Qt::QueuedConnection,
							Q_ARG(QStringList,files));
}

void MainWindowImpl::stagedClicked(const QModelIndex &index)
{
	QString file;
	file = stagedModel->filepath(index);
	QMetaObject::invokeMethod(gt->git,"getDiffCached",Qt::QueuedConnection,
                           Q_ARG(QString,file));
}


void MainWindowImpl::unstagedClicked(const QModelIndex &index)
{
	QString file;
	file = unstagedModel->filepath(index);
	QMetaObject::invokeMethod(gt->git,"getDiff",Qt::QueuedConnection,
                           Q_ARG(QString,file));
}

void MainWindowImpl::tagsViewClicked(const QModelIndex &index)
{
	branchesView->selectionModel()->clear();
	remoteBranchesView->selectionModel()->clear();
	
	QString text = tagsModel->itemFromIndex(index)->text();
	QMetaObject::invokeMethod(gt->git,"getNamedLog",Qt::QueuedConnection,
                           Q_ARG(QString,text));
}

void MainWindowImpl::branchesViewClicked(const QModelIndex &index)
{
	remoteBranchesView->selectionModel()->clear();
	tagsView->selectionModel()->clear();
	
	QString text = branchModel->itemFromIndex(index)->text();
	if(text.startsWith("*")) {
		resetLog();
		return;
	}
	text = text.trimmed();
	commit_diff->append(text);
	QMetaObject::invokeMethod(gt->git,"getNamedLog",Qt::QueuedConnection,
                           Q_ARG(QString,text));
}

void MainWindowImpl::remoteBranchesViewClicked(const QModelIndex &index)
{
	branchesView->selectionModel()->clear();
	tagsView->selectionModel()->clear();
	QString branch;
	branch = remoteBranchesModel->filepath(index);
	branch = branch.trimmed();
	QMetaObject::invokeMethod(gt->git,"getNamedLog",Qt::QueuedConnection,
                           Q_ARG(QString,branch));
}

void MainWindowImpl::logClicked(const QModelIndex &index)
{
	QStandardItemModel *model = (QStandardItemModel *)logView->model();
	QStandardItem *item = model->itemFromIndex(index);
	QMetaObject::invokeMethod(gt->git,"getCommit",Qt::QueuedConnection,
                           Q_ARG(QString,model->item(item->row(),3)->text()));
}


void MainWindowImpl::newTag()
{
	bool ok;
	QString tag = QInputDialog::getText(this, tr("Add New Tag"),
										tr("Please enter the tag name :"), QLineEdit::Normal,
										"", &ok);
	QMetaObject::invokeMethod(gt->git,"tag",Qt::QueuedConnection,
								Q_ARG(QString,tag));
	GIT_INVOKE("getTags");
}

void MainWindowImpl::projectFilesViewClicked(const QModelIndex &index)
{
	QModelIndex i;
	i=index;
	QStringList path;
	QString text =  projectsModel->filepath(index);
	QMetaObject::invokeMethod(gt->git,"getNamedLog",Qt::QueuedConnection,
							Q_ARG(QString,text));
	if(!projectsModel->rowCount(index)) 
		QMetaObject::invokeMethod(gt->git,"blame",Qt::QueuedConnection,
							Q_ARG(QString,text));
}

void MainWindowImpl::projectsComboBoxActivated(int index)
{
	gSettings->currProjectPath=projectsComboBox->itemText(index);
	gt->git->setWorkingDir(gSettings->teamGitWorkingDir + gSettings->currProjectPath);
	refresh();
}


void MainWindowImpl::resetLog()
{
	QStandardItemModel *prevModel;
	prevModel = (QStandardItemModel *)logView->model();	
	logView->setModel(logModel);
	if(prevModel != logModel)
		delete prevModel;
	hideLogReset();
}

void MainWindowImpl::expandStagedUnstagedSlot()
{
	unstagedFilesView->expandAll();
	stagedFilesView->expandAll();
}


void MainWindowImpl::cherryPickSelectedCommit()
{
	QModelIndex index = logView->selectionModel()->currentIndex();
	if(!index.isValid())
		return;
	QStandardItemModel *model=(QStandardItemModel *)logView->model();
	QStandardItem *item = model->itemFromIndex(index);
	QMetaObject::invokeMethod(gt->git,"cherryPick",Qt::QueuedConnection,
							Q_ARG(QString,model->item(item->row(),3)->text()));
}


void MainWindowImpl::pushSlot()
{
	GIT_INVOKE("push");
}


void MainWindowImpl::newBranchSlot()
{
	QString ref;
	QModelIndex index = tagsView->selectionModel()->currentIndex();
	if(index.isValid()) {
		ref=tagsModel->itemFromIndex(index)->text().trimmed();
	} else {
		index = branchesView->selectionModel()->currentIndex();
		if(index.isValid()){
			ref=branchModel->itemFromIndex(index)->text().trimmed();
			if(ref.startsWith("*")) {
				ref = QString();
			}
		} else  {
			index = remoteBranchesView->selectionModel()->currentIndex();
			if(index.isValid()) {
				ref = remoteBranchesModel->filepath(index);
				ref = ref.trimmed();
			}
		}
	}
	bool ok;
	QString branch = QInputDialog::getText(this, tr("Add New Branch"),
										tr("Please enter the branch name :"), QLineEdit::Normal,
										"", &ok);
	if(ok) {
		QMetaObject::invokeMethod(gt->git,"newBranch",Qt::QueuedConnection,
								Q_ARG(QString,branch),
								Q_ARG(QString,ref));
		GIT_INVOKE("getBranch");
	}
}

void MainWindowImpl::deleteBranchSlot()
{
	QString branch;
	QModelIndex index = branchesView->selectionModel()->currentIndex();
	if(index.isValid()){
		branch=branchModel->itemFromIndex(index)->text().trimmed();
		if(branch.startsWith("*")) {
			return;
		}
		branch.trimmed();
	}
	 int ret = QMessageBox::warning(this, tr("TeamGit"),
							"The branch " + branch + " will be deleted\n Do you want to continue?",
							QMessageBox::Ok | QMessageBox::Cancel,
							QMessageBox::Cancel);
	if(ret == QMessageBox::Ok )
		QMetaObject::invokeMethod(gt->git,"deleteBranch",Qt::QueuedConnection,
								Q_ARG(QString,branch));
}

void MainWindowImpl::newRemoteBranchSlot()
{
	bool ok;
	QString branch = QInputDialog::getText(this, tr("Add New Remote branch"),
										tr("Please enter the branch name (for local referece):"), QLineEdit::Normal,
										"", &ok);
	if(!ok) 
		return;

	QString url = QInputDialog::getText(this, tr("Add New Remote branch"),
									tr("Please enter the branch url:"), QLineEdit::Normal,
									"", &ok);
	url=url.trimmed();
	if(!ok) 
		return;
		
	QMetaObject::invokeMethod(gt->git,"newRemoteBranch",Qt::QueuedConnection,
							Q_ARG(QString,branch),
							Q_ARG(QString,url));
	
	QMetaObject::invokeMethod(gt->git,"fetch",Qt::QueuedConnection,
							Q_ARG(QString,branch));
	GIT_INVOKE("getRemoteBranches");
}

void MainWindowImpl::fetchRemoteBranchSlot()
{
	QString branch;
	QModelIndex index = remoteBranchesView->selectionModel()->currentIndex();
	if(index.isValid()) {
		branch = remoteBranchesModel->filepath(index);
		branch = branch.trimmed();
		if(branch.indexOf("/") >= 0)
			branch.remove(branch.indexOf("/"),branch.size()-branch.indexOf("/"));
		QMetaObject::invokeMethod(gt->git,"fetch",Qt::QueuedConnection,
							Q_ARG(QString,branch));
		GIT_INVOKE("getRemoteBranches");
	}
}

void MainWindowImpl::annotatedFileClicked()
{
	searchCommit->setChecked(true);
	searchText->setText(annotatedFileBox->document()->findBlock(annotatedFileBox->textCursor().position()).text().left(6));
}

void MainWindowImpl::gotAnnotatedFile(QString file)
{
	QString commit, author,fileLine;
	QMap<QString, QString> map;
	int commit_lines=0;
	
	annotatedFileBox->clear();
	QStringList fileLines = file.split("\n");
	for(int i=0;i<fileLines.count()-1;i++) {
		if(fileLines[i].startsWith("\t")) {
			commit_lines--;
			annotatedFileBox->append(commit.left(6) + " " + map[commit]+fileLines[i]);
		} else {
			if(!commit_lines) {
				QStringList fields = fileLines[i].split(" ");
				commit = fields[0];
				commit_lines=fields[3].toInt();
			}
			if(fileLines[i].startsWith("author ")) {
					author= fileLines[i].remove(0,7);
					map[commit] = author;
			}
		} 
	}
	QTextCursor cursor = annotatedFileBox->textCursor();
	cursor.movePosition(QTextCursor::Start);
	annotatedFileBox->setTextCursor(cursor);
	annotatedFileBox->ensureCursorVisible ();
	fileAnnotationTabIndex=commitLogTabs->addTab(annotatedFileBox,"Annotated File");
	commitLogTabs->setCurrentIndex(fileAnnotationTabIndex);
}
	
void MainWindowImpl::patchApplied()
{
	QModelIndex index = unstagedFilesView->selectionModel()->currentIndex();
	unstagedClicked(index);
	GIT_INVOKE("getStatus");
}

void MainWindowImpl::resetSlot()
{
	QString ref;
	QModelIndex index = tagsView->selectionModel()->currentIndex();
	if(index.isValid()) {
		ref=tagsModel->itemFromIndex(index)->text().trimmed();
	} else {
		index = branchesView->selectionModel()->currentIndex();
		if(index.isValid()){
			ref=branchModel->itemFromIndex(index)->text().trimmed();
			if(ref.startsWith("*")) {
				ref = ref.remove(0,1);
			}
		} else  {
			index = remoteBranchesView->selectionModel()->currentIndex();
			if(index.isValid()) {
				ref = remoteBranchesModel->filepath(index);
				ref = ref.trimmed();
			} else {
				index = logView->selectionModel()->currentIndex();
				if(index.isValid()) {
					QStandardItemModel *model = (QStandardItemModel *)logView->model();
					QStandardItem *item = model->itemFromIndex(index);
					ref = model->item(item->row(),3)->text();
				}
			}
		}
	}
	if(ref.isEmpty())
		return;
	rsd->reset(ref);
	int ret = rsd->exec();
	if( ret == QDialog::Accepted) {
		int opt = rsd->getOption();
		QMetaObject::invokeMethod(gt->git,"reset",Qt::QueuedConnection,
 					Q_ARG(QString, ref),                       
 					Q_ARG(int, opt));
 	}
}

//Used for connecting random things while devloping,
//Usefull if you want to pop a dialog for debug from git thread etc.
void MainWindowImpl::testSlot()
{
	
}
//
