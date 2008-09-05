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
#include "commitdialogimpl.h"
#include "diffviewer.h"
#include "resetdialogimpl.h"
//

#define GIT_INVOKE(action_slot)	\
		do {			\
				QMetaObject::invokeMethod(gt->git,action_slot,Qt::QueuedConnection); \
			} while(0)	

extern QMutex gitMutex;

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
private:
	
	DiffViewer *commit_diff;
	SettingsImpl *sd;
	NewProjectImpl *npd;
	QStandardItemModel *logModel;
	QStandardItemModel *tagsModel;
	QStandardItemModel *branchModel;
	ProjectsModel *remoteBranchesModel;
	
	ProjectsModel *projectsModel;
	ProjectsModel *stagedModel;
	ProjectsModel *unstagedModel;
	ProjectsModel *untrackedModel;

	QProgressBar *progressBar;
	OutputDialogImpl *opd;
	CommitDialogImpl *cmd;
	ResetDialogImpl *rsd;
	
	void readSettings();
	void checkAndSetWorkingDir(QString dir);
	void writeSettings();
	void setupConnections();
	void initSettings();
	void checkWorkingDiff();
	void hideLogReset();
	void hideStaged();
	void hideUnstaged();
	void hideUntracked();
	
	void showStaged();
	void showUnstaged();
	void showUntracked();
	
	void showLogReset();
	void populateProjects();
	QStandardItemModel *parseLog2Model(QString log);
	
public:
	GitThread *gt;
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	~MainWindowImpl();

private slots:
	void initSlot();
	
	void openRepo();
	void refresh();
	
	void settingsDialog();
	void newProjectDialog();
	void pullDialog();
	
	void logReceived(QString);
	void namedLogReceived(QString,QString);
	void filesReceived(QString);
	void progress(int);
	void cloneComplete(QString);
	void filesStatusReceived(QString);
	
	void branchListReceived(QString);
	void remoteBranchListReceived(QString);
	void tagsListReceived(QString);
	
	void branchesViewClicked(const QModelIndex &);
	void remoteBranchesViewClicked(const QModelIndex &);
	void tagsViewClicked(const QModelIndex &);
	
	void commitDetails(QStringList);
	void fileDiffReceived(QString,int);
	void hideShowUntracked();
	
	void commitSlot();
	void resetSlot();
	void checkoutSlot();
	
	void initOutputDialog();
	void notifyOutputDialog(const QString &);
	void doneOutputDialog();
	
	void userSettings(QString, QString);
	
	void stagedDoubleClicked(const QModelIndex &);
	void unstagedDoubleClicked(const QModelIndex &);
	void stagedClicked(const QModelIndex &index);
	void unstagedClicked(const QModelIndex &index);
	
	void diffDoubleClicked();
	
	void logClicked(const QModelIndex &);
	void projectFilesViewClicked(const QModelIndex &);
	void projectsComboBoxActivated(int);
	void resetLog();
	void expandStagedUnstagedSlot();
	void testSlot();
	
	void newTag();
	void cherryPickSelectedCommit();
	
	void pushSlot();
	void fetchRemoteBranchSlot();
	void newBranchSlot();
	void deleteBranchSlot();
	void newRemoteBranchSlot();
	
	void patchApplied();
	
signals:
	void test();
	void teamGitWorkingDirChanged(const QString &newDir);
};
#endif



