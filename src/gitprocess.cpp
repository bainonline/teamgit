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
#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QMessageBox>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include "gitprocess.h"
#include "gsettings.h"

GitProcess::GitProcess()
	: QProcess()
{
	setWorkingDirectory("/media/sda7/home/bain/linux-2.6");
	gitBinary = "/home/bain/bin/git";
	gitRebaseBinary = "/usr/bin/teamgit-rebase";
#if defined Q_OS_UNIX
	pty.open();
#endif
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
	QStringList args;
	args << "--no-pager";
	args << arguments;
	usePty=usePseudoTerm;
	setWorkingDirectory(workingDir);
	start(gitBinary,args);

	if(block)
		waitForFinished(-1);
	return readAllStandardOutput();
}


void GitProcess::setupChildProcess()
{
 #if defined  Q_OS_UNIX
	if(usePty) {
	 	int fd=::open(pty.ttyName(),O_RDWR);
		::dup2(fd,0);
		::dup2(fd,1);
		::dup2(fd,2);
	}
#endif
}

void GitProcess::rebaseInteractive(QString commit)
{
	emit notify("Getting help for command");
	QStringList env;
	QStringList args;
	args << "rebase";
	args << "-i";
	args << commit;
	env = QProcess::systemEnvironment();
	env << "GIT_EDITOR=" + gitRebaseBinary;
	setEnvironment(env);
	emit notify("rebase");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	env = QProcess::systemEnvironment();
	QStringList newEnv;
	for(int i = 0; i < env.size();i++) {
		if(!env[i].startsWith("GIT_EDITOR")) {
			newEnv << env[i];
		}
	}
	setEnvironment(newEnv);
	emit doneOutputDialog();
}

QByteArray GitProcess::readAllStandardOutput()
{
#if defined Q_OS_UNIX
	if(usePty) {
		QByteArray array;
		char data[1024]={
			0,};
		int	exit=0;
		while(!exit) {
			exit =::read(pty.masterFd(),data,1023);
			array.append(data);
		}
		return array;
	} else {
#endif
		return QProcess::readAllStandardOutput();
#if defined Q_OS_UNIX
	}
#endif
}

void GitProcess::sendGitOutput()
{
	QByteArray array;
	while(state()) {
		array = readAllStandardOutput();
		if(array.size())
			notifyOutputDialog(QString::fromUtf8(array));
		array = readAllStandardError();
		if(array.size())
			notifyOutputDialog(QString::fromUtf8(array));
		waitForFinished(100);
	}
	array = readAllStandardOutput();
	if(array.size())
		notifyOutputDialog(QString::fromUtf8(array));
	array = readAllStandardError();
	if(array.size())
		notifyOutputDialog(QString::fromUtf8(array));
}

void GitProcess::getCommands()
{
	QStringList args;

	emit notify("Getting help for command");
	args << "help";
	args << "-a";
	QByteArray array = runGit(args);

	emit notify("ready");
	emit commands(QString::fromUtf8(array));
}

void GitProcess::getHelp(QString command)
{
	QStringList args;

	emit notify("Getting help for command");
	args << "help";
	args << command;
	QByteArray array = runGit(args);

	emit notify("ready");
	emit helpMessage(command, QString::fromUtf8(array));
}

void GitProcess::runArgs(QStringList args,bool refrsh)
{
	emit notify("Running git " + args[0]);
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit doneOutputDialog();
	emit notify("Ready");
	if(refrsh)
		emit refresh();
}

void GitProcess::unstageFiles(QStringList files)
{
	for(int i = 0;i<files.size();i++) {
		QStringList args;
		args << "reset";
		args << "HEAD";
		args << files[i];
		runGit(args);
	}
	getStatus();
}

void GitProcess::stageFiles(QStringList files)
{
	QStringList args;

	args << "add";
	args << files;
	runGit(args);
	getStatus();
}

void GitProcess::unStageHunk(QString hunk)
{
	QFile file("/tmp/teamgitpatchdonotcreateafilelikethis");
 	if (file.open(QIODevice::ReadWrite)) {
		QTextStream stream( &file );
        	stream << hunk;
	}
	file.close();
	QStringList args;

	args << "apply";
	args << "--cached";
	args << "--reverse";
	args << file.fileName();

	runGit(args);
	file.remove();
	emit patchApplied();
}

