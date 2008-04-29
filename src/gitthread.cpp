#include "gitthread.h"
#include <stdio.h>

GitThread::GitThread()
{
}

bool GitThread::threadStarted(void (GitThread::*fn)(void))
{
	if(QThread::currentThread() != this) {
		function = fn;
		start();
		return false;
	} else {
		return true;
	}
}

void GitThread::run()
{	
	git = new GitProcess();
	exec();
	while(1)
		sleep(10);
}
