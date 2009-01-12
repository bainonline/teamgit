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
	bool rebaseMode;
	Q3ListViewItem *lastItem;
	QTextEdit *textEdit;

	void openFile(QString file);
	void processLine(QString line);
	void setEditMode();
public:
	DialogImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
   ~DialogImpl();

private slots:
	void upSlot();
	void downSlot();
	void squashSlot();
	void pickSlot();
	void editSlot();
	void dropSlot();
	void okSlot();
	void cancelSlot();
};
#endif