void GitProcess::stageHunk(QString hunk)
{
	QFile file("/tmp/teamgitpatchdonotcreateafilelikethis");
 	if (file.open(QIODevice::ReadWrite)) {
		QTextStream stream( &file );
        	stream << hunk;
	}
	file.close();
	QStringList args;

	args << "apply";
	args << "--cached";
	args << file.fileName();

	runGit(args);
	file.remove();
	emit patchApplied();
}

void  GitProcess::addFiles(QStringList files)
{
	QStringList args;
	args << "add";
	args << files;
	runGit(args);
	emit addDone();
}

void  GitProcess::commit(QString commit_msg,QString author_name,QString author_email,bool signoff,bool amend)
{
	QStringList args;
	args << "commit";
	if(!author_name.isEmpty() && !author_email.isEmpty()) {
		args << "--author";
		QString auth = author_name + " <" + author_email + "> ";
		args << auth;
	}
	if(signoff) {
		args << "--signoff";
	}
    if(amend) {
        args << "--amend";
    }
	if(!commit_msg.isEmpty()) {
		args << "-m";
		args << commit_msg;
	} else {
		return;
	}
	runGit(args);
	emit commitDone();
}

void  GitProcess::tag(QString tag,QString msg)
{
	QStringList args;
    args << "tag";
    if(!msg.isEmpty()) {
        args << "-a";
        args << "-m";
        args << msg;
    }
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
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
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
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();

	emit cloneComplete(target);
	emit notify("Ready");
	emit doneOutputDialog();
}

