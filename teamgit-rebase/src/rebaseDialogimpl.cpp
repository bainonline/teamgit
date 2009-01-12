#include <QString>
#include "rebaseDialogimpl.h"

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
	connect(editButton,SIGNAL(clicked()),this,SLOT(editSlot()));
	connect(dropButton,SIGNAL(clicked()),this,SLOT(dropSlot()));
	connect(okButton,SIGNAL(clicked()),this,SLOT(okSlot()));
	connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelSlot()));

	commitsListView->setSorting(-1);

	QStringList args = QCoreApplication::arguments ();
	if(args.size() < 2) {
		qFatal ("Usage: teamgit-rebase FILE");
	}

	if (args[1].endsWith("git-rebase-todo")) {
		setWindowTitle("REBASE");
		rebaseMode = true;
		textEdit = NULL;
		/* safely remove the first line */
		Q3ListViewItem *item = commitsListView->firstChild();
		if (item && item->text(2) == "")
			commitsListView->removeItem(item);
	} else if (args[1].endsWith("COMMIT_EDITMSG")) {
		setWindowTitle("COMMIT");
		setEditMode();
		rebaseMode = false;
	} else
		qFatal ("Usage: teamgit-rebase FILE");

	lastItem = NULL;
	openFile(args[1]);
}

DialogImpl::~DialogImpl()
{
	if (rebaseFile)
		delete rebaseFile;
	if (commitsListView)
		commitsListView->clear();
	if (textEdit)
		delete textEdit;
}

void DialogImpl::openFile(QString file)
{
	rebaseFile = new QFile(file);
	if (!rebaseFile->exists()) {
		qFatal ("The file " + file + " does not exist");
	}

	if (!rebaseFile->open(QIODevice::ReadWrite | QIODevice::Text))
		qFatal ("Check the file permissions on " + file);

	while (!rebaseFile->atEnd()) {
		QByteArray line = rebaseFile->readLine();
		processLine(QString::fromUtf8(line));
	}

	if (textEdit)
		textEdit->moveCursor(QTextCursor::Start);
}

void DialogImpl::processLine(QString line)
{
	if (!rebaseMode) {
		textEdit->insertPlainText(line);
		return;
	}

	if(line.startsWith("#"))
		return;

	QStringList lineItems = line.split(" ");
	if(lineItems.size() > 2) {
		QString headline = "";
		for (int i=2; i<lineItems.size(); i++) {
			headline.append(lineItems[i]);
			headline.append(" ");
		}
		headline.remove(QRegExp("[\r\n\t ]*$"));
		lastItem = new Q3ListViewItem(commitsListView, lastItem, lineItems[0], lineItems[1], headline);
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

void DialogImpl::editSlot()
{
	Q3ListViewItem *item = commitsListView->selectedItem();
	if (item)
		item->setText(0, "edit");
}

void DialogImpl::dropSlot()
{
	Q3ListViewItem *item = commitsListView->selectedItem();
	if (item)
		item->setText(0, "drop");
}

void DialogImpl::okSlot()
{
	QString result;

	rebaseFile->close();
	if (!rebaseFile->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
		qFatal ("Cannot reopen the file");
	}

	if (!rebaseMode) {
		result = textEdit->text().trimmed();
		rebaseFile->writeBlock(QByteArray(result.toUtf8()));
		rebaseFile->close();

		close();
		return;
	}

	Q3ListViewItemIterator it(commitsListView);
	QStringList list;

	while (it.current()) {
		Q3ListViewItem *item = it.current();
		QString str = "";

		for (int i=0;i<3;i++)
			str.append(item->text(i) + "\t");
		if (item->text(0) != "drop")
			list.append(str);
		it++;
	}

	for (QStringList::Iterator it = list.begin(); it != list.end(); it++)
		result.append(*it + "\n");

	result = result.trimmed();
	/* add a newline to the end of the file */
	result.append("\n");

	/* try to overwrite the current file with the re-ordered commits */
	rebaseFile->writeBlock(QByteArray(result.toUtf8()));
	rebaseFile->close();

	close();
}

void DialogImpl::cancelSlot()
{
	rebaseFile->close();
	reject();
}

void DialogImpl::setEditMode()
{
	delete commitsListView;
	commitsListView = NULL;

	delete upButton;
	delete downButton;
	delete pickButton;
	delete squashButton;
	delete editButton;
	delete dropButton;

	textEdit = new QTextEdit();
	verticalLayout->addWidget(textEdit);
}
