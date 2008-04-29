#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QMessageBox>

#include "gitprocess.h"

//
GitProcess::GitProcess()
	: QProcess()
{
	// TODO
	logModel = new QStandardItemModel(0,4);
	setWorkingDirectory("/media/sda7/home/bain/linux-2.6");
	gitBinary = "/home/bain/bin/git";
}
//

void GitProcess::setGitBinaryPath(const QString &path)
{
	gitBinary = path; 
}

QString GitProcess::getGitBinaryPath()
{
	return gitBinary; 
}

QByteArray GitProcess::runGit(QStringList arguments) 
{
	start(gitBinary,arguments);
	waitForFinished();
	return readAllStandardOutput();
}

void GitProcess::getLog(int numLog)
{
	QStringList args;
	QString s;
	
	emit notify("Running git log");
	emit progress(0);
	s.sprintf("-%i",numLog);
	args << "log";
	args.append("--pretty=format:%sTEAMGITFIELDMARKER%an<%ae>TEAMGITFIELDMARKER%adTEAMGITFIELDMARKER%HTEAMGITFIELDEND");
	QByteArray array = runGit(args);
	
	emit notify("Parsing log");
	emit progress(50);
	
	QString log(array);
	
	QStandardItem *it = new QStandardItem(QString("Log"));
	QStandardItem *it1 = new QStandardItem(QString("Author"));
	QStandardItem *it2 = new QStandardItem(QString("Date"));
	QStandardItem *it3 = new QStandardItem(QString("Commit"));

	logModel->setHorizontalHeaderItem(0,it);
	logModel->setHorizontalHeaderItem(1,it1);
	logModel->setHorizontalHeaderItem(2,it2);
	logModel->setHorizontalHeaderItem(3,it3);
	
	QString delimit("TEAMGITFIELDEND");
	QString delimit2("TEAMGITFIELDMARKER");
	QStringList logList=log.split(delimit);
	int numLogs=logList.count();
	int parsed=0;
	QStringListIterator iterator(logList);
	while (iterator.hasNext()) {
		parsed++;
		int percent=50 + ((float)((float)parsed/(float)numLogs)*(float)100)/2;
		if(!(percent % 10) && percent) {
			emit progress(percent);
		}
		QString singleLog = iterator.next();
		QStringList logFields = singleLog.split(delimit2);
		QStringListIterator it2(logFields);
		QList<QStandardItem*> itemlist;
		QString oneLiner = it2.next();
		if(oneLiner.startsWith("\n"))
			oneLiner = oneLiner.remove(0,1);
		QStandardItem *item1 = new QStandardItem(oneLiner);
		itemlist.append(item1);
		
		while(it2.hasNext()) {
			QStandardItem *item1 = new QStandardItem(it2.next());
			itemlist.append(item1);
		}
		logModel->appendRow(itemlist);
	}
	
	emit logReceived(logModel);
	emit notify("Ready");
	emit progress(100);
}
