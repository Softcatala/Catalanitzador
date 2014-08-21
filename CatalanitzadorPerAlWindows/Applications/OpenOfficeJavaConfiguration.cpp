/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "OpenOfficeJavaConfiguration.h"
#include "XmlParser.h"

OpenOfficeJavaConfiguration::OpenOfficeJavaConfiguration()
{	
	m_javaEnabled = false;
}

wstring OpenOfficeJavaConfiguration::_getSettingFile() 
{
	wstring file;

	assert(m_userDirectory.empty() == false);

	file = m_userDirectory;
	file += L"config\\javasettings_Windows_x86.xml";
	return file;
}

bool OpenOfficeJavaConfiguration::_readNodeCallback(XmlNode node, void *data)
{
	OpenOfficeJavaConfiguration *pThis = (OpenOfficeJavaConfiguration *) data;

	if (node.GetName().compare(L"enabled")==0)
	{
		if (node.GetText().compare(L"true") == 0)
		{
			pThis->m_javaEnabled = true;
		}		
	}
	else if (node.GetName().compare(L"javaInfo")==0)
	{		
		pThis->m_javaInfoParserBlock = true;
	} 
	else if (pThis->m_javaInfoParserBlock == true && node.GetName().compare(L"version")==0)
	{
		pThis->m_javaVersion = node.GetText();
	}

	return true;
}

// Java version numbers are in the format "1.6.0_34" but remove the last part to be able
// to processed using ApplicationVersion
void OpenOfficeJavaConfiguration::_removeJavaMinorRevision(wstring& revision)
{
	int pos = revision.find(L"_");

	if (pos != string::npos)
	{
		revision.erase(pos, string::npos);
	}
}

wstring OpenOfficeJavaConfiguration::GetDefaultJavaVersion()
{
	XmlParser parser;
	wstring file;
	
	m_javaEnabled = false;
	m_javaInfoParserBlock = false;
	m_javaVersion.erase();

	file = _getSettingFile();
	if (parser.Load(file) == false)
	{
		g_log.Log(L"OpenOfficeJavaConfiguration::GetDefaultJavaVersion. Could not open '%s'", (wchar_t *) file.c_str());
		return m_javaVersion;
	}

	parser.Parse(_readNodeCallback, this);
	_removeJavaMinorRevision(m_javaVersion);
	return m_javaVersion;
}

