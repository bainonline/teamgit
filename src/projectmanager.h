#ifndef __PROJECTMANAGER_H__
#define __PROJECTMANAGER_H__

#include <QObject>
#include <QList>
#include <QSettings>

#include "projectsettings.h"

class ProjectManager: public  QObject
{
	Q_OBJECT
	private:
	ProjectSettings ps;
	
	public:
	ProjectSettings getProjects();
	bool addNewProject(const ProjectSettings &);
	bool deleteProject(const ProjectSettings &);
	
	ProjectSettings getProjectSettings(const QString &name);
	
	void saveProjects(const QSettings &settings);
	void readSavedProjects(const QSettings &settings);
	
};

#endif // __PROJECTMANAGER_H__
