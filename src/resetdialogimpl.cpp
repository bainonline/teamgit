#include "resetdialogimpl.h"

// place your code here
ResetDialogImpl::ResetDialogImpl(QWidget *parent)
		:QDialog(parent)
{
	setupUi(this);
}

int ResetDialogImpl::getOption()
{
	if(noneOption->isChecked())
		return simple;
		
	if(softOption->isChecked())
		return soft;
		
	if(hardOption->isChecked())
		return hard;
	
	return simple;
}
