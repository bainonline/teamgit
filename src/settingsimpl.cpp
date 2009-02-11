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
#include <QSettings>
#include "settingsimpl.h"
#include "defs.h"
//#include "gsettings.h"

gsettings GlobalSettings;
gsettings *gSettings;

SettingsImpl::SettingsImpl(QWidget *parent)
			: QDialog(parent)
{
	setupUi(this);
	autosignoff->setChecked(true);
	connect(pickPathButton,SIGNAL(clicked()),this,SLOT(getGitFilePath()));
	connect(selectMergeTool,SIGNAL(clicked()),this,SLOT(getMergeToolFilePath()));

	gSettings = &GlobalSettings;//(gsettings *)malloc(sizeof(gsettings));
	readSettings();
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

QString SettingsImpl::getFilePath()
{
	return QFileDialog::getOpenFileName(this, tr("Open Binary"),
											"/home",
											"");
}

void SettingsImpl::getGitFilePath() 
{
	QString path=getFilePath();
	if(!path.isNull())
		gitBinaryPath->setText(path);
}

void SettingsImpl::getMergeToolFilePath() 
{
	QString path=getFilePath();
	if(!path.isNull())
		mergeToolPath->setText(path);
}

void SettingsImpl::getWorkingDirPath()
{
}


void SettingsImpl::refreshUi()
{
	userName->setText(gSettings->userName);
	userEmail->setText(gSettings->userEmail);
	autosignoff->setChecked(gSettings->autosignoff);
	mergeToolPath->setText(gSettings->mergeToolPath);
	if(gSettings->showAdvanced)
		showAdvancedCheckBox->setCheckState(Qt::Checked);
	else
		showAdvancedCheckBox->setCheckState(Qt::Unchecked);
}

void SettingsImpl::accept()
{
	gSettings->userName = userName->text();
	gSettings->userEmail = userEmail->text();
	gSettings->mergeToolPath = mergeToolPath->text();
	if(autosignoff->checkState() == Qt::Checked)
		gSettings->autosignoff = true;
	else
		gSettings->autosignoff = false;
	gSettings->showAdvanced = showAdvancedCheckBox->checkState() ? true : false ;
	writeSettings();
	QDialog::accept();
}

void SettingsImpl::readSettings()
{
	QSettings settings(COMPANY, "Teamgit");
	settings.beginGroup("TeamGit");
	gSettings->teamGitWorkingDir = settings.value("workspace",QString("notset")).toString();
	gSettings->currProjectPath = QString();
	gSettings->recentlyOpened = settings.value("RecentlyOpened",QStringList()).toStringList();
	gSettings->autosignoff = settings.value("autosignoff",bool()).toBool();
	gSettings->lastApplyMailPath = settings.value("applyMailStartPath",QString("/home")).toString();
	gSettings->mergeToolPath = settings.value("mergeToolPath",QString()).toString();
	settings.endGroup();
}


void SettingsImpl::writeSettings()
{
	QSettings settings(COMPANY, "Teamgit");
	settings.beginGroup("TeamGit");
	settings.setValue("workspace",gSettings->teamGitWorkingDir);
	settings.setValue("current_project",gSettings->currProjectPath);
	settings.setValue("RecentlyOpened",gSettings->recentlyOpened);
	settings.setValue("autosignoff",gSettings->autosignoff);
	settings.setValue("applyMailStartPath",gSettings->lastApplyMailPath);
	settings.setValue("mergeToolPath",gSettings->mergeToolPath);
	settings.endGroup();

}

void SettingsImpl::setAutoSignoff(bool checked)
{
	autosignoff->setChecked(checked);
}
