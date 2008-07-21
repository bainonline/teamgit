#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QMessageBox>
#include <QFile>
#include <QTemporaryFile>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "gitprocess.h"
#include "gsettings.h"

GitProcess::GitProcess()
	: QProcess()
{
	setWorkingDirectory("/media/sda7/home/bain/linux-2.6");
	gitBinary = "/home/bain/bin/git";
	
	pty.open();
}

void GitProcess::setGitBinaryPath(const QString &path)
{
	gitBinary = path; 
}

QString GitProcess::getGitBinaryPath()
{
	return gitBinary; 
}

QByteArray GitProcess::runGit(QStringList arguments,bool block,bool usePseudoTerm) 
{
	usePty=usePseudoTerm;
	setWorkingDirectory(workingDir);
	start(gitBinary,arguments);

	if(block)
		waitForFinished();
	return readAllStandardOutput();
}


void GitProcess::setupChildProcess()
{
 #if defined Q_OS_UNIX
	if(usePty) {
	 	int fd=::open(pty.ttyName(),O_RDWR);
		::dup2(fd,0);
		::dup2(fd,1);
		::dup2(fd,2);
	}
#endif
}

QByteArray GitProcess::readAllStandardOutput() 
{
	if(usePty) {
		QByteArray array;
		char data[1024]={
			0,};
		int	ret;
again:
		while(1){
			ret =::read(pty.masterFd(),data,1023);
			array.append(data);
			if(ret != 0)
				break;
			goto again;
		}
		return array;
	} else {
		return QProcess::readAllStandardOutput();
	}
}

void GitProcess::stageHunk(QString hunk)
{
	QTemporaryFile file;
 	if (file.open()) {
		QTextStream stream( &file );
        	stream << hunk;
	}
	file.close();
	QStringList args;
	
	args << "apply";
	args << "--index";
	args << "--cached";
	args << file.fileName();
	
	runGit(args);
	
     // the QTemporaryFile destructor removes the temporary file
     emit patchApplied();
}

//Used to unstage all staged changes
void  GitProcess::reset(QString ref,bool hard)
{
	QStringList args;
	args << "reset";
	if(hard)
		args << "--hard";
	else 
		args << "--soft";
	if(!ref.isEmpty())
		args << ref;
	runGit(args);
	emit resetDone();
}


void  GitProcess::addFiles(QStringList files)
{
	QStringList args;
	args << "add";
	for(int i=0;i < files.size();i++) {
		args << files[i];
	}
	runGit(args);
	emit addDone();
}

void  GitProcess::commit(QString commit_msg)
{
	QStringList args;
	args << "commit";
	if(!commit_msg.isEmpty()) {
		args << "-m";
		args << commit_msg;
	} else {
		return;
	}
	runGit(args);
	emit commitDone();
}

void  GitProcess::tag(QString tag)
{
	QStringList args;
	args << "tag";
	if(!tag.isEmpty())
		args << tag;
	runGit(args);
	emit tagDone();
}

void GitProcess::pull(QString repo, QString branch, QString mergeStrategy) 
{

	QStringList args;
	args << "pull";
	if(!repo.isEmpty()) 
		args << branch;
	if(!branch.isEmpty()) 
		args << branch;
	//fixme
	if(!mergeStrategy.isEmpty()) 
		args << "--strategy" << mergeStrategy;
		
	emit notify("Doing Pull");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString(array));	
	while(state()) {
		array = readAllStandardOutput();
		if(array.size())
			notifyOutputDialog(QString(array));
		array = readAllStandardError();
		if(array.size())
			notifyOutputDialog(QString(array));
		waitForFinished(100);
	}
	emit doneOutputDialog();
	emit notify("Ready");
}

void GitProcess::pull()
{
	pull(QString(),QString(),QString());
}

void GitProcess::clone(QString repo, QString target, QString refRepo,QString dir) 
{

	QStringList args;
	args << "clone" << repo;
	if(!target.isEmpty()) 
		args << target;
	if(!refRepo.isEmpty()) 
		args << "--reference" << refRepo;
	
	workingDir = dir;
	emit notify("Cloning");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString(array));	

	while(state()) {
		array = readAllStandardOutput();
		if(array.size())
			notifyOutputDialog(QString(array));
		//array = readAllStandardError();
		//if(array.size())
			//notifyOutputDialog(QString(array));
		waitForFinished(100);
	}
	
	emit cloneComplete(target);
	emit notify("Ready");
	emit doneOutputDialog();
}

void GitProcess::getUserSettings()
{
	QStringList args, args2;
	
	args << "config" << "--global" << "--get" << "user.name";
	emit notify("Getting user settings");
	QByteArray array = runGit(args);
	QString name(array);

	args2 << "config" << "--global" << "--get" << "user.email";
	QByteArray array2 = runGit(args2);
	QString email(array2);

	emit userSettings(name,email);
	emit notify("Ready");
}


void GitProcess::setUserSettings()
{
	QStringList args, args2;
	args << "config" << "--global" << "user.name" << gSettings->userName;
	emit notify("Setting user settings");
	runGit(args);

	args2 << "config" << "--global"  << "user.email" << gSettings->userEmail;
	runGit(args2);
	emit notify("Ready");
}

void GitProcess::getCurDiff()
{
	QStringList args;
	args << "diff";
	QByteArray array = runGit(args);
	QString diff(array);
	emit currentDiff(diff);

}

