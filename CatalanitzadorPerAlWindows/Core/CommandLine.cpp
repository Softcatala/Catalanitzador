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
#include "CommandLine.h"
#include "CatalanitzadorUpdateAction.h"
#include "ConfigurationInstance.h"


#define COMMAND_DELIMITER L' '
#define VERSION_PARAMETER L"/version"
#define NORUNNING_PARAMETER L"/NoRunningCheck"
#define NOCONFIGURATIONDOWNLOAD_PARAMETER L"/NoConfigurationDownload"
#define CONFIGURATIONDOWNLOADURL_PARAMETER L"/ConfigurationDownloadUrl"
#define HELP_PARAMETER L"/Help"
#define SILENT_PARAMETER L"/Silent"

#define HELP_TEXT L"Sintaxis d'ús:\n\n\
/version:X.Y.Z - Fa creure al Catalanitzador que és la versió indicada \n\
/NoRunningCheck- No comprovis si ja s'està executant \n\
/NoConfigurationDownload - No baixis la configuració del servidor \n\
/ConfigurationDownloadUrl:url - Usa aquesta URL per baixar la configuració \n\
/Silent - Instal·lació desatesa\n"

CommandLine::CommandLine(Actions* pActions)
{
	m_pActions = pActions;

	m_bRunningCheck = true;
	m_bSilent = false;
	NORUNNING_PARAMETER_LEN = wcslen(NORUNNING_PARAMETER);
	VERSION_PARAMETER_LEN = wcslen(VERSION_PARAMETER);
	HELP_PARAMETER_LEN = wcslen(HELP_PARAMETER);
	NOCONFIGURATIONDOWNLOAD_PARAMETER_LEN = wcslen(NOCONFIGURATIONDOWNLOAD_PARAMETER);
	CONFIGURATIONDOWNLOADURL_PARAMETER_LEN = wcslen(CONFIGURATIONDOWNLOADURL_PARAMETER);
	SILENT_PARAMETER_LEN = wcslen(SILENT_PARAMETER);
}

void CommandLine::_createCatalanitzadorUpdateAction(wstring version)
{
	if (version.size() == 0)
		return;

	CatalanitzadorUpdateAction* action;
	
	action = (CatalanitzadorUpdateAction *)m_pActions->GetActionFromID(CatalanitzadorUpdateActionID);
	action->SetVersion(version);
	action->SetStatus(Successful);
}

void CommandLine::Process(wstring commandLine)
{
	wchar_t* pch;

	pch = (wchar_t*) commandLine.c_str();
	g_log.Log(L"CommandLine::Process: '%s'", pch);

	while (*pch != NULL)
	{
		if (_wcsnicmp(pch, NORUNNING_PARAMETER, NORUNNING_PARAMETER_LEN) == 0)
		{
			wstring version;

			m_bRunningCheck = false;
			pch += NORUNNING_PARAMETER_LEN;
			if (_readCommandLineParameter(&pch, version))
			{
				_createCatalanitzadorUpdateAction(version);
			}

		}
		else if (_wcsnicmp(pch, HELP_PARAMETER, HELP_PARAMETER_LEN) == 0)
		{
			wchar_t szCaption [MAX_LOADSTRING];

			LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
			MessageBox(NULL, HELP_TEXT, szCaption, MB_OK | MB_ICONINFORMATION);
			exit(0);
		}
		else if (_wcsnicmp(pch, NOCONFIGURATIONDOWNLOAD_PARAMETER, NOCONFIGURATIONDOWNLOAD_PARAMETER_LEN) == 0)
		{
			ConfigurationInstance::Get().SetDownloadConfiguration(false);
			pch += NOCONFIGURATIONDOWNLOAD_PARAMETER_LEN;
		}		
		else if (_wcsnicmp(pch, VERSION_PARAMETER, VERSION_PARAMETER_LEN) == 0)
		{
			wstring version;
			
			pch += VERSION_PARAMETER_LEN;
			if (_readCommandLineParameter(&pch, version))
			{
				ConfigurationInstance::Get().SetVersion(ApplicationVersion(version));
			}
		}
		else if (_wcsnicmp(pch, CONFIGURATIONDOWNLOADURL_PARAMETER, CONFIGURATIONDOWNLOADURL_PARAMETER_LEN) == 0)
		{
			wstring url;
			
			pch += CONFIGURATIONDOWNLOADURL_PARAMETER_LEN;
			if (_readCommandLineParameter(&pch, url))
			{
				ConfigurationInstance::Get().SetDownloadConfigurationUrl(url);
			}
		} else if (_wcsnicmp(pch, SILENT_PARAMETER, SILENT_PARAMETER_LEN) == 0)
		{
			m_bSilent = true;
			pch += SILENT_PARAMETER_LEN;
		} 
		else
			pch++;
	}
}

bool CommandLine::_readCommandLineParameter(wchar_t** pcommandline, wstring& parameter)
{
	wchar_t* original = *pcommandline;
	wchar_t* commandline = *pcommandline;

	if (wcslen(commandline) > 0)
	{
		wchar_t szParameter[1024];
		wchar_t* start, *end;

		commandline++;
		start = commandline;
		end = wcschr(start, L' ');

		if (end == NULL)
			end = commandline + wcslen(start);

		wcsncpy_s(szParameter, start, end - start);
		commandline = end;
		parameter = szParameter;
		*pcommandline = commandline;
	}		
	return commandline != original;
}