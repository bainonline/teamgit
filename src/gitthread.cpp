/*
	Copyright 2007 2008 Abhijit Bhopatkar
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program, in file COPYING
	If not, see <http://www.devslashzero.com/teamgit/license>.
*/
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
