#ifndef __SETTINGSIMPL_H__
#define __SETTINGSIMPL_H__
#include <QObject>

#include "ui_settings.h"

struct gsettings {
	QString userName;
	QString userEmail;
	QString teamGitWorkingDir;
	QString currProjectPath;
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
		void setTeamGitWorkingDir(const QString &dir);
		QString getTeamGitWorkingDir();
		void setUserSettings(const QString &name, const QString &email);
		QString getUserName();
		QString getUserEmail();
		
	public slots:
		void refreshUi();
		void getFilePath();
		void getWorkingDirPath();
		void accept();
};
#endif // __SETTINGSIMPL_H__
