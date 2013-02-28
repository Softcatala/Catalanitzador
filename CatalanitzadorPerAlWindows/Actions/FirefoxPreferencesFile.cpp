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
#include <stdio.h>
#include "FirefoxPreferencesFile.h"
#include <cstdio>

FirefoxPreferencesFile::FirefoxPreferencesFile(wstring profileRootDir)
{	
	m_profileRootDir = profileRootDir;	
}

#define	PATHKEY L"Path="

bool FirefoxPreferencesFile::_getProfileLocationFromProfilesIni(wstring file, wstring &profileLocation)
{
	wifstream reader;
	wstring line;
	const int pathLen = wcslen(PATHKEY);

	reader.open(file.c_str());

	if (!reader.is_open())	
		return false;

	while(!(getline(reader, line)).eof())
	{
		if (_wcsnicmp(line.c_str(), PATHKEY, pathLen) != 0)
			continue;

		profileLocation = _getProfileRootDir() + (wchar_t *)&line[pathLen];
		return true;
	}

	return false;
}

void FirefoxPreferencesFile::_getProfilesIniLocation(wstring &location)
{	
	location = _getProfileRootDir();
	location += L"profiles.ini";
}

bool FirefoxPreferencesFile::_getPreferencesFile(wstring &location)
{
	wstring profileIni;

	_getProfilesIniLocation(profileIni);
	
	if (_getProfileLocationFromProfilesIni(profileIni, location))
	{	
		location += L"\\prefs.js";
		return true;
	}
	else
	{
		return false;
	}
}

#define PREF_VALUE L"pref(\"%s\", \""

bool FirefoxPreferencesFile::_readValue(wstring location, wstring key, wstring& value)
{
	wstring line, prefkey;
	wifstream reader;
	wchar_t szTempKey[1024];

	wsprintf(szTempKey, PREF_VALUE, key.c_str());
	prefkey = szTempKey;	
	
	reader.open(location.c_str());

	if (reader.is_open())
	{
		int start, end;

		while(!reader.eof())
		{	
			getline(reader,line);
			start = line.find(prefkey);

			if (start == wstring::npos)
				continue;

			start += prefkey.size();

			end = line.find(L"\"", start);

			if (end == wstring::npos)
				continue;

			value = line.substr(start, end - start);			
			break;
		}
	}
	else
	{
		g_log.Log(L"FirefoxPreferencesFile::_readValue cannot open %s", (wchar_t *) location.c_str());
		return false;
	}

	reader.close();
	g_log.Log(L"FirefoxPreferencesFile::_readValue open %s", (wchar_t *) location.c_str());	
	return true;
}
