#include <QMetaObject>
#include <QTimer>
#include <QSettings>
#include <QStringList>
#include <QStringListIterator>
#include <QVariant>
#include <QMetaType>
#include <QMessageBox> 

#include "defs.h"
#include "mainwindowimpl.h"
#include "gitthread.h"
#include "settingsimpl.h"
#include "gsettings.h"



MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f)
	: QMainWindow(parent, f)
{
	setupUi(this);
	
	gt = new GitThread();
	gt->start();
	sd = new SettingsImpl(this);
	npd = new NewProjectImpl(this);
	opd = new OutputDialogImpl(this);
	cmd = new CommitDialogImpl(this);
	
	QTimer::singleShot(0,this,SLOT(initSlot()));
	readSettings();
	setupConnections();
	
	populateProjects();
	
	projectsModel = NULL;
	stagedModel = NULL;
	unstagedModel = NULL;
	logModel = NULL;
	hideLogReset();
	hideStaged();
	hideUnstaged();
}


void MainWindowImpl::hideStaged()
{
	stagedFilesView->hide();
	commitButton->hide();
}

void MainWindowImpl::hideUnstaged()
{
	unstagedFilesView->hide();
}


void MainWindowImpl::showStaged()
{
	stagedFilesView->show();
	stagedFilesView->expandAll();
	commitButton->show();
}

void MainWindowImpl::showUnstaged()
{
	unstagedFilesView->show();
	unstagedFilesView->expandAll();
	//stageButton->show();
}

void MainWindowImpl::populateProjects()
{
	projectsComboBox->clear();
	QDir dir(gSettings->teamGitWorkingDir);
	dir.setFilter(QDir::AllDirs);
	QStringList filter;
	QStringList projects = dir.entryList(filter);
	for(int i = 0;i < projects.size();i++) {
		if(projects.at(i) != "." && projects.at(i) != "..")
			projectsComboBox->addItem(projects.at(i));
	}
}

void MainWindowImpl::hideLogReset()
{
	LogMessage->hide();
	ResetLogButton->hide();	
}


void MainWindowImpl::showLogReset()
{
	LogMessage->show();
	ResetLogButton->show();	
}

void MainWindowImpl::setupConnections()
{
//	connect(pushButton,SIGNAL(clicked()),this,SLOT(testSlot()));

	connect(action_Options,SIGNAL(triggered()),this,SLOT(settingsDialog()));
	connect(action_New,SIGNAL(triggered()),this,SLOT(newProjectDialog()));
	connect(action_Pull,SIGNAL(triggered()),this,SLOT(pullDialog()));
	connect(action_Refresh,SIGNAL(triggered()),this,SLOT(refresh()));

	connect(gt->git,SIGNAL(notify(const QString &)),this->statusBar(),SLOT(showMessage(const QString &)));
	connect(gt->git,SIGNAL(progress(int)),this,SLOT(progress(int)));
	connect(gt->git,SIGNAL(logReceived(QString)),this,SLOT(logReceived(QString)));
	connect(gt->git,SIGNAL(fileLogReceived(QString)),this,SLOT(fileLogReceived(QString)));
	connect(gt->git,SIGNAL(projectFiles(QString)),this,SLOT(filesReceived(QString)));
	connect(gt->git,SIGNAL(commitDetails(QStringList)),this,SLOT(commitDetails(QStringList)));
	connect(gt->git,SIGNAL(userSettings(QString, QString)),this,SLOT(userSettings(QString, QString)));
	connect(gt->git,SIGNAL(cloneComplete(QString)),this,SLOT(cloneComplete(QString)));
	connect(gt->git,SIGNAL(filesStatus(QString)),this,SLOT(filesStatusReceived(QString)));
	connect(gt->git,SIGNAL(commitDone()),this,SLOT(refresh()));

	connect(gt->git,SIGNAL(initOutputDialog()),this,SLOT(initOutputDialog()));
	connect(gt->git,SIGNAL(notifyOutputDialog(const QString &)),this,SLOT(notifyOutputDialog(const QString &)));
	connect(gt->git,SIGNAL(doneOutputDialog()),this,SLOT(doneOutputDialog()));
	
	connect(logView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(logClicked(const QModelIndex &)));
	connect(projectFilesView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(projectFilesViewClicked(const QModelIndex &)));
	connect(projectsComboBox,SIGNAL(activated(int)),this,SLOT(projectsComboBoxActivated(int)));
	connect(ResetLogButton,SIGNAL(clicked()),this,SLOT(resetLog()));
	connect(unstagedFilesView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(unstagedDoubleClicked(const QModelIndex &)));
	connect(stagedFilesView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(stagedDoubleClicked(const QModelIndex &)));

	connect(commitButton,SIGNAL(clicked()),this,SLOT(commitSlot()));
}

