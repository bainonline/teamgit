#ifndef __SETTINGSIMPL_H__
#define __SETTINGSIMPL_H__
#include <QObject>

#include "ui_settings.h"

class SettingsImpl : public QDialog, public Ui::SettingsDialog
{
	Q_OBJECT
	private:
		int i;
		
	public:
		SettingsImpl();
		void setGitBinaryPath(const QString &path);
		QString getGitBinaryPath();
		void setTeamGitWorkingDir(const QString &dir);
		QString getTeamGitWorkingDir();
		void setUserSettings(const QString &name, const QString &email);
		QString getUserName();
		QString getUserEmail();
		
	public slots:
		void getFilePath();
		void getWorkingDirPath();
};
#endif // __SETTINGSIMPL_H__
