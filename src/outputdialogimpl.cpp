#include <QtDebug>
#include <stdio.h>
#include <iostream>


#include "outputdialogimpl.h"

using namespace std;
// place your code here
OutputDialogImpl::OutputDialogImpl(QWidget *parent)
			: QDialog(parent)
{
	setModal(true);
	setupUi(this);


}

void OutputDialogImpl::initOutputDialog(OUTPUT_TYPE type,bool showProgressBar,bool showCancelButton)
{
	if(showProgressBar) {
		progressBar->show();
		progressBar->setValue(0);
	} else {
		progressBar->hide();
	}
	if(showCancelButton)
		cancelButton->show();
	else
		cancelButton->hide();
	
	closeButton->setEnabled(false);
	output_type = type;
	output->clear();
	replaceMode = false;
	status->clear();
	show();
}

void OutputDialogImpl::doneOutputDialog()
{
	replaceMode = false;
	outputLines.clear();
	closeButton->setEnabled(true);
}

void OutputDialogImpl::notifyOutputDialog(const QString &msg)
{
	switch(output_type) {
		case DEFAULT:
		case GIT_PULL:
		case GIT_CLONE:
		default:
			processMessage(msg);
			break;
	}
}

void OutputDialogImpl::processMessage(const QString msg)
{
	outputLines.append(msg);
	int index;
	bool if_cr;
	while(1) {
		if_cr=true;
		index = outputLines.indexOf("\r");
		if(index == -1 || (outputLines.mid(index+1,1) == "\n") ) {
			if_cr=false;
			index = outputLines.indexOf("\n");
			if(index == -1) 
				break;
		}
		
		QString line = outputLines.left(index+1);
		outputLines.remove(0,index+1);
		if(if_cr) {
			status->setText(line);
			int percentage_index=line.indexOf("%");
			if(percentage_index != -1) {
				QString percentage= line.mid(percentage_index-2,2);
				int percent = percentage.toInt();
				progressBar->setValue(percent);
			}
		} else {
			progressBar->setValue(100);
			line.remove("\n");
			if(line.size() > 1)
				output->insertPlainText(line);
		}
	}
#if 0 
	qDebug() << "BEGIN";
	const QChar *data=msg.data();
	while (!data->isNull()) {
         qDebug() << data->unicode() << "\t" <<data->toAscii();
         ++data;
    }
	qDebug() << "END";
#endif

}
