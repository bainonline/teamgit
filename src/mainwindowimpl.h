#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include <QStandardItemModel>
#include <QProgressBar>

#include "ui_mainwindow.h"
#include "gitthread.h"
#include "settingsimpl.h"
#include "newprojectimpl.h"
#include "outputdialogimpl.h"
#include "projectsmodel.h"

//

#define GIT_INVOKE(action_slot)	\
		do {			\
				QMetaObject::invokeMethod(gt->git,action_slot,Qt::QueuedConnection); \
			} while(0)	



class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
private:
	
	SettingsImpl *sd;
	NewProjectImpl *npd;
	QStandardItemModel *logModel;
	ProjectsModel *projectsModel;
	QProgressBar *progressBar;
	OutputDialogImpl *opd;
	
	void readSettings();
	void writeSettings();
	void setupConnections();
	void initSettings();
	void refresh();
	void hideLogReset();
	void showLogReset();
	
public:
	GitThread *gt;
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	~MainWindowImpl();

private slots:
	void initSlot();
	
	void settingsDialog();
	void newProjectDialog();
	
	void logReceived();
	void fileLogReceived();
	void filesReceived(QString);
	void progress(int);
	
	void commitDetails(QStringList);
	
	void initOutputDialog();
	void notifyOutputDialog(const QString &);
	void doneOutputDialog();
	
	void userSettings(QString, QString);
	
	void logClicked(const QModelIndex &);
	void projectFilesViewClicked(const QModelIndex &);
	void projectsComboBoxActivated(int);
	void resetLog();
	void testSlot();

signals:
	void teamGitWorkingDirChanged(const QString &newDir);
};
#endif



