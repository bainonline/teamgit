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
#include <QFileDialog>

NewProjectImpl::NewProjectImpl(QWidget *parent)
		:QDialog(parent)
{
	setupUi(this);
	connect(getDirButton,SIGNAL(clicked()),this,SLOT(getDirSlot()));
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

void NewProjectImpl::getDirSlot()
{
	QString startDir= "/home";
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
													startDir,
													QFileDialog::ShowDirsOnly
													| QFileDialog::DontResolveSymlinks);
	if(!dir.isNull())
		dirPath->setText(dir);
}

QString NewProjectImpl::getDir()
{
	return dirPath->text();
}
