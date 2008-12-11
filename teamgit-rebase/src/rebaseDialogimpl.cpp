
#include <QString>
#include "rebaseDialogimpl.h"

//
DialogImpl::DialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	
	commitsListView->addColumn("action");
	commitsListView->addColumn("commit");
	
	QStringList args = QCoreApplication::arguments ();
	if(args.size() > 1) {
		openFile(args[1]);
	} else {
		//TODO: quit
	}
}

void DialogImpl::openFile(QString file) 
{
	rebaseFile = new QFile(file);
	if (!rebaseFile->open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	while (!rebaseFile->atEnd()) {
		QByteArray line = rebaseFile->readLine();
		QString lineString(line);
		 processLine(lineString);
	}
}

void DialogImpl::processLine(QString line)
{
	if(line.startsWith("#"))
		return;
	else {
		QStringList lineItems = line.split(" ");
		if(lineItems.size() > 2) {
			new Q3ListViewItem(commitsListView,lineItems[0],lineItems[1]);
		}
	}
}
//
