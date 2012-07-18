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
#include "PDFCreatorInspector.h"
#include "FileVersionInfo.h"

PDFCreatorInspector::PDFCreatorInspector(IRegistry* registry)
{
	m_registry = registry;
}

#define PROGRAM_REGKEY L"Software\\PDFCreator\\Program"

void PDFCreatorInspector::_readLangInstalled()
{
	wchar_t szLanguage[1024] = L"";

	if (m_registry->OpenKey(HKEY_CURRENT_USER, PROGRAM_REGKEY, false))
	{
		m_registry->GetString(L"Language", szLanguage, sizeof(szLanguage));
		m_registry->Close();
	}

	g_log.Log(L"PDFCreator::_readLangInstalled '%s'", szLanguage);
	m_KeyValues.push_back(InspectorKeyValue(L"lang", szLanguage));
}

void PDFCreatorInspector::Execute()
{
	_readVersion();
	_readLangInstalled();	
}


void PDFCreatorInspector::_readVersion()
{	
	wchar_t szVersion[1024] = L"";

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, PROGRAM_REGKEY, false))
	{
		m_registry->GetString(L"ApplicationVersion", szVersion, sizeof(szVersion));
		m_registry->Close();
	}

	g_log.Log(L"PDFCreator::_readVersionInstalled '%s'", szVersion);
	m_KeyValues.push_back(InspectorKeyValue(L"version", szVersion));
}
