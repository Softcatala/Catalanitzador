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
#include "ConfigureDefaultLanguageAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define CATALAN_LANGCODE L"0403"
#define SPANISH_LANGCODE L"040a"
#define SLOVAK_LANGCODE L"041B"

class ConfigureDefaultLanguageActionTest : public ConfigureDefaultLanguageAction
{
public:
	
	ConfigureDefaultLanguageActionTest::ConfigureDefaultLanguageActionTest(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner)
		: ConfigureDefaultLanguageAction(OSVersion, registry, runner) {};

	public: using ConfigureDefaultLanguageAction::_isCatalanKeyboardActive;
	public: using ConfigureDefaultLanguageAction::_hasSpanishKeyboard;
	public: using ConfigureDefaultLanguageAction::_makeCatalanActiveKeyboard;	
};

#define CreateConfigureDefaultLanguageAction \
	RegistryMock registryMockobj; \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	ConfigureDefaultLanguageActionTest defLanguageAction(&osVersionExMock, &registryMockobj, &runnerMock);

void SetKeyboardLayout(RegistryMock& registryMockobj, wchar_t* lang)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(lang), Return(true)));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"2"),_ ,_)).
		WillRepeatedly(Return(false));
}

TEST(ConfigureDefaultLanguageActionTest, _isCatalanKeyboardActive_True)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	SetKeyboardLayout(registryMockobj, CATALAN_LANGCODE);
	
	EXPECT_TRUE(defLanguageAction._isCatalanKeyboardActive());
}

TEST(ConfigureDefaultLanguageActionTest, _isCatalanKeyboardActive_False)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	SetKeyboardLayout(registryMockobj, SPANISH_LANGCODE);
	
	EXPECT_FALSE(defLanguageAction._isCatalanKeyboardActive());
}

TEST(ConfigureDefaultLanguageActionTest, _hasSpanishKeyboard_True)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	SetKeyboardLayout(registryMockobj, SPANISH_LANGCODE);
	
	EXPECT_TRUE(defLanguageAction._hasSpanishKeyboard());
}

TEST(ConfigureDefaultLanguageActionTest, _hasSpanishKeyboard_SecondTrue)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(SLOVAK_LANGCODE), Return(true)));	

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"2"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(SPANISH_LANGCODE), Return(true)));
	
	EXPECT_TRUE(defLanguageAction._hasSpanishKeyboard());
}


TEST(ConfigureDefaultLanguageActionTest, MakeCatalanActiveKeyboard_XP)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Keyboard Layout\\Preload"), true)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"1"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(SPANISH_LANGCODE), Return(true)));	

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"2"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(SLOVAK_LANGCODE), Return(true)));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"3"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(CATALAN_LANGCODE), Return(true)));

	wchar_t lang1[1024], lang3[1024];

	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"1"),_)).
		WillRepeatedly(DoAll(ReadArgCharString(lang1), Return(true)));

	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"3"),_)).
		WillRepeatedly(DoAll(ReadArgCharString(lang3), Return(true)));

	defLanguageAction._makeCatalanActiveKeyboard();

	EXPECT_THAT(lang1, StrCaseEq(CATALAN_LANGCODE));
	EXPECT_THAT(lang3, StrCaseEq(SPANISH_LANGCODE));
}

TEST(ConfigureDefaultLanguageActionTest, _isNeeded_True_AlreadyApplied)
{
	CreateConfigureDefaultLanguageAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	SetKeyboardLayout(registryMockobj, CATALAN_LANGCODE);
	
	defLanguageAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(defLanguageAction.IsNeed());
	EXPECT_THAT(defLanguageAction.GetStatus(), AlreadyApplied);
}

TEST(ConfigureDefaultLanguageActionTest, _isNeeded_False_CannotBeApplied)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	SetKeyboardLayout(registryMockobj, SLOVAK_LANGCODE);
	
	defLanguageAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(defLanguageAction.IsNeed());
	EXPECT_THAT(defLanguageAction.GetStatus(), CannotBeApplied);
}

TEST(ConfigureDefaultLanguageActionTest, _isNeeded_True)
{
	CreateConfigureDefaultLanguageAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	SetKeyboardLayout(registryMockobj, SPANISH_LANGCODE);
	
	defLanguageAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(defLanguageAction.IsNeed());	
}