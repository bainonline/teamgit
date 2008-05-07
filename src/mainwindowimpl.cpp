#include <QMetaObject>
#include <QTimer>
#include <QSettings>
#include <QStringList>
#include <QStringListIterator>
#include <QVariant>
#include <QMetaType>

#include "defs.h"
#include "mainwindowimpl.h"
#include "gitthread.h"
#include "settingsimpl.h"
//
MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f)
	: QMainWindow(parent, f)
{
	setupUi(this);
	gt = new GitThread();
	gt->start();
	sd = new SettingsImpl();
	QTimer::singleShot(0,this,SLOT(initSlot()));
	readSettings();
	setupConnections();
}

void MainWindowImpl::setupConnections()
{
	connect(pushButton,SIGNAL(clicked()),this,SLOT(testSlot()));

	connect(action_Options,SIGNAL(triggered()),this,SLOT(settingsDialog()));

	connect(gt->git,SIGNAL(notify(const QString &)),this->statusBar(),SLOT(showMessage(const QString &)));
	connect(gt->git,SIGNAL(progress(int)),this,SLOT(progress(int)));
	connect(gt->git,SIGNAL(logReceived(QStandardItemModel *)),this,SLOT(logReceived(QStandardItemModel *)));
	connect(gt->git,SIGNAL(userSettings(QString, QString)),this,SLOT(userSettings(QString, QString)));
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
	settings.setValue("workspace",teamGitWorkingDir);
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
	teamGitWorkingDir = settings.value("workspace",QString("notset")).toString();
	settings.endGroup();	


}

void MainWindowImpl::initSettings()
{
	if(teamGitWorkingDir == "notset") {
		settingsDialog();
	} 
}

void MainWindowImpl::initSlot()
{
	initSettings();
	GIT_INVOKE("getLog");
	
}

void MainWindowImpl::settingsDialog()
{
	sd->setTeamGitWorkingDir(teamGitWorkingDir);
	sd->setGitBinaryPath(gt->git->getGitBinaryPath());
	int ret = sd->exec();
	if( ret == QDialog::Accepted) {
		gt->git->setGitBinaryPath(sd->getGitBinaryPath());
		if(teamGitWorkingDir != sd->getTeamGitWorkingDir()) {
			teamGitWorkingDir = sd->getTeamGitWorkingDir();
			emit teamGitWorkingDirChanged(teamGitWorkingDir);
		}
	}
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

void MainWindowImpl::testSlot()
{

}
//
