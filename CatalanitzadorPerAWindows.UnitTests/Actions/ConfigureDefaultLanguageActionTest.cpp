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
#include "WindowsLPIAction.h"
#include "ActionStatus.h"
#include "RegistryMock.h"
#include "OSVersionMock.h"
#include "RunnerMock.h"
#include "ConfigureDefaultLanguageAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define CreateConfigureDefaultLanguageAction \
	RegistryMock registryMockobj; \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	ConfigureDefaultLanguageAction defLanguageAction(&osVersionExMock, &registryMockobj, &runnerMock);


TEST(WindowsLPIActionTest, IsCatalanKeyboardActive_True)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"0403"), Return(true)));	
	
	EXPECT_EQ(true, defLanguageAction.IsCatalanKeyboardActive());
}

TEST(WindowsLPIActionTest, IsCatalanKeyboardActive_False)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"040a"), Return(true)));	
	
	EXPECT_EQ(false, defLanguageAction.IsCatalanKeyboardActive());
}

TEST(WindowsLPIActionTest, HasSpanishKeyboard_True)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"040a"), Return(true)));	
	
	EXPECT_EQ(true, defLanguageAction.HasSpanishKeyboard());
}

TEST(WindowsLPIActionTest, HasSpanishKeyboard_SecondTrue)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"0400"), Return(true)));	

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"2"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"040a"), Return(true)));
	
	EXPECT_EQ(true, defLanguageAction.HasSpanishKeyboard());
}


ACTION_P(ReadArgCharString, value)
{
	wcscpy_s(value, 255, arg1);
}

#define CATALAN_LANGCODE L"0403"
#define ANOTHER_LANGCODE L"040a"

TEST(WindowsLPIActionTest, MakeCatalanActiveKeyboard_XP)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), true)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(ANOTHER_LANGCODE), Return(true)));	

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"2"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"0400"), Return(true)));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"3"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(CATALAN_LANGCODE), Return(true)));

	wchar_t lang1[1024], lang3[1024];

	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"1"),_)).
		WillRepeatedly(DoAll(ReadArgCharString(lang1), Return(true)));

	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"3"),_)).
		WillRepeatedly(DoAll(ReadArgCharString(lang3), Return(true)));

	defLanguageAction.MakeCatalanActiveKeyboard();

	EXPECT_THAT(lang1, StrCaseEq(CATALAN_LANGCODE));
	EXPECT_THAT(lang3, StrCaseEq(ANOTHER_LANGCODE));
}