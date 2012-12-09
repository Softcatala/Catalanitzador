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
#include "StringConversion.h"

#include <sstream>

LogFile::LogFile()
{
	m_hLog = INVALID_HANDLE_VALUE;
}

LogFile::~LogFile()
{
	Close();
}

void LogFile::Close()
{
	if (m_hLog != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hLog);
		m_hLog = INVALID_HANDLE_VALUE;
	}
}

bool LogFile::CreateLogInTempDirectory(wchar_t* logFileName, wchar_t* appName)
{
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, logFileName);

	m_hLog = CreateFile(m_szFilename, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hLog == INVALID_HANDLE_VALUE)
		return false;

	_initLogFile(appName, GetLastError() == ERROR_ALREADY_EXISTS);
	return true;
}

bool LogFile::CreateLog(wchar_t* logFileName, wchar_t* appName)
{	
	wcscpy_s(m_szFilename, logFileName);
	m_hLog = CreateFile(m_szFilename, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hLog == INVALID_HANDLE_VALUE) 
		return false;

	_initLogFile(appName, GetLastError() == ERROR_ALREADY_EXISTS);
	return true;
}

void LogFile::_initLogFile(wchar_t* appName, bool fileAlreadyExists)
{	
	if (fileAlreadyExists == false)
	{
		_write(L"\xFEFF"); // Unicode mark
	}
	else
	{
		_write(L"\r\n");
	}
	_writeCompileTime(appName);
	_today();
}


#define UNICODE_MARK_LEN 1

wstring LogFile::GetContent()
{
	wstring content;
	ifstream reader(m_szFilename);

	if (reader.is_open() == false)
	{
		return content;
	}

	stringstream ss;
	ss << reader.rdbuf() << '\0';
	reader.close();
	
	content = wstring((wchar_t *)ss.str().c_str());
	return content.substr(UNICODE_MARK_LEN, content.size() - UNICODE_MARK_LEN);
}

void LogFile::Log(wchar_t* string)
{		
	_stringTime();
	_writeLine(string);
}

void LogFile::Log(wchar_t* format, wchar_t* string)
{		
	_stringTime();

	swprintf_s(m_szText, format, string);
	_writeLine(m_szText);
}

void LogFile::Log(wchar_t* format, wchar_t* string1, wchar_t* string2)
{		
	_stringTime();

	swprintf_s(m_szText, format, string1, string2);
	_writeLine(m_szText);
}

void LogFile::Log(wchar_t* format, wchar_t* string1, wchar_t* string2, wchar_t* string3)
{		
	_stringTime();

	swprintf_s(m_szText, format, string1, string2, string3);
	_writeLine(m_szText);
}

void LogFile::Log(wchar_t* format, wchar_t* string1, wchar_t* string2, wchar_t* string3, wchar_t* string4)
{		
	_stringTime();

	swprintf_s(m_szText, format, string1, string2, string3, string4);
	_writeLine(m_szText);
}

void LogFile::_writeCompileTime(wchar_t* appName)
{
	wstring date, time;

	StringConversion::ToWideChar(string(__DATE__), date);
	StringConversion::ToWideChar(string(__TIME__), time);
	
	Log(L"%s. Compiled on %s - %s", appName, (wchar_t*) date.c_str(), (wchar_t*) time.c_str());
}

void LogFile::_today()
{
	SYSTEMTIME systemTime;
	wchar_t	szTime[128];

	GetLocalTime(&systemTime);

	swprintf_s(szTime, L"Today is %04u/%02u/%02u", systemTime.wYear, systemTime.wMonth, systemTime.wDay);
	_stringTime();
	_writeLine(szTime);
}

void LogFile::_stringTime()
{
	SYSTEMTIME systemTime;
	wchar_t	szTime[128];

	GetLocalTime(&systemTime);

	swprintf_s(szTime, L"%02u:%02u:%02u.%04u - ", systemTime.wHour, systemTime.wMinute, systemTime.wSecond,
		systemTime.wMilliseconds);

	_write(szTime);
}

void LogFile::_write(wchar_t* string)
{
	DWORD dwWritten;

	if (m_hLog == INVALID_HANDLE_VALUE)
		return;

	WriteFile(m_hLog, string, wcslen (string) * sizeof (wchar_t), &dwWritten, NULL);
}

void LogFile::_writeLine(wchar_t* string)
{		
	DWORD dwWritten;

	if (m_hLog == INVALID_HANDLE_VALUE)
		return;

	wcscpy_s(m_szText, string);
	wcscat_s(m_szText, L"\r\n");

	WriteFile(m_hLog, m_szText, wcslen (m_szText) * sizeof (wchar_t), &dwWritten, NULL);
}
