#ifndef REBASEDIALOGIMPL_H
#define REBASEDIALOGIMPL_H
//
#include <QDialog>
#include <Qt3Support>
#include "ui_rebaseDialog.h"
//
class DialogImpl : public QDialog, public Ui::Dialog
{
Q_OBJECT
private:
	QFile *rebaseFile;
public:
	DialogImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
private slots:
};
#endif




