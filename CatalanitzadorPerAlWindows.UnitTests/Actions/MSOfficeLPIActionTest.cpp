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
#include "MSOfficeLPIAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;


using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::HasSubstr;
using ::testing::DoAll;
using ::testing::Eq;

#define CATALAN_LANGUAGE_CODE L"1027"
#define VALENCIAN_LANGUAGE_CODE L"2051"
DWORD CATALAN_LCID  = _wtoi(CATALAN_LANGUAGE_CODE);
DWORD VALENCIAN_LCID = _wtoi(VALENCIAN_LANGUAGE_CODE);

class MSOfficeLPIActionTest : public MSOfficeLPIAction
{
public:

	MSOfficeLPIActionTest::MSOfficeLPIActionTest(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager)
		: MSOfficeLPIAction(registry, runner, downloadManager) {};
	
	using MSOfficeLPIAction::_getVersionInstalled;
	using MSOfficeLPIAction::_isLangPackInstalled;
	using MSOfficeLPIAction::_getDownloadID;
	using MSOfficeLPIAction::_setDefaultLanguage;
};

#define CreateMSOfficeAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	MSOfficeLPIActionTest officeAction(&registryMockobj, &runnerMock, &DownloadManager());

void MockOfficeInstalled(RegistryMock& registryMockobj, MSOfficeVersion version)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2003));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2007));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2010));

	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2010_64));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2013));
	
	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2013_64));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"SomePath"), Return(true)));
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

TEST(MSOfficeLPIActionTest, _isVersionInstalled_2003)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2003);
	EXPECT_EQ(officeAction._getVersionInstalled(), MSOffice2003);
}

TEST(MSOfficeLPIActionTest, _isVersionInstalled_2007)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2007);
	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2007);
}

TEST(MSOfficeLPIActionTest, _isVersionInstalled_2010)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010);
	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2010);
}

TEST(MSOfficeLPIActionTest, _isVersionInstalled_2010_64)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010_64);
	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2010_64);
}

TEST(MSOfficeLPIActionTest, _isVersionInstalled_2013_64)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2013_64);
	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2013_64);
}

TEST(MSOfficeLPIActionTest, _isLangPackInstalled_2003)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2003);
	SetLangPacksInstalled(registryMockobj, MSOffice2003);

	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2003);
	EXPECT_TRUE(officeAction._isLangPackInstalled());
}

TEST(MSOfficeLPIActionTest, _isLangPackInstalled_2007)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2007);
	SetLangPacksInstalled(registryMockobj, MSOffice2007);

	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2007);
	EXPECT_TRUE(officeAction._isLangPackInstalled());
}

TEST(MSOfficeLPIActionTest, _isLangPackInstalled_2010)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010);	
	SetLangPacksInstalled(registryMockobj, MSOffice2010);

	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2010);
	EXPECT_TRUE(officeAction._isLangPackInstalled());
}

TEST(MSOfficeLPIActionTest, _isLangPackInstalled_2013)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2013);	
	SetLangPacksInstalled(registryMockobj, MSOffice2013);

	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2013);
	EXPECT_TRUE(officeAction._isLangPackInstalled());
}

TEST(MSOfficeLPIActionTest, _isLangPackInstalled_2013_64)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2013_64);	
	SetLangPacksInstalled(registryMockobj, MSOffice2013_64);

	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2013_64);
	EXPECT_TRUE(officeAction._isLangPackInstalled());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2003_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2003);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);

	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2007_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2007);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2010_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_201064_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010_64);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_NotInstalled)
{
	CreateMSOfficeAction;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	
	officeAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(officeAction.IsNeed());
	EXPECT_EQ(officeAction.GetStatus(), NotInstalled);
}

TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2003)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2003);
	EXPECT_EQ(officeAction._getDownloadID(), L"2003");
}


TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2007)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2007);
	EXPECT_EQ(officeAction._getDownloadID(), L"2007");
}

TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2010_32)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2010);
	EXPECT_EQ(officeAction._getDownloadID(), L"2010_32");
}
TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2010_64)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2010_64);
	EXPECT_EQ(officeAction._getDownloadID(), L"2010_64");
}

TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2013_32)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2013);
	EXPECT_EQ(officeAction._getDownloadID(), L"2013_ca_32");
}
TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2013_64)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2013_64);
	EXPECT_EQ(officeAction._getDownloadID(), L"2013_ca_64");
}

TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2013_32_va)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2013);
	officeAction.SetUseDialectalVariant(true);
	EXPECT_EQ(officeAction._getDownloadID(), L"2013_va_32");
}
TEST(MSOfficeLPIActionTest, _getDownloadID_MSOffice2013_64_va)
{
	CreateMSOfficeAction;
	MockOfficeInstalled(registryMockobj, MSOffice2013_64);
	officeAction.SetUseDialectalVariant(true);
	EXPECT_EQ(officeAction._getDownloadID(), L"2013_va_64");
}

TEST(MSOfficeLPIActionTest, _setDefaultLanguage_2003)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2003);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\11.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(0).WillRepeatedly(Return(true));
	officeAction._setDefaultLanguage();	
}

TEST(MSOfficeLPIActionTest, _setDefaultLanguage_2007)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2007);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\12.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	officeAction._setDefaultLanguage();
}

TEST(MSOfficeLPIActionTest, _setDefaultLanguage_2010)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	officeAction._setDefaultLanguage();
}

TEST(MSOfficeLPIActionTest, _setDefaultLanguage_2010_64)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010_64);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	officeAction._setDefaultLanguage();
}

TEST(MSOfficeLPIActionTest, _setDefaultLanguage_2013)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2013);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	officeAction._setDefaultLanguage();
}

TEST(MSOfficeLPIActionTest, _setDefaultLanguage_2013_64)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2013_64);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	officeAction._setDefaultLanguage();
}