void GitProcess::getUserSettings()
{
	QStringList args, args2;

	args << "config" << "--global" << "--get" << "user.name";
	emit notify("Getting user settings");
	QByteArray name = runGit(args);

	args2 << "config" << "--global" << "--get" << "user.email";
	QByteArray email = runGit(args2);

	emit userSettings(QString::fromUtf8(name), QString::fromUtf8(email));
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

void GitProcess::getDiffCached(QString file)
{
	QStringList args;
	args << "diff";
	args << "--cached";
	args << file;
	QByteArray diff = runGit(args);
	emit fileDiff(QString::fromUtf8(diff),stagedDiff);
}

void GitProcess::getDiff(QString file)
{
	QStringList args;
	args << "diff";
	args << file;
	QByteArray diff = runGit(args);
	emit fileDiff(QString::fromUtf8(diff),unstagedDiff);
}

QList< QList<QStandardItem *> > GitProcess::parseLog2Model(QString log)
{
	QList< QList<QStandardItem *> > StandardItemListList;

	QString delimit("TEAMGITFIELDEND");
	QString delimit2("TEAMGITFIELDMARKER");
	QStringList logList=log.split(delimit);
	int numLogs=logList.count();
	int parsed=0;
	QStringListIterator iterator(logList);
	while (iterator.hasNext()) {
		parsed++;
		int percent=50 + ((float)((float)parsed/(float)numLogs)*(float)100)/2;
		if(percent && !(percent % 10)) {
			progress(pb,percent);
		}
		QString singleLog = iterator.next();
		if(!singleLog.contains(delimit2))
			continue;
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
		StandardItemListList.append(itemlist);
	}
	return StandardItemListList;
}

void GitProcess::getNamedLog(QString ref)
{
	QStringList args;
	QString s;

	emit notify("Running git log for file");
	emit progress(pb,0,"Running git log for file");
	args << "log";
	args.append("--pretty=format:%sTEAMGITFIELDMARKER%an<%ae>TEAMGITFIELDMARKER%adTEAMGITFIELDMARKER%HTEAMGITFIELDEND");
	args << ref;

	QByteArray array = runGit(args);

	emit notify("Parsing log");
	emit progress(pb,50,"Parsing log");

	QString log(QString::fromUtf8(array));
	QList< QList<QStandardItem *> >itemListList = parseLog2Model(log);
	emit namedLogReceived(ref,itemListList);
}

void GitProcess::getLog(int numLog)
{
	QStringList args,args2;
	QString s;

	emit notify("Running git log");
	emit progress(pb,0,"Running git log");
	s.sprintf("-%i",numLog);
	args << "log";
	args.append("--pretty=format:%sTEAMGITFIELDMARKER%an<%ae>TEAMGITFIELDMARKER%adTEAMGITFIELDMARKER%HTEAMGITFIELDEND");
	QByteArray array = runGit(args);
	QString log(QString::fromUtf8(array));
	QList< QList<QStandardItem *> >itemListList;
	itemListList = parseLog2Model(log);
	emit logReceived(itemListList);
	emit notify("Parsing initial log");
	emit progress(pb,100,"Parsing log");
}


void GitProcess::getCommit(QString commitHash)
{
	QStringList args;
	QString s;

	emit notify("Gathering commit information");
	emit progress(pb,0,"Gathering commit information");
	args << "show";
	args << commitHash;

	QByteArray array = runGit(args);

	emit notify("Parsing commit blob");
	emit progress(pb,50,"Parsing commit blob");

	QString commit(QString::fromUtf8(array));
	QStringList commitDet;
	commitDet << commit.left(commit.indexOf(QChar('\n')));
	commit.remove(0,commit.indexOf(QChar('\n'))+1);
	emit progress(pb,65);
	if(commit.startsWith("Merge:"))
		commit.remove(0,commit.indexOf(QChar('\n'))+1);
	commitDet << commit.left(commit.indexOf(QChar('\n')));
	commit.remove(0,commit.indexOf(QChar('\n'))+1);
	emit progress(pb,70);
	commitDet << commit.left(commit.indexOf(QChar('\n')));
	commit.remove(0,commit.indexOf(QChar('\n'))+1);
	emit progress(pb,75);
	QString log;
	while(commit.size() > 1) {
		if(commit.startsWith("diff"))
			break;
		if(!commit.startsWith("\n")) {
            log.append(commit.left(commit.indexOf(QChar('\n'))+1).remove(0,4));
		}
		commit.remove(0,commit.indexOf(QChar('\n'))+1);
	}

	commitDet << log;
	emit progress(pb,90);
	//if(commit.size() < 1024*100)
		commitDet << commit;
	//else
		//commitDet << "too huge commit to parse";
	emit commitDetails(commitDet);
	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::getFiles()
{
	QStringList args;

	emit notify("Gathering files");
	emit progress(pb,0,"Gathering files");
	args << "ls-files";

	QByteArray files = runGit(args);
	emit progress(pb,50);

	emit projectFiles(QString::fromUtf8(files));

	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::getStatus()
{
	QStringList args;

	emit notify("Gathering files status");
	emit progress(pb,0,"Gathering files status");
        args << "status" << "--porcelain";

	QByteArray status= runGit(args);
	emit progress(pb,50);

	emit filesStatus(QString::fromUtf8(status));

	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::revert(QString ref)
{
        QStringList args;

        args << "revert";
        args << "--";
        args << ref;

        emit notify("Revert");
        emit initOutputDialog();
        QByteArray array = runGit(args,false,true);
        notifyOutputDialog(QString::fromUtf8(array));
        sendGitOutput();
        emit doneOutputDialog();
        emit notify("Ready");
        emit refresh();
}

void GitProcess::cherryPick(QString ref)
{
	QStringList args;

	args << "cherry-pick";
	args << ref;

	emit notify("Cherry pick");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit doneOutputDialog();
	emit notify("Ready");
	emit refresh();
}

void GitProcess::getBranches()
{
	QStringList args;

	emit notify("Gathering branches");
	emit progress(pb,0,"Gathering branches");
	args << "branch";

	QByteArray branches = runGit(args);
	emit progress(pb,50);

	emit branchList(QString::fromUtf8(branches));

	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::getRemoteBranches()
{
	QStringList args;

	emit notify("Gathering branches");
	emit progress(pb,0,"Gathering branches");
	args << "branch";
	args << "-r";

	QByteArray branches = runGit(args);
	emit progress(pb,50);

	emit remoteBranchesList(QString::fromUtf8(branches));

	emit notify("Ready");
	emit progress(pb,100);
}


void GitProcess::getTags()
{
	QStringList args;

	emit notify("Gathering tags");
	emit progress(pb,0);
	args << "tag";

	QByteArray tags = runGit(args);
	emit progress(pb,50,"Gathering tags");

	emit tagList(QString::fromUtf8(tags));

	emit notify("Ready");
	emit progress(pb,100);
}

void  GitProcess::checkout(QString ref)
{
	QStringList args;

	args << "checkout";
	args << ref;

	emit notify("Checking out");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit doneOutputDialog();
	emit notify("Ready");
	emit refresh();
}

void  GitProcess::merge(QString ref)
{
	QStringList args;

	args << "merge";
	args << ref;

	emit notify("Merging");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit doneOutputDialog();
	emit notify("Ready");
	emit refresh();
}

void GitProcess::push(QString repo)
{
	QStringList args;

	args << "push";
	if(!repo.isEmpty())
		args << repo;

	emit notify("Pushing out");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit doneOutputDialog();
	emit notify("Ready");
}

void GitProcess::applyMail(QString mailPath,bool signoff)
{
	QStringList args;

	args << "am";
	if(signoff)
		args << "--signoff";
	args << mailPath;
	emit notify("Applying mailed patches");
	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit refresh();
	emit doneOutputDialog();
	emit notify("Ready");
}

void GitProcess::newBranch(QString branch,QString ref)
{
	QStringList args;

	emit notify("Creating new branch");
	emit progress(pb,0,"Creating new branch");
	args << "branch";
	args << branch;
	if(!ref.isEmpty())
		args << ref;

	QByteArray array = runGit(args);
	emit progress(pb,100);

	emit refresh();
	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::newRemoteBranch(QString name,QString repo)
{
	QStringList args;

	emit notify("Creating new remote branch");
	emit progress(pb,0,"Creating new remote branch");
	args << "remote";
	args << "add";
	args << name;
	args << repo;

	QByteArray array = runGit(args);
	emit progress(pb,100);

	emit refresh();
	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::deleteBranch(QString branch)
{
	QStringList args;

	emit notify("Deleting branch");
	emit progress(pb,0,"Deleting branch");
	args << "branch";
	args << "-d";
	args << branch;

	QByteArray array = runGit(args);
	emit progress(pb,100);

	emit refresh();
	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::blame(QString file)
{
	QStringList args;

	emit notify("Getting annotated file");
	args << "blame";
	args << "-p";
	args << file;

	QByteArray array = runGit(args);
	emit annotatedFile(QString::fromUtf8(array));
	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::fetch(QString ref)
{
	QStringList args;

	emit notify("fetching");
	args << "fetch";
	args << ref;

	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit doneOutputDialog();
	emit notify("Ready");
}

void GitProcess::reset(QString ref,int type)
{
	QStringList args;

	emit notify("reset");
	args << "reset";
	if(type==soft)
		args << "--soft";
	else if (type==hard)
		args << "--hard";
	args << ref;

	emit initOutputDialog();
	QByteArray array = runGit(args,false,true);
	notifyOutputDialog(QString::fromUtf8(array));
	sendGitOutput();
	emit doneOutputDialog();
	emit notify("Ready");
	emit refresh();
}

void GitProcess::checkUnMerged()
{
	QStringList args;

	emit notify("Gathering merge conflicts");
	emit progress(pb,0,"Gathering merge conflicts");
	args << "ls-files";
	args << "-u";

	QByteArray files = runGit(args);
	emit progress(pb,50);

	emit unMerged(QString::fromUtf8(files));

	emit notify("Ready");
	emit progress(pb,100);
}

void GitProcess::getUnMergedFileContents(QString file)
{
	QStringList args;

	emit notify("Gathering unmerged files contents");
	emit progress(pb,0,"Gathering unmerged files contents");
	args << "cat-file";
	args << "blob";
	args << ":1:" + file;
	QByteArray content1 = runGit(args);
	args.removeLast();
	args <<  ":2:" + file;
	emit progress(pb,33);
	QByteArray content2 = runGit(args);
	args.removeLast();
	args <<  ":3:" + file;
	emit progress(pb,66);
	QByteArray content3 = runGit(args);

	QStringList contents;
	contents << QString::fromUtf8(content1);
	contents << QString::fromUtf8(content2);
	contents << QString::fromUtf8(content3);
	contents << workingDir+"/"+file;
	emit unmergedFileContents(contents);
	emit notify("Ready");
	emit progress(pb,100);
}


void GitProcess::gerritPush(QString gerritBranch,QString topic)
{
    QStringList args;

    args << "push";
    args << "gerrit";

    QString gerritRef;
    gerritRef = "HEAD:refs/for/" + gerritBranch + "/" + topic;
    args << gerritRef;

    emit notify("Pushing out to gerrit");
    emit initOutputDialog();
    QByteArray array = runGit(args,false,true);
    notifyOutputDialog(QString::fromUtf8(array));
    sendGitOutput();
    emit doneOutputDialog();
    emit notify("Ready");
}


