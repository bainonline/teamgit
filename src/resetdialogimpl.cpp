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
#include "resetdialogimpl.h"

// place your code here
ResetDialogImpl::ResetDialogImpl(QWidget *parent)
		:QDialog(parent)
{
	setupUi(this);
}

int ResetDialogImpl::getOption()
{
	if(noneOption->isChecked())
		return simple;
		
	if(softOption->isChecked())
		return soft;
		
	if(hardOption->isChecked())
		return hard;
	
	return simple;
}
