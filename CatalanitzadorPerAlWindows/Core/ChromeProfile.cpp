/* 
 * Copyright (C) 2013 Xavier Ivars-Ribes <xavi.ivars@gmail.com>
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


#include "StdAfx.h"
#include "ChromeProfile.h"

enum JSONChromeState { NoState, SchemaFound, EndParsing };

#define CHROME_LANGUAGECODE L"ca"

ChromeProfile::ChromeProfile()
{

}

void ChromeProfile::SetPath(wstring newPath)
{
	m_installLocation = newPath;
}

bool ChromeProfile::_findIntl(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"\"intl\"",pos);

	return pos != wstring::npos;
}

bool ChromeProfile::_readSchema(wstring line, int& pos)
{
	int currentState = NoState;

	if (_findIntl(line,pos))
	{
		if (_findSemicolon(line,pos))
		{
			if(_findStartBlock(line,pos))
			{
				return true;
			}
		}
	}
	return false;
}


bool ChromeProfile::_findSemicolon(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L":",pos);

	return pos != wstring::npos;
}

bool ChromeProfile::_findStartBlock(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"{",pos);

	return pos != wstring::npos;
}

bool  ChromeProfile::_findProperty(wstring line, wstring key, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(key, pos);
	return pos != wstring::npos;
}

bool ChromeProfile::_readPropertyValue(wstring line, wstring key, int& pos, wstring& value)
{
	if (pos == wstring::npos) pos = 0;
	
	if (_findProperty(line, key, pos))
	{
		if(_findSemicolon(line,pos))
		{
			_findLanguageString(line, pos, value);
			return true;
		}
	}

	return false;
}

bool ChromeProfile::_findLanguageString(wstring line,int & pos,wstring & langcode) 
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

bool ChromeProfile::IsUiLocaleOk()
{	
	wifstream reader;
	wstring path = m_installLocation + GetUIRelPathAndFile();
	reader.open(path.c_str());

	if (reader.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::IsUiLocaleOk. Cannot open for reading %s", (wchar_t*) path.c_str());
		return false;
	}

	int currentState = NoState;
	int pos = 0;
	wstring line, langcode;	

	while(!(getline(reader,line)).eof())
	{
		if(currentState == NoState) {
			if(_readSchema(line,pos))
				currentState = SchemaFound;
		}

		if (currentState == SchemaFound) {
			if (_readPropertyValue(line, L"\"app_locale\"", pos, langcode)) {
				break;
			}
		}		

		pos = wstring::npos;
	}	

	return langcode.compare(CHROME_LANGUAGECODE) == 0;
}

bool ChromeProfile::ReadAcceptLanguages(wstring& langcode)
{
	bool isLanguageFound = false;
	wifstream reader;
	wstring path = m_installLocation + GetPreferencesRelPathAndFile();
	
	reader.open(path.c_str());

	if (reader.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::ReadAcceptLanguages. Cannot open for reading %s", (wchar_t*) path.c_str());
		return isLanguageFound;
	}

	int currentState = NoState;
	int pos = 0;
	wstring line;	

	while(!(getline(reader,line)).eof() && !isLanguageFound)
	{
		if(currentState == NoState) {
			if(_readSchema(line,pos))
				currentState = SchemaFound;
		}

		if(currentState == SchemaFound) {
			if (_readPropertyValue(line, L"\"accept_languages\"", pos, langcode)) {
				isLanguageFound = true;
			}
		}

		pos = wstring::npos;
	}

	reader.close();
	return isLanguageFound;
}

bool ChromeProfile::WriteUILocale()
{
	bool languageWrittenSuccessfully = false;
	wifstream reader;
	wofstream writer;	
	wstring pathr = m_installLocation + GetUIRelPathAndFile();
	wstring pathw = m_installLocation + GetUIRelPathAndFile() + L".new";

	reader.open(pathr.c_str());
	if (reader.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::WriteUILocale. Cannot open for reading %s", (wchar_t*) pathr.c_str());
		return languageWrittenSuccessfully;
	}

	writer.open(pathw.c_str());
	if (writer.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::WriteUILocale. Cannot open for writing %s", (wchar_t*) pathw.c_str());
		reader.close();
		return languageWrittenSuccessfully;
	}
	
	int currentState = NoState;	
	int pos = 0;
	wstring oldLang, line, lastLine;
	
	while (!(getline(reader,line)).eof())
	{
		if(currentState == NoState) {
			if(_readSchema(line,pos))
				currentState = SchemaFound;
		}

		if(currentState == SchemaFound) {
			if (_readPropertyValue(line, L"\"app_locale\"", pos, oldLang)) {				
				currentState = EndParsing;
				line.replace(pos,oldLang.length(),CHROME_LANGUAGECODE);
				languageWrittenSuccessfully = true;
			}		
		}
		
		pos = wstring::npos;
		writer << lastLine << L"\n";
		lastLine = line;
	}
	
	if(!languageWrittenSuccessfully) {
		writer << "\t,\"intl\":{\"app_locale\":\"ca\"}\n";
		languageWrittenSuccessfully = true;
	}

	writer << lastLine << L"\n";	
	reader.close();
	writer.close();
	
	if(languageWrittenSuccessfully) {
		languageWrittenSuccessfully = MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
	}

	return languageWrittenSuccessfully;
}

bool ChromeProfile::_writeAcceptLanguageCode(wstring langcode)
{	
	bool languageWrittenSuccessfully = false;	
	wifstream reader;
	wofstream writer;
	wstring pathr = m_installLocation + GetPreferencesRelPathAndFile();
	wstring pathw = m_installLocation + GetPreferencesRelPathAndFile() + L".new";
	
	reader.open(pathr.c_str());
	if (reader.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::_writeAcceptLanguageCode. Cannot open for reading %s", (wchar_t*) pathr.c_str());
		return languageWrittenSuccessfully;
	}

	writer.open(pathw.c_str());
	if (writer.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::_writeAcceptLanguageCode. Cannot open for writing %s", (wchar_t*) pathw.c_str());
		reader.close();
		return languageWrittenSuccessfully;
	}
	
	int currentState = NoState;
	
	int pos = 0;
	wstring oldLang, line, lastLine;		

	while(!(getline(reader,line)).eof())
	{
		if(currentState == NoState) {
			if(_readSchema(line,pos))
				currentState = SchemaFound;
		}

		if(currentState == SchemaFound) {
			if (_readPropertyValue(line, L"\"accept_languages\"", pos, oldLang)) {
				currentState = EndParsing;
				line.replace(pos,oldLang.length(),langcode);
				languageWrittenSuccessfully = true;
			}
		}

		pos = wstring::npos;
		writer << lastLine << L"\n";
		lastLine = line;
	}
	
	if(!languageWrittenSuccessfully) {
		writer << "\t,\"intl\":{\"accept_languages\":\"ca\"}\n";
		languageWrittenSuccessfully = true;
	}

	writer << lastLine << L"\n";
	reader.close();
	writer.close();
	
	if(languageWrittenSuccessfully) {
		languageWrittenSuccessfully = MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
	}

	return languageWrittenSuccessfully;
}

void ChromeProfile::_getFirstLanguage(wstring& jsonvalue)
{
	if(m_languages.size() > 0)
		jsonvalue = m_languages[0];

	return;
}

void ChromeProfile::AddCatalanToArrayAndRemoveOldIfExists()
{	
	wstring regvalue;
	vector <wstring>languages;
	vector<wstring>::iterator it;

	// Delete previous occurrences of Catalan locale that were not first
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

bool ChromeProfile::UpdateAcceptLanguages()
{
	wstring langCode;
		
	AddCatalanToArrayAndRemoveOldIfExists();
	CreateJSONString(langCode);
	return _writeAcceptLanguageCode(langCode);
}

void ChromeProfile::CreateJSONString(wstring &jsonvalue)
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

void ChromeProfile::ParseLanguage(wstring value)
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

bool ChromeProfile::IsAcceptLanguagesOk() 
{
	bool acceptLanguagesFound;
	wstring langcode, firstlang;

	acceptLanguagesFound = ReadAcceptLanguages(langcode);	

	if(acceptLanguagesFound)
	{
		ParseLanguage(langcode);
		_getFirstLanguage(firstlang);
		
		if(firstlang.compare(CHROME_LANGUAGECODE) == 0) 
		{
			return true;
		}
	}
	else
	{
		if(IsUiLocaleOk())
			return true;
	}	

	return false;
}
