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
#include "MSOffice.h"
#include "ConfigurationRemote.h"
#include "ConfigurationInstance.h"
#include "ConfigurationFileActionDownload.h"
#include "ConfigurationFileActionDownloads.h"


using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::SetArgPointee;
using ::testing::StrCaseNe;

#define CATALAN_LANGUAGE_CODE L"1027"
#define VALENCIAN_LANGUAGE_CODE L"2051"
#define OFFICE2016_LAPKEY L"SOFTWARE\\Microsoft\\Office\\16.0\\Common\\LanguageResources"

DWORD CATALAN_LCID  = _wtoi(CATALAN_LANGUAGE_CODE);
DWORD VALENCIAN_LCID = _wtoi(VALENCIAN_LANGUAGE_CODE);
DWORD NOFOLLOWSYSTEMOFF_FALSE_LCID_2003 = 0;
DWORD SPANISH_LCID = 1034;


class MSOfficeTest: public testing::Test
{
protected:

	virtual void TearDown()
	{
		ConfigurationInstance::Reset();
	}
};

class MSOfficeForTest : public MSOffice
{
public:

	MSOfficeForTest::MSOfficeForTest(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner, MSOfficeVersion version)
		: MSOffice(OSVersion, registry, win32I18N, runner, version) {};
	
	void _setPackageCodeToSet(wstring packageCodeToSet)
	{
		m_packageCodeToSet = packageCodeToSet;		
	}

	void _setInstalledLangPackCode(wstring installedLangPackCode)
	{		
		m_installedLangPackCode = installedLangPackCode;
	}

	using MSOffice::_getDownloadID;	
	using MSOffice::_readDefaultLanguageForOffice2016LangAccessoryPack;
	using MSOffice::_setDefaultLanguageForOffice2016LangAccessoryPack;
};

#define CreateMSoffice(version) \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	OSVersionMock osVersionMock; \
	Win32I18NMock win32I18NMockobj; \
	MSOfficeForTest office(&osVersionMock, &registryMockobj, &win32I18NMockobj, &runnerMock, version);

extern void MockOfficeInstalled(RegistryMock& registryMockobj, MSOfficeVersion version);

void SetLocaleMockForIsDefaultLanguage(RegistryMock& registryMockobj, bool FollowSystemUIOff, DWORD language, const wchar_t* version = L"15.0")
{
	wchar_t szKeyName[2048];
	swprintf_s(szKeyName, L"Software\\Microsoft\\Office\\%s\\Common\\LanguageResources", version);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(szKeyName), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"FollowSystemUI"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(FollowSystemUIOff ? L"Off" : L"On"), Return(true)));
	EXPECT_CALL(registryMockobj, GetDWORD(StrCaseEq(L"UILanguage"),_)).
		WillRepeatedly(DoAll(SetArgPointee<1>(language), Return(true)));
}

void SetLocaleMockForIsDefaultLanguageForLAP(RegistryMock& registryMockobj, bool FollowSystemUIOff, const wchar_t* language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(OFFICE2016_LAPKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetDWORD(StrCaseEq(L"FollowSystemUILanguage"),_)).
		WillRepeatedly(DoAll(SetArgPointee<1>(FollowSystemUIOff ? 0 : 1), Return(true)));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"UILanguageTag"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
}

void SetDefaultLanguageForOffice2016LAP(RegistryMock& registryMockobj)
{
	const int DISABLED = 0;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(OFFICE2016_LAPKEY), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"UILanguageTag"), StrCaseEq(L"ca-es"))).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"FollowSystemUILanguage"), Eq(DISABLED))).Times(1).WillRepeatedly(Return(true));
}

void SetLangPacksInstalled(RegistryMock& registryMockobj, MSOfficeVersion version, const wchar_t* langCode)
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

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\16.0\\Common\\LanguageResources\\InstalledUIs"), false)).
		WillRepeatedly(Return(version == MSOffice2016));

	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\16.0\\Common\\LanguageResources\\InstalledUIs"), false)).
		WillRepeatedly(Return(version == MSOffice2016_64));

	if (version == MSOffice2003)
	{
		EXPECT_CALL(registryMockobj, GetDWORD(StrCaseEq(langCode),_)).WillRepeatedly(Return(true));
	}
	else if (version != MSOfficeUnKnown && version != NoMSOffice)
	{
		EXPECT_CALL(registryMockobj, GetString(StrCaseEq(langCode),_,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(L"1"), Return(true)));		
		if (_wcsnicmp(langCode, CATALAN_LANGUAGE_CODE, wcslen(CATALAN_LANGUAGE_CODE)) == 0)
		{
			EXPECT_CALL(registryMockobj, GetString(StrCaseEq(VALENCIAN_LANGUAGE_CODE),_,_)).WillRepeatedly(Return(false));
		}
		else
		{
			EXPECT_CALL(registryMockobj, GetString(StrCaseEq(CATALAN_LANGUAGE_CODE),_,_)).WillRepeatedly(Return(false));
		}
	}
}

