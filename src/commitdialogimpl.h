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
#ifndef __COMMITDIALOGIMPL_H__
#define __COMMITDIALOGIMPL_H__


#include "ui_commitdialog.h"

class CommitDialogImpl : public QDialog, public Ui::commitDialog
{
	public:
		CommitDialogImpl(QWidget *parent = 0);
		void init(const QString &author,const QString &email,bool autosignoff);
		QString getCommitMessage();
		void setCommitMessage(QString);
		QString getAuthorName() {
			return author_name->text();
		}
		QString getAuthorEmail(){
			return author_email->text();
		}
		bool signoff(){
			if(signOff->checkState()==Qt::Checked) {
				return true;
			} else {
				return false;
			}
		}
};

#endif // __COMMITDIALOGIMPL_H__
