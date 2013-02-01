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

#include "OpenOfficeAction.h"
#include "Url.h"
#include "ApplicationVersion.h"
#include "ConfigurationInstance.h"

// message used to communicate with try icon process
#define LISTENER_WINDOWCLASS    L"SO Listener Class"
#define KILLTRAY_MESSAGE        L"SO KillTray"
#define SOFFICE_PROCESSNAME		L"soffice.bin"


OpenOfficeAction::OpenOfficeAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_registry = registry;	
	m_runner = runner;
	m_szFilename[0]=NULL;
	m_szTempPathCAB[0] = NULL;

	GetTempPath(MAX_PATH, m_szTempPath);

	_addExecutionProcess(ExecutionProcess(SOFFICE_PROCESSNAME, L"", true));
}

OpenOfficeAction::~OpenOfficeAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}

	_removeCabTempFiles();
}

wchar_t* OpenOfficeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_OPENOFFICEACTION_NAME, szName);
}

wchar_t* OpenOfficeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_OPENOFFICEACTION_DESCRIPTION, szDescription);
}

ExecutionProcess OpenOfficeAction::GetExecutingProcess()
{
	ExecutionProcess process;

	process = ActionExecution::GetExecutingProcess();

	if (process.IsEmpty() == false)
	{
		return process;
	}

	if (FindWindowEx(NULL, NULL, LISTENER_WINDOWCLASS, NULL) == NULL)
	{
		return ExecutionProcess();
	}
	else
	{
		return ExecutionProcess(KILLTRAY_MESSAGE, L"", true);
	}
}

void OpenOfficeAction::FinishExecution(ExecutionProcess process)
{
	HWND hwndTray; 
		
	hwndTray = FindWindow( LISTENER_WINDOWCLASS, NULL);

    if (hwndTray)
	{
		SendMessage(hwndTray, RegisterWindowMessage(KILLTRAY_MESSAGE), 0, 0);
	}

	Runner runner;
	vector <DWORD> processIDs = runner.GetProcessID(wstring(SOFFICE_PROCESSNAME));

	for (unsigned int i = 0; i < processIDs.size(); i++)
	{
		runner.TerminateProcessID(processIDs.at(i));
	}
}


// TODO: You can several versions installed, just read first one for now
void OpenOfficeAction::_readVersionInstalled()
{
	bool bKeys = true;
	DWORD dwIndex = 0;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, OPENOFFICCE_PROGRAM_REGKEY, false))
	{
		while (bKeys)
		{
			wstring key;

			bKeys = m_registry->RegEnumKey(dwIndex, key);
			dwIndex++;

			if (bKeys)
			{
				m_version = key;
				break;
			}
		}
		m_registry->Close();
	}
	g_log.Log(L"OpenOfficeAction::_readVersionInstalled '%s'", (wchar_t *) m_version.c_str());
}

bool OpenOfficeAction::IsNeed()
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
	g_log.Log(L"OpenOfficeAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}


bool OpenOfficeAction::Download(ProgressStatus progress, void *data)
{
	wstring filename;
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(GetVersion()));
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());

	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, progress, data);
}

void OpenOfficeAction::Execute()
{	
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	// Unique temporary file (needs to create it)
	GetTempFileName(m_szTempPath, L"CAT", 0, m_szTempPathCAB);
	DeleteFile(m_szTempPathCAB);

	if (CreateDirectory(m_szTempPathCAB, NULL) == FALSE)
	{
		g_log.Log(L"OpenOfficeAction::Execute. Cannot create temp directory '%s'", m_szTempPathCAB);
		return;
	}

	_extractCabFile(m_szFilename, m_szTempPathCAB);

	wcscpy_s(szApp, m_szTempPathCAB);
	wcscat_s(szApp, L"\\setup.exe /q");
	SetStatus(InProgress);
	g_log.Log(L"OpenOfficeAction::Execute '%s' with params '%s'", szApp, szParams);
	m_runner->Execute(NULL, szApp);
}

bool OpenOfficeAction::_extractCabFile(wchar_t * file, wchar_t * path)
{
	Runner runnerCab;
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];
	
	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\expand.exe ");

	swprintf_s (szParams, L" %s %s -f:*", file, path);
	g_log.Log(L"OpenOfficeAction::_extractCabFile '%s' with params '%s'", szApp, szParams);
	runnerCab.Execute(szApp, szParams);
	runnerCab.WaitUntilFinished();
	return true;
}

// This deletes the contents of the extracted CAB file for the language pack
void OpenOfficeAction::_removeCabTempFiles()
{
	if (m_szTempPathCAB[0] == NULL)
		return;
	
	wchar_t szFile[MAX_PATH];
	wchar_t* files[] = {L"openofficeorg1.cab", L"openofficeorg33.msi", L"setup.exe", L"setup.ini", NULL};

	for(int i = 0; files[i] != NULL; i++)
	{
		wcscpy_s(szFile, m_szTempPathCAB);
		wcscat_s(szFile, L"\\");
		wcscat_s(szFile, files[i]);

		if (DeleteFile(szFile) == FALSE)
		{
			g_log.Log(L"OpenOfficeAction::_removeCabTempFiles. Cannot delete '%s'", (wchar_t *) szFile);
		}		
	}

	RemoveDirectory(m_szTempPathCAB);
}

enum LanguageParsingState
{
	ItemOther,
	ItemLinguisticGeneral,
	PropUILocale
};

LanguageParsingState parsing_state = ItemOther;

