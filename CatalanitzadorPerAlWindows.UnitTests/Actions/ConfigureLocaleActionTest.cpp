﻿/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "RunnerMock.h"
#include "RegistryMock.h"
#include "OSVersionMock.h"
#include "ConfigureLocaleAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::StrCaseEq;
using ::testing::HasSubstr;
using ::testing::Assign;

class ConfigureLocaleActionTest : public ConfigureLocaleAction
{
public:
	
	ConfigureLocaleActionTest::ConfigureLocaleActionTest(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner)
		: ConfigureLocaleAction(OSVersion, registry, runner) {};
	
	public:
			using ConfigureLocaleAction::_userLocaleFromLanguageProfileOptOut;
};

#define CreateConfigureLocaleAction \
	OSVersionMock osVersionMock; \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	ConfigureLocaleActionTest configureLocaleAction(&osVersionMock, &registryMockobj, &runnerMock);


void SetLocaleMockForLanguage(RegistryMock& registryMockobj, const wchar_t* language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\International"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Locale"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
}

void SetUserLocaleFromLanguageProfileOptOutMock(RegistryMock& registryMockobj, bool &rslt)
{	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\International\\User Profile"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UserLocaleFromLanguageProfileOptOut"), _)).
		WillRepeatedly(DoAll(Assign(&rslt, true), Return(true)));
}

TEST(ConfigureLocaleActionTest, IsCatalanLocaleActive_Yes)
{
	CreateConfigureLocaleAction;

	SetLocaleMockForLanguage(registryMockobj, L"0403");
	EXPECT_TRUE(configureLocaleAction.IsCatalanLocaleActive());
}

TEST(ConfigureLocaleActionTest, IsCatalanLocaleActive_No_Spanish)
{
	CreateConfigureLocaleAction;

	SetLocaleMockForLanguage(registryMockobj, L"0C0A");
	EXPECT_FALSE(configureLocaleAction.IsCatalanLocaleActive());
}

TEST(ConfigureLocaleActionTest, Execute_Windows7)
{
	CreateConfigureLocaleAction;

	EXPECT_CALL(osVersionMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"\\control.exe"), HasSubstr(L"regopts.xml"), false)).Times(1).WillRepeatedly(Return(true));

	configureLocaleAction.Execute();
}


TEST(ConfigureLocaleActionTest, _userLocaleFromLanguageProfileOptOut_W8)
{
	CreateConfigureLocaleAction;
	bool rslt = false;

	EXPECT_CALL(osVersionMock, GetVersion()).WillRepeatedly(Return(Windows8));
	SetUserLocaleFromLanguageProfileOptOutMock(registryMockobj, rslt);

	configureLocaleAction._userLocaleFromLanguageProfileOptOut();
	EXPECT_TRUE(rslt);
}

