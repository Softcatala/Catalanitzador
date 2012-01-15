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
	szVersionAscii[0] = NULL;
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
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\International", true))
	{
		if (m_registry->SetString (L"AcceptLanguage", (wchar_t *)langcode.c_str()) == true)
		{	
			status = Successful;
		}
		m_registry->Close();
	}
	g_log.Log(L"IEAcceptLanguagesAction::_writeLanguageCode, set AcceptLanguage %u", (wchar_t *) (status == Successful));
	return status == Successful;
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

bool IEAcceptLanguagesAction::IsNeed()
{
	bool bNeed = true;
	wstring langcode, firstlang;

	_readLanguageCode(langcode);
	ParseLanguage(langcode);
	_getFirstLanguage(firstlang);	

	// IE 6.0 uses two digit language codes, after IE 6 can also include country
	bNeed = firstlang.compare(L"ca-es") != 0 && firstlang.compare(L"ca") != 0;

	if (bNeed == false)
		status = AlreadyApplied;

	g_log.Log(L"IEAcceptLanguagesAction::IsNeed returns %u (first lang:%s)", (wchar_t *) bNeed, (wchar_t *) firstlang.c_str());
	return bNeed;
}

void IEAcceptLanguagesAction::AddCatalanToArrayAndRemoveOldIfExists()
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

	AddCatalanToArrayAndRemoveOldIfExists();
	CreateRegistryString(regvalue);
	_writeLanguageCode(regvalue);
}

void IEAcceptLanguagesAction::_readVersion()
{
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer", false))
	{
		wchar_t szVersion[1024];

		if (m_registry->GetString(L"Version", szVersion, sizeof(szVersion)))
		{
			WideCharToMultiByte(CP_ACP, 0, szVersion, wcslen(szVersion) + 1, szVersionAscii, sizeof(szVersionAscii), 
				NULL, NULL);

			g_log.Log(L"IEAcceptLanguagesAction::_readVersion. IE version %s", szVersion);
		}		
	}	
}

char* IEAcceptLanguagesAction::GetVersion()
{
	if (*szVersionAscii == 0x0)
	{
		_readVersion();
	}
	return szVersionAscii;
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

void IEAcceptLanguagesAction::CreateRegistryString(wstring &regvalue)
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

void IEAcceptLanguagesAction::ParseLanguage(wstring regvalue)
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
