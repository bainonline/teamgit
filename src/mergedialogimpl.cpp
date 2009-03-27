#include "mergedialogimpl.h"



MergeDialogImpl::MergeDialogImpl(GitThread *gitthread,QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	delete diffedit;
	conflict_diff = new DiffViewer(this);
	diffEditLayout->addWidget(conflict_diff);
	
	gt=gitthread;
}

void MergeDialogImpl::fileDiffReceived(QString diff,int i)
{
	qDebug() << diff;
}

void MergeDialogImpl::init(QString files)
{
	connect(gt->git,SIGNAL(fileDiff(QString,int)),this,SLOT(fileDiffReceived(QString,int)));
}


void MergeDialogImpl::cleanUp()
{
	disconnect(gt->git,SIGNAL(fileDiff(QString,int)),this,SLOT(fileDiffReceived(QString,int)));
}
