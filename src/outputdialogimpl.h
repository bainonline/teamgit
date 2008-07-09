#ifndef __OUTPUTDIALOGIMPL_H__
#define __OUTPUTDIALOGIMPL_H__

#include "ui_outputdialog.h"
enum OUTPUT_TYPE {DEFAULT=0,GIT_PULL, GIT_CLONE};

class OutputDialogImpl : public QDialog, public Ui::OutputDialog
{
	public:
	OutputDialogImpl(QWidget *parent = 0);
	
	private:
	
	QString outputLines;
	bool replaceMode;
	OUTPUT_TYPE output_type;
	
	void processMessage(const QString msg);
	
	public slots:
	void initOutputDialog(OUTPUT_TYPE type=DEFAULT,bool showProgressBar=false,bool showCancelButton=false);
	void doneOutputDialog();
	void notifyOutputDialog(const QString &);
};
#endif // __OUTPUTDIALOGIMPL_H__