MainWindowImpl::~MainWindowImpl()
{
	writeSettings();
}

void MainWindowImpl::writeSettings()
{
	QSettings settings(COMPANY, "Teamgit");
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("horizontalSplitter1", horizontalSplitter1->saveState());
//	settings.setValue("horizontalSplitter2_2", horizontalSplitter2_2->saveState());
	settings.setValue("verticalSplitter1", verticalSplitter1->saveState());
	settings.endGroup();
	settings.beginGroup("Git");
	settings.setValue("gitbinary",gt->git->getGitBinaryPath());
	settings.endGroup();
	
	settings.beginGroup("TeamGit");
	settings.setValue("workspace",gSettings->teamGitWorkingDir);
	settings.setValue("current_project",gSettings->currProjectPath);
	settings.endGroup();
}

void MainWindowImpl::readSettings()
{
	 QSettings settings(COMPANY, "Teamgit");

     settings.beginGroup("MainWindow");
     resize(settings.value("size", QSize(400, 400)).toSize());
     move(settings.value("pos", QPoint(200, 200)).toPoint());
     horizontalSplitter1->restoreState(settings.value("horizontalSplitter1").toByteArray());
     verticalSplitter1->restoreState(settings.value("verticalSplitter1").toByteArray());
//     horizontalSplitter2_2->restoreState(settings.value("horizontalSplitter2_2").toByteArray());
     QList<int> list;
     list << 1;
     list << 0;
//     horizontalSplitter2_2->setSizes(list);
     settings.endGroup();
     settings.beginGroup("Git");
     gt->git->setGitBinaryPath(settings.value("gitbinary").toString());
     settings.endGroup();
     
	settings.beginGroup("TeamGit");
	gSettings->teamGitWorkingDir = settings.value("workspace",QString("notset")).toString();
	if(!gSettings->teamGitWorkingDir.endsWith("/")) {
		gSettings->teamGitWorkingDir.append("/");
	}
	gSettings->currProjectPath = settings.value("current_project",QString("notset")).toString();
	settings.endGroup();	
	
	GIT_INVOKE("getUserSettings");
}

void MainWindowImpl::initSettings()
{
	if(gSettings->teamGitWorkingDir == "notset") {
		settingsDialog();
	} 
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
	commit_author->clear();
	commit_date->clear();
	commit_log->clear();
	commit_diff->clear();
	hideLogReset();
	GIT_INVOKE("getFiles");
	GIT_INVOKE("getLog");
	GIT_INVOKE("getStatus");
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
                           Q_ARG(QString, gSettings->teamGitWorkingDir));
	}
}	

