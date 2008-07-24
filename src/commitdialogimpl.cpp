#include "commitdialogimpl.h"

// place your code here
CommitDialogImpl::CommitDialogImpl(QWidget *parent)
		:QDialog(parent)
{
	setupUi(this);
	signOff->hide();
	signofftext->hide();
}

void CommitDialogImpl::setAuthor(const QString &author,const QString &email)
{
	author_name->setText(author);
	author_email->setText(email);
}

QString CommitDialogImpl::getCommitMessage()
{
	return commitMessage->toPlainText();
	
}
