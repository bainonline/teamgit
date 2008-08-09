#include "diffviewer.h"

// place your code here
DiffViewer::DiffViewer(QWidget * parent) : QTextEdit(parent)
{
	
}

DiffViewer::~DiffViewer()
{
	
}

void DiffViewer::setDiffText(QString diff)
{
	clear();
	
	if(diff.size() > DIFF_LIMIT) {
		diff.remove(DIFF_LIMIT,diff.size()-(DIFF_LIMIT));
		diff.append("\n- Commit too huge: trimmed!");
	}
	QStringList diffLines=diff.split(QChar('\n'));
	 
	for (int i = 0; i < diffLines.size(); ++i) {
		QString line = diffLines.at(i);
		/* preprocess */
		if(line.startsWith("+")) {
			setTextColor(QColor("darkgreen"));
		} else if (line.startsWith("-")){
			setTextColor(QColor("darkred"));
	 	} else if (line.startsWith("@@")){
			setTextColor(QColor("blue"));
		} else if (line.startsWith("commit")){
			clear();
			setFontWeight(QFont::Bold);
		} else if (line.startsWith("Author:")){
			setFontWeight(QFont::Bold);
		} else if (line.startsWith("Date:")){
			setFontWeight(QFont::Bold);
		} else if (line.startsWith("diff")){
			setFontWeight(QFont::Bold);
		} else if (line.startsWith("index")){
			setFontWeight(QFont::Bold);
		} 

		append(line);
	
		/* post process */
		if(line.startsWith("+")) {
			setTextColor(QColor("black"));
		} else if (line.startsWith("-")){
			setTextColor(QColor("black"));
	 	} else if (line.startsWith("@@")){
			setTextColor(QColor("black"));
		} else if (line.startsWith("commit")){
			setFontWeight(QFont::Normal);
		} else if (line.startsWith("Author:")){
			setFontWeight(QFont::Normal);
		} else if (line.startsWith("Date:")){
			setFontWeight(QFont::Normal);
			insertPlainText("\n");
		} else if (line.startsWith("diff")){
			setFontWeight(QFont::Normal);
		} else if (line.startsWith("index")){
			setFontWeight(QFont::Normal);
		}
	}
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::Start);
	setTextCursor(cursor);
	ensureCursorVisible ();
}