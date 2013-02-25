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

#include "ChromeAction.h"
#include <string>
#include <fstream>

using namespace std;


enum JSONChromeState { NoState, InIntl, InIntlSemicolon,
				InIntlBlock, InAcceptedKey, InAcceptedSemicolon, 
				InAcceptedValue, InAppLocaleKey, InAppLocaleSemicolon,
				InAppLocaleValue, EndParsing };

ChromeAction::ChromeAction() : Action()
{

}

bool ChromeAction::IsApplicationRunning()
{
    NSDictionary *info;
    bool foundApp = false;
    OSErr err;
    ProcessSerialNumber psn = {0, kNoProcess};
    
    while (!foundApp)
    {
        err = GetNextProcess(&psn);
        
        if (!err)
        {
            info = (NSDictionary *)ProcessInformationCopyDictionary(&psn,   kProcessDictionaryIncludeAllInformationMask);
            foundApp = [@"com.google.Chrome" isEqual:[info objectForKey:(NSString *)kCFBundleIdentifierKey]];
        }
        else
        {
            break;
        }
    }
    
    NSLog(@"ChromeAction::IsApplicationRunning. Result %u", foundApp);
    return foundApp;
}

bool ChromeAction::_findIntl(string line, int & pos)
{
	if(pos == string::npos) pos = 0;
	
	pos = line.find("\"intl\"",pos);

	return pos != string::npos;
}

bool ChromeAction::_findSemicolon(string line, int & pos)
{
	if(pos == string::npos) pos = 0;
	
	pos = line.find(":",pos);

	return pos != string::npos;
}

bool ChromeAction::_findStartBlock(string line, int & pos)
{
	if(pos == string::npos) pos = 0;
	
	pos = line.find("{",pos);

	return pos != string::npos;
}

bool ChromeAction::_findAcceptedKey(string line, int & pos)
{
	if(pos == string::npos) pos = 0;
	
	pos = line.find("\"accept_languages\"",pos);

	return pos != string::npos;
}

bool ChromeAction::_findAppLocaleKey(string line, int & pos)
{
	if(pos == string::npos) pos = 0;
	
	pos = line.find("\"app_locale\"",pos);

	return pos != string::npos;
}

bool ChromeAction::_findLanguageString(string line,int & pos,string & langcode) 
{
	string tempLang;

	if(pos == string::npos) pos = 0;

	wchar_t prev = L'0';
	bool reading = false;
	bool found = false;

	for(unsigned int i = pos; i < line.length() && found == false; ++i) {
		if(line[i] == '"' && prev != '\\') {
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

#define LANGUAGECODE "ca"

bool ChromeAction::_isUILocaleOk()
{
	string path_t, langcode;
	_readInstallLocation(path_t);

	if(path_t.empty() == false)
	{
		fstream reader;
		string line;
		string path = "/Local State";
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

				pos = string::npos;
			}
		}
	}

	return langcode.compare(LANGUAGECODE) == 0;
}

bool ChromeAction::_readLanguageCode(string& langcode)
{
	bool ret = false;
	string path_t;
	_readInstallLocation(path_t);

	if(path_t.empty() == false) 
	{
		fstream reader;
		string line;
		string path = "/Default/Preferences";
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

				pos = string::npos;
			}
			reader.close();
		}
	}
    NSLog(@"ChromeAction::_readLanguageCode. Result %u, lang %s", ret, langcode.c_str());
	return ret;
}

bool ChromeAction::_writeUILocale(string langcode)
{
	bool ret = false;
	string path_t;
	_readInstallLocation(path_t);

	if(path_t.empty() == false)
	{
		fstream reader;
		fstream writer;
		string line;
		string pathr = "/Local State";
		string pathw = "/Local State.new";
		pathr = path_t + pathr;
		pathw = path_t + pathw;
		reader.open(pathr.c_str());
		writer.open(pathw.c_str());
		
		if(reader.is_open() && writer.is_open()) 
		{
			int currentState = NoState;
			
			int pos = 0;
			string oldLang;
			string lastLine = "";

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
						line.replace(pos,oldLang.length(),LANGUAGECODE);
						ret = true;
					}
				}

				pos = string::npos;

				writer << lastLine << L"\n";
				lastLine = line;
			}
			
			if(ret == false) {
				writer << "\t,\"intl\":{\"app_locale\":\"ca\"}\n";
				ret = true;

			}
			writer << lastLine << L"\n";
		}

		if(reader.is_open())
			reader.close();

		if(writer.is_open())
			writer.close();
		
		if(ret) {
			//ret = MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
            rename(pathw.c_str(),pathr.c_str());
		}
	}

	if(ret) {
		uiStatus = Successful;

	} else {
		uiStatus = FinishedWithError;
	}

	return ret;
}

