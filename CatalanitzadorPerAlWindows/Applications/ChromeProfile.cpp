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
#include "StringConversion.h"
#include "json/json.h"

#define CHROME_ACCEPT_LANGUAGECODE L"ca"

#define CHROMEAPP_LANGUAGECODE L"ca"
#define CHROMEAPP_LANGUAGECODE_STR "ca"

#define CHROME_SPELLCHECKER_LANGUAGECODE L"ca"
#define CHROME_SPELLCHECKER_LANGUAGECODE_STR "ca"


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

bool ChromeProfile::IsUiLocaleOk()
{	
	Json::Value root;
	wstring path = m_installLocation + GetUIRelPathAndFile();
	std::ifstream in(path.c_str());
	Json::Reader reader;
	string langcode;
	bool rslt;

	rslt = reader.parse (in, root);
	if (rslt == false)
	{
		g_log.Log(L"ChromeProfile::IsUiLocaleOk. Cannot open for reading %s", (wchar_t*) path.c_str());
		return false;
	}

	langcode = root["intl"]["app_locale"].asString();
	rslt = langcode.compare(CHROMEAPP_LANGUAGECODE_STR) == 0;
	g_log.Log(L"ChromeProfile::IsUiLocaleOk: %u", (wchar_t*) rslt);
	in.close();
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
		
	wstring path = m_installLocation + GetPreferencesRelPathAndFile();
	Json::Value root;
	std::ifstream in(path.c_str());
	Json::Reader reader;
	string langcode;
	bool rslt;
	
	m_prefCacheAcceptLanguage.erase();
	m_prefCacheSpellLanguage.erase();
	m_prefCacheIsValid = true;
	
	rslt = reader.parse(in, root);
	if (rslt == false)
	{
		g_log.Log(L"ChromeProfile::ReadAcceptLanguages. Cannot open for reading %s", (wchar_t*) path.c_str());
		return;
	}

	string acceptLanguages = root["intl"]["accept_languages"].asString();
	StringConversion::ToWideChar (acceptLanguages, m_prefCacheAcceptLanguage);

	string spellLanguage = root["spellcheck"]["dictionary"].asString();
	StringConversion::ToWideChar (spellLanguage, m_prefCacheSpellLanguage);
	in.close();
}

bool ChromeProfile::WriteUILocale()
{
	Json::Value root;
	wstring path = m_installLocation + GetUIRelPathAndFile();
	std::ifstream in(path.c_str());
	Json::Reader reader;
	Json::FastWriter writer;
	string langcode;
	bool rslt;

	rslt = reader.parse(in, root);
	if (rslt == false)
	{
		g_log.Log(L"ChromeProfile::WriteUILocale. Cannot open for reading %s", (wchar_t*) path.c_str());
		return false;
	}
	in.close();

	root["intl"]["app_locale"] = CHROMEAPP_LANGUAGECODE_STR;

	std::ofstream out(path.c_str());

	if (out.fail() == true)
	{
		g_log.Log(L"ChromeProfile::WriteUILocale. Cannot open for writing %s", (wchar_t*) path.c_str());
		return false;
	}

	std::string jsonMessage  = writer.write(root);
	out << jsonMessage;
	out.close();
	return true;	
}

bool ChromeProfile::WriteSpellAndAcceptLanguages()
{
	Json::Value root;
	wstring path = m_installLocation + GetPreferencesRelPathAndFile();
	std::ifstream in(path.c_str());
	Json::Reader reader;
	Json::FastWriter writer;
	string acceptLanguages;
	wstring wLang;
	bool rslt;

	rslt = reader.parse(in, root);
	if (rslt == false)
	{
		g_log.Log(L"ChromeProfile::WriteSpellAndAcceptLanguages. Cannot open for reading %s", (wchar_t*) path.c_str());
		return false;
	}
	in.close();

	if (m_setCatalanAsAcceptLanguage)
	{	
		acceptLanguages = root["intl"]["accept_languages"].asString();
		StringConversion::ToWideChar(acceptLanguages, wLang);

		AcceptLanguagePropertyValue propertyValue(wLang);
		wLang = propertyValue.GetWithCatalanAdded();
		StringConversion::ToMultiByte(wLang, acceptLanguages);
		root["intl"]["accept_languages"] = acceptLanguages;
	}

	if (m_setCatalanAsSpellLanguage)
	{
		root["spellcheck"]["dictionary"] = CHROME_SPELLCHECKER_LANGUAGECODE_STR;
	}

	std::ofstream out(path.c_str());
	if (out.fail() == true)
	{
		g_log.Log(L"ChromeProfile::WriteAcceptLanguageCode. Cannot open for writing %s", (wchar_t*) path.c_str());
		return false;
	}

	m_prefCacheIsValid = false;
	std::string jsonMessage = writer.write(root);
	out << jsonMessage;
	out.close();
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

	if (acceptLanguagesFound)
	{
		AcceptLanguagePropertyValue propertyValue(langcode);
		firstlang = propertyValue.GetFirstLanguage();
		
		if (firstlang.compare(CHROME_ACCEPT_LANGUAGECODE) == 0)
		{
			bRslt = true;
		}
	}
	else
	{
		if (IsUiLocaleOk())
		{
			bRslt = true;
		}
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
		if (IsUiLocaleOk())
		{
			bRslt = true;
		}
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