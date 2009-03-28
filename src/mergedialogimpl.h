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
#ifndef __MERGEDIALOGIMPL_H__
#define __MERGEDIALOGIMPL_H__

#include "ui_mergedialog.h"
#include "diffviewer.h"
#include "gitthread.h"

class MergeDialogImpl : public QDialog, public Ui::mergeDialog
{
	Q_OBJECT
	private:
		DiffViewer *conflict_diff;
		GitThread *gt;
		
		void mergeFile(QString file);
		
	private slots:
		void fileClicked(QTreeWidgetItem*);
		void fileDoubleClicked(QTreeWidgetItem*);
	public slots:
		void fileDiffReceived(QString);
		void runMergeTool(QStringList);
	public:
		void init(QString);
		void cleanUp();
		MergeDialogImpl(GitThread *gt,QWidget *parent = 0);
};

#endif // __MERGEDIALOGIMPL_H__
