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

enum JSONChromeState { NoState, InIntl, InIntlSemicolon,
				InIntlBlock, InAcceptedKey, InAcceptedSemicolon, 
				InAcceptedValue, InAppLocaleKey, InAppLocaleSemicolon,
				InAppLocaleValue, EndParsing };

#ifndef CHROME_LANGUAGECODE
#define CHROME_LANGUAGECODE L"ca"
#endif

ChromeProfile::ChromeProfile()
{
	SetPath(L"");
}

ChromeProfile::ChromeProfile(wstring installLocation)
{
	SetPath(installLocation);
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

bool ChromeProfile::_findAcceptedKey(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"\"accept_languages\"",pos);

	return pos != wstring::npos;
}

bool ChromeProfile::_findAppLocaleKey(wstring line, int & pos)
{
	if(pos == wstring::npos) pos = 0;
	
	pos = line.find(L"\"app_locale\"",pos);

	return pos != wstring::npos;
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
	wstring langcode;
	
	if(m_installLocation.empty() == false)
	{
		wifstream reader;
		wstring line;
		wstring path = GetUIRelPathAndFile();
		path = m_installLocation + path;
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

	return langcode.compare(CHROME_LANGUAGECODE) == 0;
}

bool ChromeProfile::ReadAcceptLanguages(wstring& langcode)
{
	bool isLanguageFound = false;
	
	if(m_installLocation.empty() == false) 
	{
		wifstream reader;
		wstring line;
		wstring path = GetPreferencesRelPathAndFile();
		path = m_installLocation + path;
		reader.open(path.c_str());

		if(reader.is_open()) 
		{
			int currentState = NoState;			
			int pos = 0;

			while(!(getline(reader,line)).eof() && !isLanguageFound)
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
						isLanguageFound = true;
				}

				pos = wstring::npos;
			}
			reader.close();
		}
	}

	return isLanguageFound;
}

bool ChromeProfile::WriteUILocale()
{
	bool languageWrittenSuccessfully = false;

	if(m_installLocation.empty() == false)
	{
		wifstream reader;
		wofstream writer;
		wstring line;
		wstring pathr = GetUIRelPathAndFile();
		wstring pathw = GetUIRelPathAndFile() + L".new";
		pathr = m_installLocation + pathr;
		pathw = m_installLocation + pathw;
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
					if(_findAppLocaleKey(line,pos))
						currentState = InAppLocaleKey;
				}
				
				if(currentState == InAppLocaleKey) {
					if(_findSemicolon(line,pos))
						currentState = InAppLocaleSemicolon;
				}

				if(currentState == InAppLocaleSemicolon) {
					if(_findLanguageString(line,pos,oldLang)) {
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
		}

		if(reader.is_open())
			reader.close();

		if(writer.is_open())
			writer.close();
		
		if(languageWrittenSuccessfully) {
			languageWrittenSuccessfully = MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
		}
	}

	return languageWrittenSuccessfully;
}

bool ChromeProfile::_writeAcceptLanguageCode(wstring langcode)
{	
	bool languageWrittenSuccessfully = false;
	
	if(m_installLocation.empty() == false)
	{
		wifstream reader;
		wofstream writer;
		wstring line;
		wstring pathr = GetPreferencesRelPathAndFile();
		wstring pathw = GetPreferencesRelPathAndFile() + L".new";
		pathr = m_installLocation + pathr;
		pathw = m_installLocation + pathw;
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
		}

		if(reader.is_open())
			reader.close();

		if(writer.is_open())
			writer.close();
		
		if(languageWrittenSuccessfully) {
			languageWrittenSuccessfully = MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
		}
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
	bool acceptLanguagesFound, localeOk;
	wstring langcode, firstlang;

	if(_isInstalled()) 
	{
		acceptLanguagesFound = ReadAcceptLanguages(langcode);
		localeOk = IsUiLocaleOk();

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
			if(localeOk)
				return true;
		}
		
	}

	return false;
}

bool ChromeProfile::_isInstalled()
{
	return m_installLocation.size() > 0;
}