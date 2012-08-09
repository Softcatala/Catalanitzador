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

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::ReturnRef;
using ::testing::AnyNumber;
using ::testing::DoAll;

class WindowsLiveActionTest : public WindowsLiveAction
{
public:
	
	WindowsLiveActionTest::WindowsLiveActionTest(IRegistry* registry, IRunner* runner, IFileVersionInfo* fileVersionInfo)
		: WindowsLiveAction(registry, runner, fileVersionInfo) {};	
	
	public: using WindowsLiveAction::_getMajorVersion;
	public: using WindowsLiveAction::_isLangSelected;

};

#define CreateWindowsLiveAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMockobj; \
	FileVersionInfoMock fileVersionInfoMockobj; \
	WindowsLiveActionTest lipAction(&registryMockobj, &runnerMockobj, &fileVersionInfoMockobj);


TEST(WindowsLiveActionTest, GetVersion)
{
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	CreateWindowsLiveAction;
	
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(VERSION_STRING));
	EXPECT_CALL(fileVersionInfoMockobj, GetMajorVersion()).WillRepeatedly(Return(VERSION));
	EXPECT_THAT(lipAction.GetVersion(), StrCaseEq(VERSION_STRING));
	EXPECT_THAT(lipAction._getMajorVersion(), VERSION);
}

TEST(WindowsLiveActionTest, _isLangSelected_2011_Yes)
{
	const wchar_t* CATALAN_LANG = L"CA";
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	CreateWindowsLiveAction;	
	
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(VERSION_STRING));
	EXPECT_CALL(fileVersionInfoMockobj, GetMajorVersion()).WillRepeatedly(Return(VERSION));
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Windows Live\\Common\\"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"UserLanguage"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(CATALAN_LANG), Return(true)));
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(lipAction._isLangSelected());
	EXPECT_THAT(lipAction.GetStatus(), AlreadyApplied);	
}

TEST(WindowsLiveActionTest, _isLangSelected_2011_No)
{
	const wchar_t* CATALAN_LANG = L"CA";
	wstring VERSION_STRING = L"15.0.1";
	const int VERSION = 15;

	CreateWindowsLiveAction;	
	
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(VERSION_STRING));
	EXPECT_CALL(fileVersionInfoMockobj, GetMajorVersion()).WillRepeatedly(Return(VERSION));
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Windows Live\\Common\\"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"UserLanguage"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(L""), Return(true)));
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(lipAction._isLangSelected());
	EXPECT_THAT(lipAction.GetStatus(), NotSelected);
}

TEST(WindowsLiveActionTest, _isLangSelected_2009_Yes)
{
	const wchar_t* CATALAN_LANG = L"CA";
	wstring VERSION_STRING = L"14.0.1.4";
	const int VERSION = 14;
	const int CATALAN_WINLANGCODE = 3;

	CreateWindowsLiveAction;	
	
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(VERSION_STRING));
	EXPECT_CALL(fileVersionInfoMockobj, GetMajorVersion()).WillRepeatedly(Return(VERSION));
	EXPECT_CALL(fileVersionInfoMockobj, GetLanguageCode()).WillRepeatedly(Return(CATALAN_WINLANGCODE));	
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(lipAction._isLangSelected());
	EXPECT_THAT(lipAction.GetStatus(), AlreadyApplied);	
}

TEST(WindowsLiveActionTest, _isLangSelected_2009_No)
{
	const wchar_t* CATALAN_LANG = L"CA";
	wstring VERSION_STRING = L"14.0.1.4";
	const int VERSION = 14;	

	CreateWindowsLiveAction;	
	
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(VERSION_STRING));
	EXPECT_CALL(fileVersionInfoMockobj, GetMajorVersion()).WillRepeatedly(Return(VERSION));
	EXPECT_CALL(fileVersionInfoMockobj, GetLanguageCode()).WillRepeatedly(Return(0));	
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(lipAction._isLangSelected());
	EXPECT_THAT(lipAction.GetStatus(), NotSelected);	
}