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
 
void ConfigurationRemoteEmbedded::Load()
{
	wchar_t szFilename[MAX_PATH];

	GetTempPath(MAX_PATH, szFilename);
	wcscat_s(szFilename, L"embedded_configuration.xml");	
	Resources::DumpResource(L"APPLICATION_CONFXML", MAKEINTRESOURCE(IDR_APPLICATION_CONFXML), szFilename);

	_readConfiguration(szFilename);
	_getSha1Sum(szFilename);
}

void ConfigurationRemoteEmbedded::_getSha1Sum(wstring file)
{
	Sha1Sum sha1sum(file);
	sha1sum.ComputeforFile();
	m_sha1sum = sha1sum.GetSum();
}

 void ConfigurationRemoteEmbedded::_readConfiguration(wstring file)
{
	ConfigurationRemoteXmlParser configurationXmlParser(file);
	configurationXmlParser.Parse();
	m_configuration = configurationXmlParser.GetConfiguration();
}
