/* 
 * Copyright (C) 2015 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "x64BitsProcess.h"
#include "Resources.h"

#define EXECUTABLE_RSCTYPE L"EXECUTABLE_FILES"


x64BitsProcess::x64BitsProcess()
{
	m_extracted = false;
}

void x64BitsProcess::_extractExecutable()
{
	if (m_extracted == true)
	{
		return;
	}

	Resources::DumpResource(EXECUTABLE_RSCTYPE, MAKEINTRESOURCE(IDR_BINARY_LIST64PROCESSESS), 
		(wchar_t *)m_executable.GetFileName().c_str());

	m_extracted = true;
}


bool x64BitsProcess::_execute(wchar_t* program, wchar_t* params)
{
	BOOL bRslt;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;	

	ZeroMemory (&si, sizeof (si));
	si.cb = sizeof(si);
	ZeroMemory (&pi, sizeof (pi));
	
	bRslt = CreateProcess(program, params, NULL,  NULL,  FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	if (!bRslt)
	{
		int error = GetLastError();
		g_log.Log(L"x64BitsProcess::_execute. Error %u running '%s' with params '%s'", (wchar_t *)error,
			program, params);
		return false;
	}

	g_log.Log(L"x64BitsProcess::_execute. Done running '%s' with params '%s'", program, params);
	WaitForSingleObject(pi.hProcess, INFINITE);
	return true;
}

vector <wstring> x64BitsProcess::GetRunningProcessesNames()
{
	vector <wstring> names;
	TempFile result;

	_extractExecutable();

	wchar_t szParam[MAX_PATH];
	wcscat_s(szParam, L" ");
	wcscat_s(szParam, result.GetFileName().c_str());
	_execute((wchar_t*)m_executable.GetFileName().c_str(), (wchar_t*)szParam);

	wifstream reader;
	wstring line;

	reader.open(result.GetFileName().c_str());
	if (!reader.is_open())
	{
		g_log.Log (L"x64BitsProcess::GetRunningProcessesNames. Cannot open '%s'", (wchar_t *)result.GetFileName().c_str());
		return names;
	}

	while(!(getline(reader, line)).eof())
	{		
		names.push_back(line);
	}

	g_log.Log(L"x64BitsProcess::GetRunningProcessesNames. Names %u", (wchar_t *)names.size());
	return names;
}