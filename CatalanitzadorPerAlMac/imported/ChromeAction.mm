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
#include "json/json.h"

#define CHROME_ACCEPT_LANGUAGECODE_1 "ca"
#define CHROME_ACCEPT_LANGUAGECODE_2 "ca-es"

using namespace std;


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

#define LANGUAGECODE "ca"

bool ChromeAction::_readLanguageCode(string& langcode)
{
	bool ret = false;
	string path_t;
	_readInstallLocation(path_t);
	
	if(path_t.empty() == false)
	{
		string path = "/Default/Preferences";
		path = path_t + path;
		
		Json::Value root;
		std::ifstream in(path.c_str());
		Json::Reader reader;
		bool rslt;

		if (in.fail())
		{
			NSLog(@"ChromeAction::_readLanguageCode. Cannot open for reading %s", path.c_str());
			return false;
		}
		
		rslt = reader.parse(in, root);
		in.close();
		
		if (rslt == false)
		{
			NSLog(@"ChromeAction::_readLanguageCode. Cannot parse %s", path.c_str());
			return false;
		}
		
		langcode = root["intl"]["accept_languages"].asString();
		ret = langcode.empty() == false;
		
	}
	NSLog(@"ChromeAction::_readLanguageCode. Result %u, lang %s", ret, langcode.c_str());
	return ret;
}

bool ChromeAction::_writeAcceptLanguageCode(string langcode)
{
	Json::Value root;
	Json::Reader reader;
	Json::FastWriter writer;
	string acceptLanguages;
	wstring wLang;
	bool rslt;
	
	string path;
	_readInstallLocation(path);
	path += "/Default/Preferences";
	std::ifstream in(path.c_str());

	if (in.fail())
	{
		NSLog(@"ChromeAction::_writeAcceptLanguageCode. Cannot open for reading %s", path.c_str());
		return false;
	}
	
	rslt = reader.parse(in, root);
	in.close();
	
	if (rslt == false)
	{
		NSLog(@"ChromeAction::_writeAcceptLanguageCode. Cannot parse %s", path.c_str());
		return false;
	}
	
	root["intl"]["accept_languages"] = langcode;
	
	std::ofstream out(path.c_str());
	if (out.fail())
	{
		NSLog(@"ChromeAction::_writeAcceptLanguageCode. Cannot open for writing %s", path.c_str());
		return false;
	}
	
	std::string jsonMessage = writer.write(root);
	out << jsonMessage;
	out.close();
	return true;
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

const char* ChromeAction::GetVersion()
{
	if (m_version.empty())
	{
		NSBundle* bundle;
		
		bundle = [[NSBundle alloc] initWithPath: @"/Applications/Google Chrome.app/"];
		NSDictionary* infoDict = [bundle infoDictionary];
		NSString* version = [infoDict objectForKey:@"CFBundleShortVersionString"];
		
		if (version != NULL)
		{
			m_version = [version UTF8String];
		}
	}
	
	return m_version.c_str();
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
	
	NSLog(@"ChromeAction::IsNeed. %u", bNeed);
	return bNeed;
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
	bool isOk;
	
	AddCatalanToArrayAndRemoveOldIfExists();
	CreateJSONString(langCode);
	_writeAcceptLanguageCode(langCode);
	
	isOk = _isAcceptLanguagesOk();
	SetStatus(isOk ? Successful : FinishedWithError);
	NSLog(@"ChromeAction::Execute. Result %u", isOk);
}

void ChromeAction::_readInstallLocation(string & path)
{
	NSString* homeDir = NSHomeDirectory();
	
	path = [homeDir UTF8String];
	path += "/Library/Application Support/Google/Chrome";
}

void ChromeAction::CreateJSONString(string &jsonvalue)
{
	unsigned long languages = m_languages.size();
	
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

bool ChromeAction::_isAcceptLanguagesOk()
{
	string langcode, firstlang;
	
	_readLanguageCode(langcode);
	ParseLanguage(langcode);
	_getFirstLanguage(firstlang);

	std::transform(firstlang.begin(), firstlang.end(), firstlang.begin(), ::tolower);
	
	return (firstlang.compare(CHROME_ACCEPT_LANGUAGECODE_1) == 0
			|| firstlang.compare(CHROME_ACCEPT_LANGUAGECODE_2)  == 0);
}

void ChromeAction::CheckPrerequirements(Action * action)
{
	bool isInstalled;
	isInstalled = _isInstalled();
	
	if (isInstalled)
	{
		if (_isAcceptLanguagesOk())
		{
			SetStatus(AlreadyApplied);
		}
	}
	else
	{
		_setStatusNotInstalled();
	}
}
