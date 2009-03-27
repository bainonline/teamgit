#include "mergedialogimpl.h"


void MergeDialogImpl::init(QString files)
{
	
}


MergeDialogImpl::MergeDialogImpl(GitThread *gitthread,QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	delete diffedit;
	conflict_diff = new DiffViewer(this);
	diffEditLayout->addWidget(conflict_diff);
	
	gt=gitthread;
}

