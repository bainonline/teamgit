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
#include "diffviewer.h"
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QMouseEvent>
// place your code here
DiffViewer::DiffViewer(QWidget * parent) : QTextEdit(parent)
{
	this->setReadOnly(true);
	QFont font;
	font.setFamily(QString::fromUtf8("Any"));
	font.setFixedPitch(true);
	setFont(font);
	setReadOnly(true);
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

	setTextColor(QColor("black"));
	setFontWeight(QFont::Normal);

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
		setTextColor(QColor("black"));
		setFontWeight(QFont::Normal);
	}
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::Start);
	setTextCursor(cursor);
	ensureCursorVisible ();
}


void DiffViewer::cursorPosChanged(QTextCursor cursor)
{
	if(!diffType)
		return;
	int pos= cursor.position();
	QTextDocument *doc= this->document();
	QString paraText;
	int paraFrom=-1,paraTo=-1;
	int para;
	int tmp;
	int totalPara=doc->blockCount();
	para = doc->findBlock(pos).blockNumber();

	/* if a diff line is clicked select entire file */
	if(doc->findBlockByNumber(para).text().startsWith("diff")) {
		paraFrom = para;
		paraTo=totalPara-1;
		tmp=para+1;
		while(tmp<totalPara) {
			paraText = doc->findBlockByNumber(tmp).text();
			if(paraText.startsWith("diff")) {
				paraTo = tmp;
				break;
			}
			tmp++;
		}
	} else {
		/* Find starting line */
		tmp=para;
		while(tmp) {
			paraText = doc->findBlockByNumber(tmp).text();
			if(paraText.startsWith("@@")) {
				paraFrom = tmp;
				break;
			} else if(paraText.startsWith("diff")) {
				return;
			}
			tmp--;
		}

		paraTo=totalPara-1;
		tmp=para;
		while(tmp<totalPara) {
			paraText = doc->findBlockByNumber(tmp).text();
			if(paraText.startsWith("@@")
			   || paraText.startsWith("diff")) {
				paraTo = tmp;
				break;
			}
			tmp++;
		}
	}
	select(doc->findBlockByNumber(paraFrom).position(),doc->findBlockByNumber(paraTo).position());
}

void DiffViewer::select(int from,int to)
{
	QTextCursor cursor = this->textCursor();
	cursor.setPosition(from);
	cursor.setPosition(to,QTextCursor::KeepAnchor);
	setTextCursor(cursor);
}

void DiffViewer::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		cursorPosChanged(cursorForPosition(event->pos()));
	} else {
		// pass on other buttons to base class
		QTextEdit::mousePressEvent(event);
	}
}

void DiffViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		emit doubleClicked();
	}
}
