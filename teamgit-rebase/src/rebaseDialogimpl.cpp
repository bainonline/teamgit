#include "rebaseDialogimpl.h"
//
DialogImpl::DialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	QStringList args = QCoreApplication::arguments ();
	if(args.size() > 1) {
		rebaseFile = new QFile(args[1]);
	
	} else {
		//TODO: quit
	}
}
//
