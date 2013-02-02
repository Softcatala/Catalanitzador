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
#include "ConfigurationFileActionDownload.h"

using ::testing::StrCaseEq;

#define MIN_VERSION L"1.2.0"
#define MAX_VERSION L"1.2.1"

TEST(ConfigurationFileActionDownloadTest, GetSetMinVersion)
{	
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.SetMinVersion(ApplicationVersion(MIN_VERSION));

	EXPECT_TRUE(fileActionDownload.GetMinVersion() == ApplicationVersion(MIN_VERSION));
}

TEST(ConfigurationFileActionDownloadTest, GetSetMaxVersion)
{	
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.SetMaxVersion(ApplicationVersion(MAX_VERSION));

	EXPECT_TRUE(fileActionDownload.GetMaxVersion() == ApplicationVersion(MAX_VERSION));
}


#define LATEST_URL1 L"http://url1.com/file.bin"
#define LATEST_URL2 L"http://url2.com/file.bin"
#define LATEST_URL1_SHA1 L"http://url1.com/file.bin.sha1"
#define LATEST_URL2_SHA1 L"http://url2.com/file.bin.sha1"
#define LATEST_URL1_SHA1_SET L"ftp://url1.com/file.bin.sha1"

TEST(ConfigurationFileActionDownloadTest, AddUrl)
{	
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.AddUrl(LATEST_URL1);
	fileActionDownload.AddUrl(LATEST_URL2);
	
	EXPECT_THAT(fileActionDownload.GetUrls().size(), 2);
	EXPECT_THAT(fileActionDownload.GetUrls()[0], StrCaseEq(LATEST_URL1));
	EXPECT_THAT(fileActionDownload.GetUrls()[1], StrCaseEq(LATEST_URL2));
}

TEST(ConfigurationFileActionDownloadTest, AddUrlSha1Auto)
{	
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.AddUrl(LATEST_URL1);
	fileActionDownload.AddUrl(LATEST_URL2);
	
	EXPECT_THAT(fileActionDownload.GetSha1Urls().size(), 2);
	EXPECT_THAT(fileActionDownload.GetSha1Urls()[0], StrCaseEq(LATEST_URL1_SHA1));
	EXPECT_THAT(fileActionDownload.GetSha1Urls()[1], StrCaseEq(LATEST_URL2_SHA1));
}

TEST(ConfigurationFileActionDownloadTest, SetSha1Url)
{	
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.AddUrl(LATEST_URL1);
	fileActionDownload.SetSha1Url(0, LATEST_URL1_SHA1_SET);
	
	EXPECT_THAT(fileActionDownload.GetSha1Urls()[0], StrCaseEq(LATEST_URL1_SHA1_SET));	
}

TEST(ConfigurationFileActionDownloadTest, SetUrl)
{	
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.AddUrl(LATEST_URL1);
	fileActionDownload.SetUrl(0, LATEST_URL2);
	
	EXPECT_THAT(fileActionDownload.GetUrls()[0], StrCaseEq(LATEST_URL2));
}

TEST(ConfigurationFileActionDownloadTest, IsEmpty)
{	
	ConfigurationFileActionDownload fileActionDownload;
	
	EXPECT_TRUE(fileActionDownload.IsEmpty());
}

#define FILENAME_TEST L"file.bin"

TEST(ConfigurationFileActionDownloadTest, GetSetFilename)
{
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.SetFilename(FILENAME_TEST);

	EXPECT_THAT(fileActionDownload.GetFilename(), StrCaseEq(FILENAME_TEST));
}

#define VERSION_TEST L"XP"

TEST(ConfigurationFileActionDownloadTest, GetSetVersion)
{
	ConfigurationFileActionDownload fileActionDownload;
	fileActionDownload.SetVersion(VERSION_TEST);

	EXPECT_THAT(fileActionDownload.GetVersion(), StrCaseEq(VERSION_TEST));
}
