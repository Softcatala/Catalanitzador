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
#include "ConfigurationRemoteXmlParser.h"
#include "Application.h"

using ::testing::StrCaseEq;

void _getConfigurationFileLocation(wstring &location)
{
	Application::GetExecutionLocation(location);
	location+=L"Application\\configuration.xml";
}

TEST(ConfigurationRemoteXmlParserTest, GetCompatibility)
{
	wstring file, version;

 	_getConfigurationFileLocation(file);
	ConfigurationRemoteXmlParser configurationXmlParser(file);
	configurationXmlParser.Parse();

	version = configurationXmlParser.GetConfiguration().GetCompatibility();
	EXPECT_THAT(version, StrCaseEq(L"1.3.0"));
}

TEST(ConfigurationRemoteXmlParserTest, GetFileActionsDownloads)
{
	wstring file, version;
	vector <wstring> urls;

 	_getConfigurationFileLocation(file);
	ConfigurationRemoteXmlParser configurationXmlParser(file);
	configurationXmlParser.Parse();

	EXPECT_THAT(configurationXmlParser.GetConfiguration().GetFileActionsDownloads().size(), 2);
}

#define OPENOFFICEACTION_INDEX 1
#define VERSION32_INDEX 0
#define VERSION33_INDEX 1

#define OO32_URL L"http://www.softcatala.org/pub/softcatala/catalanitzador/OpenOffice.org/openoffice32-langpack.cab&id=3490&mirall=catalanitzador&so=win32&versio=1.0"
#define OO32_FALLBACK1 L"ftp://ftp.udl.cat/pub/softcatala/catalanitzador/OpenOffice.org/openoffice32-langpack.cab"
#define OO32_FALLBACK2 L"ftp://ftp.urv.es/pub/softcatala/catalanitzador/OpenOffice.org/openoffice32-langpack.cab"

#define OO33_URL L"http://www.softcatala.org/pub/softcatala/catalanitzador/OpenOffice.org/openoffice33-langpack.cab&id=3490&mirall=catalanitzador&so=win32&versio=1.0"
#define OO33_FALLBACK1 L"ftp://ftp.udl.cat/pub/softcatala/catalanitzador/OpenOffice.org/openoffice33-langpack.cab"
#define OO33_FALLBACK2 L"ftp://ftp.urv.es/pub/softcatala/catalanitzador/OpenOffice.org/openoffice33-langpack.cab"

TEST(ConfigurationRemoteXmlParserTest, GetAction)
{
	ConfigurationFileActionDownloads downloads;
	ConfigurationFileActionDownload download32, download33;
	wstring file;

 	_getConfigurationFileLocation(file);
	ConfigurationRemoteXmlParser configurationXmlParser(file);
	configurationXmlParser.Parse();

	downloads = configurationXmlParser.GetConfiguration().GetFileActionsDownloads().at(OPENOFFICEACTION_INDEX);
	download32 = downloads.GetFileActionDownloadCollection().at(VERSION32_INDEX);
	download33 = downloads.GetFileActionDownloadCollection().at(VERSION33_INDEX);

	EXPECT_THAT(downloads.GetFileActionDownloadCollection().size(), 2);
	
	EXPECT_TRUE(download32.GetMinVersion() == ApplicationVersion(L"3.2.0"));
	EXPECT_TRUE(download32.GetMaxVersion() == ApplicationVersion(L"3.2.99"));
	EXPECT_THAT(download32.GetUrls().at(0), StrCaseEq(OO32_URL));
	EXPECT_THAT(download32.GetUrls().at(1), StrCaseEq(OO32_FALLBACK1));
	EXPECT_THAT(download32.GetUrls().at(2), StrCaseEq(OO32_FALLBACK2));
	EXPECT_THAT(download32.GetFilename(), StrCaseEq(L"openoffice32-langpack.cab"));

	EXPECT_TRUE(download33.GetMinVersion() == ApplicationVersion(L"3.3"));
	EXPECT_TRUE(download33.GetMaxVersion() == ApplicationVersion(L"3.3"));
	EXPECT_THAT(download33.GetFilename(), StrCaseEq(L"openoffice33-langpack.cab"));
	EXPECT_THAT(download33.GetUrls().at(0), StrCaseEq(OO33_URL));
	EXPECT_THAT(download33.GetUrls().at(1), StrCaseEq(OO33_FALLBACK1));
	EXPECT_THAT(download33.GetUrls().at(2), StrCaseEq(OO33_FALLBACK2));
}
