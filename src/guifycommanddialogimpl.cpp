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

#include "guifycommanddialogimpl.h"

optionItem::optionItem()
{
	commandArg=false;
	argumentOptional=false;
	textArg=false;
}

optionItem::~optionItem()
{
	argumentOptional=false;
	textArg=false;
}

void optionItem::checkItem()
{
	checkBox->setCheckState(Qt::Checked);
}
// place your code here
guifyCommandDialogImpl::guifyCommandDialogImpl(QWidget *parent,QString cmd, QString help)
		:QDialog(parent)
{
	setupUi(this);
	command=cmd;
	gitCommandName->setText(cmd);
	parseHelpLines(help);
}

guifyCommandDialogImpl::~guifyCommandDialogImpl()
{
	for(int j=0;j < optionItems.count();j++)
		delete optionItems[j];
}


/*
 * Why couldn't Bill try out his regular expressions?
 * His mom wouldn't let him play with matches.
 */
void guifyCommandDialogImpl::parseHelpLines(QString help)
{
	QStringList helpLines = help.split("\n");
	int i = 0;
	optionItem *it = NULL;
	QRegExp optionLine("^[\\s]+-");
	QRegExp boolOption("^[\\s]+(-[-\\w]+)$");
	QRegExp boolLongOption("^[\\s]+(--[-\\w]+)$");
	QRegExp textOption("^[\\s]+(--[-\\w]+)(=.*)$");	
	QRegExp optionalTextOption("^[\\s]+(--[-\\w]+)(\\[=.*)$");
	QRegExp commandArg("^[\\s]+(<.*)$");
	QRegExp optionsEnd("^\\w");
	QRegExp helpLinePattern("^[\\s]+([^\\s].*)$");
	while(!helpLines[i].startsWith("OPTIONS"))
		i++;
	while(optionsEnd.indexIn(helpLines[++i])<0) {
		if(helpLines[i].contains(optionLine)) {
			if(it && !it->name.isEmpty())
				optionItems.append(it);
			it = new optionItem();
			if(boolOption.indexIn(helpLines[i])>=0) {
				it->name=boolOption.cap(1);
			} else if(boolLongOption.indexIn(helpLines[i])>=0) {
				it->name=boolLongOption.cap(1);
			} else if(textOption.indexIn(helpLines[i])>=0) {
				it->textArg = true;
				it->name = textOption.cap(1) ;
				it->textArgumentCaption = textOption.cap(2);
			} else if(optionalTextOption.indexIn(helpLines[i]) >= 0) {
				it->textArg = true;
				it->argumentOptional = true;
				it->name = optionalTextOption.cap(1) ;
				it->textArgumentCaption = optionalTextOption.cap(2);
			} 
		} else if(commandArg.indexIn(helpLines[i]) >= 0) {
			if(it && !it->name.isEmpty())
				optionItems.append(it);
			it = new optionItem();
			qDebug() << helpLines[i];
			it->commandArg=true;
			it->name = commandArg.cap(1);
		} else {
			if(!it->name.isEmpty()) {
				helpLinePattern.indexIn(helpLines[i]);
				if(!it->help.isEmpty() && !helpLinePattern.cap(1).isEmpty())
					it->help.append("\n");
				it->help.append(helpLinePattern.cap(1));
			}
		}
	}
	if(it && !it->name.isEmpty())
				optionItems.append(it);
	int brow=0,bcol=0;
	int trow=0,tcol=0;
	int crow=0;
	for(int j=0;j < optionItems.count();j++) {
		optionItem *it = optionItems[j];
		it->checkBox = new QCheckBox(it->name);
		it->checkBox->setToolTip(it->help);
		if(it->textArg) {
			it->textArgument = new QLineEdit(it->textArgumentCaption);
			QObject::connect((QObject *)it->textArgument,SIGNAL(textChanged(const QString&)),(QObject *)it,SLOT(checkItem()));
			it->textArgument->setMinimumWidth(250);
			textArgOptionsLayout->addWidget(it->checkBox,trow,tcol);
			if(++tcol > 1) {
				tcol=0;
				trow++;
			}
			textArgOptionsLayout->addWidget(it->textArgument,trow,tcol);
			if(++tcol > 1) {
				tcol=0;
				trow++;
			}
		} else if(it->commandArg) {
			QLabel *l= new QLabel(it->name); 
			l->setToolTip(it->help);
			it->textArgument = new QLineEdit();
			commandArgsLayout->addWidget(l,crow,0);
			commandArgsLayout->addWidget(it->textArgument,crow,1);
			crow++;
		} else {
			boolOptionsLayout->addWidget(it->checkBox,brow,bcol);
			if(++bcol > 2) {
				bcol=0;
				brow++;
			}
		}
	}
}

QStringList guifyCommandDialogImpl::getFinalCommandArgs() 
{
	QStringList args;
	QString arg;
	args << command;
	for(int j=0;j < optionItems.count();j++) {
		optionItem *it=optionItems[j];
		if(it->checkBox->isChecked()) {
			arg = it->name;
			if(it->textArg && !it->textArgument->text().isEmpty()) {
				arg += "=";
				arg += it->textArgument->text();
			}
			args << arg;
		} else if (it->commandArg && !it->textArgument->text().isEmpty()) {
			arg = it->textArgument->text();
			args << arg;
		}
	}
	return args;
}

bool guifyCommandDialogImpl::refresh()
{
	return refreshCheckBox->isChecked();
}
