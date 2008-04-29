#include <QFileDialog>

#include "settingsimpl.h"

SettingsImpl::SettingsImpl()
{
	setupUi(this);
	connect(pickPathButton,SIGNAL(clicked()),this,SLOT(getFilePath()));
	connect(pickWorkingDir,SIGNAL(clicked()),this,SLOT(getWorkingDirPath()));	
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
