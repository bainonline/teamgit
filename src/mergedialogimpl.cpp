#include <QTreeWidget>
#include "mergedialogimpl.h"

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
}

void MergeDialogImpl::fileDiffReceived(QString diff)
{
	conflict_diff->setDiffText(diff);
}

void MergeDialogImpl::init(QString files)
{
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
	
}
