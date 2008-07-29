#ifndef __DIFFVIEWER_H__
#define __DIFFVIEWER_H__

#include <QTextEdit>
// place your code here
class DiffViewer : public QTextEdit
{
	public:
	DiffViewer(QWidget * parent);
	~DiffViewer();
};

#endif // __DIFFVIEWER_H__
