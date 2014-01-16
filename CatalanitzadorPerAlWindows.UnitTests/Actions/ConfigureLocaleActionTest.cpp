/* 
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

#define CreateConfigureLocaleAction \
	OSVersionMock osVersionMock; \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	ConfigureLocaleAction configureLocaleAction(&osVersionMock, &registryMockobj, &runnerMock);


void SetLocaleMockForLanguage(RegistryMock& registryMockobj, const wchar_t* language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\International"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Locale"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
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

TEST(ConfigureLocaleActionTest, Execute_WindowsXP)
{
	CreateConfigureLocaleAction;

	EXPECT_CALL(osVersionMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"\\control.exe"), HasSubstr(L"regopts.txt"), false)).Times(1).WillRepeatedly(Return(true));

	configureLocaleAction.Execute();
}

TEST(ConfigureLocaleActionTest, Execute_Windows7)
{
	CreateConfigureLocaleAction;

	EXPECT_CALL(osVersionMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"\\control.exe"), HasSubstr(L"regopts.xml"), false)).Times(1).WillRepeatedly(Return(true));

	configureLocaleAction.Execute();
}
