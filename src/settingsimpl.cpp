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
        connect(pickEditorButton,SIGNAL(clicked()),this,SLOT(getEditorFilePath()));
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

void SettingsImpl::setEditorPath(const QString &path)
{
    editorPath->setText(path);
}

QString SettingsImpl::getEditorPath()
{
    return editorPath->text();
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

void SettingsImpl::getEditorFilePath()
{
    QString path=getFilePath();
    if(!path.isNull())
            editorPath->setText(path);
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
    editorPath->setText(gSettings->editorPath);
	if(gSettings->showAdvanced)
		showAdvancedCheckBox->setCheckState(Qt::Checked);
	else
		showAdvancedCheckBox->setCheckState(Qt::Unchecked);

    if(gSettings->useGerrit)
        useGerrit->setCheckState(Qt::Checked);
    else
        useGerrit->setCheckState(Qt::Unchecked);
    gerritBranch->setText(gSettings->gerritBranch);
}

void SettingsImpl::accept()
{
	gSettings->userName = userName->text();
	gSettings->userEmail = userEmail->text();
	gSettings->mergeToolPath = mergeToolPath->text();
        gSettings->editorPath = editorPath->text();
	if(autosignoff->checkState() == Qt::Checked)
		gSettings->autosignoff = true;
	else
		gSettings->autosignoff = false;
	gSettings->showAdvanced = showAdvancedCheckBox->checkState() ? true : false ;
    gSettings->useGerrit = useGerrit->checkState() ? true : false ;
    gSettings->gerritBranch = gerritBranch->text();
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
	gSettings->mergeToolPath = settings.value("mergeToolPath",QString("/usr/bin/meld")).toString();
    gSettings->editorPath = settings.value("editorPath",QString("/usr/bin/kate")).toString();
    gSettings->useGerrit = settings.value("useGerrit",bool()).toBool();
    gSettings->gerritBranch = settings.value("gerritBranch",QString("master")).toString();
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
    settings.setValue("editorPath",gSettings->editorPath);
    settings.setValue("useGerrit",gSettings->useGerrit);
    settings.setValue("gerritBranch",gSettings->gerritBranch);
    settings.endGroup();

}

void SettingsImpl::setAutoSignoff(bool checked)
{
	autosignoff->setChecked(checked);
}
