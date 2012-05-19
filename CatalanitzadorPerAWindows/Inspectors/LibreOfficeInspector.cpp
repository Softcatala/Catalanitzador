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
#include "LibreOfficeInspector.h"


LibreOfficeInspector::LibreOfficeInspector(IRegistry* registry)
{
	m_registry = registry;
}

#define PROGRAM_REGKEY L"SOFTWARE\\LibreOffice\\LibreOffice"

void LibreOfficeInspector::_readVersionInstalled()
{
	bool bKeys = true;
	DWORD dwIndex = 0;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, PROGRAM_REGKEY, false))
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
	g_log.Log(L"LibreOfficeInspector::_readVersionInstalled '%s'", (wchar_t *) m_version.c_str());

	m_KeyValues.push_back(InspectorKeyValue(L"version",m_version));
}

void LibreOfficeInspector::Execute()
{	
	_readVersionInstalled();
}
