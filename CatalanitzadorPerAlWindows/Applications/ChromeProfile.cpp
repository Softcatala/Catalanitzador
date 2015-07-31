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
#define CHROME_SPELLCHECKER_LANGUAGECODE L"ca"

#define INTL_SCHEMA_NAME L"\"intl\""

ChromeProfile::ChromeProfile()
{
	m_prefCacheIsValid = false;
	m_setCatalanAsAcceptLanguage = false;
	m_setCatalanAsSpellLanguage = false;
}

void ChromeProfile::SetPath(wstring newPath)
{
	m_installLocation = newPath;
}

bool ChromeProfile::_readSchema(wstring key, wstring line, int& pos)
{
	if (pos == wstring::npos) pos = 0;

	pos = line.find(key, pos);

	if ( pos != wstring::npos)
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
	bool rslt;

	while (getline(reader,line))
	{
		if(currentState == NoState) {
			if(_readSchema(INTL_SCHEMA_NAME, line,pos))
				currentState = SchemaFound;
		}

		if (currentState == SchemaFound) {
			if (_readPropertyValue(line, L"\"app_locale\"", pos, langcode)) {
				break;
			}
		}		

		pos = wstring::npos;
	}	

	rslt = langcode.compare(CHROME_LANGUAGECODE) == 0;
	g_log.Log(L"ChromeProfile::IsUiLocaleOk: %u", (wchar_t*) rslt);
	return rslt;
}

bool ChromeProfile::ReadAcceptLanguages(wstring& langcode)
{	
	_readAcceptAndSpellLanguagesFromPreferences();	
	langcode = m_prefCacheAcceptLanguage;
	return m_prefCacheAcceptLanguage.empty() == false;
}

