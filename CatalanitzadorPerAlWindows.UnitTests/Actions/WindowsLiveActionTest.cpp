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
#include "WindowsLiveAction.h"
#include "ConfigurationInstance.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::ReturnRef;
using ::testing::AnyNumber;
using ::testing::DoAll;

class WindowsLiveActionTest : public testing::Test
{
public:
	
		virtual void TearDown()
		{
			ConfigurationInstance::Reset();
		}
};


class WindowsLiveActionForTest : public WindowsLiveAction
{
public:
	
	WindowsLiveActionForTest::WindowsLiveActionForTest(IRegistry* registry, IRunner* runner, IFileVersionInfo* fileVersionInfo, DownloadManager* downloadManager)
		: WindowsLiveAction(registry, runner, fileVersionInfo, downloadManager) {};

	public: using WindowsLiveAction::_getMajorVersion;
	public: using WindowsLiveAction::_isLangSelected;
	public: using WindowsLiveAction::IsPreRebootRequired;
	public: using WindowsLiveAction::_isDownloadAvailable;
};

#define CreateWindowsLiveAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMockobj; \
	FileVersionInfoMock fileVersionInfoMockobj; \
	WindowsLiveActionForTest lipAction(&registryMockobj, &runnerMockobj, &fileVersionInfoMockobj, &DownloadManager());

void SetGetMajorVersion(FileVersionInfoMock &fileVersionInfoMockobj, wstring& versionString, int version)
{
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(versionString));
	EXPECT_CALL(fileVersionInfoMockobj, GetMajorVersion()).WillRepeatedly(Return(version));
	EXPECT_CALL(fileVersionInfoMockobj, GetLanguageCode()).WillRepeatedly(Return(0));
}

TEST_F(WindowsLiveActionTest, GetVersion)
{
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	CreateWindowsLiveAction;
	
	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_THAT(lipAction.GetVersion(), StrCaseEq(VERSION_STRING));
	EXPECT_THAT(lipAction._getMajorVersion(), VERSION);
}

TEST_F(WindowsLiveActionTest, _isLangSelected_2011_Yes)
{
	const wchar_t* CATALAN_LANG = L"CA";
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	CreateWindowsLiveAction;
	
	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Windows Live\\Common\\"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"UserLanguage"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(CATALAN_LANG), Return(true)));
		
	EXPECT_TRUE(lipAction._isLangSelected());
}

TEST_F(WindowsLiveActionTest, _isLangSelected_2011_No)
{
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	CreateWindowsLiveAction;
	
	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Windows Live\\Common\\"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"UserLanguage"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(L""), Return(true)));	
	
	EXPECT_FALSE(lipAction._isLangSelected());
}

TEST_F(WindowsLiveActionTest, _isLangSelected_2009_Yes)
{
	wstring VERSION_STRING = L"14.0.1.4";
	const int VERSION = 14;
	const int CATALAN_WINLANGCODE = 3;

	CreateWindowsLiveAction;
	
	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_CALL(fileVersionInfoMockobj, GetLanguageCode()).WillRepeatedly(Return(CATALAN_WINLANGCODE));
	EXPECT_TRUE(lipAction._isLangSelected());
}

TEST_F(WindowsLiveActionTest, _isLangSelected_2009_No)
{	
	wstring VERSION_STRING = L"14.0.1.4";
	const int VERSION = 14;

	CreateWindowsLiveAction;
	
	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_FALSE(lipAction._isLangSelected());
}

TEST_F(WindowsLiveActionTest, _isRebootRequired_Live2009_No)
{
	const int VERSION = 14;
	wstring VERSION_STRING = L"14.0";
	CreateWindowsLiveAction;

	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumValue(_,_)).WillRepeatedly(Return(true));
	EXPECT_FALSE(lipAction.IsPreRebootRequired());
}

TEST_F(WindowsLiveActionTest, _isRebootRequired_Live2011_Yes)
{
	const int VERSION = 15;
	wstring VERSION_STRING = L"15.0";
	CreateWindowsLiveAction;

	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumValue(_,_)).WillRepeatedly(Return(true));
	EXPECT_TRUE(lipAction.IsPreRebootRequired());
}

TEST_F(WindowsLiveActionTest, _isRebootRequired_Live2012_Yes)
{
	const int VERSION = 16;
	wstring VERSION_STRING = L"16.0";
	CreateWindowsLiveAction;

	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumValue(_,_)).WillRepeatedly(Return(true));
	EXPECT_TRUE(lipAction.IsPreRebootRequired());
}

TEST_F(WindowsLiveActionTest, _isDownloadAvailable_No)
{
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	CreateWindowsLiveAction;
	ConfigurationRemote remote;

	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	ConfigurationInstance::Get().SetRemote(remote);
	EXPECT_FALSE(lipAction._isDownloadAvailable());
}

TEST_F(WindowsLiveActionTest, _isDownloadAvailable_Yes)
{
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	ConfigurationRemote remote;
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload;
	CreateWindowsLiveAction;
	
	SetGetMajorVersion(fileVersionInfoMockobj, VERSION_STRING, VERSION);
	fileActionDownload.SetMaxVersion(ApplicationVersion(L"15"));
	fileActionDownload.SetMinVersion(ApplicationVersion(L"15"));
	fileActionDownloads.SetActionID(WindowsLiveActionID);
	fileActionDownload.AddUrl(L"http://www.softcatala.org/");
	fileActionDownloads.AddFileActionDownload(fileActionDownload);
	remote.AddFileActionDownloads(fileActionDownloads);

	ConfigurationInstance::Get().SetRemote(remote);
	EXPECT_TRUE(lipAction._isDownloadAvailable());
}