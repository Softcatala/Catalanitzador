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

#include "ConfigurationRemoteEmbedded.h"
#include "Resources.h"
#include "Sha1Sum.h"
#include "ConfigurationRemoteXmlParser.h"

ConfigurationRemoteEmbedded::~ConfigurationRemoteEmbedded()
{
	if (m_filename.size() > 0  && GetFileAttributes(m_filename.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_filename.c_str());
	}
}
 
void ConfigurationRemoteEmbedded::Load()
{
	wchar_t szFilename[MAX_PATH];

	GetTempPath(MAX_PATH, szFilename);
	wcscat_s(szFilename, L"embedded_configuration.xml");
	m_filename = szFilename;
	Resources::DumpResource(L"APPLICATION_CONFXML", MAKEINTRESOURCE(IDR_APPLICATION_CONFXML), szFilename);

	_readConfiguration();
	_getSha1Sum();
}

void ConfigurationRemoteEmbedded::_getSha1Sum()
{
	Sha1Sum sha1sum(m_filename);
	sha1sum.ComputeforFile();
	m_sha1sum = sha1sum.GetSum();
}

 void ConfigurationRemoteEmbedded::_readConfiguration()
{
	ConfigurationRemoteXmlParser configurationXmlParser(m_filename);
	configurationXmlParser.Parse();
	m_configuration = configurationXmlParser.GetConfiguration();
}
