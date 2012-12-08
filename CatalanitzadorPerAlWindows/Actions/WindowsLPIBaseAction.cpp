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

#include "WindowsLPIBaseAction.h"
#include "LogExtractor.h"

#define KEYWORD_TOSEARCH L"Error"
#define LINES_TODUMP 7

wstring WindowsLPIBaseAction::_getMostRecentLpkSetupLogFile()
{	
	HANDLE hFind;
	WIN32_FIND_DATA findFileData;
	wchar_t szFilename[MAX_PATH];
	wchar_t szRecentFilename[MAX_PATH];
	FILETIME filetime = {0};
	wstring filename;
	
	GetTempPath(MAX_PATH, szFilename);
	wcscat_s(szFilename, L"lpksetup-*");
	hFind = FindFirstFile(szFilename, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		g_log.Log(L"WindowsLPIBaseAction::_getMostRecentLpkSetupLogFile. Cannot open '%s'", szFilename);
		return wstring();
	}

	// If there are multiple files, select the most recent one
	do
	{
		if (findFileData.ftLastWriteTime.dwHighDateTime > filetime.dwHighDateTime ||
			findFileData.ftLastWriteTime.dwHighDateTime == filetime.dwHighDateTime && findFileData.ftLastWriteTime.dwLowDateTime > filetime.dwLowDateTime)
		{
			wcscpy_s(szRecentFilename, findFileData.cFileName);
			filetime = findFileData.ftLastWriteTime;
		}
	} while (FindNextFile(hFind, &findFileData) != 0);


	FindClose(hFind);

	GetTempPath(MAX_PATH, szFilename);
	filename = szFilename;
	filename += szRecentFilename;
	return filename;
}

void WindowsLPIBaseAction::_dumpLpkSetupErrors()
{
	wstring filename;

	filename = _getMostRecentLpkSetupLogFile();

	if (filename.empty())
	{
		return;
	}

	Sleep(10000); // Since the lpksetup.exe process exists until you can read the file it takes some seconds
	LogExtractor logExtractor(filename, LINES_TODUMP);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD_TOSEARCH);
	logExtractor.DumpLines();
}