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
#include "Application.h"
#include "ChromeProfileMock.h"
#include "ChromeAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;

class ChromeActionTest : public ChromeAction
{
public:

	ChromeActionTest(IRegistry* registry, IOSVersion* OSVersion) : ChromeAction(registry, OSVersion) {};

	public:

		vector <ExecutionProcess> GetExecutionProcesses()
		{
			return m_processes;
		}

		using ChromeAction::_readVersion;
		using ChromeAction::_isInstalled;
		using ChromeAction::_readLanguageCode;
		using ChromeAction::_addExecutionProcess64OrAnd32;
};


#define CreateChromeAction \
	RegistryMock registryMockobj; \
	OSVersionMock osVersionMock; \
	ChromeActionTest chromeAction(&registryMockobj, &osVersionMock);

void SetLocation(RegistryMock& registryMockobj, const wchar_t* location)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(CHROME_REGISTRY_PATH), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"InstallLocation"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(location), Return(true)));
}

void SetVersion(RegistryMock& registryMockobj, const wchar_t* version)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(CHROME_REGISTRY_PATH), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Version"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(version), Return(true)));
}

TEST(ChromeActionTest, _readVersion)
{
	CreateChromeAction;
	const wchar_t* VERSION = L"15.0";
	
	SetVersion(registryMockobj, VERSION);
	chromeAction._readVersion();
	EXPECT_THAT(chromeAction.GetVersion(), StrCaseEq(VERSION));
}

TEST(ChromeActionTest, _isInstalled_True)
{
	CreateChromeAction;
	
	SetLocation(registryMockobj, L"SomePath");
	EXPECT_TRUE(chromeAction._isInstalled());
}

TEST(ChromeActionTest, _isInstalled_False)
{
	CreateChromeAction;
	
	SetLocation(registryMockobj, L"");
	EXPECT_FALSE(chromeAction._isInstalled());
}

TEST(ChromeActionTest, _isInstalled_True_System)
{
	CreateChromeAction;
	wchar_t* LOCATION = L"location";
	
	// No user install
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(CHROME_REGISTRY_PATH), false)).WillRepeatedly(Return(false));

	// There is system install
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CHROME_REGISTRY_PATH), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"InstallLocation"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(LOCATION), Return(true)));

	EXPECT_TRUE(chromeAction._isInstalled());
}

TEST(ChromeActionTest, CheckPrerequirements_NotInstalled)
{
	CreateChromeAction;
	
	SetLocation(registryMockobj, L"");
	chromeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(chromeAction.GetStatus(), NotInstalled);
}

TEST(ChromeActionTest, CheckPrerequirements_AlreadyApplied)
{
	CreateChromeAction;
	const wchar_t* VERSION = L"15.0";
	
	SetLocation(registryMockobj, L"SomePath");	
	SetVersion(registryMockobj, VERSION);

	ChromeProfileMock chromeProfileMock;
	EXPECT_CALL(chromeProfileMock, IsAcceptLanguagesOk()).WillRepeatedly(Return(true));
	EXPECT_CALL(chromeProfileMock, IsUiLocaleOk()).WillRepeatedly(Return(true));
	chromeAction.SetChromeProfile(&chromeProfileMock);

	chromeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(chromeAction.GetStatus(), AlreadyApplied);
}

TEST(ChromeActionTest, CheckPrerequirements_UINotApplied)
{
	CreateChromeAction;
	const wchar_t* VERSION = L"15.0";
	
	SetLocation(registryMockobj, L"SomePath");	
	SetVersion(registryMockobj, VERSION);

	ChromeProfileMock chromeProfileMock;
	EXPECT_CALL(chromeProfileMock, IsAcceptLanguagesOk()).WillRepeatedly(Return(true));
	EXPECT_CALL(chromeProfileMock, IsUiLocaleOk()).WillRepeatedly(Return(false));
	chromeAction.SetChromeProfile(&chromeProfileMock);

	chromeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(chromeAction.GetStatus(), NotSelected);
	EXPECT_THAT(chromeAction.IsNeed(), true);
}

TEST(ChromeActionTest, CheckPrerequirements_AcceptLanguagesNotApplied)
{
	CreateChromeAction;
	const wchar_t* VERSION = L"15.0";
	
	SetLocation(registryMockobj, L"SomePath");	
	SetVersion(registryMockobj, VERSION);

	ChromeProfileMock chromeProfileMock;
	EXPECT_CALL(chromeProfileMock, IsAcceptLanguagesOk()).WillRepeatedly(Return(false));
	EXPECT_CALL(chromeProfileMock, IsUiLocaleOk()).WillRepeatedly(Return(true));
	chromeAction.SetChromeProfile(&chromeProfileMock);

	chromeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(chromeAction.GetStatus(), NotSelected);
	EXPECT_THAT(chromeAction.IsNeed(), true);
}

TEST(ChromeActionTest, addExecutionProcess64OrAnd32_32bits)
{
	CreateChromeAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	chromeAction._addExecutionProcess64OrAnd32();

	vector <ExecutionProcess> executionProcesses = chromeAction.GetExecutionProcesses();
	EXPECT_THAT(executionProcesses.size(), 1);
	EXPECT_THAT(executionProcesses[0].Is64Bits(), false);
}

TEST(ChromeActionTest, addExecutionProcess64OrAnd32_64bits)
{
	CreateChromeAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	chromeAction._addExecutionProcess64OrAnd32();

	vector <ExecutionProcess> executionProcesses = chromeAction.GetExecutionProcesses();
	EXPECT_THAT(executionProcesses.size(), 2);
	EXPECT_THAT(executionProcesses[0].Is64Bits(), false);
	EXPECT_THAT(executionProcesses[1].Is64Bits(), true);
}
