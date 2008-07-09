#include "newprojectimpl.h"

NewProjectImpl::NewProjectImpl(QWidget *parent)
		:QDialog(parent)
{
	setupUi(this);
}

QString NewProjectImpl::getRepository()
{
	return originUrl->text();
}

QString NewProjectImpl::getTarget()
{
	return target->text();
}

QString NewProjectImpl::getRefRepo()
{
	return refRepo->text();
}
