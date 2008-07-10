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

//
MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f)
	: QMainWindow(parent, f)
{
	setupUi(this);
	
	gt = new GitThread();
	gt->start();
	sd = new SettingsImpl(this);
	npd = new NewProjectImpl(this);
	opd = new OutputDialogImpl(this);
	
	
	QTimer::singleShot(0,this,SLOT(initSlot()));
	readSettings();
	setupConnections();
	
	populateProjects();
	
	projectsModel = NULL;
	logModel = NULL;
	hideLogReset();
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
	connect(gt->git,SIGNAL(logReceived()),this,SLOT(logReceived()));
	connect(gt->git,SIGNAL(fileLogReceived()),this,SLOT(fileLogReceived()));
	connect(gt->git,SIGNAL(projectFiles(QString)),this,SLOT(filesReceived(QString)));
	connect(gt->git,SIGNAL(commitDetails(QStringList)),this,SLOT(commitDetails(QStringList)));
	connect(gt->git,SIGNAL(userSettings(QString, QString)),this,SLOT(userSettings(QString, QString)));
	connect(gt->git,SIGNAL(cloneComplete(QString)),this,SLOT(cloneComplete(QString)));

	connect(gt->git,SIGNAL(initOutputDialog()),this,SLOT(initOutputDialog()));
	connect(gt->git,SIGNAL(notifyOutputDialog(const QString &)),this,SLOT(notifyOutputDialog(const QString &)));
	connect(gt->git,SIGNAL(doneOutputDialog()),this,SLOT(doneOutputDialog()));
	
	connect(logView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(logClicked(const QModelIndex &)));
	connect(projectFilesView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(projectFilesViewClicked(const QModelIndex &)));
	connect(projectsComboBox,SIGNAL(activated(int)),this,SLOT(projectsComboBoxActivated(int)));
	connect(ResetLogButton,SIGNAL(clicked()),this,SLOT(resetLog()));
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
	GIT_INVOKE("getLog");
	GIT_INVOKE("getFiles");
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
	eventReceived();
}


void MainWindowImpl::logReceived()
{
	QStandardItemModel *prevModel;
	prevModel = (QStandardItemModel *)logView->model();
	if(prevModel != logModel && logModel!=NULL)
		delete logModel;
	logModel = gt->git->logModel;
	logView->setModel(logModel);
	if(prevModel)
		delete prevModel;
	logView->setColumnWidth(0,450);
	logView->setColumnWidth(1,200);

	eventReceived();
}

void MainWindowImpl::fileLogReceived()
{
	QStandardItemModel *prevModel;
	prevModel = (QStandardItemModel *)logView->model();
	

	QStandardItemModel *model;
	model = gt->git->logModel;
	logView->setModel(model);
	if(prevModel != logModel)
		delete prevModel;
	eventReceived();
}

void MainWindowImpl::filesReceived(QString files)
{
	if(projectsModel)
		delete projectsModel;
	projectsModel = new ProjectsModel(files);
	projectFilesView->setModel(projectsModel);
	eventReceived();
}

void MainWindowImpl::commitDetails(QStringList cd)
{
	commit_author->setText(cd[1].remove(0,8));
	commit_date->setText(cd[2].remove(0,6));
	commit_log->setText(cd[3]);
	commit_diff->clear();
	QString diff=cd.at(4);
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
	
	eventReceived();
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
	
	eventReceived();
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

//Used for connecting random things while devloping,
//Usefull if you want to pop a dialog for debug from git thread etc.
void MainWindowImpl::testSlot()
{

}
//
