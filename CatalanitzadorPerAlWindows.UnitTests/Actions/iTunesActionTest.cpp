/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "iTunesAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::ReturnRefOfCopy;

class iTunesActionTest : public iTunesAction
{
public:
	
	iTunesActionTest(IRegistry* registry, IFileVersionInfo* fileVersionInfo)
		: iTunesAction(registry, fileVersionInfo) {};
	
	public:

		using iTunesAction::_isDefaultLanguageForUser;
		using iTunesAction::_isDefaultLanguage;
};


#define CreateiTunesAction \
	RegistryMock registryMockobj; \
	FileVersionInfoMock fileVersionInfoMock; \
	iTunesActionTest iTunes(&registryMockobj, &fileVersionInfoMock);

#define CATALAN_LANGCODE L"1027"
#define SPANISH_LANGCODE L"1034"

void _setMockForUserLanguage(RegistryMock& registryMockobj, const wchar_t* language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Apple Computer, Inc.\\iTunes"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"LangID"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
}

void _setMockForUserLanguageFalse(RegistryMock& registryMockobj)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Apple Computer, Inc.\\iTunes"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"LangID"),_ ,_)).WillRepeatedly(Return(false));
}

void _setMockForMachineLanguage(RegistryMock& registryMockobj, const wchar_t* language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Apple Computer, Inc.\\iTunes"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"InstalledLangID"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
}

void _setMockForProgramFolderTrue(RegistryMock& registryMockobj)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Apple Computer, Inc.\\iTunes"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"ProgramFolder"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(L"Folder"), Return(true)));
}

void _setiTunesVersion(FileVersionInfoMock& fileVersionInfoMock, wchar_t* version)
{
	wstring s(version);
	EXPECT_CALL(fileVersionInfoMock, GetVersion()).WillRepeatedly(ReturnRefOfCopy(s));
}

TEST(iTunesActionTest, _isDefaultLanguageForUser_True)
{
	CreateiTunesAction;

	_setMockForUserLanguage(registryMockobj, CATALAN_LANGCODE);	
	EXPECT_TRUE(iTunes._isDefaultLanguageForUser() == true);
}

TEST(iTunesActionTest, _isDefaultLanguageForUser_False)
{
	CreateiTunesAction;

	_setMockForUserLanguage(registryMockobj, SPANISH_LANGCODE);	
	EXPECT_FALSE(iTunes._isDefaultLanguageForUser() == true);
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineSpanish_UserCatalan_True)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, SPANISH_LANGCODE);
	_setMockForUserLanguage(registryMockobj, CATALAN_LANGCODE);
	EXPECT_TRUE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineCatalan_UserSpanish_False)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, CATALAN_LANGCODE);
	_setMockForUserLanguage(registryMockobj, SPANISH_LANGCODE);
	EXPECT_FALSE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineCatalan_UserNone_True)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, CATALAN_LANGCODE);
	_setMockForUserLanguageFalse(registryMockobj);
	EXPECT_TRUE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, CheckPrerequirements_OldVersion_False)
{
	CreateiTunesAction;

	_setMockForProgramFolderTrue(registryMockobj);
	_setiTunesVersion(fileVersionInfoMock, L"10.6.2.0");
	iTunes.CheckPrerequirements(NULL);	
	EXPECT_THAT(iTunes.GetStatus(), CannotBeApplied);
}