bool ChromeAction::_writeAcceptLanguageCode(string langcode)
{	
	bool ret = false;
	string path_t;
	_readInstallLocation(path_t);

	if(path_t.empty() == false)
	{
		fstream reader;
		ofstream writer;
		string line;
		string pathr = "/Default/Preferences";
		string pathw = "/Default/Preferences.new";
		pathr = path_t + pathr;
		pathw = path_t + pathw;
		reader.open(pathr.c_str());
		writer.open(pathw.c_str());
		
		if(reader.is_open() && writer.is_open()) 
		{
			int currentState = NoState;
			
			int pos = 0;
			string oldLang;
			string lastLine = "";

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

				pos = string::npos;

				writer << lastLine << "\n";
				lastLine = line;
			}
			
			if(ret == false) {
				writer << "\t,\"intl\":{\"accept_languages\":\"ca\"}\n";
				ret = true;

			}
			writer << lastLine << "\n";
		}

		if(reader.is_open())
			reader.close();

		if(writer.is_open())
			writer.close();
		
		if(ret) {
            rename(pathw.c_str(),pathr.c_str());
			//ret = MoveFileEx(pathw.c_str(),pathr.c_str(),MOVEFILE_REPLACE_EXISTING) != 0;
		}
	}

	if(ret) {
		status = Successful;

	} else {
		status = FinishedWithError;
	}

	return ret;
}

void ChromeAction::_getFirstLanguage(string& jsonvalue)
{
	if(m_languages.size() > 0)
		jsonvalue = m_languages[0];

	return;
}

bool ChromeAction::_isInstalled()
{
    string path_t;
    fstream reader;
    
    _readInstallLocation(path_t);
    string path = "/Default/Preferences";
    path = path_t + path;

    reader.open(path.c_str());
    
    if(reader.is_open())
    {
        reader.close();
        return true;
    }
    return false;
}

bool ChromeAction::IsNeed()
{
    bool isInstalled;
	string langcode, firstlang;
    isInstalled = _isInstalled();
    bool isNeed = false;
    
	if (isInstalled)
	{
        _readLanguageCode(langcode);
		//localeOk = _isUILocaleOk();
        ParseLanguage(langcode);
        _getFirstLanguage(firstlang);
			
        isNeed = firstlang.compare("ca") != 0;
	}

    NSLog(@"ChromeAction::IsNeed. %u", isNeed);
    return isNeed;
}

void ChromeAction::AddCatalanToArrayAndRemoveOldIfExists()
{	
	string regvalue;
	vector <string>languages;
	vector<string>::iterator it;

	// Delete previous ocurrences of Catalan locale that were not first
	for (it = m_languages.begin(); it != m_languages.end(); ++it)
	{
		string element = *it;
		std::transform(element.begin(), element.end(), element.begin(), ::tolower);

		if (element.compare("ca") == 0)
		{
			m_languages.erase(it);
			break;
		}
	}

	string str("ca");
	it = m_languages.begin();
	m_languages.insert(it, str);
}

void ChromeAction::Execute()
{
	string langCode;

	AddCatalanToArrayAndRemoveOldIfExists();
	CreateJSONString(langCode);
	_writeAcceptLanguageCode(langCode);
}

void ChromeAction::_readInstallLocation(string & path)
{
    NSString* homeDir = NSHomeDirectory();
    
    path = [homeDir UTF8String];
    path += "/Library/Application Support/Google/Chrome";
}

void ChromeAction::CreateJSONString(string &jsonvalue)
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
		jsonvalue += "," + m_languages.at(i);
	}
}

void ChromeAction::ParseLanguage(string value)
{
	string language;

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
