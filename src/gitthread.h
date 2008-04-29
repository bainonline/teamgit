#ifndef __GITTHREAD_H__
#define __GITTHREAD_H__

#include <QObject>
#include <QThread>
#include "gitprocess.h"

class GitThread  : public QThread
{
	Q_OBJECT;
private:
	bool threadStarted(void (GitThread::*fn)(void));
	void (GitThread::*function)(void);

public:
	GitProcess *git;
	GitThread();
	void run();

};

#endif // __GITTHREAD_H__
