/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "MSOffice.h"
#include "ConfigureLocaleAction.h"

#include <algorithm>

#define CATALAN_LCID L"1027" // 0x403
#define VALENCIAN_LCID L"2051" // 0x803


// TODO: http://stackoverflow.com/questions/6243487/initialisation-lists-in-constructors-trying-to-initialize-a-structure
RegKeyVersion RegKeys2003 = 
{
	L"11.0", 
	L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\LanguageResources\\ParentFallback",	
	true
};

RegKeyVersion RegKeys2007 = 
{
	L"12.0",
	L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

RegKeyVersion RegKeys2010 = 
{
	L"14.0",
	L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

RegKeyVersion RegKeys2013 =
{
	L"15.0",
	L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

MSOffice::MSOffice(IRegistry* registry, IRunner* runner)
{
	m_registry = registry;
	m_runner = runner;	
}

bool MSOffice::_isVersionInstalled(RegKeyVersion regkeys, bool b64bits)
{
	wchar_t szValue[1024];
	wchar_t szKey[1024];
	bool Installed = false;

	swprintf_s(szKey, L"SOFTWARE\\Microsoft\\Office\\%s\\Common\\InstallRoot", regkeys.VersionNumber);

	if (b64bits ? m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, szKey, false) :
		m_registry->OpenKey(HKEY_LOCAL_MACHINE, szKey, false))
	{	
		if (m_registry->GetString(L"Path", szValue, sizeof (szValue)))
		{
			if (wcslen(szValue) > 0)
				Installed = true;
		}
		m_registry->Close();
	}
	return Installed;
}

MSOfficeVersion MSOffice::_readVersionInstalled()
{
	MSOfficeVersion version;

	if (_isVersionInstalled(RegKeys2013, false))
	{
		version = MSOffice2013;
	} else if (_isVersionInstalled(RegKeys2010, false))
	{
		version = MSOffice2010;
	} else if (_isVersionInstalled(RegKeys2007, false))
	{
		version = MSOffice2007;
	} else if (_isVersionInstalled(RegKeys2003, false))
	{
		version = MSOffice2003;
	} else if (_isVersionInstalled(RegKeys2013, true))
	{
		version = MSOffice2013_64;
	} else if (_isVersionInstalled(RegKeys2010, true))
	{
		version = MSOffice2010_64;
	} else
	{
		version = NoMSOffice;
	}

	//g_log.Log(L"MSOfficeLPIAction::_readVersionInstalled '%s'", (wchar_t*) GetVersion());
	return version;
}

bool MSOffice::_isLangPackForVersionInstalled(MSOfficeVersion version, bool b64bits)
{	
	bool Installed = false;

	RegKeyVersion regkeys = _getRegKeys(version);

	if (b64bits ? m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, regkeys.InstalledLangMapKey, false) :
		m_registry->OpenKey(HKEY_LOCAL_MACHINE, regkeys.InstalledLangMapKey, false))	
	{		
		if (regkeys.InstalledLangMapKeyIsDWord)
		{
			DWORD dwValue;
			if (m_registry->GetDWORD(CATALAN_LCID, &dwValue) || m_registry->GetDWORD(VALENCIAN_LCID, &dwValue))
				Installed = true;
		}		
		else
		{
			wchar_t szValue[1024];
			if (m_registry->GetString(CATALAN_LCID, szValue, sizeof (szValue)))
			{
				if (wcslen (szValue) > 0)
					Installed = true;
			}

			if (m_registry->GetString(VALENCIAN_LCID, szValue, sizeof (szValue)))
			{
				if (wcslen (szValue) > 0)
					Installed = true;
			}
		}
		m_registry->Close();
	}

	g_log.Log(L"MSOfficeLPIAction::_isLangPackForVersionInstalled returns '%s', 64 bits %u, installed %u", regkeys.InstalledLangMapKey,
		(wchar_t*) b64bits, (wchar_t *)Installed);

	return Installed;
}


void MSOffice::_readIsLangPackInstalled(MSOfficeVersion version, TriBool& langPackInstalled, bool& langPackInstalled64bits)
{
	switch (version)
	{
	case MSOffice2013:
		langPackInstalled = _isLangPackForVersionInstalled(version, false);
		langPackInstalled64bits = false;
		break;
	case MSOffice2013_64:
		langPackInstalled = _isLangPackForVersionInstalled(version, true);
		langPackInstalled64bits = true;
		break;
	case MSOffice2010:
		langPackInstalled = _isLangPackForVersionInstalled(version, false);
		langPackInstalled64bits = false;
		break;
	case MSOffice2010_64:
		langPackInstalled = _isLangPackForVersionInstalled(version, true);
		langPackInstalled64bits = true;
		break;
	case MSOffice2007:
		langPackInstalled = _isLangPackForVersionInstalled(version, false);
		langPackInstalled64bits = false;
		break;
	case MSOffice2003:
		langPackInstalled = _isLangPackForVersionInstalled(version, false);
		langPackInstalled64bits = false;
		break;
	default:
		assert(false);
		break;
	}

	//g_log.Log(L"MSOfficeLPIAction::_readIsLangPackInstalled returns '%s', 64 bits %u", langPackInstalled.ToString(), 
	//	(wchar_t*) langPackInstalled64bits);
}

void MSOffice::_readDefaultLanguage(MSOfficeVersion officeVersion, bool& isCatalanSetAsDefaultLanguage, bool& followSystemUIOff)
{
	wchar_t szKeyName [1024];

	swprintf_s(szKeyName, L"Software\\Microsoft\\Office\\%s\\Common\\LanguageResources", _getRegKeys(officeVersion).VersionNumber);
	if (m_registry->OpenKey(HKEY_CURRENT_USER, szKeyName, false))
	{
		DWORD lcid;
		if (m_registry->GetDWORD(L"UILanguage", &lcid))
		{			
			if (lcid == _wtoi(VALENCIAN_LCID) || lcid == _wtoi(CATALAN_LCID))
				isCatalanSetAsDefaultLanguage = true;
		}

		if (officeVersion != MSOffice2003)
		{
			wstring value;
			wchar_t szValue[2048];
			if (m_registry->GetString(L"FollowSystemUI", szValue, sizeof(szValue)))
			{
				value = szValue;
				std::transform(value.begin(), value.end(), value.begin(), ::tolower);
				if (value.compare(L"off") == 0)
					followSystemUIOff = true;
			}
		}
		m_registry->Close();
	}
}

bool MSOffice::_isDefaultLanguage(MSOfficeVersion officeVersion)
{
	ConfigureLocaleAction configureLocaleAction((IOSVersion*) NULL, m_registry, (IRunner*)NULL);
	bool isDefaultLanguage = false;
	bool isCatalanSetAsDefaultLanguage = false;
	bool followSystemUIOff = false;

	_readDefaultLanguage(officeVersion, isCatalanSetAsDefaultLanguage, followSystemUIOff);

	if (followSystemUIOff)
	{
		if (isCatalanSetAsDefaultLanguage)
			isDefaultLanguage = true;
	}
	else
	{
		 if (configureLocaleAction.IsCatalanLocaleActive())
			isDefaultLanguage = true;
	}

	return isDefaultLanguage;
}


void MSOffice::_setDefaultLanguage(MSOfficeVersion officeVersion)
{
	BOOL bSetKey = FALSE;
	wchar_t szKeyName [1024];

	swprintf_s(szKeyName, L"Software\\Microsoft\\Office\\%s\\Common\\LanguageResources", _getRegKeys(officeVersion).VersionNumber);
	if (m_registry->OpenKey(HKEY_CURRENT_USER, szKeyName, true))
	{
		//TODO
		int lcid = _wtoi(CATALAN_LCID); //_wtoi(GetUseDialectalVariant() ? VALENCIAN_LCID : CATALAN_LCID);
		bSetKey = m_registry->SetDWORD(L"UILanguage", lcid);

		// This key setting tells Office do not use the same language that the Windows UI to determine the Office Language
		// and use the specified language instead
		if (officeVersion != MSOffice2003)
		{
			BOOL bSetFollowKey = m_registry->SetString(L"FollowSystemUI", L"Off");
			g_log.Log(L"MSOfficeLPIAction::_setDefaultLanguage, set FollowSystemUI %u", (wchar_t *) bSetFollowKey);	
		}		
		m_registry->Close();
	}
	g_log.Log(L"MSOfficeLPIAction::_setDefaultLanguage, set UILanguage %u", (wchar_t *) bSetKey);	
}

RegKeyVersion MSOffice::_getRegKeys(MSOfficeVersion officeVersion)
{
	switch (officeVersion)
	{
		case MSOffice2003:
			return RegKeys2003;
		case MSOffice2007:
			return RegKeys2007;
		case MSOffice2010:
		case MSOffice2010_64:
			return RegKeys2010;
		case MSOffice2013:
		case MSOffice2013_64:
		default:
			return RegKeys2013;
	}
}
