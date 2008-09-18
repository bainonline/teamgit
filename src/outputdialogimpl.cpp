/*
	Copyright 2007 2008 Abhijit Bhopatkar
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 2 of the License.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program, in file COPYING
	If not, see <http://www.devslashzero.com/teamgit/license>.
*/
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
	cancelButton->setEnabled(false);

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
