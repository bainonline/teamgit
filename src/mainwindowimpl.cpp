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
	sd = new SettingsImpl();
	npd = new NewProjectImpl();
	opd = new OutputDialogImpl();
	
	QTimer::singleShot(0,this,SLOT(initSlot()));
	readSettings();
	setupConnections();
}

void MainWindowImpl::setupConnections()
{
	connect(pushButton,SIGNAL(clicked()),this,SLOT(testSlot()));

	connect(action_Options,SIGNAL(triggered()),this,SLOT(settingsDialog()));
	connect(action_New,SIGNAL(triggered()),this,SLOT(newProjectDialog()));

	connect(gt->git,SIGNAL(notify(const QString &)),this->statusBar(),SLOT(showMessage(const QString &)));
	connect(gt->git,SIGNAL(progress(int)),this,SLOT(progress(int)));
	connect(gt->git,SIGNAL(logReceived(QStandardItemModel *)),this,SLOT(logReceived(QStandardItemModel *)));
	connect(gt->git,SIGNAL(commitDetails(QStringList)),this,SLOT(commitDetails(QStringList)));
	connect(gt->git,SIGNAL(userSettings(QString, QString)),this,SLOT(userSettings(QString, QString)));

	connect(gt->git,SIGNAL(initOutputDialog()),this,SLOT(initOutputDialog()));
	connect(gt->git,SIGNAL(notifyOutputDialog(const QString &)),this,SLOT(notifyOutputDialog(const QString &)));
	connect(gt->git,SIGNAL(doneOutputDialog()),this,SLOT(doneOutputDialog()));
	
	connect(logView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(logClicked(const QModelIndex &)));
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
	settings.setValue("horizontalSplitter2_2", horizontalSplitter2_2->saveState());
	settings.setValue("verticalSplitter1", verticalSplitter1->saveState());
	settings.endGroup();
	settings.beginGroup("Git");
	settings.setValue("gitbinary",gt->git->getGitBinaryPath());
	settings.endGroup();
	
	settings.beginGroup("TeamGit");
	settings.setValue("workspace",gSettings->teamGitWorkingDir);
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
     horizontalSplitter2_2->restoreState(settings.value("horizontalSplitter2_2").toByteArray()); 
     settings.endGroup();
     settings.beginGroup("Git");
     gt->git->setGitBinaryPath(settings.value("gitbinary").toString());
     settings.endGroup();
     
	settings.beginGroup("TeamGit");
	gSettings->teamGitWorkingDir = settings.value("workspace",QString("notset")).toString();
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
	gt->git->setWorkingDir(gSettings->teamGitWorkingDir + "teamgit");
	GIT_INVOKE("getLog");
	
}

void MainWindowImpl::newProjectDialog()
{
	int ret = npd->exec();
	if( ret == QDialog::Accepted) {
		QMetaObject::invokeMethod(gt->git,"clone",
                           Q_ARG(QString, npd->getRepository()),                       
                           Q_ARG(QString, npd->getBranch()),
                           Q_ARG(QString, npd->getRefRepo()));
	}
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
	opd->initOutputDialog();
}

void MainWindowImpl::notifyOutputDialog(const QString &msg)
{
	opd->notifyOutputDialog(msg);
}

void MainWindowImpl::doneOutputDialog()
{
	opd->doneOutputDialog();
	QApplication::restoreOverrideCursor();
}

void MainWindowImpl::logReceived(QStandardItemModel *model)
{
	QStandardItemModel *prevModel;
	prevModel = (QStandardItemModel *)logView->model();
	logModel = model;
	logView->setModel(logModel);
	if(prevModel)
		delete prevModel;
	logView->setColumnWidth(0,450);
	logView->setColumnWidth(1,200);
}

void MainWindowImpl::commitDetails(QStringList cd)
{
	commit_author->setText(cd.at(1).remove(0,8));
	commit_date->setText(cd.at(2).remove(0,6));
	commit_log->setText(cd.at(3));
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
	
}

void MainWindowImpl::progress(int i)
{
	if(i == 0) {
		progressBar = new QProgressBar(this);
		statusBar()->addPermanentWidget(progressBar);
	} else if(i == 100) {
		if(progressBar) {
			statusBar()->removeWidget(progressBar);
			delete progressBar;
			progressBar = NULL;
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

void MainWindowImpl::logClicked(const QModelIndex &index)
{
	QStandardItem *item = logModel->itemFromIndex(index);
	QString str;
	//str.sprintf("%i",item->row());
	//logView->item(item->row(),3)->text();
	
	QMetaObject::invokeMethod(gt->git,"getCommit",
                           Q_ARG(QString,logModel->item(item->row(),3)->text()));
}


//Used for connecting random things while devloping,
//Usefull if you want to pop a dialog for debug from git thread etc.
void MainWindowImpl::testSlot()
{

}
//
