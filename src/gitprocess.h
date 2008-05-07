#ifndef GITPROCESS_H
#define GITPROCESS_H
//
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStandardItemModel>
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
	QString gitBinary;
	QStandardItemModel *logModel;
	public:
	GitProcess();
	QByteArray runGit(QStringList arguments); 
	QString getGitBinaryPath();
	 
	public slots:
	void setGitBinaryPath(const QString &path); 
	void getLog(int numLog=1000);
	void getUserSettings();
	
	signals:
	void logReceived(QStandardItemModel *);
	void notify(const QString &);
	void progress(int);
	void userSettings(QString, QString);
};
#endif
