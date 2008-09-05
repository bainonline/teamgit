#ifndef __RESETDIALOGIMPL_H__
#define __RESETDIALOGIMPL_H__

#include "gitprocess.h"
#include "ui_resetdialog.h"



class ResetDialogImpl : public QDialog, public Ui::resetDialog
{
	
	public:
		ResetDialogImpl(QWidget *parent = 0);
		int getOption();
		void reset(const QString &ref) {
			optionsBox->setTitle("Reseting to " + ref);
			noneOption->setChecked(true);
		}
};

// place your code here

#endif // __RESETDIALOGIMPL_H__
