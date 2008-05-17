#include <QFileDialog>

#include "settingsimpl.h"
//#include "gsettings.h"

gsettings GlobalSettings;
gsettings *gSettings;

SettingsImpl::SettingsImpl()
{
	setupUi(this);
	connect(pickPathButton,SIGNAL(clicked()),this,SLOT(getFilePath()));
	connect(pickWorkingDir,SIGNAL(clicked()),this,SLOT(getWorkingDirPath()));	
	
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
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
	if(!dir.isNull())
    	teamGitWorkingDir->setText(dir);
}

void SettingsImpl::setTeamGitWorkingDir(const QString &dir)
{
	teamGitWorkingDir->setText(dir);
}

QString SettingsImpl::getTeamGitWorkingDir()
{
	return teamGitWorkingDir->text();
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
