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
#ifndef __DIFFVIEWER_H__
#define __DIFFVIEWER_H__

#include <QObject>
#include <QTextEdit>

#define DIFF_LIMIT (1024 * 1024)
// place your code here
class DiffViewer : public QTextEdit
{
	Q_OBJECT
	private:
		int diffType;
		
		void select(int blockFrom,int blockTo);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
	public slots:
		void cursorPosChanged(QTextCursor);
	
	public:
		DiffViewer(QWidget * parent);
		~DiffViewer();
		void setDiffText(QString text);
		void setDiffType(int type) {
			diffType=type;
		}
		int getDiffType() {
			return diffType;
		}
	
	signals:
		void clicked();
		void doubleClicked();
	
};

#endif // __DIFFVIEWER_H__
