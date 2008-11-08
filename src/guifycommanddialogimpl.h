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
#ifndef __GUIFYCOMMANDDIALOGIMPL_H__
#define __GUIFYCOMMANDDIALOGIMPL_H__

#include "gitprocess.h"
#include "ui_guifycommand.h"



class guifyCommandDialogImpl : public QDialog, public Ui::guifyCommandDialog
{
	public:
		guifyCommandDialogImpl(QWidget *parent,QString command,QString help);
		QString getFinalCommand();
		
};

// place your code here

#endif // __RESETDIALOGIMPL_H__
