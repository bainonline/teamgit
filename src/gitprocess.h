/*
	Copyright 2007 2008 Abhijit Bhopatkar
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program, in file COPYING
	If not, see <http://www.devslashzero.com/teamgit/license>.
*/
#ifndef GITPROCESS_H
#define GITPROCESS_H
//
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStandardItemModel>
#include <QFile>
#include <QWaitCondition>
#include <QMutex>

#include "kpty.h"
//

enum gitDiffType {commitDiff=0,stagedDiff,unstagedDiff};
enum gitResetType {simple=0,soft,hard};

//Used byg git and main thread to sync on event delivery;
extern QWaitCondition eventDelivered; 
extern QMutex mutex;
/*
This is the class which is responsible for both executing the 
git process _and_ parsing its output.
The plan is to run all the required functions of this class in
a separate thread from the main GUI thread keeping the GUI
active all the time.
To do that we create an object of this class in a separate 
qthread object (gitthread.cpp) and invoke the slots of this object 
from main  thread using QT4 new feature called queued connections.
We emit results back as signal with whatever resulting object 
passed as arguement (again this is possible only in qt4 with 
queued connections being able to pass standard qt obejects).
Make sure you do not use any object derived from QWidget in
this thread.
(e.g. for git log we use Model/view architecture and pass
QStandardModel as a result back to Main UI thread) 
*/
class GitProcess : public QProcess
{
	
Q_OBJECT
	private:
	
	bool usePty;
	KPty pty;
	QString gitBinary;	

	QString workingDir;
	
	void sendGitOutput();
	
	public:
	GitProcess();

	QByteArray runGit(QStringList arguments, bool block=true,bool usePseudoTerm=false); 
	QString getGitBinaryPath();
	void setupChildProcess();
	public:
	
	QByteArray readAllStandardOutput();
	void setWorkingDir(QString dir) {
		workingDir=dir;
	}
	
	public slots:
	void setGitBinaryPath(const QString &path); 
	void getLog(int numLog=1000);
	void getNamedLog(QString);
	void getCommit(QString commitHash);
	void getFiles();
	void stageFiles(QStringList files);
	void unstageFiles(QStringList files);
	void stageHunk(QString hunk);
	void unStageHunk(QString hunk);
	void commit(QString commit_msg,QString author_name,QString author_email);
	void tag(QString);
	void addFiles(QStringList);
	void getDiff(QString);
	void getDiffCached(QString);
	void getTags();
	void getBranches();
	void getRemoteBranches();
	void getStatus();
	void cherryPick(QString);
	//void checkout(QString ref);
	void reset(QString ref,int type=0);
	
	//Async slots
	void getUserSettings();
	void setUserSettings();
	void clone(QString,QString,QString,QString);
	void pull();
	void pull(QString,QString,QString);
	void push();
	void fetch(QString);
	
	void checkout(QString);
	void merge(QString);
	
	void newBranch(QString,QString);
	void newRemoteBranch(QString,QString);
	void deleteBranch(QString);
	
	signals:
	void logReceived(QString);
	void namedLogReceived(QString,QString);
	void notify(const QString &);
	
	void initOutputDialog();
	void notifyOutputDialog(const QString &);
	void doneOutputDialog();
	
	void progress(int);
	void userSettings(QString, QString);
	
	void commitDetails(QStringList);
	void projectFiles(QString);

	void cloneComplete(QString);
	
	void patchApplied();
	void resetDone();
	void tagDone();
	void addDone();
	void commitDone();
	
	void branchList(QString);	
	void remoteBranchesList(QString);
	void tagList(QString);
	
	void filesStatus(QString);
	void fileDiff(QString,int);
	
	void refresh();
};
#endif
