#include "commitdialogimpl.h"

// place your code here
CommitDialogImpl::CommitDialogImpl(QWidget *parent)
		:QDialog(parent)
{
	setupUi(this);
	signOff->hide();
	signofftext->hide();
}

void CommitDialogImpl::init(const QString &author,const QString &email)
{
	commitMessage->selectAll();
	commitMessage->setFocus();
	author_name->setText(author);
	author_email->setText(email);
}

QString CommitDialogImpl::getCommitMessage()
{
	return commitMessage->toPlainText();
	
}