void MainWindowImpl::commitSlot()
{
	cmd->setAuthor(gSettings->userName,gSettings->userEmail);
	int ret = cmd->exec();
	if( ret == QDialog::Accepted) {
		QMetaObject::invokeMethod(gt->git,"commit",Qt::QueuedConnection,
						Q_ARG(QString, cmd->getCommitMessage()),
						Q_ARG(QString, cmd->getAuthorName()),
						Q_ARG(QString, cmd->getAuthorEmail()));
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
	sd->setTeamGitWorkingDir(gSettings->teamGitWorkingDir);
	sd->setGitBinaryPath(gt->git->getGitBinaryPath());
	sd->refreshUi();
	int ret = sd->exec();
	if( ret == QDialog::Accepted) {
		gt->git->setGitBinaryPath(sd->getGitBinaryPath());
		if(gSettings->teamGitWorkingDir != sd->getTeamGitWorkingDir()) {
			gSettings->teamGitWorkingDir = sd->getTeamGitWorkingDir();
			emit teamGitWorkingDirChanged(gSettings->teamGitWorkingDir);
		}
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
	gSettings->currProjectPath = dir;
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

void MainWindowImpl::fileLogReceived(QString log)
{	
	QStandardItemModel *prevModel;
	prevModel = (QStandardItemModel *)logView->model();
	
	QStandardItemModel *model = parseLog2Model(log);
	logView->setModel(model);
	if(prevModel != logModel)
		delete prevModel;
	this->statusBar()->showMessage("Ready");
	progress(100);
}

void MainWindowImpl::filesStatusReceived(QString status)
{
	QStringList lines=status.split("\n");
	QString unstagedChanged,stagedChanged;
	QString *curList;
	for(int i=0;i<lines.size();i++) {
		lines[i]=lines[i].simplified();
		if(lines[i].startsWith("# Changes to be committed:")) {
			curList=&stagedChanged;
		} else if(lines[i].startsWith("# Changed but not updated:")) {
			curList=&unstagedChanged;
		} else if(lines[i].startsWith("# modified: ")) {
			lines[i].remove(0,sizeof("# modified:"));
			curList->append(lines[i]+"\n");
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

#define DIFF_LIMIT (1024 * 1024)
void MainWindowImpl::commitDetails(QStringList cd)
{
	commit_author->setText(cd[1].remove(0,8));
	commit_date->setText(cd[2].remove(0,6));
	commit_log->setText(cd[3]);
	commit_diff->clear();
	QString diff=cd.at(4);
	if(diff.size() > DIFF_LIMIT) {
		diff.remove(DIFF_LIMIT,diff.size()-(DIFF_LIMIT));
		diff.append("\n- Commit too huge: trimmed!");
	}
	QStringList diffLines=diff.split(QChar('\n'));
	 
	for (int i = 0; i < diffLines.size(); ++i) {
		QString line = diffLines.at(i);
		/* preprocess */
		if(line.startsWith("+")) {
			commit_diff->setTextColor(QColor("darkgreen"));
		} else if (line.startsWith("-")){
			commit_diff->setTextColor(QColor("darkred"));
	 	} else if (line.startsWith("@@")){
			commit_diff->setTextColor(QColor("blue"));
		} else if (line.startsWith("commit")){
			commit_diff->clear();
			commit_diff->setFontWeight(QFont::Bold);
		} else if (line.startsWith("Author:")){
			commit_diff->setFontWeight(QFont::Bold);
		} else if (line.startsWith("Date:")){
			commit_diff->setFontWeight(QFont::Bold);
		} else if (line.startsWith("diff")){
			commit_diff->setFontWeight(QFont::Bold);
		} else if (line.startsWith("index")){
			commit_diff->setFontWeight(QFont::Bold);
		} 

		commit_diff->append(line);
	
		/* post process */
		if(line.startsWith("+")) {
			commit_diff->setTextColor(QColor("black"));
		} else if (line.startsWith("-")){
			commit_diff->setTextColor(QColor("black"));
	 	} else if (line.startsWith("@@")){
			commit_diff->setTextColor(QColor("black"));
		} else if (line.startsWith("commit")){
			commit_diff->setFontWeight(QFont::Normal);
		} else if (line.startsWith("Author:")){
			commit_diff->setFontWeight(QFont::Normal);
		} else if (line.startsWith("Date:")){
			commit_diff->setFontWeight(QFont::Normal);
			commit_diff->insertPlainText("\n");
		} else if (line.startsWith("diff")){
			commit_diff->setFontWeight(QFont::Normal);
		} else if (line.startsWith("index")){
			commit_diff->setFontWeight(QFont::Normal);
		}
	}
	QTextCursor cursor = commit_diff->textCursor();
	cursor.movePosition(QTextCursor::Start);
	commit_diff->setTextCursor(cursor);
	commit_diff->ensureCursorVisible ();
}
//test
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

void MainWindowImpl::stagedDoubleClicked(const QModelIndex &index)
{
	
	QStringList files;
	files << stagedModel->filepath(index);
	QMetaObject::invokeMethod(gt->git,"unstageFiles",Qt::QueuedConnection,
                           Q_ARG(QStringList,files));
}

void MainWindowImpl::unstagedDoubleClicked(const QModelIndex &index)
{
	QStringList files;
	files << unstagedModel->filepath(index);
	QMetaObject::invokeMethod(gt->git,"stageFiles",Qt::QueuedConnection,
                           Q_ARG(QStringList,files));
}

void MainWindowImpl::logClicked(const QModelIndex &index)
{
	QStandardItemModel *model = (QStandardItemModel *)logView->model();
	QStandardItem *item = model->itemFromIndex(index);
	QMetaObject::invokeMethod(gt->git,"getCommit",Qt::QueuedConnection,
                           Q_ARG(QString,model->item(item->row(),3)->text()));
}

void MainWindowImpl::projectFilesViewClicked(const QModelIndex &index)
{
	QModelIndex i;
	i=index;
	QStringList path;
	
	while(i.isValid()) {
		path << projectsModel->data(i,0).toString();
		i = projectsModel->parent(i);
	}
	QString text;
	for(int j=path.size()-1;j>=0;j--) {
		text += path[j];
		if(j)
			text+= "/";
	}
	LogMessage->setText("Showing log for : " + text);
	showLogReset();;
	QMetaObject::invokeMethod(gt->git,"getFileLog",Qt::QueuedConnection,
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

//Used for connecting random things while devloping,
//Usefull if you want to pop a dialog for debug from git thread etc.
void MainWindowImpl::testSlot()
{
}
//
