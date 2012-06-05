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
#include "RegistryMock.h"
#include "ChromeAction.h"
#include "Application.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;

class ChromeActionTest : public ChromeAction
{
public:

	ChromeActionTest(IRegistry* registry) : ChromeAction(registry) {};

	public:

		using ChromeAction::_readVersion;
		using ChromeAction::_isInstalled;
};

#define CreateChromeAction \
	RegistryMock registryMockobj; \
	ChromeActionTest chromeAction(&registryMockobj);

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

TEST(ChromeActionTest, CheckPrerequirements_NotInstalled)
{
	CreateChromeAction;
	
	SetLocation(registryMockobj, L"");
	chromeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(chromeAction.GetStatus(), NotInstalled);
}

TEST(ChromeActionTest, IsNeed_No_SpanishUI_NoAcceptLanguage)
{
	wstring location;
	CreateChromeAction;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_NoAcceptLanguage\\User Data\\";

	SetVersion(registryMockobj, L"11.0");
	SetLocation(registryMockobj, location.c_str());
	chromeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(chromeAction.IsNeed());
}

