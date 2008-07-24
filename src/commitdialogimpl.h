#ifndef __COMMITDIALOGIMPL_H__
#define __COMMITDIALOGIMPL_H__


#include "ui_commitdialog.h"

class CommitDialogImpl : public QDialog, public Ui::commitDialog
{
	public:
		CommitDialogImpl(QWidget *parent = 0);
		void setAuthor(const QString &author,const QString &email);
		QString getCommitMessage();
		QString getAuthorName() {
			return author_name->text();
		}
		QString getAuthorEmail(){
			return author_email->text();
		}
};

#endif // __COMMITDIALOGIMPL_H__
