/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Runner.h"

bool Runner::Execute(wchar_t* program, wchar_t* params, bool b64bits)
{
	STARTUPINFO si;
	PVOID OldValue = NULL;

	ZeroMemory (&si, sizeof (si));
	si.cb = sizeof(si);
	ZeroMemory (&pi, sizeof (pi));
	
	if (b64bits)
	{
		Wow64DisableWow64FsRedirection(&OldValue);
	}

	// Start the child process
	if (!CreateProcess(program, params,         
		NULL,  NULL,  FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		int error = GetLastError();
		g_log.Log (L"Runner::Execute. Error %u running '%s' with params '%s'", (wchar_t *)error,
			program, params);

		return false;
	}

	if (b64bits)
	{
		Wow64RevertWow64FsRedirection(OldValue);
	}
	
	return true;
}

bool Runner::IsRunning()
{
	DWORD dwStatus;

	GetExitCodeProcess(pi.hProcess, &dwStatus);
	return dwStatus == STILL_ACTIVE;
}

void Runner::WaitUntilFinished()
{
	WaitForSingleObject(pi.hProcess, INFINITE);
}