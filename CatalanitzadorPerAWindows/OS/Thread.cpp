/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */
 
#include "stdafx.h"
#include "Thread.h"

#define DEFAULT_MILISECONDS_FORWAIT 10000

Thread::Thread()
{
	m_hThread = NULL;
	SetWaitTime(DEFAULT_MILISECONDS_FORWAIT);
}

Thread::~Thread()
{
	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void Thread::Start()
{
	assert(m_hThread == NULL);

	m_hThread = CreateThread(NULL, 0, _callbackThread, this, 0, NULL);
}

DWORD WINAPI Thread::_callbackThread(LPVOID lpParam)
{
	Thread* pThis = (Thread *) lpParam;
	pThis->OnStart();
	return 0;
}

void Thread::Wait()
{
	if (m_hThread != NULL)
		WaitForSingleObject(m_hThread, m_nMilliseconds);
}