/*
	Copyright 2007 2008 Abhijit Bhopatkar
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more detailautosignoffs.

	You should have received a copy of the GNU General Public License
	along with this program, in file COPYING
	If not, see <http://www.devslashzero.com/teamgit/license>.
*/
#ifndef __SETTINGSIMPL_H__
#define __SETTINGSIMPL_H__
#include <QObject>

#include "ui_settings.h"

struct gsettings {
	QString userName;
	QString userEmail;
	bool autosignoff;
	QString teamGitWorkingDir;
	QString currProjectPath;
	QStringList recentlyOpened;
	QString lastApplyMailPath;
	bool showAdvanced;

};

class SettingsImpl : public QDialog, public Ui::SettingsDialog
{
	Q_OBJECT
	private:
		int i;

	public:
		SettingsImpl(QWidget *parent = 0);
		void setGitBinaryPath(const QString &path);
		QString getGitBinaryPath();
		void setUserSettings(const QString &name, const QString &email);
		QString getUserName();
		QString getUserEmail();
		void setAutoSignoff(bool checked);
	public slots:
		void refreshUi();
		void getFilePath();
		void getWorkingDirPath();
		void accept();
};
#endif // __SETTINGSIMPL_H__