bool OpenOfficeAction::_readNodeCallback(XmlNode node, void *data)
{
	vector <XmlAttribute>* attributes;
	bool bIsItem;

	if (parsing_state == PropUILocale && node.GetName().compare(L"value")==0)
	{		
		wstring* lang_found = (wstring *) data;
		*lang_found = node.GetText();
		parsing_state = ItemOther;
		return false;
	}

	bIsItem = node.GetName().compare(L"item") == 0;

	if (bIsItem && parsing_state != ItemOther)
	{
		parsing_state = ItemOther;
	}	

	attributes = node.GetAttributes();
	for (unsigned int i = 0; i < attributes->size(); i++)
	{
		XmlAttribute attribute;

		attribute = attributes->at(i);

		if (parsing_state == ItemOther && bIsItem && attribute.GetName() == L"oor:path" && attribute.GetValue() == L"/org.openoffice.Office.Linguistic/General")
		{
			parsing_state = ItemLinguisticGeneral;			
		}

		if (parsing_state == ItemLinguisticGeneral && attribute.GetName() == L"oor:name" && attribute.GetValue() == L"UILocale")
		{
			parsing_state = PropUILocale;
		}
	}
	
	return true;
}

void OpenOfficeAction::_getPreferencesFile(wstring& location)
{
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\OpenOffice.org\\3\\user\\registrymodifications.xcu";
}

#define DEFAULT_LANGUAGE L"ca"


void OpenOfficeAction::_setDefaultLanguage()
{
	XmlParser parser;
	wstring file;
	bool bRslt;

	_getPreferencesFile(file);
	if (parser.Load(file) == false)
	{
		g_log.Log(L"OpenOfficeAction::_setDefaultLanguage. Could not open '%s'", (wchar_t *) file.c_str());
		return;
	}

	/*
		XML fragment to generate

		<item oor:path="/org.openoffice.Office.Linguistic/General">
			<prop oor:name="UILocale" oor:op="fuse">
				<value>DEFAULT_LANGUAGE</value>
			</prop>
		</item>
	*/

	XmlNode item(parser.getDocument()), prop(parser.getDocument()), value(parser.getDocument());
	value.SetName(wstring(L"value"));
	value.SetText(wstring(DEFAULT_LANGUAGE));
	
	prop.SetName(wstring(L"prop"));
	prop.AddAttribute(XmlAttribute(L"oor:name", L"UILocale"));
	prop.AddAttribute(XmlAttribute(L"oor:op", L"fuse"));
	prop.AddChildren(value);

	item.SetName(wstring(L"item"));
	item.AddAttribute(XmlAttribute(L"oor:path", L"/org.openoffice.Office.Linguistic/General"));	
	item.AddChildren(prop);

	parser.AppendNode(item);
	bRslt = parser.Save(file);

	g_log.Log(L"OpenOfficeAction::_setDefaultLanguage. Saved file '%s', result %u", (wchar_t *)file.c_str(), (wchar_t *) bRslt);
}

bool OpenOfficeAction::_isDefaultLanguage()
{
	XmlParser parser;
	wstring lang_found, file;
	bool bRslt;

	_getPreferencesFile(file);

	if (parser.Load(file) == false)
	{
		g_log.Log(L"OpenOfficeAction::_isDefaultLanguage. Could not open '%s'", (wchar_t *) file.c_str());
		return false;
	}
	parser.Parse(_readNodeCallback, &lang_found);
	// TODO: We should look for != "ca" but right now we can only add Catalan if no language is present
	bRslt = lang_found.size() != 0;
	g_log.Log(L"OpenOfficeAction::_isDefaultLanguage. Preferences file '%s', isdefault %u", (wchar_t *)file.c_str(), (wchar_t *) bRslt);
	return bRslt;
}

bool OpenOfficeAction::_isLangPackInstalled()
{
	wstring key;
	bool bRslt = false;

	key = OPENOFFICCE_PROGRAM_REGKEY;
	key += L"\\";
	key += m_version;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*) key.c_str(), false))
	{
		wchar_t szFileName[MAX_PATH];

		if (m_registry->GetString(L"path", szFileName, sizeof(szFileName)))
		{
			int i;

			for (i = wcslen(szFileName); i > 0 && szFileName[i] != '\\' ; i--);
			
			szFileName[i + 1] = NULL;
			wcscat_s(szFileName, L"resource\\oooca.res");

			bRslt = GetFileAttributes(szFileName) != INVALID_FILE_ATTRIBUTES;
		}
		m_registry->Close();
	}
	g_log.Log(L"OpenOfficeAction::_isLangPackInstalled '%u'", (wchar_t *) bRslt);
	return bRslt;
}

ActionStatus OpenOfficeAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (_isLangPackInstalled())
		{
			if (_isDefaultLanguage() == false)
			{
				_setDefaultLanguage();
			}

			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
		}
		
		g_log.Log(L"OpenOfficeAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

void OpenOfficeAction::CheckPrerequirements(Action * action)
{
	_readVersionInstalled();

	if (m_version.size() > 0)
	{
		if (_isLangPackInstalled() == true)
		{
			SetStatus(AlreadyApplied);
			return;
		}

		if (_doesDownloadExist() == false)
		{			
			_getStringFromResourceIDName(IDS_OPENOFFICEACTION_NOTSUPPORTEDVERSION, szCannotBeApplied);
			g_log.Log(L"OpenOfficeAction::CheckPrerequirements. Version not supported");
			SetStatus(CannotBeApplied);
			return;
		}
	}
	else
	{
		_setStatusNotInstalled();
		return;
	}
}
