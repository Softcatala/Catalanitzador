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
#include "ConfigurationRemoteXmlParser.h"


ConfigurationRemoteXmlParser::ConfigurationRemoteXmlParser(wstring file)
{
	m_file = file;
}

bool ConfigurationRemoteXmlParser::_readNodeCallback(XmlNode node, void *data)
{
	ConfigurationRemoteXmlParser* pThis = (ConfigurationRemoteXmlParser *)data;
	pThis->ParseNode(node);
	return true;
}

void ConfigurationRemoteXmlParser::ParseNode(XmlNode node)
{
	switch (m_configurationBlock)
	{
		case ConfigurationBlockNone:
			break;
		case ConfigurationBlockCompatibility:
			ParseBlockCompatibility(node);
			break;
		case ConfigurationBlockActions:
			ParseBlockActions(node);
		//case ConfigurationBlockAction:
		//	ParseBlockAction(node);
			break;
		default:
			break;
	}

	if (node.GetName().compare(L"compatibility")==0)
	{
		m_configurationBlock = ConfigurationBlockCompatibility;
	}
	else if (node.GetName().compare(L"actions")==0)
	{
		m_configurationBlock = ConfigurationBlockActions;
	}	
}

void ConfigurationRemoteXmlParser::ParseBlockCompatibility(XmlNode node)
{
	if (node.GetName().compare(L"version")==0)
	{
		m_configuration.SetCompatibility(node.GetText());
	}
}

#define URL_N L"url_"

void ConfigurationRemoteXmlParser::ParseBlockActions(XmlNode node)
{
	if (node.GetName().compare(L"action")==0)
	{
		ConfigurationFileActionDownloads fileDownloads;
		int index;

		index = m_configuration.AddFileActionDownloads(fileDownloads);
		m_pFileActionDownloads = &m_configuration.GetFileActionsDownloads().at(index);
	}

	ParseBlockAction(node);
}


void ConfigurationRemoteXmlParser::ParseBlockAction(XmlNode node)
{
	if (node.GetName().compare(L"id")==0)
	{
		ActionID actionID = (ActionID) _wtoi(node.GetText().c_str());
		m_pFileActionDownloads->SetActionID(actionID);
	} 
	else if (node.GetName().compare(L"version")==0)
	{
		ConfigurationFileActionDownload fileDownload;
		int index;

		index  = m_pFileActionDownloads->AddFileActionDownload(fileDownload);
		m_pFileActionDownload = &m_pFileActionDownloads->GetFileActionDownloadCollection().at(index);
	}
	else if (node.GetName().compare(L"min_version")==0)
	{
		ApplicationVersion version(node.GetText());
		m_pFileActionDownload->SetMinVersion(version);
	} 
	else if (node.GetName().compare(L"max_version")==0)
	{
		ApplicationVersion version(node.GetText());
		m_pFileActionDownload->SetMaxVersion(version);
	}
	else if (node.GetName().compare(L"filename")==0)
	{		
		m_pFileActionDownload->SetFilename(node.GetText());
	}
	else if (node.GetName().compare(0, wcslen(URL_N), URL_N)==0)
	{
		m_pFileActionDownload->AddUrl(node.GetText());
	}
}

void ConfigurationRemoteXmlParser::Parse()
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

