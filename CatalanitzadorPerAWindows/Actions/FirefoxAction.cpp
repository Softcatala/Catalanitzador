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
#include <Shlobj.h>

#include "FirefoxAction.h"
#include "OSVersion.h"
#include "StringConversion.h"

#include <fstream>
#include <cstdio>


FirefoxAction::FirefoxAction(IRegistry* registry)
{
	m_registry = registry;
	szVersionAscii[0] = NULL;
	m_CachedLanguageCode = false;
}

wchar_t* FirefoxAction::GetName()
{
	return _getStringFromResourceIDName(IDS_FIREFOXACTION_NAME, szName);
}

wchar_t* FirefoxAction::GetDescription()
{	
	return _getStringFromResourceIDName(IDS_FIREFOXACTION_DESCRIPTION, szDescription);
}

DWORD FirefoxAction::_getProcessID()
{
	Runner runner;
	return runner.GetProcessID(wstring(L"firefox.exe"));
}

bool FirefoxAction::IsExecuting()
{
	return _getProcessID() != 0;
}

void FirefoxAction::FinishExecution()
{
	Runner runner;
	runner.RequestQuitToProcessID(_getProcessID());
}

bool FirefoxAction::IsNeed()
{
	bool bNeed = false;
	wstring firstlang;

	_readVersionAndLocale();

	if (_readLanguageCode())
	{
		if (m_languages.size() == 0)
		{
			if (m_locale == L"ca")
			{
				bNeed = false;
			}
			else
			{
				bNeed = true;
			}
		}
		else
		{
			_getFirstLanguage(firstlang);
			bNeed = firstlang.compare(L"ca-es") != 0 && firstlang.compare(L"ca") != 0;
		}

		if (bNeed == false)
			status = AlreadyApplied;
	}
	else
	{
		status = CannotBeApplied;
	}
	
	g_log.Log(L"FirefoxAction::IsNeed returns %u (first lang:%s)", (wchar_t *) bNeed, (wchar_t *) firstlang.c_str());
	return bNeed;
}

void FirefoxAction::_getFirstLanguage(wstring& regvalue)
{
	if (m_languages.size() > 0)
	{		
		regvalue = m_languages[0];
		std::transform(regvalue.begin(), regvalue.end(), regvalue.begin(), ::tolower);
		return;
	}
	
	regvalue.clear();
	return;
}

void FirefoxAction::_getProfileRootDir(wstring &location)
{
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Mozilla\\Firefox\\";
}


void FirefoxAction::_getProfilesIniLocation(wstring &location)
{	
	_getProfileRootDir(location);
	location += L"profiles.ini";
}

#define	PATHKEY L"Path="

bool FirefoxAction::_getProfileLocationFromProfilesIni(wstring file, wstring &profileLocation)
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

		_getProfileRootDir(profileLocation);
		profileLocation += (wchar_t *)&line[pathLen];
		return true;
	}

	return false;
}

bool FirefoxAction::_getPreferencesFile(wstring &location)
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

void FirefoxAction::_parseLanguage(wstring regvalue)
{
	wstring language;
	
	m_languages.clear();
	for (unsigned int i = 0; i < regvalue.size (); i++)
	{
		if (regvalue[i] == L',')
		{
			m_languages.push_back(language);
			language.clear();
			continue;
		}

		language += regvalue[i];
	}

	if (language.empty() == false)
	{
		m_languages.push_back(language);
	}
}


#define USER_PREF L"user_pref(\"intl.accept_languages\", \""

bool FirefoxAction::_readLanguageCode()
{	
	wstring location, line, langcode;
	wifstream reader;

	if (m_CachedLanguageCode == true)
		return true;	
	
	if (_getPreferencesFile(location) == false)
	{
		g_log.Log(L"FirefoxAction::_readLanguageCode. No preferences file found. Firefox is not installed");
		return false;
	}
	reader.open(location.c_str());

	if (reader.is_open())
	{
		int start, end;

		while(!reader.eof())
		{	
			getline(reader,line);
			start = line.find(USER_PREF);

			if (start == wstring::npos)
				continue;

			start+=wcslen(USER_PREF);

			end = line.find(L"\"", start);

			if (end == wstring::npos)
				continue;

			langcode = line.substr(start, end - start);
			_parseLanguage(langcode);
			break;
		}
	}
	else
	{
		g_log.Log(L"FirefoxAction::_readLanguageCode cannot open %s", (wchar_t *) location.c_str());
		return false;
	}

	reader.close();
	g_log.Log(L"FirefoxAction::_readLanguageCode open %s", (wchar_t *) location.c_str());
	m_CachedLanguageCode = true;
	return true;
}
void FirefoxAction::_addCatalanToArrayAndRemoveOldIfExists()
{
	vector<wstring>::iterator it;

	// Delete previous ocurrences of Catalan locale that were not first
	for (it = m_languages.begin(); it != m_languages.end(); ++it)
	{
		wstring element = *it;
		std::transform(element.begin(), element.end(), element.begin(), ::tolower);

		if (element.compare(L"ca-es") == 0 || element.compare(L"ca") == 0)
		{
			m_languages.erase(it);
			break;
		}
	}

	wstring str(L"ca");
	it = m_languages.begin();
	m_languages.insert(it, str);
}

