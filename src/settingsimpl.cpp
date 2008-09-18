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
#include <QFileDialog>

#include "settingsimpl.h"
//#include "gsettings.h"

gsettings GlobalSettings;
gsettings *gSettings;

SettingsImpl::SettingsImpl(QWidget *parent)
			: QDialog(parent)
{
	setupUi(this);
	connect(pickPathButton,SIGNAL(clicked()),this,SLOT(getFilePath()));
	gSettings = &GlobalSettings;//(gsettings *)malloc(sizeof(gsettings));
}

void SettingsImpl::setUserSettings(const QString &name, const QString &email)
{
	userName->setText(name);
	userEmail->setText(email);
}

QString SettingsImpl::getUserName()
{
	return userName->text();
}


QString SettingsImpl::getUserEmail()
{
	return userEmail->text();
}

void SettingsImpl::setGitBinaryPath(const QString &path)
{
	gitBinaryPath->setText(path);
}

QString SettingsImpl::getGitBinaryPath()
{
	return gitBinaryPath->text();
}

void SettingsImpl::getFilePath()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open Binary"),
                                                 "/home",
                                                 "");
    if(!path.isNull())
    	gitBinaryPath->setText(path);
}


void SettingsImpl::getWorkingDirPath()
{
}


void SettingsImpl::refreshUi()
{
	userName->setText(gSettings->userName);
	userEmail->setText(gSettings->userEmail);
}



void SettingsImpl::accept()
{
	gSettings->userName = userName->text();
	gSettings->userEmail = userEmail->text();
	
	QDialog::accept();
}
