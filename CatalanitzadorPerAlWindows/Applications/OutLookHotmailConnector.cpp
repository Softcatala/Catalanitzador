/* 
 * Copyright (C) 2012-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "OutLookHotmailConnector.h"
#include "ConfigurationInstance.h"

#define CONNECTOR_REGKEY L"SOFTWARE\\Microsoft\\Office\\Outlook\\Addins\\MSNCON.Addin.1"

OutLookHotmailConnector::OutLookHotmailConnector(bool MSOffice2013OrHigher, IRegistry *registry)
{
	m_registry = registry;
	m_MSOffice2013OrHigher = MSOffice2013OrHigher;	
}

OutLookHotmailConnector::~OutLookHotmailConnector()
{	
	if (m_installerLocation.empty() == false && GetFileAttributes(m_installerLocation.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_installerLocation.c_str());
	}
}

void OutLookHotmailConnector::GetDownloadConfigurationAndTempFile(ConfigurationFileActionDownload& downloadVersion, wstring& tempFile)
{
	wchar_t szTempPath[MAX_PATH];

	GetTempPath(MAX_PATH, szTempPath);

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(MSOfficeLPIActionID, wstring(L"OutlookHotmailConnector"));
	tempFile = szTempPath;
	tempFile += downloadVersion.GetFilename().c_str();
	m_installerLocation = tempFile;
}

wstring OutLookHotmailConnector::_getOutLookPathToCatalanLibrary()
{
	wchar_t szPath[MAX_PATH];
	wstring path;

	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	path=szPath;
	path += L"\\Common Files\\System\\MSMAPI\\1027\\MSNCON32.DLL";
	return path;
}

bool OutLookHotmailConnector::IsNeed()
{
	bool bNeed = false;	
	
	if (m_MSOffice2013OrHigher == false)
	{	
		wstring path;		

		// Connector installed
		if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, CONNECTOR_REGKEY, false) == true)
		{
			// Is the Catalan version?
			path = _getOutLookPathToCatalanLibrary();
			bNeed = GetFileAttributes(path.c_str()) == INVALID_FILE_ATTRIBUTES;
			m_registry->Close();
		}
	}
	g_log.Log(L"OutLookHotmailConnector::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;
}

bool OutLookHotmailConnector::Execute(IRunner* runner)
{
	if (IsNeed() == false)
		return false;

	wchar_t szParams[MAX_PATH];

	wcscpy_s(szParams, L" /quiet");
	runner->Execute((wchar_t *)m_installerLocation.c_str(), szParams);
	g_log.Log(L"OutLookHotmailConnector::Execute. Microsoft Office Connector '%s' with params '%s'", (wchar_t *)m_installerLocation.c_str(), szParams);
	return true;
}
