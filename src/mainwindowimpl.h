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
#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QProgressBar>
#include <QAction>
#include <QActionGroup>
#include <QLineEdit>

#include "ui_mainwindow.h"
#include "gitthread.h"
#include "settingsimpl.h"
#include "newprojectimpl.h"
#include "outputdialogimpl.h"
#include "projectsmodel.h"
#include "commitdialogimpl.h"
#include "mergedialogimpl.h"
#include "diffviewer.h"
#include "resetdialogimpl.h"
#include "bonjourserviceregister.h"
//

#define MAX_RECENT 5
#define TEAMGIT_PORT 10000

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
private:

	QLineEdit *searchText;
	QActionGroup *searchOptionGroup;
	QAction *searchClear;
	QAction *searchNext, *searchPrevious;
	QAction *searchLog, *searchAuthor, *searchDate, *searchCommit;
	QList<QStandardItem *> searchItemsFoundList;
	int currentSearch;
	QTextEdit *annotatedFileBox;

	QAction *recentRepos[5];

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

	OutputDialogImpl *opd;
	CommitDialogImpl *cmd;
	ResetDialogImpl *rsd;
	MergeDialogImpl *md;

	QString mergeConflicts;

	BonjourServiceRegister *bonjourRegister;

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

	void addRecentlyOpened(QString dir);
	void updateRecentlyOpened();

	int fileAnnotationTabIndex;

	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);

public:
	GitThread *gt;
	GitThread *glt; //Git log thread
	GitThread *gft; //Git file status thread
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	~MainWindowImpl();

private slots:

	void about();

	void textSearch(const QString &);
	void nextSearch();
	void prevSearch();

	void initSlot();

	void openRepo(const QString path=QString());
	void openRecent();
	void refresh();

	void settingsDialog();
	void newProjectDialog();
	void pullDialog();

	void logReceived(QList< QList<QStandardItem *> >);
	void namedLogReceived(QString,QList< QList<QStandardItem *> >itemListList);
	void filesReceived(QString);
	void progress(QProgressBar *,int,QString msg=QString());
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

	void untrackedDoubleClicked(const QModelIndex &index);

	void diffDoubleClicked();

	void logClicked(const QModelIndex &);
	void projectFilesViewClicked(const QModelIndex &);
        void projectFilesViewDoubleClicked(const QModelIndex &);
	void projectsComboBoxActivated(int);
	void resetLog();
	void expandStagedUnstagedSlot();
	void testSlot();

	void newTag();
	void cherryPickSelectedCommit();
	void revertSelectedCommit();
	void merge();

	void pushSlot();
	void fetchRemoteBranchSlot();
	void newBranchSlot();
	void deleteBranchSlot();
	void newRemoteBranchSlot();
	void applyMail(QString patchPath=QString());

	void patchApplied();

	void gotAnnotatedFile(QString);
	void annotatedFileClicked();

	void sendPatchByMail();

	void gotHelpMessage(QString,QString);
	void gotCommands(QString);
	void guifyCommand();

	void rebaseInteractive();
	void gotUnMerged(QString);
	void resolvMerged();

signals:
	void test();
	void teamGitWorkingDirChanged(const QString &newDir);
};
#endif
