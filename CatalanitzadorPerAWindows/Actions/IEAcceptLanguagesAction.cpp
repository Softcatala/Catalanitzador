/* 
 * Copyright (C) 2011-2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "IEAcceptLanguagesAction.h"
#include <math.h>

IEAcceptLanguagesAction::IEAcceptLanguagesAction(IRegistry* registry)
{
	m_registry = registry;
}

wchar_t* IEAcceptLanguagesAction::GetName()
{
	return _getStringFromResourceIDName(IDS_IEACCEPTEDLANGUAGESACTION_NAME, szName);
}

wchar_t* IEAcceptLanguagesAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_IEACCEPTEDLANGUAGESACTION_DESCRIPTION, szDescription);	
}

void IEAcceptLanguagesAction::_readLanguageCode(wstring& langcode)
{
	wchar_t szValue[1024];

	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\International", false))
	{
		if (m_registry->GetString(L"AcceptLanguage", szValue, sizeof (szValue)))
		{
			langcode = szValue;
		}
		m_registry->Close();
	}
}

bool IEAcceptLanguagesAction::_writeLanguageCode(wstring langcode)
{
	bool bOk = false;

	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\International", true))
	{
		if (m_registry->SetString (L"AcceptLanguage", (wchar_t *)langcode.c_str()) == true)
		{	
			bOk = true;
		}
		m_registry->Close();
	}
	g_log.Log(L"IEAcceptLanguagesAction::_writeLanguageCode, set AcceptLanguage %u", (wchar_t *) bOk);
	return bOk;
}

void IEAcceptLanguagesAction::_getFirstLanguage(wstring& regvalue)
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

bool IEAcceptLanguagesAction::_isCurrentLanguageOk(wstring& firstlang)
{
	wstring langcode;

	_readLanguageCode(langcode);
	_parseLanguage(langcode);
	_getFirstLanguage(firstlang);

	// IE 6.0 uses two digit language codes, after IE 6 can also include country
	return firstlang.compare(L"ca-es") == 0 || firstlang.compare(L"ca") == 0;
}

bool IEAcceptLanguagesAction::IsNeed()
{
	bool bNeed;
	wstring firstlang;

	bNeed = _isCurrentLanguageOk(firstlang) == false;

	if (bNeed == false)
		status = AlreadyApplied;

	g_log.Log(L"IEAcceptLanguagesAction::IsNeed returns %u (first lang:%s)", (wchar_t *) bNeed, (wchar_t *) firstlang.c_str());
	return bNeed;
}

void IEAcceptLanguagesAction::_addCatalanToArrayAndRemoveOldIfExists()
{	
	wstring regvalue;
	vector <wstring>languages;
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

void IEAcceptLanguagesAction::Execute()
{
	wstring regvalue;

	_addCatalanToArrayAndRemoveOldIfExists();
	_createRegistryString(regvalue);
	_writeLanguageCode(regvalue);
	
	if (_isCurrentLanguageOk(regvalue) == true)
		status = Successful;
	else
		status = FinishedWithError;

	g_log.Log(L"IEAcceptLanguagesAction::Execute returns %s", status == Successful ? L"Successful" : L"FinishedWithError");
}

void IEAcceptLanguagesAction::_readVersion()
{
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer", false))
	{
		wchar_t szVersion[1024];

		if (m_registry->GetString(L"Version", szVersion, sizeof(szVersion)))
		{
			StringConversion::ToMultiByte(wstring(szVersion), m_version);
			g_log.Log(L"IEAcceptLanguagesAction::_readVersion. IE version %s", szVersion);
		}
		m_registry->Close();
	}	
}

const char* IEAcceptLanguagesAction::GetVersion()
{
	if (m_version.length() == 0)
	{
		_readVersion();
	}
	return m_version.c_str();
}

void IEAcceptLanguagesAction::CheckPrerequirements(Action * action)
{
	_readVersion();
}

void IEAcceptLanguagesAction::_createRegistryStringTwoLangs(wstring &regvalue, float average)
{
	wchar_t szFormat[128];
	float value;
	
	value = floor(100 - average) / 100;
	swprintf_s(szFormat, L"%s,%s;q=%1.1f",  m_languages.at(0).c_str(),  m_languages.at(1).c_str(), value);
	regvalue = szFormat;
}

void IEAcceptLanguagesAction::_createRegistryString(wstring &regvalue)
{
	wchar_t szFormat[128];
	int languages = m_languages.size();	
	float average, value, cal;

	if (languages == 1)
	{
		regvalue = m_languages.at(0);
		return;
	}

	average = 100 / (float) languages;
	regvalue.empty();
	_createRegistryStringTwoLangs(regvalue, average);
	
	// More than 2 languages	
	for (int i = 2; i < languages; i++)
	{
		cal = 100 - (average * i);
		value = floor(cal) / 100;
		swprintf_s(szFormat, L",%s;q=%1.1f",  m_languages.at(i).c_str(), value);
		regvalue += szFormat;		
	}
}

void IEAcceptLanguagesAction::_parseLanguage(wstring regvalue)
{
	wstring language;
	bool reading_quality = false;

	m_languages.clear();
	for (unsigned int i = 0; i < regvalue.size (); i++)
	{
		if (regvalue[i] == L';')
		{
			m_languages.push_back(language);
			language.clear();
			reading_quality = true;
			continue;
		}

		if (regvalue[i] == L',')
		{
			if (reading_quality == false)
			{
				m_languages.push_back(language);
				language.clear();
			}
			else
			{
				reading_quality = false;
			}
			continue;
		}

		if (reading_quality == false)
		{
			language += regvalue[i];
		}		
	}

	if (language.empty() == false)
	{
		m_languages.push_back(language);
	}
}
