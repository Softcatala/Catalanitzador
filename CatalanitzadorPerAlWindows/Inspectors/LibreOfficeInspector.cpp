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
#include "LibreOfficeInspector.h"


LibreOfficeInspector::LibreOfficeInspector(IRegistry* registry) : m_LibreOffice(&m_OSVersion, registry)
{
	m_registry = registry;
}

void LibreOfficeInspector::_readVersionInstalled()
{
	bool bKeys = true;
	DWORD dwIndex = 0;

	m_version = m_LibreOffice.GetVersion();
	g_log.Log(L"LibreOfficeInspector::_readVersionInstalled '%s'", (wchar_t *) m_version.c_str());
	m_KeyValues.push_back(InspectorKeyValue(L"version", m_version));
}

void LibreOfficeInspector::Execute()
{	
	_readVersionInstalled();
	_readLocale();
	_getUIFilesInstalled();
	_getDictInstalled();
}

void LibreOfficeInspector::_getPreferencesFile(wstring& location)
{
	if (m_version.size() > 0)
	{
		location = m_LibreOffice._getPreferencesDirectory();
		location += L"registrymodifications.xcu";
	}
}

enum LanguageParsingState
{
	ItemOther,
	ItemLinguisticGeneral,
	PropUILocale,
	ItemSetupL10N,
	PropOoLocale
};

struct LOOConfigData
{
	wstring UILocale;
	wstring ooLocale;
};


LanguageParsingState g_parsing_state = ItemOther;

bool LibreOfficeInspector::_readNodeCallback(XmlNode node, void *data)
{
	if (node.GetName().compare(L"value")==0)
	{
		LOOConfigData* configData = (LOOConfigData*) data;

		if (g_parsing_state == PropUILocale)
			configData->UILocale = node.GetText();
		else if (g_parsing_state == PropOoLocale)
			configData->ooLocale = node.GetText();
		
		g_parsing_state = ItemOther;
		return true;
	}

	bool bIsItem, bIsProp;

	bIsItem = node.GetName().compare(L"item") == 0;
	bIsProp = node.GetName().compare(L"prop") == 0;

	if (bIsItem && g_parsing_state != ItemOther)	
		g_parsing_state = ItemOther;	

	vector <XmlAttribute>* attributes = node.GetAttributes();
	for (unsigned int i = 0; i < attributes->size(); i++)
	{
		XmlAttribute attribute;

		attribute = attributes->at(i);

		if (g_parsing_state == ItemOther && bIsItem && attribute.GetName() == L"oor:path")
		{
			if (attribute.GetValue() == L"/org.openoffice.Office.Linguistic/General")
			{
				g_parsing_state = ItemLinguisticGeneral;
				continue;
			}
			if (attribute.GetValue() == L"/org.openoffice.Setup/L10N")
			{
				g_parsing_state = ItemSetupL10N;
				continue;
			}
		}		

		if (bIsProp && g_parsing_state == ItemLinguisticGeneral && attribute.GetName() == L"oor:name" && attribute.GetValue() == L"UILocale")
		{
			g_parsing_state = PropUILocale;
			continue;
		}

		if (bIsProp && g_parsing_state == ItemSetupL10N && attribute.GetName() == L"oor:name" && attribute.GetValue() == L"ooLocale")
		{
			g_parsing_state = PropOoLocale;
			continue;
		}
	}
	
	return true;
}

// See how LibreOffice manages this
//	http://code.woboq.org/libreoffice/libreoffice/cui/source/options/optgdlg.cxx.html
//	http://code.woboq.org/libreoffice/libreoffice/desktop/source/app/langselect.cxx.html
//
void LibreOfficeInspector::_readLocale()
{
	XmlParser parser;
	wstring file, locale;
	LOOConfigData configData;

	if (m_version.size() >  0)
	{
		_getPreferencesFile(file);

		if (parser.Load(file))
		{			
			parser.Parse(_readNodeCallback, &configData);

			if (configData.UILocale.length() > 0)
			{
				locale = configData.UILocale;
			} else if (configData.ooLocale.length() > 0)
			{
				locale = configData.ooLocale;
			}

			g_log.Log(L"LibreOfficeInspector::_readLocale. Preferences file '%s', language '%s'", (wchar_t *)file.c_str(), 
				(wchar_t *) locale.c_str());
		}
		else
		{
			g_log.Log(L"LibreOfficeInspector::_readLocale. Could not open '%s'", (wchar_t *) file.c_str());
		}
	}

	m_KeyValues.push_back(InspectorKeyValue(L"lang", locale));
}

void LibreOfficeInspector::_getUIFilesInstalled()
{	
	wchar_t szUIFound[20]=L"";
	wstring path = m_LibreOffice._getInstallationPath();

	if (path.empty() == false)
	{
		wchar_t szFileName[MAX_PATH];
		int i;

		wcscpy_s(szFileName, path.c_str());
		for (i = wcslen(szFileName); i > 0 && szFileName[i] != '\\' ; i--);
		
		szFileName[i + 1] = NULL;
		wcscat_s(szFileName, L"resource\\svxca.res");

		if(GetFileAttributes(szFileName) != INVALID_FILE_ATTRIBUTES) wcscat_s(szUIFound, L"ca");

		for (i = wcslen(szFileName); i > 0 && szFileName[i] != '.' ; i--);
		szFileName[i] = NULL;
		wcscat_s(szFileName, L"-XV.res");
		if(wcslen(szUIFound) > 1) wcscat_s(szUIFound, L";");
		if(GetFileAttributes(szFileName) != INVALID_FILE_ATTRIBUTES) wcscat_s(szUIFound, L"ca-XV");	
	}
	g_log.Log(L"LibreOfficeInspector::_getUIFilesInstalled '%s'", (wchar_t *) szUIFound);

	m_KeyValues.push_back(InspectorKeyValue(L"ui_files",szUIFound));
}

void LibreOfficeInspector::_getDictInstalled()
{
	wchar_t szDictFound[10]=L"";

	wstring path = m_LibreOffice._getInstallationPath();
	if (path.empty() == false)
	{
		wchar_t szFileName[MAX_PATH];
		int i;

		wcscpy_s(szFileName, path.c_str());
		for (i = wcslen(szFileName); i > 0 && szFileName[i] != '\\' ; i--);
		if (i>0) i--;
		for (; i > 0 && szFileName[i] != '\\' ; i--);
		szFileName[i + 1] = NULL;
		wcscat_s(szFileName, L"share\\extensions\\dict-ca\\nul");

		if(GetFileAttributes(szFileName) != INVALID_FILE_ATTRIBUTES) wcscat_s(szDictFound, L"ca");
	}
	g_log.Log(L"LibreOfficeInspector::_getDictInstalled '%s'", (wchar_t *) szDictFound);

	m_KeyValues.push_back(InspectorKeyValue(L"dict_files",szDictFound));
}
