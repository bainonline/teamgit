#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include <QStandardItemModel>
#include <QProgressBar>

#include "ui_mainwindow.h"
#include "gitthread.h"
#include "settingsimpl.h"
#include "projectmanager.h"
//

#define GIT_INVOKE(action_slot)	\
		do {			\
				QMetaObject::invokeMethod(gt->git,action_slot,Qt::QueuedConnection); \
			} while(0)	


class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
private:
	
	//settings
	QString teamGitWorkingDir;
	QString userName;
	QString userEmail;
	
	SettingsImpl *sd;
	QStandardItemModel *logModel;
	QProgressBar *progressBar;
	
	void readSettings();
	void writeSettings();
	void setupConnections();
	void initSettings();
	ProjectSettings projectsettings;
public:
	GitThread *gt;
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	~MainWindowImpl();

private slots:
	void initSlot();
	void settingsDialog();
	void logReceived(QStandardItemModel *);
	void progress(int);
	void userSettings(QStringList);
	
	void testSlot();

signals:
	void teamGitWorkingDirChanged(const QString &newDir);
};
#endif




