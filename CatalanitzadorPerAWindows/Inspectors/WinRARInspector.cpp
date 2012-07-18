/*
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
 * Copyright (C) 2012 Joan Montané <joan@montane.cat>
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
#include "WinRARInspector.h"
#include "FileVersionInfo.h"

WinRARInspector::WinRARInspector(IRegistry* registry)
{
	m_registry = registry;
}

#define PROGRAM_REGKEY L"Software\\WinRAR"


void WinRARInspector::Execute()
{
	_readVersion();	
}

bool WinRARInspector::_readFilePath(wstring &path)
{
	wchar_t szFile[1024];

	path.empty();
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, PROGRAM_REGKEY, false))
	{
		if (m_registry->GetString(L"exe32", szFile, sizeof(szFile)))
		{
			path = szFile;
		}
		m_registry->Close();

	}

	g_log.Log(L"WinRARInspector::_readFilePath '%s'", (wchar_t *) path.c_str());
	return path.size() > 0;
}

bool WinRARInspector::_readVersion()
{	
	wstring file;
	wstring version;

	_readFilePath(file);

	FileVersionInfo fileVersion(file);
	version = fileVersion.GetVersion();

	g_log.Log(L"WinRARInspector::_readVersion version %s", (wchar_t*) version.c_str());
	m_KeyValues.push_back(InspectorKeyValue(L"version", version.c_str()));
	return version.size() > 0;
}