void SetLangPacksInstalled(RegistryMock& registryMockobj, MSOfficeVersion version)
{
	SetLangPacksInstalled(registryMockobj, version, CATALAN_LANGUAGE_CODE);
}

void setMSOfficeMockActionDownload(wstring version)
{
	ConfigurationRemote remote;
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload;

	fileActionDownload.AddUrl(L"http://url");
	fileActionDownload.SetVersion(version);
	fileActionDownloads.SetActionID(MSOfficeLPIActionID);
	fileActionDownloads.AddFileActionDownload(fileActionDownload);
	remote.AddFileActionDownloads(fileActionDownloads);
	ConfigurationInstance::Get().SetRemote(remote);
}

TEST_F(MSOfficeTest, SetDefaultLanguage_2003)
{
	CreateMSoffice(MSOffice2003);
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\11.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(0);
	office._setInstalledLangPackCode(CATALAN_LANGUAGE_CODE);
	office.SetDefaultLanguage();
}

TEST_F(MSOfficeTest, SetDefaultLanguage_2007)
{
	CreateMSoffice(MSOffice2007);
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\12.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setInstalledLangPackCode(CATALAN_LANGUAGE_CODE);
	office.SetDefaultLanguage();
}

TEST_F(MSOfficeTest, SetDefaultLanguage_2010)
{
	CreateMSoffice(MSOffice2010);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setInstalledLangPackCode(CATALAN_LANGUAGE_CODE);
	office.SetDefaultLanguage();
}

TEST_F(MSOfficeTest, SetDefaultLanguage_2010_64)
{
	CreateMSoffice(MSOffice2010_64);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\14.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setInstalledLangPackCode(CATALAN_LANGUAGE_CODE);
	office.SetDefaultLanguage();
}

TEST_F(MSOfficeTest, SetDefaultLanguage_2013)
{
	CreateMSoffice(MSOffice2013);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setInstalledLangPackCode(CATALAN_LANGUAGE_CODE);
	office.SetDefaultLanguage();
}

TEST_F(MSOfficeTest, SetDefaultLanguage_2013_64)
{
	CreateMSoffice(MSOffice2013_64);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(CATALAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));
	office._setInstalledLangPackCode(CATALAN_LANGUAGE_CODE);
	office.SetDefaultLanguage();
}

TEST_F(MSOfficeTest, SetDefaultLanguage_SetLanguageOnly_Valencian)
{
	CreateMSoffice(MSOffice2013);
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Office\\15.0\\Common\\LanguageResources"), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetDWORD(StrCaseEq(L"UILanguage"), Eq(VALENCIAN_LCID) )).Times(1).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"FollowSystemUI"), StrCaseEq(L"Off"))).Times(1).WillRepeatedly(Return(true));	
	office._setPackageCodeToSet(VALENCIAN_LANGUAGE_CODE);
	office.SetDefaultLanguage();
}

TEST_F(MSOfficeTest, IsDefaultLanguage_True_YesFollowOffAndUiCatalan)
{
	CreateMSoffice(MSOffice2013);

	bool FollowSystemUIOff = true;
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);
	EXPECT_TRUE(office.IsDefaultLanguage());
}

TEST_F(MSOfficeTest, IsDefaultLanguage_False_YesFollowOffAndUiSpanish)
{
	CreateMSoffice(MSOffice2013);

	bool FollowSystemUIOff = true;	
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, SPANISH_LCID);
	EXPECT_FALSE(office.IsDefaultLanguage());
}

#define SPANISH_PRIMARY_LANG 0xc
#define CATALAN_PRIMARY_LANG 0x3

TEST_F(MSOfficeTest, IsDefaultLanguage_False_NoFollowOffAndUiCatalanSysSpanish)
{
	CreateMSoffice(MSOffice2013);

	bool FollowSystemUIOff = false;
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);
	EXPECT_CALL(win32I18NMockobj, GetUserDefaultUILanguage()).Times(1).WillRepeatedly(Return(SPANISH_PRIMARY_LANG));

	EXPECT_FALSE(office.IsDefaultLanguage());
}

