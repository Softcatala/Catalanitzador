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
#include "Defines.h"
#include "ConfigurationXmlParser.h"
#include "Application.h"

using ::testing::StrCaseEq;

void _getConfigurationFileLocation(wstring &location)
{
	Application::GetExecutionLocation(location);
	location+=L"Application\\configuration.xml";
}

TEST(ConfigurationXmlParserTest, GetCompatibility)
{
	wstring file, version;

 	_getConfigurationFileLocation(file);
	ConfigurationXmlParser configurationXmlParser(file);
	configurationXmlParser.Parse();

	version = configurationXmlParser.GetConfiguration().GetCompatibility();
	EXPECT_THAT(version, StrCaseEq(L"1.2.0"));
}

#define LATEST_URL1 L"http://baixades.softcatala.org/?url=http://www.softcatala.org/pub/softcatala/catalanitzador/AdobeReader/AdbeRdr1010_ca_ES.exe&id=4712&mirall=catalanitzador&so=win32&versio=1.0"
#define LATEST_FALLBACK1 L"http://udl.cat/pub/softcatala/catalanitzador/AdobeReader/AdbeRdr1010_ca_ES.exe"
#define LATEST_FALLBACK2 L"http://upf.edu/pub/softcatala/catalanitzador/AdobeReader/AdbeRdr1010_ca_ES.exe"

TEST(ConfigurationXmlParserTest, GetLatest)
{
	wstring file, version;
	vector <wstring> urls;

 	_getConfigurationFileLocation(file);
	ConfigurationXmlParser configurationXmlParser(file);
	configurationXmlParser.Parse();

	version = configurationXmlParser.GetConfiguration().GetLatest().GetVersion();
	EXPECT_THAT(version, StrCaseEq(L"1.2.1"));

	urls = configurationXmlParser.GetConfiguration().GetLatest().GetUrls();
	
	EXPECT_THAT(urls.size(), 3);
	EXPECT_THAT(urls[0], StrCaseEq(LATEST_URL1));
	EXPECT_THAT(urls[1], StrCaseEq(LATEST_FALLBACK1));
	EXPECT_THAT(urls[2], StrCaseEq(LATEST_FALLBACK2));
}
