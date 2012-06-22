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

enum JSONChromeState { NoState, InIntl, InIntlSemicolon,
				InIntlBlock, InAcceptedKey, InAcceptedSemicolon, 
				InAcceptedValue,EndParsing };

ChromeAction::ChromeAction(IRegistry* registry)
{
	m_registry = registry;
}

wchar_t* ChromeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_CHROMEACTION_NAME, szName);
}

wchar_t* ChromeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_CHROMEACTION_DESCRIPTION, szDescription);	
}

vector <DWORD> ChromeAction::_getProcessIDs()
{
	Runner runner;
	return runner.GetProcessID(wstring(L"chrome.exe"));
}

bool ChromeAction::IsExecuting()
{
	return _getProcessIDs().size() != 0;
}

void ChromeAction::FinishExecution()
{
	if (_getProcessIDs().size() > 0)
	{
		Runner runner;
		runner.RequestCloseToProcessID(_getProcessIDs().at(0));
	}
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

bool ChromeAction::_findAppLocaleKey(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"\"app_locale\"",pos);

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

#define LANGUAGECODE L"ca"

bool ChromeAction::_isChromeAppLocaleOk()
{	
	wstring path_t, langcode;
	_readInstallLocation(path_t);

	if(path_t.empty() == false)
	{
		wifstream reader;
		wstring line;
		wstring path = L"/../User Data/Local State";
		path = path_t + path;
		reader.open(path.c_str());

		if(reader.is_open()) 
		{
			int currentState = NoState;
			int pos = 0;

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
					if(_findAppLocaleKey(line,pos))
						currentState = InAcceptedKey;
				}
				
				if(currentState == InAcceptedKey) {
					if(_findSemicolon(line,pos))
						currentState = InAcceptedSemicolon;
				}

				if(currentState == InAcceptedSemicolon) {
					if(_findLanguageString(line,pos,langcode))						
						break;
				}

				pos = wstring::npos;
			}
		}
	}

	return langcode.compare(LANGUAGECODE) == 0;
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
			reader.close();
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
		}

		if(reader.is_open())
			reader.close();

		if(writer.is_open())
			writer.close();
		
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
	bool bNeed;

	switch(GetStatus())
	{
		case NotInstalled:
		case AlreadyApplied:
		case CannotBeApplied:
			bNeed = false;
			break;
		default:
			bNeed = true;
			break;
	}
	g_log.Log(L"ChromeAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
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
	if (m_registry->OpenKey(HKEY_CURRENT_USER, CHROME_REGISTRY_PATH, false))
	{
		wchar_t szVersion[1024];

		if (m_registry->GetString(L"Version", szVersion, sizeof(szVersion)))
		{
			m_version = szVersion;
			g_log.Log(L"ChromeAction::_readVersion. Chrome version %s", szVersion);
		}
		m_registry->Close();
	}	
}

bool ChromeAction::_isInstalled()
{
	if (m_isInstalled.IsUndefined())
	{
		wstring path;

		_readInstallLocation(path);
		m_isInstalled = path.size() > 0;
	}

	return m_isInstalled == true;
}

void ChromeAction::_readInstallLocation(wstring & path)
{
	path.erase();

	if (m_registry->OpenKey(HKEY_CURRENT_USER, CHROME_REGISTRY_PATH, false))
	{
		wchar_t szLocation[MAX_PATH];
		
		if (m_registry->GetString(L"InstallLocation", szLocation, sizeof(szLocation)))
		{
			path = szLocation;
			g_log.Log(L"ChromeAction::_readInstallLocation. InstallLocation %s", szLocation);		
		}		
		m_registry->Close();
	} 
	else 
	{
		g_log.Log(L"ChromeAction::_readInstallLocation - Chrome is not installed");
	}
}

const wchar_t* ChromeAction::GetVersion()
{
	if (m_version.length() == 0)
	{
		_readVersion();
	}
	return m_version.c_str();
}

void ChromeAction::CreateJSONString(wstring &jsonvalue)
{
	int languages = m_languages.size();	
	
	if (languages == 1)
	{
		jsonvalue = m_languages.at(0);
		return;
	}

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

void ChromeAction::CheckPrerequirements(Action * action)
{	
	bool langcodeFound, localeOk = false;
	wstring langcode, firstlang;
	
	if (_isInstalled())
	{
		_readVersion();
		langcodeFound = _readLanguageCode(langcode);
		localeOk = _isChromeAppLocaleOk();
		
		if(langcodeFound) {
			ParseLanguage(langcode);
			_getFirstLanguage(firstlang);
			
			if(firstlang.compare(L"ca") == 0) {
				SetStatus(AlreadyApplied);
			}
		} else {
			if (localeOk) {
				SetStatus(AlreadyApplied);
			}
		}
	} else {
		_setStatusNotInstalled();
	}
}
