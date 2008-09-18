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
#include "newprojectimpl.h"

NewProjectImpl::NewProjectImpl(QWidget *parent)
		:QDialog(parent)
{
	setupUi(this);
}

QString NewProjectImpl::getRepository()
{
	return originUrl->text();
}

QString NewProjectImpl::getTarget()
{
	return target->text();
}

QString NewProjectImpl::getRefRepo()
{
	return refRepo->text();
}
