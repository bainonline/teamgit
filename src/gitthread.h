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
