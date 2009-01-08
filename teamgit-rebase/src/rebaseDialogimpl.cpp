#include <QString>
#include "rebaseDialogimpl.h"

//
DialogImpl::DialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	
	commitsListView->addColumn("action");
	commitsListView->addColumn("commit");
	commitsListView->addColumn("headline", -1);
	
	connect(upButton,SIGNAL(clicked()),this,SLOT(upSlot()));
	connect(downButton,SIGNAL(clicked()),this,SLOT(downSlot()));
	connect(pickButton,SIGNAL(clicked()),this,SLOT(pickSlot()));
	connect(squashButton,SIGNAL(clicked()),this,SLOT(squashSlot()));
	connect(okButton,SIGNAL(clicked()),this,SLOT(okSlot()));
	connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelSlot()));

	commitsListView->setSorting(-1);

	QStringList args = QCoreApplication::arguments ();
	if(args.size() > 1) {
		if (args[1].endsWith("COMMIT_EDITMSG")) {
			setWindowTitle("COMMIT");
			rebaseMode = false;
		} else {
			setWindowTitle("REBASE");
			rebaseMode = true;
		}
		openFile(args[1]);
	} else {
		/* nothing to do */
		close();
	}
}

void DialogImpl::openFile(QString file) 
{
	rebaseFile = new QFile(file);
	if (!rebaseFile->open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	while (!rebaseFile->atEnd()) {
		QByteArray line = rebaseFile->readLine();
		processLine(QString::fromUtf8(line));
	}
}

void DialogImpl::processLine(QString line)
{
	if(line.startsWith("#"))
		return;

	if (!rebaseMode) {
		new Q3ListViewItem(commitsListView, "pick", "line", line.remove(QRegExp("[\r\n\t ]*$")));
		return;
	}

	QStringList lineItems = line.split(" ");
	if(lineItems.size() > 2) {
		QString headline = "";
		for (int i=2; i<lineItems.size(); i++) {
			headline.append(lineItems[i]);
			headline.append(" ");
		}
		headline.remove(QRegExp("[\r\n\t ]*$"));
		new Q3ListViewItem(commitsListView,lineItems[0],lineItems[1], headline);
	}
}

void DialogImpl::upSlot()
{
	Q3ListViewItem *item = commitsListView->selectedItem();
	Q3ListViewItem *cur, *next;

	if ((cur = item) == NULL)
		return;
	if ((next = cur->itemAbove()) == NULL)
		return;

	next->moveItem(cur);
	commitsListView->setSelected(cur, true);
}

void DialogImpl::downSlot()
{
	Q3ListViewItem *item = commitsListView->selectedItem();
	Q3ListViewItem *cur, *next;

	if ((cur = item) == NULL)
		return;
	if ((next = cur->itemBelow()) == NULL)
		return;

	cur->moveItem(next);
	commitsListView->setSelected(cur, true);
}

void DialogImpl::pickSlot()
{
	Q3ListViewItem *item = commitsListView->selectedItem();
	if (item)
		item->setText(0, "pick");
}

void DialogImpl::squashSlot()
{
	Q3ListViewItem *item = commitsListView->selectedItem();
	if (item)
		item->setText(0, "squash");
}

void DialogImpl::okSlot()
{
	Q3ListViewItemIterator it(commitsListView);
	QStringList list;
	QByteArray result;

	while (it.current()) {
		Q3ListViewItem *item = it.current();
		QString str = "";

		if (rebaseMode) {
			for (int i=0;i<3;i++)
				str.append(item->text(i) + "\t");
			list.append(str);
		} else {
			if (item->text(0) == "pick") {
				str = item->text(2);
				if (str != "")
					list.append(str);
			}
		}
		it++;
	}

	/* Reverse the list :) */
	for (QStringList::Iterator it = list.end()-1; it != list.begin()-1; it--)
		result.append(*it + "\n");

	rebaseFile->close();
	if (!rebaseFile->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
		qWarning ("Cannot reopen the file");
		close();
	}

	/* try to overwrite the current file with the re-ordered commits */
	rebaseFile->writeBlock(result);
	rebaseFile->close();

	close();
}

void DialogImpl::cancelSlot()
{
	rebaseFile->close();
	close();
}
