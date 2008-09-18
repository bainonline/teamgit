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
#ifndef __NEWPROJECTIMPL_H__
#define __NEWPROJECTIMPL_H__

#include "ui_newproject.h"

class NewProjectImpl : public QDialog, public Ui::NewProjectDialog
{
	public:
		NewProjectImpl(QWidget *parent = 0);
		QString getRepository();
		QString getTarget();
		QString getRefRepo();
};
#endif // __NEWPROJECTIMPL_H__