TEST_F(MSOfficeTest, IsDefaultLanguage_True_NoFollowOffAndUiSpanishSysCatalan)
{
	CreateMSoffice(MSOffice2013);

	bool FollowSystemUIOff = false;
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, SPANISH_LCID);
	EXPECT_CALL(win32I18NMockobj, GetUserDefaultUILanguage()).Times(1).WillRepeatedly(Return(CATALAN_PRIMARY_LANG));

	EXPECT_TRUE(office.IsDefaultLanguage());
}

TEST_F(MSOfficeTest, IsDefaultLanguage_False_MSOffice2003_NoFollowOnAndUiCatalanSysSpanish)
{
	CreateMSoffice(MSOffice2003);

	// No follow in 2003 is driven by LCID
	bool FollowSystemUIOff = true;
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, NOFOLLOWSYSTEMOFF_FALSE_LCID_2003, L"11.0");
	EXPECT_CALL(win32I18NMockobj, GetUserDefaultUILanguage()).Times(1).WillRepeatedly(Return(SPANISH_PRIMARY_LANG));
	EXPECT_FALSE(office.IsDefaultLanguage());
}

TEST_F(MSOfficeTest, IsDefaultLanguage_True_MSOffice2003_NoFollowOnAndUiCatalanSysSpanish)
{
	CreateMSoffice(MSOffice2003);

	// No follow in 2003 is driven by LCID
	bool FollowSystemUIOff = true;
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, NOFOLLOWSYSTEMOFF_FALSE_LCID_2003, L"11.0");
	EXPECT_CALL(win32I18NMockobj, GetUserDefaultUILanguage()).Times(1).WillRepeatedly(Return(CATALAN_PRIMARY_LANG));
	EXPECT_TRUE(office.IsDefaultLanguage());
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2003)
{
	CreateMSoffice(MSOffice2003);
	EXPECT_EQ(office._getDownloadID(), L"2003");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2007)
{
	CreateMSoffice(MSOffice2007);
	EXPECT_EQ(office._getDownloadID(), L"2007");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2010_32)
{
	CreateMSoffice(MSOffice2010);
	EXPECT_EQ(office._getDownloadID(), L"2010_32");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2010_64)
{
	CreateMSoffice(MSOffice2010_64);
	EXPECT_EQ(office._getDownloadID(), L"2010_64");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2013_32)
{
	CreateMSoffice(MSOffice2013);
	EXPECT_EQ(office._getDownloadID(), L"2013_ca_32");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2013_64)
{
	CreateMSoffice(MSOffice2013_64);
	EXPECT_EQ(office._getDownloadID(), L"2013_ca_64");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2013_32_va)
{
	CreateMSoffice(MSOffice2013);
	office.SetUseDialectalVariant(true);
	EXPECT_EQ(office._getDownloadID(), L"2013_va_32");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2013_64_va)
{
	CreateMSoffice(MSOffice2013_64);
	office.SetUseDialectalVariant(true);
	EXPECT_EQ(office._getDownloadID(), L"2013_va_64");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2016_32)
{
	CreateMSoffice(MSOffice2016);
	EXPECT_EQ(office._getDownloadID(), L"2016_ca_32");
}

TEST_F(MSOfficeTest, _getDownloadID_MSOffice2016_64)
{
	CreateMSoffice(MSOffice2016_64);
	EXPECT_EQ(office._getDownloadID(), L"2016_ca_64");
}

