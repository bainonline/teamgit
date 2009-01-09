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
#ifndef __OUTPUTDIALOGIMPL_H__
#define __OUTPUTDIALOGIMPL_H__

#include "ui_outputdialog.h"
enum OUTPUT_TYPE {DEFAULT=0,GIT_PULL, GIT_CLONE};

class OutputDialogImpl : public QDialog, public Ui::OutputDialog
{
	public:
	OutputDialogImpl(QWidget *parent = 0);

	private:

	QString outputLines;
	bool replaceMode;
	OUTPUT_TYPE output_type;

	void processMessage(const QString msg);

	public slots:
	void initOutputDialog(OUTPUT_TYPE type=DEFAULT,bool showProgressBar=false,bool showCancelButton=false);
	void doneOutputDialog();
	void notifyOutputDialog(const QString &);
};
#endif // __OUTPUTDIALOGIMPL_H__
