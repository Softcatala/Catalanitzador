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

	MSOfficeTest::MSOfficeTest(IRegistry* registry, IRunner* runner, MSOfficeVersion version)
		: MSOffice(registry, runner, version) {};

	using MSOffice::_getDownloadID;	
};

#define CreateMSoffice(version) \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	MSOfficeTest office(&registryMockobj, &runnerMock, version);

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

void SetLangPacksInstalled(RegistryMock& registryMockobj, MSOfficeVersion version)
{	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\LanguageResources\\ParentFallback"), false)).
		WillRepeatedly(Return(version == MSOffice2003));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\LanguageResources\\InstalledUIs"), false)).
		WillRepeatedly(Return(version == MSOffice2007));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\LanguageResources\\InstalledUIs"), false)).
		WillRepeatedly(Return(version == MSOffice2010));

	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\LanguageResources\\InstalledUIs"), false)).
		WillRepeatedly(Return(version == MSOffice2010_64));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\LanguageResources\\InstalledUIs"), false)).
		WillRepeatedly(Return(version == MSOffice2013));

	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\LanguageResources\\InstalledUIs"), false)).
		WillRepeatedly(Return(version == MSOffice2013_64));

	if (version == MSOffice2003)
	{
		EXPECT_CALL(registryMockobj, GetDWORD(StrCaseEq(CATALAN_LANGUAGE_CODE),_)).WillRepeatedly(Return(true));
	}
	else if (version != MSOfficeUnKnown && version != NoMSOffice)
	{
		EXPECT_CALL(registryMockobj, GetString(StrCaseEq(CATALAN_LANGUAGE_CODE),_,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(L"1"), Return(true)));
		EXPECT_CALL(registryMockobj, GetString(StrCaseEq(VALENCIAN_LANGUAGE_CODE),_,_)).WillRepeatedly(Return(false));
	}
}


TEST(MSOfficeTest, SetDefaultLanguage_2003)
{
	CreateMSoffice(MSOffice2003);
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\11.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(0);
	office.SetDefaultLanguage();
}

TEST(MSOfficeTest, SetDefaultLanguage_2007)
{
	CreateMSoffice(MSOffice2007);
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\12.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office.SetDefaultLanguage();
}

TEST(MSOfficeTest, SetDefaultLanguage_2010)
{
	CreateMSoffice(MSOffice2010);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office.SetDefaultLanguage();
}

TEST(MSOfficeTest, SetDefaultLanguage_2010_64)
{
	CreateMSoffice(MSOffice2010_64);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office.SetDefaultLanguage();
}

TEST(MSOfficeTest, SetDefaultLanguage_2013)
{
	CreateMSoffice(MSOffice2013);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office.SetDefaultLanguage();
}

TEST(MSOfficeTest, SetDefaultLanguage_2013_64)
{
	CreateMSoffice(MSOffice2013_64);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office.SetDefaultLanguage();
}

TEST(MSOfficeTest, IsDefaultLanguage_True_YesFollowOffAndUiCatalan)
{
	CreateMSoffice(MSOffice2013);

	bool FollowSystemUIOff = true;
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);
	EXPECT_TRUE(office.IsDefaultLanguage());
}

TEST(MSOfficeTest, IsDefaultLanguage_True_YesFollowOffAndUiSpanish)
{
	CreateMSoffice(MSOffice2013);

	bool FollowSystemUIOff = true;	
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, SPANISH_LCID);	
	EXPECT_FALSE(office.IsDefaultLanguage());
}

TEST(MSOfficeTest, IsDefaultLanguage_True_NoFollowOffAndUiCatalan)
{
	CreateMSoffice(MSOffice2013);

	bool FollowSystemUIOff = false;
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);
	SetLocaleMockForLanguage(registryMockobj, L"0xc0a");
	EXPECT_FALSE(office.IsDefaultLanguage());
}

TEST(MSOfficeTest, _getDownloadID_MSOffice2003)
{
	CreateMSoffice(MSOffice2003);	
	EXPECT_EQ(office._getDownloadID(), L"2003");
}

TEST(MSOfficeTest, _getDownloadID_MSOffice2007)
{
	CreateMSoffice(MSOffice2007);	
	EXPECT_EQ(office._getDownloadID(), L"2007");
}

TEST(MSOfficeTest, _getDownloadID_MSOffice2010_32)
{
	CreateMSoffice(MSOffice2010);	
	EXPECT_EQ(office._getDownloadID(), L"2010_32");
}
TEST(MSOfficeTest, _getDownloadID_MSOffice2010_64)
{
	CreateMSoffice(MSOffice2010_64);	
	EXPECT_EQ(office._getDownloadID(), L"2010_64");
}

TEST(MSOfficeTest, _getDownloadID_MSOffice2013_32)
{
	CreateMSoffice(MSOffice2013);	
	EXPECT_EQ(office._getDownloadID(), L"2013_ca_32");
}
TEST(MSOfficeTest, _getDownloadID_MSOffice2013_64)
{
	CreateMSoffice(MSOffice2013_64);	
	EXPECT_EQ(office._getDownloadID(), L"2013_ca_64");
}

TEST(MSOfficeTest, _getDownloadID_MSOffice2013_32_va)
{
	CreateMSoffice(MSOffice2013);	
	office.SetUseDialectalVariant(true);
	EXPECT_EQ(office._getDownloadID(), L"2013_va_32");
}

TEST(MSOfficeTest, _getDownloadID_MSOffice2013_64_va)
{
	CreateMSoffice(MSOffice2013_64);	
	office.SetUseDialectalVariant(true);
	EXPECT_EQ(office._getDownloadID(), L"2013_va_64");
}

TEST(MSOfficeTest, IsLangPackInstalled_2003)
{
	CreateMSoffice(MSOffice2003);
	
	SetLangPacksInstalled(registryMockobj, MSOffice2003);	
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST(MSOfficeTest, IsLangPackInstalled_2007)
{
	CreateMSoffice(MSOffice2007);
	
	SetLangPacksInstalled(registryMockobj, MSOffice2007);
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST(MSOfficeTest, IsLangPackInstalled_2010)
{
	CreateMSoffice(MSOffice2010);

	SetLangPacksInstalled(registryMockobj, MSOffice2010);	
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST(MSOfficeTest, IsLangPackInstalled_2013)
{
	CreateMSoffice(MSOffice2013);

	SetLangPacksInstalled(registryMockobj, MSOffice2013);	
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST(MSOfficeTest, IsLangPackInstalled_2013_64)
{
	CreateMSoffice(MSOffice2013_64);
	
	SetLangPacksInstalled(registryMockobj, MSOffice2013_64);
	EXPECT_TRUE(office.IsLangPackInstalled());
}
