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
#include "MSOffice.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::SetArgPointee;

#define CATALAN_LANGUAGE_CODE L"1027"
#define VALENCIAN_LANGUAGE_CODE L"2051"
DWORD CATALAN_LCID  = _wtoi(CATALAN_LANGUAGE_CODE);
DWORD VALENCIAN_LCID = _wtoi(VALENCIAN_LANGUAGE_CODE);
DWORD SPANISH_LCID = 1034;


class MSOfficeTest : public MSOffice
{
public:

	MSOfficeTest::MSOfficeTest(IRegistry* registry, IRunner* runner)
		: MSOffice(registry, runner) {};

	using MSOffice::_setDefaultLanguage;
	using MSOffice::_isDefaultLanguage;
};

#define CreateMSoffice \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	MSOfficeTest office(&registryMockobj, &runnerMock);

extern void MockOfficeInstalled(RegistryMock& registryMockobj, MSOfficeVersion version);
extern void SetLocaleMockForLanguage(RegistryMock& registryMockobj, const wchar_t* language);

void SetLocaleMockForIsDefaultLanguage(RegistryMock& registryMockobj, bool FollowSystemUIOff, DWORD language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"FollowSystemUI"),_ ,_)).
			WillRepeatedly(DoAll(SetArgCharStringPar2(FollowSystemUIOff ? L"Off" : L"On"), Return(true)));	
	EXPECT_CALL(registryMockobj, GetDWORD(StrCaseEq(L"UILanguage"),_)).
		WillRepeatedly(DoAll(SetArgPointee<1>(language), Return(true)));
}

TEST(MSOfficeTest, _isVersionInstalled_2003)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2003);
	EXPECT_EQ(office._readVersionInstalled(), MSOffice2003);
}

TEST(MSOfficeTest, _isVersionInstalled_2007)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2007);
	EXPECT_THAT(office._readVersionInstalled(), MSOffice2007);
}

TEST(MSOfficeTest, _isVersionInstalled_2010)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2010);
	EXPECT_THAT(office._readVersionInstalled(), MSOffice2010);
}

TEST(MSOfficeTest, _isVersionInstalled_2010_64)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2010_64);
	EXPECT_THAT(office._readVersionInstalled(), MSOffice2010_64);
}

TEST(MSOfficeTest, _isVersionInstalled_2013_64)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2013_64);
	EXPECT_THAT(office._readVersionInstalled(), MSOffice2013_64);
}



TEST(MSOfficeTest, _setDefaultLanguage_2003)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2003);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\11.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(0).WillRepeatedly(Return(true));
	office._setDefaultLanguage(MSOffice2003);	
}

TEST(MSOfficeTest, _setDefaultLanguage_2007)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2007);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\12.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setDefaultLanguage(MSOffice2007);
}

TEST(MSOfficeTest, _setDefaultLanguage_2010)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2010);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setDefaultLanguage(MSOffice2010);
}

TEST(MSOfficeTest, _setDefaultLanguage_2010_64)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2010_64);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setDefaultLanguage(MSOffice2010_64);
}

TEST(MSOfficeTest, _setDefaultLanguage_2013)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2013);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setDefaultLanguage(MSOffice2013);
}

TEST(MSOfficeTest, _setDefaultLanguage_2013_64)
{
	CreateMSoffice;

	MockOfficeInstalled(registryMockobj, MSOffice2013_64);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setDefaultLanguage(MSOffice2013_64);
}

TEST(MSOfficeTest, _isDefaultLanguage_True_YesFollowOffAndUiCatalan)
{
	CreateMSoffice;
	bool FollowSystemUIOff = true;
	MockOfficeInstalled(registryMockobj, MSOffice2013);
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);
	
	EXPECT_TRUE(office._isDefaultLanguage(MSOffice2013));
}

TEST(MSOfficeTest, _isDefaultLanguage_True_YesFollowOffAndUiSpanish)
{
	CreateMSoffice;
	bool FollowSystemUIOff = true;
	MockOfficeInstalled(registryMockobj, MSOffice2013);
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, SPANISH_LCID);
	
	EXPECT_FALSE(office._isDefaultLanguage(MSOffice2013));
}

TEST(MSOfficeTest, _isDefaultLanguage_True_NoFollowOffAndUiCatalan)
{
	CreateMSoffice;
	bool FollowSystemUIOff = false;
	MockOfficeInstalled(registryMockobj, MSOffice2013);
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);
	SetLocaleMockForLanguage(registryMockobj, L"0xc0a");
		
	EXPECT_FALSE(office._isDefaultLanguage(MSOffice2013));
}