void FirefoxAction::_createPrefsString(wstring& string)
{
	int languages = m_languages.size();	
	
	if (languages == 1)
	{
		string = m_languages.at(0);
		return;
	}
		
	string = m_languages.at(0);
	for (int i = 1; i < languages; i++)
	{
		string += L"," + m_languages.at(i);
	}
}

void FirefoxAction::_getPrefLine(wstring langcode, wstring& line)
{
	line = USER_PREF;
	line += langcode;
	line += L"\");";
}

void FirefoxAction::_writeLanguageCode(wstring &langcode)
{
	wifstream reader;
	wofstream writer;
	wstring line, filer, filew;
	bool ret, written;
	
	_getPreferencesFile(filer);
	filew += filer;
	filew += L".new";

	reader.open(filer.c_str());

	if (!reader.is_open())
		return;

	writer.open(filew.c_str());

	if (!writer.is_open())
	{
		reader.close();
		return;
	}

	written = false;
	while(!(getline(reader,line)).eof())
	{
		if (line.find(USER_PREF) != wstring::npos)
		{
			written = true;
			_getPrefLine(langcode, line);
		}

		writer << line << L"\n";
	}

	if (written == false)
	{
		_getPrefLine(langcode, line);
		writer << line << L"\n";
	}

	writer.close();
	reader.close();

	ret = MoveFileEx(filew.c_str(), filer.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
	
	if (ret)
	{
		status = Successful;
	} 
	else 
	{
		status = FinishedWithError;
	}
}

// The Firefox locale determines which languages are going to be used in the 
// accept_languages. Since by setting to Catalan we reset the default value
// add at the end of the locale of the langpack as secondary language, then
// if you install Firefox in English you will have ca, es as language codes
void FirefoxAction::_addFireForLocale()
{
	if (m_locale == L"ca")
		return;

	if (m_languages.size() == 0)
	{			
		if (m_locale.size() > 0)
		{
			m_languages.push_back(m_locale);
		}
	}
}

void FirefoxAction::Execute()
{
	wstring value;

	_readVersionAndLocale();
	_readLanguageCode();

	_addFireForLocale();
	_addCatalanToArrayAndRemoveOldIfExists();
	_createPrefsString(value);
	_writeLanguageCode(value);
}

const wchar_t* FirefoxAction::GetVersion()
{
	_readVersionAndLocale();
	return m_version.c_str();
}

bool FirefoxAction::_readVersionAndLocale()
{
	if (m_version.length() > 0)
	{
		return true;
	}

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Mozilla\\Mozilla Firefox", false) == false)
	{
		g_log.Log(L"FirefoxAction::_readVersionAndLocale. Cannot open registry key");
		return false;
	}
	
	wstring sreg, locale;
	wchar_t szVersion[1024];
	int start, end;

	if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
	{
		sreg = szVersion;
		start = sreg.find(L" ");

		if (start != wstring::npos)
		{
			m_version = sreg.substr(0, start);			

			start = sreg.find(L"(", start);

			if (start != wstring::npos)
			{
				start++;
				end = sreg.find(L")", start);
				if (end != wstring::npos)
				{
					m_locale = sreg.substr(start, end-start);
				}
			}
		}

		g_log.Log(L"FirefoxAction::_readVersionAndLocale. Firefox version %s, version %s, locale %s", 
			(wchar_t*) szVersion, (wchar_t*) m_version.c_str(), (wchar_t*)  m_locale.c_str());
	}
	m_registry->Close();
	return m_locale.empty() != true;
}