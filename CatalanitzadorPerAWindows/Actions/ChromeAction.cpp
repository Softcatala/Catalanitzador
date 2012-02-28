/* 
 * Copyright (C) 2012 Xavier Ivars-Ribes <xavi.ivars@gmail.com>
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
#include "ChromeAction.h"
#include "Runner.h"
#include <fstream>
#include <cstdio>

#define ChromeRegistryPath L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome"

enum JSONChromeState { NoState, InIntl, InIntlSemicolon,
				InIntlBlock, InAcceptedKey, InAcceptedSemicolon, 
				InAcceptedValue,EndParsing };

ChromeAction::ChromeAction(IRegistry* registry)
{
	m_registry = registry;
	szVersionAscii[0] = NULL;
	isInstalled = false;
}

wchar_t* ChromeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_CHROMEACTION_NAME, szName);
}

wchar_t* ChromeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_CHROMEACTION_DESCRIPTION, szDescription);	
}

DWORD ChromeAction::GetProcessIDForRunningApp() 
{
	Runner runner;

	return runner.GetProcessID(wstring(L"chrome.exe"));
}

bool ChromeAction::_findIntl(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"\"intl\"",pos);

	return pos != wstring::npos;
}

bool ChromeAction::_findSemicolon(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L":",pos);

	return pos != wstring::npos;
}

bool ChromeAction::_findStartBlock(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"{",pos);

	return pos != wstring::npos;
}

bool ChromeAction::_findAcceptedKey(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"\"accept_languages\"",pos);

	return pos != wstring::npos;
}

bool ChromeAction::_findLanguageString(wstring line,int & pos,wstring & langcode) 
{

	wstring tempLang;

	if(pos == wstring::npos) pos = 0;

	wchar_t prev = L'0';
	bool reading = false;
	bool found = false;

	for(unsigned int i = pos; i < line.length() && found == false; ++i) {
		if(line[i] == L'"' && prev != L'\\') {
			if(reading == false) {
				// we start parsing the langcode
				reading = true;
				pos = i+1;
			} else {
				// all langcode string parsed
				langcode = tempLang;
				found = true;
			}
		} else {
			if(reading) {
				tempLang.push_back(line[i]);
			}
		}
		
		prev = line[i];
	}
	
	return found;
}

bool ChromeAction::_readLanguageCode(wstring& langcode)
{
	bool ret = false;
	wstring path_t;
	_readInstallLocation(path_t);

	if(path_t.empty() == false) 
	{
		wifstream reader;
		wstring line;
		wstring path = L"/../User Data/Default/Preferences";
		path = path_t + path;
		reader.open(path.c_str());

		if(reader.is_open()) 
		{
			int currentState = NoState;
			
			int pos = 0;

			while(!(getline(reader,line)).eof() && ret == false)
			{
				if(currentState == NoState) {
					if(_findIntl(line,pos))
						currentState = InIntl;
				}

				if(currentState == InIntl) {
					if(_findSemicolon(line,pos))
						currentState = InIntlSemicolon;
				}

				if(currentState == InIntlSemicolon) {
					if(_findStartBlock(line,pos))
						currentState = InIntlBlock;
				}

				if(currentState == InIntlBlock) {
					if(_findAcceptedKey(line,pos))
						currentState = InAcceptedKey;
				}
				
				if(currentState == InAcceptedKey) {
					if(_findSemicolon(line,pos))
						currentState = InAcceptedSemicolon;
				}

				if(currentState == InAcceptedSemicolon) {
					if(_findLanguageString(line,pos,langcode))
						ret = true;
				}

				pos = wstring::npos;
			}
		}
	}

	return ret;
}

bool ChromeAction::_writeLanguageCode(wstring langcode)
{	
	bool ret = false;
	wstring path_t;
	_readInstallLocation(path_t);

	if(path_t.empty() == false) 
	{
		wifstream reader;
		wofstream writer;
		wstring line;
		wstring pathr = L"/../User Data/Default/Preferences";
		wstring pathw = L"/../User Data/Default/Preferences.new";
		pathr = path_t + pathr;
		pathw = path_t + pathw;
		reader.open(pathr.c_str());
		writer.open(pathw.c_str());
		
		if(reader.is_open() && writer.is_open()) 
		{
			int currentState = NoState;
			
			int pos = 0;
			wstring oldLang;
			wstring lastLine = L"";

			while(!(getline(reader,line)).eof())
			{
				if(currentState == NoState) {
					if(_findIntl(line,pos))
						currentState = InIntl;
				}

				if(currentState == InIntl) {
					if(_findSemicolon(line,pos))
						currentState = InIntlSemicolon;
				}

				if(currentState == InIntlSemicolon) {
					if(_findStartBlock(line,pos))
						currentState = InIntlBlock;
				}

				if(currentState == InIntlBlock) {
					if(_findAcceptedKey(line,pos))
						currentState = InAcceptedKey;
				}
				
				if(currentState == InAcceptedKey) {
					if(_findSemicolon(line,pos))
						currentState = InAcceptedSemicolon;
				}

				if(currentState == InAcceptedSemicolon) {
					if(_findLanguageString(line,pos,oldLang)) {
						currentState = EndParsing;
						line.replace(pos,oldLang.length(),langcode);
						ret = true;
					}
				}

				pos = wstring::npos;

				writer << lastLine << L"\n";
				lastLine = line;
			}
			
			if(ret == false) {
				writer << "\t,\"intl\":{\"accept_languages\":\"ca\"}\n";
				ret = true;

			}
			writer << lastLine << L"\n";
			writer.close();
			reader.close();
		}
		
		if(ret) {
			ret = MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
		}
	}

	if(ret) {
		status = Successful;

	} else {
		status = FinishedWithError;
	}

	return ret;
}


void ChromeAction::_getFirstLanguage(wstring& jsonvalue)
{
	if(m_languages.size() > 0)
		jsonvalue = m_languages[0];

	return;
}

bool ChromeAction::IsNeed()
{
	bool bNeed = true;
	wstring langcode, firstlang;

	bool langcodeFound = _readLanguageCode(langcode);

	if(isInstalled) {
		if(langcodeFound) {
			ParseLanguage(langcode);
			_getFirstLanguage(firstlang);	
	
			bNeed = firstlang.compare(L"ca") != 0;
			if(bNeed == false) {
				status = AlreadyApplied;
			}
		}
	} else {
		bNeed = false;
		status = CannotBeApplied;
	}
	
	g_log.Log(L"ChromeAction::IsNeed returns %u (first lang:%s)", (wchar_t *) bNeed, (wchar_t *) firstlang.c_str());
	return bNeed;
}

void ChromeAction::AddCatalanToArrayAndRemoveOldIfExists()
{	
	wstring regvalue;
	vector <wstring>languages;
	vector<wstring>::iterator it;

	// Delete previous ocurrences of Catalan locale that were not first
	for (it = m_languages.begin(); it != m_languages.end(); ++it)
	{
		wstring element = *it;
		std::transform(element.begin(), element.end(), element.begin(), ::tolower);

		if (element.compare(L"ca") == 0)
		{
			m_languages.erase(it);
			break;
		}
	}

	wstring str(L"ca");
	it = m_languages.begin();
	m_languages.insert(it, str);
}

void ChromeAction::Execute()
{
	wstring regvalue;

	AddCatalanToArrayAndRemoveOldIfExists();
	CreateJSONString(regvalue);
	_writeLanguageCode(regvalue);
}

void ChromeAction::_readVersion()
{
	if (m_registry->OpenKey(HKEY_CURRENT_USER, ChromeRegistryPath, false))
	{
		wchar_t szVersion[1024];

		if (m_registry->GetString(L"Version", szVersion, sizeof(szVersion)))
		{
			WideCharToMultiByte(CP_ACP, 0, szVersion, wcslen(szVersion) + 1, szVersionAscii, sizeof(szVersionAscii), 
				NULL, NULL);

			g_log.Log(L"ChromeAction::_readVersion. Chrome version %s", szVersion);
		}
		m_registry->Close();
	}	
}

void ChromeAction::_readInstallLocation(wstring & path)
{
	wchar_t * szInstallLocation = NULL;
	path = L"";

	if (m_registry->OpenKey(HKEY_CURRENT_USER, ChromeRegistryPath, false))
	{
		szInstallLocation = new wchar_t[1024];
		
		if (m_registry->GetString(L"InstallLocation", szInstallLocation, sizeof(szInstallLocation)))
		{
			isInstalled = true;
			path = szInstallLocation;
			g_log.Log(L"ChromeAction::_readVersion. Chrome version %s", szInstallLocation);
			delete(szInstallLocation);
			szInstallLocation = NULL;
		} 
		else 
		{
			delete(szInstallLocation);
			szInstallLocation = NULL;
		}
		m_registry->Close();
	} else {
		g_log.Log(L"ChromeAction::_readInstallLocation - Chrome is not installed");
	}
}

char* ChromeAction::GetVersion()
{
	if (*szVersionAscii == 0x0)
	{
		_readVersion();
	}
	return szVersionAscii;
}

void ChromeAction::CheckPrerequirements(Action * action)
{
	_readVersion();
}

void ChromeAction::CreateJSONString(wstring &jsonvalue)
{
	int languages = m_languages.size();	
	
	if (languages == 1)
	{
		jsonvalue = m_languages.at(0);
		return;
	}

	jsonvalue.empty();
	
	jsonvalue = m_languages.at(0);
		
	for (int i = 1; i < languages; i++)
	{
		jsonvalue += L"," + m_languages.at(i);
	}
}

void ChromeAction::ParseLanguage(wstring value)
{
	wstring language;

	m_languages.clear();
	
	for (unsigned int i = 0; i < value.size (); i++)
	{
		if (value[i] == L',')
		{
			m_languages.push_back(language);
			language.clear();
		} else {
			language += value[i];
		}
	}

	if (language.empty() == false)
	{
		m_languages.push_back(language);
	}
}
