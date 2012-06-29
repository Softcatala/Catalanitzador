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
#include <Shlobj.h>
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
	_readLanguage();
}

void LibreOfficeInspector::_getPreferencesFile(wstring& location)
{
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\LibreOffice\\3\\user\\registrymodifications.xcu";
}

void LibreOfficeInspector::_readLocale(wstring &locale)
{
	wchar_t szValue[1024];

	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\International", false))
	{
		if (m_registry->GetString(L"Locale", szValue, sizeof (szValue)))
		{
			locale = szValue;
		}
		m_registry->Close();
	}
}


enum LanguageParsingState
{
	ItemOther,
	ItemLinguisticGeneral,
	PropUILocale
};


LanguageParsingState g_parsing_state = ItemOther;

bool LibreOfficeInspector::_readNodeCallback(XmlNode node, void *data)
{
	vector <XmlAttribute>* attributes;
	bool bIsItem;

	if (g_parsing_state == PropUILocale && node.GetName().compare(L"value")==0)
	{		
		wstring* lang_found = (wstring *) data;
		*lang_found = node.GetText();
		g_parsing_state = ItemOther;
		return false;
	}

	bIsItem = node.GetName().compare(L"item") == 0;

	if (bIsItem && g_parsing_state != ItemOther)
	{
		g_parsing_state = ItemOther;
	}	

	attributes = node.GetAttributes();
	for (unsigned int i = 0; i < attributes->size(); i++)
	{
		XmlAttribute attribute;

		attribute = attributes->at(i);

		if (g_parsing_state == ItemOther && bIsItem && attribute.GetName() == L"oor:path" && attribute.GetValue() == L"/org.openoffice.Office.Linguistic/General")
		{
			g_parsing_state = ItemLinguisticGeneral;			
		}

		if (g_parsing_state == ItemLinguisticGeneral && attribute.GetName() == L"oor:name" && attribute.GetValue() == L"UILocale")
		{
			g_parsing_state = PropUILocale;
		}		
	}
	
	return true;
}


void LibreOfficeInspector::_readLanguage()
{
	XmlParser parser;
	wstring lang_found, file;	

	_getPreferencesFile(file);

	if (parser.Load(file))
	{
		parser.Parse(_readNodeCallback, &lang_found);

		if (m_version.size() > 0 && lang_found.size() == 0)
		{
			wstring locale;

			_readLocale(locale);
			lang_found = L"locale:" + locale;
		}

		g_log.Log(L"LibreOfficeInspector::_readLanguage. Preferences file '%s', language '%s'", (wchar_t *)file.c_str(), 
			(wchar_t *) lang_found.c_str());		
	}
	else
	{
		g_log.Log(L"LibreOfficeInspector::_readLanguage. Could not open '%s'", (wchar_t *) file.c_str());
	}
	
	m_KeyValues.push_back(InspectorKeyValue(L"lang",lang_found));
}

