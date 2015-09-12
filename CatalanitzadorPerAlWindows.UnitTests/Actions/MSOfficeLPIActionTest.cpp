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
#include "ConfigurationInstance.h"
#include "ConfigurationRemote.h"

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::HasSubstr;

#define CreateMSOfficeAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	OSVersionMock osVersionMock; \
	Win32I18NMock win32I18NMockobj; \
	MSOfficeLPIAction officeAction(&osVersionMock, &registryMockobj, &win32I18NMockobj, &runnerMock, &DownloadManager());

extern void MockOfficeInstalled(OSVersionMock& osVersionMock, RegistryMock& registryMockobj, MSOfficeVersion version);
extern void SetLangPacksInstalled(RegistryMock& registryMockobj, MSOfficeVersion version);
extern void SetLocaleMockForIsDefaultLanguage(RegistryMock& registryMockobj, bool FollowSystemUIOff, DWORD language, const wchar_t* version = L"15.0");
extern DWORD CATALAN_LCID;
extern DWORD VALENCIAN_LCID;
extern DWORD SPANISH_LCID;

TEST(MSOfficeLPIActionTest, _IsNeeded_2003_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2003);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);

	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2007_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock,registryMockobj, MSOffice2007);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2010_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2010);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2010_64_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2010_64);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2013_64_LangPackAndnOnDefaultLang)
{
	CreateMSOfficeAction;
	bool FollowSystemUIOff = true;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2013_64);
	SetLangPacksInstalled(registryMockobj, MSOffice2013_64);
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, SPANISH_LCID);

	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_2013_64_LangPackAndDefaultLang)
{
	CreateMSOfficeAction;
	bool FollowSystemUIOff = true;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2013_64);
	SetLangPacksInstalled(registryMockobj, MSOffice2013_64);
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);

	officeAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_NotInstalled)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock, registryMockobj, NoMSOffice);
	officeAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(officeAction.IsNeed());
	EXPECT_EQ(officeAction.GetStatus(), NotInstalled);
}

TEST(MSOfficeLPIActionTest, Serialize_One_Instance)
{
	CreateMSOfficeAction;
	ostringstream stream;
	bool FollowSystemUIOff = true;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2013_64);
	SetLangPacksInstalled(registryMockobj, MSOffice2013_64);
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, CATALAN_LCID);
	officeAction.CheckPrerequirements(NULL);

	officeAction.Serialize(&stream);	
	EXPECT_THAT(stream.str(), HasSubstr("<action id='2' version='2013_64bits' result='3'/>"));
}

TEST(MSOfficeLPIActionTest, Serialize_None)
{
	CreateMSOfficeAction;
	ostringstream stream;

	MockOfficeInstalled(osVersionMock, registryMockobj, NoMSOffice);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	officeAction.CheckPrerequirements(NULL);

	officeAction.Serialize(&stream);
	EXPECT_THAT(stream.str(), HasSubstr("<action id='2' version='' result='7'/>"));
}


TEST(MSOfficeLPIActionTest, _CheckPrerequirements_NotInstalled)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock, registryMockobj, NoMSOffice);
	officeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(officeAction.GetStatus(), NotInstalled);
}


TEST(MSOfficeLPIActionTest, _CheckPrerequirements_CannotBeApplied)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2013);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);

	ConfigurationRemote remote;	
	ConfigurationInstance::Get().SetRemote(remote);
	officeAction.CheckPrerequirements(NULL);

	EXPECT_THAT(officeAction.GetStatus(), CannotBeApplied);
}