void GitProcess::getFileLog(QString files)
{
	QStringList args;
	QString s;

	logModel = new QStandardItemModel(0,4);	
	emit notify("Running git log for file");
	emit progress(0);
	args << "log";
	args.append("--pretty=format:%sTEAMGITFIELDMARKER%an<%ae>TEAMGITFIELDMARKER%adTEAMGITFIELDMARKER%HTEAMGITFIELDEND");
	args << files;
	
	QByteArray array = runGit(args);
	
	emit notify("Parsing log");
	emit progress(50);
	
	QString log(array);
	//
	//QStandardItem *it = new QStandardItem(QString("Log"));
	//QStandardItem *it1 = new QStandardItem(QString("Author"));
	//QStandardItem *it2 = new QStandardItem(QString("Date"));
	//QStandardItem *it3 = new QStandardItem(QString("Commit"));
	//logModel->setHorizontalHeaderItem(0,it);
	//logModel->setHorizontalHeaderItem(1,it1);
	//logModel->setHorizontalHeaderItem(2,it2);
	//logModel->setHorizontalHeaderItem(3,it3);
	//
	//QString delimit("TEAMGITFIELDEND");
	//QString delimit2("TEAMGITFIELDMARKER");
	//QStringList logList=log.split(delimit);
	//int numLogs=logList.count();
	//int parsed=0;
	//QStringListIterator iterator(logList);
	//while (iterator.hasNext()) {
		//parsed++;
		//int percent=50 + ((float)((float)parsed/(float)numLogs)*(float)100)/2;
		//if(!(percent % 10) && percent) {
			//emit progress(percent);
		//}
		//QString singleLog = iterator.next();
		//QStringList logFields = singleLog.split(delimit2);
		//QStringListIterator it2(logFields);
		//QList<QStandardItem*> itemlist;
		//QString oneLiner = it2.next();
		//if(oneLiner.startsWith("\n"))
			//oneLiner = oneLiner.remove(0,1);
		//QStandardItem *item1 = new QStandardItem(oneLiner);
		//item1->setEditable(false);
		//itemlist.append(item1);
		//
		//while(it2.hasNext()) {
			//QStandardItem *item1 = new QStandardItem(it2.next());
			//item1->setEditable(false);	
			//itemlist.append(item1);
		//}
		//logModel->appendRow(itemlist);
	//}
	
	emit fileLogReceived(log);
}

void GitProcess::getLog(int numLog)
{
	QStringList args;
	QString s;
	

	logModel = new QStandardItemModel(0,4);	
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
		item1->setEditable(false);
		itemlist.append(item1);
		
		while(it2.hasNext()) {
			QStandardItem *item1 = new QStandardItem(it2.next());
			item1->setEditable(false);	
			itemlist.append(item1);
		}
		logModel->appendRow(itemlist);
	}
	
	emit logReceived();
	emit notify("Ready");
	emit progress(100);
	
}

void GitProcess::getCommit(QString commitHash)
{
	QStringList args;
	QString s;
	
	emit notify("Gathering commit information");
	emit progress(0);
	args << "show";
	args << commitHash;
	
	QByteArray array = runGit(args);
	
	emit notify("Parsing commit blob");
	emit progress(50);
	
	QString commit(array);
	QStringList commitDet;
	commitDet << commit.left(commit.indexOf(QChar('\n')));
	commit.remove(0,commit.indexOf(QChar('\n'))+1);
	emit progress(65);
	if(commit.startsWith("Merge:"))
		commit.remove(0,commit.indexOf(QChar('\n'))+1);
	commitDet << commit.left(commit.indexOf(QChar('\n')));
	commit.remove(0,commit.indexOf(QChar('\n'))+1);
	emit progress(70);
	commitDet << commit.left(commit.indexOf(QChar('\n')));
	commit.remove(0,commit.indexOf(QChar('\n'))+1);
	emit progress(75);
	QString log;
	while(1) {
		if(commit.startsWith("diff"))
			break;
		if(!commit.startsWith("\n")) {
			log.append(commit.left(commit.indexOf(QChar('\n'))+1));
		}
		commit.remove(0,commit.indexOf(QChar('\n'))+1);
		if(commit.size() <= 1) 
			break;
	}
	commitDet << log;
	emit progress(90);
	commitDet << commit;
	emit commitDetails(commitDet);
	emit notify("Ready");
	emit progress(100);
}

void GitProcess::getFiles()
{
	QStringList args;
	
	emit notify("Gathering files");
	emit progress(0);
	args << "ls-files";
	
	QByteArray array = runGit(args);
	emit progress(50);
	
	QString files(array);
	emit projectFiles(files);

	emit notify("Ready");
	emit progress(100);
	WaitForEventDelivery();
}

void GitProcess::getBranches()
{
	QStringList args;
	
	LockEvent();

	emit notify("Gathering branches");
	emit progress(0);
	args << "branch";
	
	QByteArray array = runGit(args);
	emit progress(50);
	
	QString branches(array);
	emit branchList(branches);

	emit notify("Ready");
	emit progress(100);
	WaitForEventDelivery();
}

void GitProcess::getTags()
{
	QStringList args;
	
	LockEvent();

	emit notify("Gathering tags");
	emit progress(0);
	args << "tag";
	
	QByteArray array = runGit(args);
	emit progress(50);
	
	QString tags(array);
	emit tagList(tags);

	emit notify("Ready");
	emit progress(100);
}

