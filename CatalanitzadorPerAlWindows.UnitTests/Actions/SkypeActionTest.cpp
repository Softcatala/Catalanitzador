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
#include "SkypeAction.h"
#include "RegistryMock.h"
#include "FileVersionInfoMock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::ReturnRef;
using ::testing::AnyNumber;
using ::testing::DoAll;

class SkypeActionTest : public SkypeAction
{
public:	
	
	SkypeActionTest::SkypeActionTest(IRegistry* registry, IFileVersionInfo* fileVersionInfo)
		: SkypeAction(registry, fileVersionInfo)
	{
		m_defaultLanguage = false;
	}

	bool _isDefaultLanguage()
	{
		return m_defaultLanguage;
	}
	
	public: 
			using SkypeAction::_isDefaultInstallLanguage;
			using SkypeAction::_getProgramLocation;
			bool m_defaultLanguage;

};


#define CreateSkypeAction \
	RegistryMock registryMockobj; \
	FileVersionInfoMock fileVersionInfoMock; \
	SkypeActionTest skypeAction(&registryMockobj, &fileVersionInfoMock);

#define PROGRAM_REGKEY L"Software\\Skype\\Phone\\UI\\General"
#define PROGRAM_REGKEY_LOCATION L"Software\\Skype\\Phone"

void _setMockForDefaultLanguage(RegistryMock& registryMockobj, const wchar_t* language)
{	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, PROGRAM_REGKEY, false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Language"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
}

void _setProgramLocation(RegistryMock& registryMockobj, const wchar_t* location)
{	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, PROGRAM_REGKEY_LOCATION, false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"SkypePath"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(location), Return(true)));
}

void SetFileVersion(FileVersionInfoMock &fileVersionInfoMockobj, wstring& versionString, int version)
{
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(versionString));
	EXPECT_CALL(fileVersionInfoMockobj, GetMajorVersion()).WillRepeatedly(Return(version));
	EXPECT_CALL(fileVersionInfoMockobj, GetLanguageCode()).WillRepeatedly(Return(0));
}

TEST(SkypeActionTest, _isDefaultInstallLanguage_Yes)
{
	const wchar_t* LANGUAGE = L"ca";

	CreateSkypeAction;
	_setMockForDefaultLanguage(registryMockobj, LANGUAGE);
	
	EXPECT_TRUE(skypeAction._isDefaultInstallLanguage());
}

TEST(SkypeActionTest, _isDefaultInstallLanguage_No)
{
	const wchar_t* LANGUAGE = L"es";

	CreateSkypeAction;
	_setMockForDefaultLanguage(registryMockobj, LANGUAGE);
	
	EXPECT_FALSE(skypeAction._isDefaultInstallLanguage());
}

TEST(SkypeActionTest, CheckPrerequirements_NotInstalled)
{
	wstring VERSION_STRING = L"";
	const int VERSION = 0;

	CreateSkypeAction;

	_setProgramLocation(registryMockobj, L"None");
	SetFileVersion(fileVersionInfoMock, VERSION_STRING, VERSION);
	skypeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(skypeAction.GetStatus(), NotInstalled);
}

TEST(SkypeActionTest, _getProgramLocation)
{
	wstring PATH = L"/location/file.exe";
	wstring location;

	CreateSkypeAction;
	_setProgramLocation(registryMockobj, PATH.c_str());
	skypeAction._getProgramLocation(location);
	
	EXPECT_TRUE(location == PATH);
}

TEST(SkypeActionTest, CheckPrerequirements_NotSelected)
{
	wstring VERSION_STRING = L"6.0.0.0";
	const int VERSION = 6;

	CreateSkypeAction;
	_setProgramLocation(registryMockobj, L"None");
	SetFileVersion(fileVersionInfoMock, VERSION_STRING, VERSION);
	skypeAction.CheckPrerequirements(NULL);

	EXPECT_TRUE(skypeAction.GetStatus() == NotSelected);
}

TEST(SkypeActionTest, CheckPrerequirements_CannotBeApplied)
{
	wstring VERSION_STRING = L"5.0.0.0";
	const int VERSION = 5;

	CreateSkypeAction;
	_setProgramLocation(registryMockobj, L"None");
	SetFileVersion(fileVersionInfoMock, VERSION_STRING, VERSION);
	skypeAction.CheckPrerequirements(NULL);
	
	EXPECT_THAT(skypeAction.GetStatus(), CannotBeApplied);
}

TEST(SkypeActionTest, CheckPrerequirements_AlreadyApplied)
{
	wstring VERSION_STRING = L"6.0.0.0";
	const int VERSION = 6;

	CreateSkypeAction;
	_setProgramLocation(registryMockobj, L"None");
	SetFileVersion(fileVersionInfoMock, VERSION_STRING, VERSION);
	skypeAction.m_defaultLanguage = true;
	skypeAction.CheckPrerequirements(NULL);

	EXPECT_TRUE(skypeAction.GetStatus() == AlreadyApplied);
}

