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
#include "AdobeAcrobatInspector.h"

#define ACROBAT_REGKEY L"Software\\Adobe\\Acrobat Reader"

AdobeAcrobatInspector::AdobeAcrobatInspector(IRegistry* registry)
{
	m_registry = registry;
}

void AdobeAcrobatInspector::_enumVersions(vector <wstring>& versions)
{
	bool bKeys = true;
	DWORD dwIndex = 0;

	if (m_registry->OpenKey(HKEY_CURRENT_USER, ACROBAT_REGKEY, false))
	{
		while (bKeys)
		{
			wstring key;

			bKeys = m_registry->RegEnumKey(dwIndex, key);
			dwIndex++;

			if (bKeys)
			{
				versions.push_back(key);
			}
		}
		m_registry->Close();
	}
}

void AdobeAcrobatInspector::_enumInstalledLangs(vector <wstring>& versions)
{
	for (unsigned int i = 0; i < versions.size(); i++)
	{
		wstring key(ACROBAT_REGKEY);

		key+=L"\\";
		key+=versions[i];
		key+=L"\\";
		key+=L"Language";

		if (m_registry->OpenKey(HKEY_CURRENT_USER, (wchar_t*)key.c_str(), false))
		{
			wchar_t szLang[2048];
			wstring lang;

			if (m_registry->GetString(L"UI", szLang, sizeof(szLang)))
			{
				lang = szLang;
				m_KeyValues.push_back(InspectorKeyValue(versions[i], lang));
			}

			m_registry->Close();
		}
	}
}

void AdobeAcrobatInspector::Execute()
{	
	vector <wstring> versions;

	_enumVersions(versions);
	_enumInstalledLangs(versions);
}
