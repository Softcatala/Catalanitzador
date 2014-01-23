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

extern void MockOfficeInstalled(RegistryMock& registryMockobj, MSOfficeVersion version);

#define CreateMSOfficeAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	MSOfficeLPIAction officeAction(&registryMockobj, &runnerMock, &DownloadManager());

extern void SetLangPacksInstalled(RegistryMock& registryMockobj, MSOfficeVersion version);

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

/*TEST(MSOfficeLPIActionTest, _IsNeeded_201064_NoLangPack)
{
	CreateMSOfficeAction;

	MockOfficeInstalled(registryMockobj, MSOffice2010_64);
	SetLangPacksInstalled(registryMockobj, NoMSOffice);
	
	officeAction.CheckPrerequirements(NULL);
	EXPECT_TRUE(officeAction.IsNeed());
}*/

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
