/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "SkypeAction.h"

SkypeAction::SkypeAction(IRegistry* registry, IFileVersionInfo* fileVersionInfo)
{	
	m_fileVersionInfo = fileVersionInfo;
	m_registry = registry;
	m_majorVersion = -1;
}

wchar_t* SkypeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_SKYPEACTION_NAME, szName);	
}

wchar_t* SkypeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_SKYPEACTION_DESCRIPTION, szDescription);
}

bool SkypeAction::IsNeed()
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
	g_log.Log(L"SkypeAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

void SkypeAction::Execute()
{
	SetStatus(InProgress);
	
	_setDefaultLanguage();

	if (_isDefaultLanguage())
		SetStatus(Successful);
	else
		SetStatus(FinishedWithError);

	g_log.Log(L"SkypeAction::Execute is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
}

void SkypeAction::_getProgramLocation(wstring& location)
{
	wchar_t szFile[1024];
	
	location.empty();
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Software\\Skype\\Phone", false))
	{
		if (m_registry->GetString(L"SkypePath", szFile, sizeof(szFile)))
		{
			location = szFile;
		}
		m_registry->Close();
	}	
	g_log.Log(L"SkypeAction::_getProgramLocation '%s'", (wchar_t *) location.c_str());	
}

const wchar_t* SkypeAction::GetVersion()
{
	if (m_version.length() == 0)
	{
		_readVersionInstalled();
	}
	return m_version.c_str();
}

void SkypeAction::_readVersionInstalled()
{
	if (m_version.size() > 0)
		return;

	wstring location;

	_getProgramLocation(location);

	if (location.size() > 0)
	{
		m_fileVersionInfo->SetFilename(location);
		m_version = m_fileVersionInfo->GetVersion();
		m_majorVersion = m_fileVersionInfo->GetMajorVersion();
	}
}

int SkypeAction::_getMajorVersion()
{
	if (m_version.length() == 0)
	{
		_readVersionInstalled();
	}
	return m_majorVersion;
}

#define PROGRAM_REGKEY L"Software\\Skype\\Phone\\UI\\General"
#define CATALAN_LANGCODE_REGISTRY L"ca"

bool SkypeAction::_isDefaultLanguage()
{	
	bool defaultLang = false;

	if (m_profile.CanReadDefaultProfile() == true)
	{
		defaultLang = m_profile.IsDefaultAccountLanguage();
		g_log.Log(L"SkypeAction::_isDefaultLanguage. Returns %u (account)", (wchar_t *) defaultLang);
		return defaultLang;
	}

	defaultLang = _isDefaultInstallLanguage();
	g_log.Log(L"SkypeAction::_isDefaultLanguage. Returns %u (install)", (wchar_t *) defaultLang);
	return defaultLang;
}

void SkypeAction::_setDefaultLanguage()
{
	if (m_profile.CanReadDefaultProfile() == true)
	{
		if (m_profile.IsDefaultAccountLanguage() == false)
		{
			m_profile.SetDefaultLanguage();
		}
	}

	_setDefaultInstallLanguage();	
}

bool SkypeAction::_isDefaultInstallLanguage()
{
	bool defaultLang = false;
	wchar_t szLang[1024];	

	if (m_registry->OpenKey(HKEY_CURRENT_USER, PROGRAM_REGKEY, false))
	{
		if (m_registry->GetString(L"Language", szLang, sizeof(szLang)))
		{
			g_log.Log(L"SkypeAction::_isDefaultInstallLanguage %s", (wchar_t *) szLang);
			defaultLang = _wcsnicmp(szLang, CATALAN_LANGCODE_REGISTRY, sizeof(CATALAN_LANGCODE_REGISTRY)) == 0;
		}
		m_registry->Close();
	}	

	g_log.Log(L"SkypeAction::_isDefaultInstallLanguage. Returns %u", (wchar_t *) (defaultLang == true));
	return defaultLang == true;
}

void SkypeAction::_setDefaultInstallLanguage()
{
	if (m_registry->OpenKey(HKEY_CURRENT_USER, PROGRAM_REGKEY, true))
	{
		m_registry->SetString(L"Language", CATALAN_LANGCODE_REGISTRY);
		m_registry->Close();
	}	
}

#define FIRST_MAJOR_VERSION_WITH_CATALAN 6

void SkypeAction::CheckPrerequirements(Action * action)
{
	wstring language;

	if (wcslen(GetVersion()) > 0)
	{
		if (_getMajorVersion() < FIRST_MAJOR_VERSION_WITH_CATALAN)
		{
			_getStringFromResourceIDName(IDS_NOTSUPPORTEDVERSION, szCannotBeApplied);
			g_log.Log(L"SkypeAction::CheckPrerequirements. Version not supported");
			SetStatus(CannotBeApplied);
			return;
		}
			
		if (_isDefaultLanguage())
		{
			SetStatus(AlreadyApplied);
			return;
		}
	}
	else
	{
		_setStatusNotInstalled();
		return;
	}
}