TEST_F(MSOfficeTest, IsLangPackInstalled_2003)
{
	CreateMSoffice(MSOffice2003);
	
	SetLangPacksInstalled(registryMockobj, MSOffice2003);
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST_F(MSOfficeTest, IsLangPackInstalled_2007)
{
	CreateMSoffice(MSOffice2007);
	
	SetLangPacksInstalled(registryMockobj, MSOffice2007);
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST_F(MSOfficeTest, IsLangPackInstalled_2010)
{
	CreateMSoffice(MSOffice2010);

	SetLangPacksInstalled(registryMockobj, MSOffice2010);
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST_F(MSOfficeTest, IsLangPackInstalled_2013)
{
	CreateMSoffice(MSOffice2013);

	SetLangPacksInstalled(registryMockobj, MSOffice2013);
	EXPECT_TRUE(office.IsLangPackInstalled());
}

TEST_F(MSOfficeTest, IsLangPackInstalled_2013_64)
{
	CreateMSoffice(MSOffice2013_64);
	
	SetLangPacksInstalled(registryMockobj, MSOffice2013_64);
	EXPECT_TRUE(office.IsLangPackInstalled());
}


TEST_F(MSOfficeTest, IsLangPackAvailable_No)
{
	CreateMSoffice(MSOffice2010_64);
	
	ConfigurationRemote remote;	
	ConfigurationInstance::Get().SetRemote(remote);	
	EXPECT_FALSE(office.IsLangPackAvailable());
}

TEST_F(MSOfficeTest, IsLangPackAvailable_Yes)
{
	CreateMSoffice(MSOffice2010_64);	

	setMSOfficeMockActionDownload(L"2010_64");
	EXPECT_TRUE(office.IsLangPackAvailable());
}

TEST_F(MSOfficeTest, _CheckPrerequirements_AlreadyApplied)
{
	bool FollowSystemUIOff = true;
	CreateMSoffice(MSOffice2013);

	SetLangPacksInstalled(registryMockobj, MSOffice2013);	
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID, L"15.0");
	office.CheckPrerequirements(NULL);
	EXPECT_THAT(office.GetStatus(), AlreadyApplied);
}

TEST_F(MSOfficeTest, _CheckPrerequirements_CannotBeApplied)
{
	CreateMSoffice(MSOffice2013);

	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	ConfigurationRemote remote;
	ConfigurationInstance::Get().SetRemote(remote);

	office.CheckPrerequirements(NULL);
	EXPECT_THAT(office.GetStatus(), CannotBeApplied);
}

TEST_F(MSOfficeTest, _CheckPrerequirements_Ok)
{
	CreateMSoffice(MSOffice2013);

	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	setMSOfficeMockActionDownload(L"2013_ca_32");

	office.CheckPrerequirements(NULL);
	EXPECT_THAT(office.GetStatus(), NotSelected);
}


TEST_F(MSOfficeTest, _readDefaultLanguageForOffice2016LangAccessoryPack_followSystemUIOff_LangCat)
{
	CreateMSoffice(MSOffice2016);
	bool isCatalanSetAsDefaultLanguage = false;
	bool followSystemUIOff = false;
	
	SetLocaleMockForIsDefaultLanguageForLAP(registryMockobj, true, L"ca-ES");
	office._readDefaultLanguageForOffice2016LangAccessoryPack(isCatalanSetAsDefaultLanguage, followSystemUIOff);
	EXPECT_TRUE(followSystemUIOff);
	EXPECT_TRUE(isCatalanSetAsDefaultLanguage);
}

TEST_F(MSOfficeTest, _readDefaultLanguageForOffice2016LangAccessoryPack_followSystemUIOff_LangCatVal)
{
	CreateMSoffice(MSOffice2016);
	bool isCatalanSetAsDefaultLanguage = false;
	bool followSystemUIOff = false;

	SetLocaleMockForIsDefaultLanguageForLAP(registryMockobj, true, L"ca-ES-valencia");
	office._readDefaultLanguageForOffice2016LangAccessoryPack(isCatalanSetAsDefaultLanguage, followSystemUIOff);
	EXPECT_TRUE(followSystemUIOff);
	EXPECT_TRUE(isCatalanSetAsDefaultLanguage);
}

TEST_F(MSOfficeTest, _readDefaultLanguageForOffice2016LangAccessoryPack_followSystemUIOn_Spanish)
{
	CreateMSoffice(MSOffice2016);
	bool isCatalanSetAsDefaultLanguage = false;
	bool followSystemUIOff = false;
	
	SetLocaleMockForIsDefaultLanguageForLAP(registryMockobj, true, L"es-ES");
	office._readDefaultLanguageForOffice2016LangAccessoryPack(isCatalanSetAsDefaultLanguage, followSystemUIOff);
	EXPECT_TRUE(followSystemUIOff);
	EXPECT_FALSE(isCatalanSetAsDefaultLanguage);
}

TEST_F(MSOfficeTest, _setDefaultLanguageForOffice2016LangAccessoryPack_Catalan)
{
	CreateMSoffice(MSOffice2016);
	
	SetDefaultLanguageForOffice2016LAP(registryMockobj);
	office._setDefaultLanguageForOffice2016LangAccessoryPack();	
}

