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
#include "InternetExplorerVersion.h"

InternetExplorerVersion::InternetExplorerVersion(IRegistry* registry, IFileVersionInfo* fileVersionInfo)
{
	m_registry = registry;
	m_fileVersionInfo = fileVersionInfo;
	m_version = IEUnread;
}

InternetExplorerVersion::IEVersion InternetExplorerVersion::GetVersion()
{
	if (m_version == IEUnread)
	{
		m_version = _readIEVersion();
	}
	return m_version;
}

wstring InternetExplorerVersion::GetVersionString()
{	
	wchar_t szPath[255] = L"";

	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	wcscat_s(szPath, L"\\Internet Explorer\\iexplore.exe");

	m_fileVersionInfo->SetFilename(szPath);
	return m_fileVersionInfo->GetVersion();	
}

InternetExplorerVersion::IEVersion InternetExplorerVersion::_readIEVersion()
{
	IEVersion version;
	wchar_t szMajorVersion[255];
	unsigned int cnt;
	wstring sversion;
	
	sversion = GetVersionString();
	
	// read the first part of the number
	for (cnt = 0; cnt < sversion.size() && (sversion.at(cnt) >= L'0' && sversion.at(cnt) <= L'9'); cnt++);
	
	if (cnt == 0)
	{
		version = IEUnknown;
	}
	else
	{
		wcsncpy_s(szMajorVersion, sversion.c_str(), cnt);

		switch (_wtoi(szMajorVersion))
		{
			case 6:
				version = IE6;
				break;
			case 7:
				version = IE7;
				break;
			case 8:
				version = IE8;
				break;
			case 9:
				version = IE9;
				break;
			case 10:
				version = IE10;
				break;
			default:
				version = IEUnknown;
				break;
		}
	}

	g_log.Log(L"IELPIAction::_readIEVersion returns IE '%u'", (wchar_t *) version);
	return version;
}
