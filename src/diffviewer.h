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
