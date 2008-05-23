#ifndef GITPROCESS_H
#define GITPROCESS_H
//
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStandardItemModel>
#include <QFile>

#include "kpty.h"
//

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
	QStandardItemModel *logModel;
	
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
	void getCommit(QString commitHash);
	
	//Async slots
	void getUserSettings();
	void setUserSettings();
	void clone(QString,QString,QString);
	
	signals:
	void logReceived(QStandardItemModel *);
	void notify(const QString &);
	
	void initOutputDialog();
	void notifyOutputDialog(const QString &);
	void doneOutputDialog();
	
	void progress(int);
	void userSettings(QString, QString);
	
	void commitDetails(QStringList);
};
#endif
