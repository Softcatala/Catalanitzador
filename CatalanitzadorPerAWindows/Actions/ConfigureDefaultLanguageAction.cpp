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

#include "stdafx.h"
#include "ConfigureDefaultLanguageAction.h"
#include "Resources.h"

ConfigureDefaultLanguageAction::ConfigureDefaultLanguageAction(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner)
{
	m_registry = registry;
	m_OSVersion = OSVersion;
	m_runner = runner;	
	szCfgFile[0] = NULL;
}

ConfigureDefaultLanguageAction::~ConfigureDefaultLanguageAction()
{
	if (szCfgFile[0] != NULL && GetFileAttributes(szCfgFile) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(szCfgFile);
	}	
}

wchar_t* ConfigureDefaultLanguageAction::GetName()
{
	return _getStringFromResourceIDName(IDS_DEFLANGACTION_NAME, szName);
}

wchar_t* ConfigureDefaultLanguageAction::GetDescription()
{	
	return _getStringFromResourceIDName(IDS_DEFLANGACTION_DESCRIPTION, szDescription);
}

bool ConfigureDefaultLanguageAction::_isCatalanKeyboardActive()
{
	wchar_t szValue[1024];
	bool bCatalanActive = false;
	
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Keyboard Layout\\Preload", false))
	{
		if (m_registry->GetString(L"1", szValue, sizeof (szValue)))
		{
			// 0403 locale code for CA-ES
			if (wcsstr(szValue, L"0403") != NULL)
				bCatalanActive = true;
		}
		m_registry->Close();
	}
	g_log.Log(L"ConfigureDefaultLanguageAction::_isCatalanKeyboardActive returns %u", (wchar_t *) bCatalanActive);
	return bCatalanActive;
}

bool ConfigureDefaultLanguageAction::IsNeed()
{	
	if (status == CannotBeApplied)
		return false;

	bool bNeed;

	bNeed = _isCatalanKeyboardActive() == false;

	if (bNeed == false)
		status = AlreadyApplied;

	g_log.Log(L"ConfigureDefaultLanguageAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;
}

void ConfigureDefaultLanguageAction::Execute()
{	
	wchar_t szConfigFileName[MAX_PATH];
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];
	LPCWSTR resource;	

	GetTempPath(MAX_PATH, szCfgFile);

	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\control.exe ");

	if (m_OSVersion->GetVersion() == WindowsXP)
	{
		//Documentation: http://support.microsoft.com/default.aspx?scid=kb;en-us;289125
		wcscpy_s(szConfigFileName, L"regopts.txt");
		resource = (LPCWSTR)IDR_CONFIG_DEFLANGUAGE_WINXP;
	}
	else // Windows Vista and 7
	{
		//Documentation: http://blogs.msdn.com/b/michkap/archive/2006/05/30/610505.aspx
		wcscpy_s(szConfigFileName, L"regopts.xml");
		resource = (LPCWSTR)IDR_CONFIG_DEFLANGUAGE_WINVISTA;
	}
	wcscat_s(szCfgFile, szConfigFileName);

	Resources::DumpResource(L"CONFIG_FILES", resource, szCfgFile);	
	swprintf_s(szParams, L" intl.cpl,,/f:\"%s\"", szCfgFile);

	SetStatus(InProgress);
	g_log.Log(L"ConfigureDefaultLanguageAction::Execute '%s' with params '%s'", szApp, szParams);
	m_runner->Execute(szApp, szParams);
}

bool ConfigureDefaultLanguageAction::_hasSpanishKeyboard()
{
	wchar_t szValue[1024];
	wchar_t szNum[16];
	bool bSpanishActive = false;
	
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Keyboard Layout\\Preload", false))
	{
		for (int i = 1; i < 100; i++)
		{
			swprintf_s(szNum, L"%u", i);
			if (m_registry->GetString(szNum, szValue, sizeof (szValue)) == false)
				break;			

			if (wcsstr(szValue, L"040a") != NULL ||wcsstr(szValue, L"0c0a") != NULL)
			{
				bSpanishActive = true;
				break;
			}
		}
			
		m_registry->Close();
	}
	g_log.Log(L"ConfigureDefaultLanguageAction::_hasSpanishKeyboard is '%u'", (wchar_t *)bSpanishActive);
	return bSpanishActive;	
}

void ConfigureDefaultLanguageAction::CheckPrerequirements(Action * action)
{
	if (_isCatalanKeyboardActive() == false && _hasSpanishKeyboard() == false)
	{
		_getStringFromResourceIDName(IDS_DEFLANGACTION_NOSPANISHKEYBOARD, szCannotBeApplied);
		g_log.Log(L"ConfigureDefaultLanguageAction::CheckPrerequirements. No Spanish keyboard installed.");
		SetStatus(CannotBeApplied);
		return;
	}
}

bool ConfigureDefaultLanguageAction::IsRebootNeed() const
{
	// Default language in Windows XP only changes after reboot
	if (m_OSVersion->GetVersion() == WindowsXP)
	{
		if (status == Successful)
			return true;
	}
	return false;
}

// In Windows XP, you cannot signal to make a keyboard the default
void ConfigureDefaultLanguageAction::_makeCatalanActiveKeyboard()
{
	wchar_t szValue[1024], szNum[16];

	g_log.Log(L"ConfigureDefaultLanguageAction::MakeCatalanActiveKeyboard needed");
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Keyboard Layout\\Preload", true))
	{
		int nCatIndex = 0;
		for (int i = 1; i < 100; i++)
		{
			swprintf_s(szNum, L"%u", i);
			if (m_registry->GetString(szNum, szValue, sizeof (szValue)) == false)
				break;		

			if (wcsstr(szValue, L"0403") != NULL)
			{
				nCatIndex = i;
				break;
			}
		}

		if (nCatIndex > 0)
		{
			wchar_t szFirstLang[1024];
			
			if (m_registry->GetString(L"1", szFirstLang, sizeof (szFirstLang)))
			{
				swprintf_s(szNum, L"%u", nCatIndex);
				if (m_registry->GetString(szNum, szValue, sizeof (szValue)))
				{
					m_registry->SetString(L"1", szValue);
					m_registry->SetString(szNum, szFirstLang);
					g_log.Log(L"ConfigureDefaultLanguageAction::MakeCatalanActiveKeyboard. Switching language positions");
				}
			}
		}
			
		m_registry->Close();
	}	
}

ActionStatus ConfigureDefaultLanguageAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (m_OSVersion->GetVersion() == WindowsXP)
		{
			if (_isCatalanKeyboardActive() == false)
			{
				_makeCatalanActiveKeyboard();
			}
		}

		if (_isCatalanKeyboardActive())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
		}
		
		g_log.Log(L"ConfigureDefaultLanguageAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}
