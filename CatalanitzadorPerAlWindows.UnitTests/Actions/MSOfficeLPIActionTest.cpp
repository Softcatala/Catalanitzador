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
using ::testing::Eq;

#define CreateMSOfficeAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	OSVersionMock osVersionMock; \
	Win32I18NMock win32I18NMockobj; \
	MSOfficeLPIAction officeAction(&osVersionMock, &registryMockobj, &win32I18NMockobj, &runnerMock, &DownloadManager());

extern void MockOfficeInstalled(OSVersionMock& osVersionMock, RegistryMock& registryMockobj, MSOfficeVersion version);
extern void SetLangPacksInstalled(RegistryMock& registryMockobj, MSOfficeVersion version);
extern void SetLocaleMockForIsDefaultLanguage(RegistryMock& registryMockobj, bool FollowSystemUIOff, DWORD language);
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

TEST(MSOfficeLPIActionTest, _IsNeeded_201064_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2010_64);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_201364_LangPackAndnODefaultLang)
{
	CreateMSOfficeAction;
	bool FollowSystemUIOff = true;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2013_64);
	SetLangPacksInstalled(registryMockobj, MSOffice2013_64);
	SetLocaleMockForIsDefaultLanguage(registryMockobj, FollowSystemUIOff, SPANISH_LCID);

	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}

TEST(MSOfficeLPIActionTest, _IsNeeded_201364_LangPackAndDefaultLang)
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
