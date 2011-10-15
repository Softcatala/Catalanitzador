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
#include "LogFile.h"
#include <stdio.h>

LogFile::LogFile()
{
	m_hLog = NULL;
}

LogFile::~LogFile()
{
	Close ();
}

void LogFile::Close ()
{
	if (m_hLog != NULL)
	{
		CloseHandle (m_hLog);
		m_hLog = NULL;
	}
}
bool LogFile::CreateLog(wchar_t* logFileName)
{
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, logFileName);	

	m_hLog = CreateFile(m_szFilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!m_hLog) return false;

	Write(L"\xFEFF"); // Unicode mark
	WriteCompileTime ();
	return true;
}

void LogFile::Log(wchar_t* string)
{		
	StringTime();
	WriteLine (string);
}

void LogFile::Log(wchar_t* format, wchar_t* string)
{		
	StringTime();

	swprintf_s (m_szText, format, string);
	WriteLine (m_szText);
}

void LogFile::Log(wchar_t* format, wchar_t* string1, wchar_t* string2)
{		
	StringTime();

	swprintf_s (m_szText, format, string1, string2);
	WriteLine (m_szText);
}

void LogFile::WriteCompileTime ()
{
	wchar_t szDate [256];
	wchar_t szTime [256];

	MultiByteToWideChar(CP_ACP, 0,  __DATE__, strlen ( __DATE__) + 1,
                  szDate, sizeof (szDate));

	MultiByteToWideChar(CP_ACP, 0,  __TIME__, strlen ( __TIME__) + 1,
                  szTime, sizeof (szTime));	
	
	Log(L"Compiled on %s - %s", szDate, szTime);
}

void LogFile::StringTime()
{
	SYSTEMTIME systemTime;
	wchar_t	szTime[128];

	GetSystemTime(&systemTime);

	swprintf_s (szTime, L"%02u:%02u:%02u.%04u - ", systemTime.wHour, systemTime.wMinute, systemTime.wSecond,
		systemTime.wMilliseconds);

	Write (szTime);
}

void LogFile::Write (wchar_t* string)
{
	DWORD dwWritten;
	WriteFile (m_hLog, string, wcslen (string) * sizeof (wchar_t), &dwWritten, NULL);
}

void LogFile::WriteLine (wchar_t* string)
{		
	DWORD dwWritten;

	wcscpy_s (m_szText, string);
	wcscat_s (m_szText, L"\r\n");

	WriteFile (m_hLog, m_szText, wcslen (m_szText) * sizeof (wchar_t), &dwWritten, NULL);
}
