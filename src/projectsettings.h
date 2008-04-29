#ifndef __PROJECTSETTINGS_H__
#define __PROJECTSETTINGS_H__
#include <QObject>
#include <QString>
#include <QMetaType>

class Project
{
	public:
		QString name;
		QString path;
};

class ProjectSettings
{
	QList<Project> ProjectList;
	public:
		ProjectSettings() { }
		~ProjectSettings() { }
		ProjectSettings(const ProjectSettings&) {
		}

};

#endif // __PROJECTSETTINGS_H__