void ChromeProfile::_readAcceptAndSpellLanguagesFromPreferences()
{
	if (m_prefCacheIsValid == true)
		return;
	
	wifstream reader;
	wstring path = m_installLocation + GetPreferencesRelPathAndFile();
	
	m_prefCacheAcceptLanguage.erase();
	m_prefCacheSpellLanguage.erase();
	m_prefCacheIsValid = true;

	reader.open(path.c_str());

	if (reader.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::ReadAcceptLanguages. Cannot open for reading %s", (wchar_t*) path.c_str());
		return;
	}
	
	int pos = 0;
	wstring line;
	bool acceptLanguagesRead = false;
	bool spellLanguageRead = false;

	while(getline(reader,line))
	{
		// We are try to read two properties from differents schemas
		// Since we do not have a real parser. We do not know when a schema starts or ends
		if (acceptLanguagesRead == false && _readPropertyValue(line, L"\"accept_languages\"", pos, m_prefCacheAcceptLanguage))
			acceptLanguagesRead = true;

		if (spellLanguageRead == false && _readPropertyValue(line, L"\"dictionary\"", pos, m_prefCacheSpellLanguage))
			spellLanguageRead = true;

		pos = wstring::npos;
	}

	reader.close();
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
	
	while (getline(reader,line))
	{
		if(currentState == NoState) {
			if(_readSchema(INTL_SCHEMA_NAME, line,pos))
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

bool ChromeProfile::WriteSpellAndAcceptLanguages()
{	
	wifstream reader;
	wofstream writer;
	wstring pathr = m_installLocation + GetPreferencesRelPathAndFile();
	wstring pathw = m_installLocation + GetPreferencesRelPathAndFile() + L".new";
	
	reader.open(pathr.c_str());
	if (reader.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::WriteSpellAndAcceptLanguages. Cannot open for reading %s", (wchar_t*) pathr.c_str());
		return false;
	}

	writer.open(pathw.c_str());
	if (writer.is_open() == false)
	{
		g_log.Log(L"ChromeProfile::WriteAcceptLanguageCode. Cannot open for writing %s", (wchar_t*) pathw.c_str());
		reader.close();
		return false;
	}

	m_prefCacheIsValid = false;

	int currentState = NoState, pos = 0;
	wstring oldLang, line, lastLine;
	bool acceptLanguagesDone = false;
	bool spellLanguageDone = false;
	
	while (getline(reader,line))
	{
		// We are try to read two properties from differents schemas
		// Since we do not have a real parser. We do not know when a schema starts or ends		
		if (acceptLanguagesDone == false && _readPropertyValue(line, L"\"accept_languages\"", pos, oldLang))
		{
			acceptLanguagesDone = true;

			if (m_setCatalanAsAcceptLanguage)
			{
				AcceptLanguagePropertyValue propertyValue(oldLang);
				wstring newLang = propertyValue.GetWithCatalanAdded();

				line.replace(pos,oldLang.length(), newLang);
			}
		}

		if (spellLanguageDone == false && _readPropertyValue(line, L"\"dictionary\"", pos, oldLang))
		{
			spellLanguageDone = true;

			if (m_setCatalanAsSpellLanguage)
			{
				line.replace(pos,oldLang.length(), CHROME_SPELLCHECKER_LANGUAGECODE);
			}
		}

		pos = wstring::npos;
		writer << lastLine << L"\n";
		lastLine = line;
	}
	
	if (m_setCatalanAsAcceptLanguage && !acceptLanguagesDone) {
		writer << "\t,\"intl\":{\"accept_languages\":\"ca\"}\n";
		acceptLanguagesDone = true;
	}

	if (m_setCatalanAsSpellLanguage && !spellLanguageDone) {
		writer << "\t,\"spellcheck\":{\"dictionary\":\"ca\"}\n";
		spellLanguageDone = true;
	}

	writer << lastLine << L"\n";
	reader.close();
	writer.close();
	
	if (acceptLanguagesDone|| spellLanguageDone) {
		return MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
	}

	return true;
}

void ChromeProfile::SetCatalanAsAcceptLanguages()
{
	m_setCatalanAsAcceptLanguage = true;
}

bool ChromeProfile::IsAcceptLanguagesOk() 
{
	bool acceptLanguagesFound, bRslt;
	wstring langcode, firstlang;

	bRslt = false;
	acceptLanguagesFound = ReadAcceptLanguages(langcode);	

	if(acceptLanguagesFound)
	{
		AcceptLanguagePropertyValue propertyValue(langcode);
		firstlang = propertyValue.GetFirstLanguage();
		
		if(firstlang.compare(CHROME_LANGUAGECODE) == 0) 
		{
			bRslt = true;
		}
	}
	else
	{
		if(IsUiLocaleOk())
			bRslt = true;
	}	

	g_log.Log(L"ChromeProfile::IsAcceptLanguagesOk: %u", (wchar_t*) bRslt);
	return bRslt;
}

bool ChromeProfile::IsSpellCheckerLanguageOk() 
{
	bool bRslt = false;

	_readAcceptAndSpellLanguagesFromPreferences();

	if (m_prefCacheSpellLanguage.empty() == false)
	{
		if (m_prefCacheSpellLanguage.compare(CHROME_SPELLCHECKER_LANGUAGECODE) == 0)
		{
			bRslt = true;
		}
	}
	else
	{
		if(IsUiLocaleOk())
			bRslt = true;
	}

	g_log.Log(L"ChromeProfile::IsSpellCheckerLanguageOk: %u", (wchar_t*) bRslt);
	return bRslt;
}

void ChromeProfile::SetCatalanAsSpellCheckerLanguage()
{
	m_setCatalanAsSpellLanguage = true;	
}

//
// AcceptLanguagePropertyValue
// 

AcceptLanguagePropertyValue::AcceptLanguagePropertyValue(wstring _value)
{
	value = _value;
}

wstring AcceptLanguagePropertyValue::GetWithCatalanAdded()
{
	vector<wstring> languages;
	
	languages = _getLanguagesFromAcceptString(value);
	_addCatalanToArrayAndRemoveOldIfExists(languages);
	return _createJSONString(languages);
}

wstring AcceptLanguagePropertyValue::_createJSONString(vector<wstring> languages)
{	
	if (languages.size() == 1)
	{
		return languages.at(0);
	}

	wstring jsonvalue;
	jsonvalue = languages.at(0);
		
	for (unsigned int i = 1; i < languages.size(); i++)
	{
		jsonvalue += L"," + languages.at(i);
	}
	return jsonvalue;
}

vector<wstring> AcceptLanguagePropertyValue::_getLanguagesFromAcceptString(wstring value)
{
	wstring language;
	vector<wstring> languages;	
	
	for (unsigned int i = 0; i < value.size (); i++)
	{
		if (value[i] == L',')
		{
			languages.push_back(language);
			language.clear();
		} else {
			language += value[i];
		}
	}

	if (language.empty() == false)
	{
		languages.push_back(language);
	}
	return languages;
}

wstring AcceptLanguagePropertyValue::GetFirstLanguage()
{
	wstring jsonvalue;
	vector<wstring> languages;

	languages = _getLanguagesFromAcceptString(value);

	if(languages.size() > 0)
		jsonvalue = languages[0];	

	return jsonvalue;
}

void AcceptLanguagePropertyValue::_addCatalanToArrayAndRemoveOldIfExists(vector<wstring>& languages)
{	
	wstring regvalue;	
	vector<wstring>::iterator it;

	// Delete previous occurrences of Catalan locale that were not first
	for (it = languages.begin(); it != languages.end(); ++it)
	{
		wstring element = *it;
		std::transform(element.begin(), element.end(), element.begin(), ::tolower);

		if (element.compare(L"ca") == 0)
		{
			languages.erase(it);
			break;
		}
	}

	wstring str(L"ca");
	it = languages.begin();
	languages.insert(it, str);
}