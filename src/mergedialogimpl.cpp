#include <QTreeWidget>
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>
#include <QMessageBox>
#include "mergedialogimpl.h"
#include "gsettings.h"

MergeDialogImpl::MergeDialogImpl(GitThread *gitthread,QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	delete diffedit;
	conflict_diff = new DiffViewer(this);
	diffEditLayout->addWidget(conflict_diff);
	gt=gitthread;
	connect(filesTreeWidget,SIGNAL(itemClicked (QTreeWidgetItem*,int)),this,SLOT(fileClicked(QTreeWidgetItem*)));
	connect(filesTreeWidget,SIGNAL(itemDoubleClicked (QTreeWidgetItem*,int)),this,SLOT(fileDoubleClicked(QTreeWidgetItem*)));
	connect(gt->git,SIGNAL(unmergedFileContents(QStringList)),this,SLOT(runMergeTool(QStringList)));
}

void MergeDialogImpl::fileDiffReceived(QString diff)
{
	conflict_diff->setDiffText(diff);
}

void MergeDialogImpl::init(QString files)
{
	filesTreeWidget->clear();
	conflict_diff->clear();
	connect(gt->git,SIGNAL(fileDiff(QString,int)),this,SLOT(fileDiffReceived(QString)));
	QStringList filesList=files.split("\n");
	QList<QTreeWidgetItem *> items;
	for(int i=0;i<filesList.size()-1;i+=3) {
		QString file = filesList[i].split(QRegExp("\\s+"))[3];
		items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(file)));
	}
	filesTreeWidget->insertTopLevelItems(0, items);
}

void MergeDialogImpl::cleanUp()
{
	disconnect(gt->git,SIGNAL(fileDiff(QString,int)),this,SLOT(fileDiffReceived(QString)));
}

void MergeDialogImpl::fileDoubleClicked(QTreeWidgetItem *item)
{
	QString file = item->data(0,Qt::DisplayRole).toString();
	mergeFile(file);
}

void MergeDialogImpl::fileClicked(QTreeWidgetItem *item)
{
	QString file = item->data(0,Qt::DisplayRole).toString();
	QMetaObject::invokeMethod(gt->git,"getDiff",Qt::QueuedConnection,
							Q_ARG(QString,file));
}

void MergeDialogImpl::mergeFile(QString file)
{
	QMetaObject::invokeMethod(gt->git,"getUnMergedFileContents",Qt::QueuedConnection,
							Q_ARG(QString,file));
}

void MergeDialogImpl::runMergeTool(QStringList fileContents)
{
	QTemporaryFile base,local,remote;
	base.open();
	local.open();
	remote.open();
	QString baseFile,localFile,remoteFile,mergedFile;
	baseFile = base.fileName();
	localFile = local.fileName();
	remoteFile = remote.fileName();
	mergedFile = fileContents[3];
	QTextStream outBase(&base), outLocal(&local), outRemote(&remote);
	outBase << fileContents[0];
	outLocal << fileContents[1];
	outRemote << fileContents[2];
	base.close();
	local.close();
	remote.close();
	QStringList args;
	args << baseFile << mergedFile << remoteFile;
	if(!gSettings->mergeToolPath.contains("meld")){
		QMessageBox::warning((QWidget *)this, tr("Merge Tool"),
					tr("Only meld is supported for now?"),
					QMessageBox::Ok);
		return;
	}
	QProcess::execute(gSettings->mergeToolPath,args);
	int ret = QMessageBox::question((QWidget *)this, tr("Merge Tool"),
			tr("Was this merge successfull?"),
			QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
	if(ret == QMessageBox::Yes) {
		QMetaObject::invokeMethod(gt->git,"addFiles",Qt::QueuedConnection,
							Q_ARG(QStringList,QStringList(mergedFile)));
	}
	conflict_diff->clear();
}
