#ifndef __NEWPROJECTIMPL_H__
#define __NEWPROJECTIMPL_H__

#include "ui_newproject.h"

class NewProjectImpl : public QDialog, public Ui::NewProjectDialog
{
	public:
		NewProjectImpl(QWidget *parent = 0);
		QString getRepository();
		QString getTarget();
		QString getRefRepo();
};
#endif // __NEWPROJECTIMPL_H__
