/* 
 * Copyright (C) 2016 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "FirefoxMozillaServer.h"
#include "Application.h"
#include "ConfigurationRemote.h"
#include "ConfigurationInstance.h"

using ::testing::StrCaseEq;
using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;


class FirefoxMozillaServerForTest : public FirefoxMozillaServer
{
public:	

	FirefoxMozillaServerForTest::FirefoxMozillaServerForTest(wstring version, bool is64Bits)
		: FirefoxMozillaServer(&DownloadManager(), version, is64Bits) {}

	using FirefoxMozillaServer::_getSha1SignatureForFilename;
};

class FirefoxMozillaServerTest : public testing::Test
{
public:

		virtual void TearDown()
		{
			ConfigurationInstance::Reset();
		}
};


void _configureFirefoxDownloadFile()
{
	ConfigurationRemote remote;
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload, fileActionDownloadResult;

	fileActionDownload.SetMinVersion(ApplicationVersion(L"3.0.0"));
	fileActionDownload.SetMaxVersion(ApplicationVersion(L"41.99.99"));
	fileActionDownload.AddUrl(L"http://ftp.mozilla.cat/pub/firefox/releases/[version]/[platform]/ca/Firefox%20Setup%20[version].exe");
	fileActionDownloads.SetActionID(FirefoxActionID);
	fileActionDownloads.AddFileActionDownload(fileActionDownload);
	remote.AddFileActionDownloads(fileActionDownloads);

	ConfigurationInstance::Get().SetRemote(remote);
}


TEST_F(FirefoxMozillaServerTest, _getSha1SignatureForFilename_Sha1)
{
	TempFile tempFile;
	const wchar_t* VERSION = L"win32/ca/Firefox Setup 41.0.exe";
	wstring location;
	bool is64Bits = false;

	Application::GetExecutionLocation(location);
	location += L"Firefox\\FtpSha1\\SHA1SUMS";	
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);

	FirefoxMozillaServerForTest firefoxMozillaServer(VERSION, is64Bits);
	wstring sha1 = firefoxMozillaServer._getSha1SignatureForFilename(tempFile.GetFileName(), VERSION);
	EXPECT_THAT(sha1.c_str(), StrCaseEq(L"3cda76c598655a2bfd907cb79e178f21cf5112ad"));
}

TEST_F(FirefoxMozillaServerTest, _getSha1SignatureForFilename_Sha1_512)
{
	TempFile tempFile;
	const wchar_t* VERSION = L"win32/ca/Firefox Setup 43.0.exe";
	wstring location;
	bool is64Bits = false;

	Application::GetExecutionLocation(location);
	location += L"Firefox\\FtpSha1\\SHA512SUMS";	
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);

	FirefoxMozillaServerForTest firefoxMozillaServer(VERSION, is64Bits);
	wstring sha1 = firefoxMozillaServer._getSha1SignatureForFilename(tempFile.GetFileName(), VERSION);
	EXPECT_THAT(sha1.c_str(), StrCaseEq(L"70c2f3d6906579a910bdfbf268e1f17cb973b87636d05f2b91f07ac7775b6835f1562c07e0abe32f17e47b97c6d071e96047e47145654e03f41ae176d014d836"));
}


TEST_F(FirefoxMozillaServerTest, GetConfigurationFileActionDownload_32bits)
{
	bool is64Bits = false;
	const wchar_t* VERSION = L"5.0.0";
	_configureFirefoxDownloadFile();

	FirefoxMozillaServerForTest firefoxMozillaServer(VERSION, is64Bits);
	ConfigurationFileActionDownload configurationFileActionDownload = firefoxMozillaServer.GetConfigurationFileActionDownload();

	wstring url = configurationFileActionDownload.GetUrls().at(0);
	EXPECT_THAT(url.c_str(), StrCaseEq(L"http://ftp.mozilla.cat/pub/firefox/releases/5.0.0/win32/ca/Firefox%20Setup%205.0.0.exe"));
}

TEST_F(FirefoxMozillaServerTest, GetConfigurationFileActionDownload_64bits)
{
	bool is64Bits = true;
	const wchar_t* VERSION = L"5.0.0";
	_configureFirefoxDownloadFile();

	FirefoxMozillaServerForTest firefoxMozillaServer(VERSION, is64Bits);
	ConfigurationFileActionDownload configurationFileActionDownload = firefoxMozillaServer.GetConfigurationFileActionDownload();

	wstring url = configurationFileActionDownload.GetUrls().at(0);
	EXPECT_THAT(url.c_str(), StrCaseEq(L"http://ftp.mozilla.cat/pub/firefox/releases/5.0.0/win64/ca/Firefox%20Setup%205.0.0.exe"));
}
