#ifndef __DIFFVIEWER_H__
#define __DIFFVIEWER_H__

#include <QTextEdit>

#define DIFF_LIMIT (1024 * 1024)
// place your code here
class DiffViewer : public QTextEdit
{
	public:
	DiffViewer(QWidget * parent);
	~DiffViewer();
	void setDiffText(QString text);
	
};

#endif // __DIFFVIEWER_H__
