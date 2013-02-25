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

//#include "stdafx.h"
#include <stdio.h>

#include "FirefoxAcceptLanguages.h"
#include <cstdio>
#include <fstream>  

FirefoxAcceptLanguages::FirefoxAcceptLanguages(string profileRootDir, string locale)
{	
	m_profileRootDir = profileRootDir;
	m_locale = locale;
	m_CachedLanguageCode = false;
}

void FirefoxAcceptLanguages::_getFirstLanguage(string& regvalue)
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


bool FirefoxAcceptLanguages::_getProfileLocationFromProfilesIni(string file, string &profileLocation)
{
    string PATHKEY = "Path=";
	fstream reader;
	string line;
	unsigned long pos = 0;

	reader.open(file.c_str());

	if (!reader.is_open())	
		return false;

	while(!(getline(reader, line)).eof())
	{
        pos = line.find(PATHKEY);
		if (pos == string::npos)
            continue;
        
		profileLocation = _getProfileRootDir() + line.substr(pos + PATHKEY.size());
		return true;
	}

	return false;
}

void FirefoxAcceptLanguages::_getProfilesIniLocation(string &location)
{	
	location = _getProfileRootDir();
	location += "profiles.ini";
}

bool FirefoxAcceptLanguages::_getPreferencesFile(string &location)
{
	string profileIni;

	_getProfilesIniLocation(profileIni);
	
	if (_getProfileLocationFromProfilesIni(profileIni, location))
	{
		location += "/prefs.js";
        
        //int idx = location.find("\\");
        //location.replace(idx, 1, "/");
		return true;
	}
	else
	{
		return false;
	}
}


bool FirefoxAcceptLanguages::IsNeed()
{
    bool bNeed = false;
    
	if (ReadLanguageCode())
	{
		if (_getLanguages()->size() == 0)
		{
			bNeed = m_locale != "ca";
		}
		else
		{
            string LOCALES_PREFIX("ca-");
            string firstlang;
            
            _getFirstLanguage(firstlang);
            
           bNeed = firstlang.compare("ca") != 0 && firstlang.compare(0, LOCALES_PREFIX.size(), LOCALES_PREFIX) != 0;
		}
	}
    NSLog(@"FirefoxAcceptLanguages::IsNeed. Needed %u", bNeed);
	return bNeed;
}


void FirefoxAcceptLanguages::_parseLanguage(string regvalue)
{
	string language;
	
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

string USER_PREF("user_pref(\"intl.accept_languages\", \"");

bool FirefoxAcceptLanguages::ReadLanguageCode()
{
	string location, line, langcode;
	fstream reader;

	if (m_CachedLanguageCode == true)
		return true;	
	
	if (_getPreferencesFile(location) == false)
	{
		NSLog(@"FirefoxAcceptLanguages::_readLanguageCode. No preferences file found. Firefox is not installed");
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

			start+=USER_PREF.size();

			end = line.find("\"", start);

			if (end == wstring::npos)
				continue;

			langcode = line.substr(start, end - start);
			_parseLanguage(langcode);
			break;
		}
	}
	else
	{
		//g_log.Log(L"FirefoxAcceptLanguages::_readLanguageCode cannot open %s", (wchar_t *) location.c_str());
		return false;
	}

	reader.close();
	NSLog(@"FirefoxAcceptLanguages::_readLanguageCode open %s", location.c_str());
	m_CachedLanguageCode = true;
	return true;
}


void FirefoxAcceptLanguages::_addCatalanToArrayAndRemoveOldIfExists()
{
	vector<string>::iterator it;

	// Delete previous ocurrences of Catalan locale that were not first
	for (it = m_languages.begin(); it != m_languages.end(); ++it)
	{
		string element = *it;
		std::transform(element.begin(), element.end(), element.begin(), ::tolower);

		if (element.compare("ca-es") == 0 || element.compare("ca") == 0)
		{
			m_languages.erase(it);
			break;
		}
	}

	string str("ca");
	it = m_languages.begin();
	m_languages.insert(it, str);
}

void FirefoxAcceptLanguages::_createPrefsString(string& string)
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
		string += "," + m_languages.at(i);
	}
}

void FirefoxAcceptLanguages::_getPrefLine(string langcode, string& line)
{
	line = USER_PREF;
	line += langcode;
	line += "\");";
}

bool FirefoxAcceptLanguages::_writeLanguageCode(string &langcode)
{
	fstream reader;
	ofstream writer;
	string line, filer, filew;
	bool written;
	
	_getPreferencesFile(filer);
	filew += filer;
	filew += ".new";

	reader.open(filer.c_str());

	if (!reader.is_open())
		return false;

	writer.open(filew.c_str());

	if (!writer.is_open())
	{
		reader.close();
		return false;
	}

	written = false;
	while(!(getline(reader,line)).eof())
	{
		if (line.find(USER_PREF) != wstring::npos)
		{
			written = true;
			_getPrefLine(langcode, line);
		}

		writer << line << "\n";
	}

	if (written == false)
	{
		_getPrefLine(langcode, line);
		writer << line << "\n";
	}

	writer.close();
	reader.close();

    return rename(filew.c_str(), filer.c_str()) == 0;
   //return MoveFileEx(filew.c_str(), filer.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
}

// The Firefox locale determines which languages are going to be used in the 
// accept_languages. Since by setting to Catalan we reset the default value
// add at the end of the locale of the langpack as secondary language, then
// if you install Firefox in English you will have ca, es as language codes
void FirefoxAcceptLanguages::_addFireForLocale()
{
	if (m_locale == "ca")
		return;

	if (m_languages.size() == 0)
	{			
		if (m_locale.size() > 0)
		{
            // Since we only now that the locale is not CA catalan
            // Prevent adding 'xx' to the locale list
			//m_languages.push_back(m_locale);
		}
	}
}

bool FirefoxAcceptLanguages::Execute()
{
	string value;

	ReadLanguageCode();
	_addFireForLocale();
	_addCatalanToArrayAndRemoveOldIfExists();
	_createPrefsString(value);
	return _writeLanguageCode(value);
}
 

