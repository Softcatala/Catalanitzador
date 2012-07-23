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
#include "ConfigurationXmlParser.h"


ConfigurationXmlParser::ConfigurationXmlParser(wstring file)
{
	m_file = file;
}

bool ConfigurationXmlParser::_readNodeCallback(XmlNode node, void *data)
{
	ConfigurationXmlParser* pThis = (ConfigurationXmlParser *)data;
	pThis->ParseNode(node);
	return true;
}

void ConfigurationXmlParser::ParseNode(XmlNode node)
{
	switch (m_configurationBlock)
	{
		case ConfigurationBlockNone:
			break;
		case ConfigurationBlockCompatibility:
			ParseBlockCompatibility(node);
			break;
		case ConfigurationBlockLatest:
			ParseBlockLatest(node);
			break;
		default:
			break;
	}

	if (node.GetName().compare(L"compatibility")==0)
	{
		m_configurationBlock = ConfigurationBlockCompatibility;
	} 
	else if (node.GetName().compare(L"latest")==0)
	{
		m_configurationBlock = ConfigurationBlockLatest;
	}
}

void ConfigurationXmlParser::ParseBlockCompatibility(XmlNode node)
{
	if (node.GetName().compare(L"version")==0)
	{
		m_configuration.SetCompatibility(node.GetText());
	}
}

#define FALLBACK_URL L"FallbackURL"

void ConfigurationXmlParser::ParseBlockLatest(XmlNode node)
{
	if (node.GetName().compare(L"version")==0)
	{
		m_configuration.GetLatest().SetVersion(node.GetText());
	}
	else if (node.GetName().compare(L"URL")==0 || node.GetName().compare(0, wcslen(FALLBACK_URL), FALLBACK_URL)==0)
	{
		m_configuration.GetLatest().AddUrl(node.GetText());
	} 
}

void ConfigurationXmlParser::Parse()
{
	XmlParser parser;	
	
	if (parser.Load(m_file) == false)
	{
		g_log.Log(L"ConfigurationXmlParser::Parse. Could not open '%s'", (wchar_t *) m_file.c_str());
		return;
	}

	m_configurationBlock = ConfigurationBlockNone;
	parser.Parse(_readNodeCallback, this);
}

