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
#include "IWin32I18N.h"
#include "WindowsLPIAction.h"
#include "ActionStatus.h"
#include "RegistryMock.h"
#include "OSVersionMock.h"
#include "Win32I18NMock.h"
#include "RunnerMock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;

#define SPANISH_LOCALE 0x0c0a
#define US_LOCALE 0x0409

#define CreateWindowsLIPAction \
	RegistryMock registryMockobj; \
	Win32I18NMock win32I18NMockobj; \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	WindowsLPIAction lipAction(&osVersionExMock, &registryMockobj, &win32I18NMockobj, &runnerMock);


TEST(WindowsLPIActionTest, CheckPrerequirements_WindowsSpanish)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(win32I18NMockobj, GetSystemDefaultUILanguage()).Times(1).WillRepeatedly(Return(SPANISH_LOCALE));
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_NE(CannotBeApplied, lipAction.GetStatus());
}

TEST(WindowsLPIActionTest, CheckPrerequirements_WindowsEnglish)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(win32I18NMockobj, GetSystemDefaultUILanguage()).Times(1).WillRepeatedly(Return(US_LOCALE));
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_EQ(CannotBeApplied, lipAction.GetStatus());
}


TEST(WindowsLPIActionTest, CheckPrerequirementsWindows2008)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(win32I18NMockobj, GetSystemDefaultUILanguage()).Times(1).WillRepeatedly(Return(SPANISH_LOCALE));
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows2008));
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_EQ(CannotBeApplied, lipAction.GetStatus());
}

TEST(WindowsLPIActionTest, CheckPrerequirementsWindowsXP)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(win32I18NMockobj, GetSystemDefaultUILanguage()).Times(1).WillRepeatedly(Return(SPANISH_LOCALE));
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_NE(CannotBeApplied, lipAction.GetStatus());
}

TEST(WindowsLPIActionTest, IsLangPackInstalled_XPFTrue)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));

	EXPECT_EQ(false, lipAction.IsLangPackInstalled());
}

TEST(WindowsLPIActionTest, IsLangPackInstalled_XPFalse)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"MUILanguagePending"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"0403"), Return(true)));	
	
	EXPECT_EQ(true, lipAction.IsLangPackInstalled());
}

TEST(WindowsLPIActionTest, IsLangPackInstalled_7True)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(true));

	EXPECT_EQ(true, lipAction.IsLangPackInstalled());
}

TEST(WindowsLPIActionTest, IsLangPackInstalled_7False)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));

	EXPECT_EQ(false, lipAction.IsLangPackInstalled());
}

TEST(WindowsLPIActionTest, ExecuteWindowsXP)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));	
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"msiexec.exe"), HasSubstr(L"/qn"))).Times(1).WillRepeatedly(Return(true));

	lipAction.Execute();
}

TEST(WindowsLPIActionTest, ExecuteWindows7)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));	
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"lpksetup.exe"), HasSubstr(L"/i ca-ES /r /s /p"))).Times(1).WillRepeatedly(Return(true));

	lipAction.Execute();
}

TEST(WindowsLPIActionTest, GetPackageName7)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));	
	EXPECT_THAT(lipAction.GetPackageName(), HasSubstr(L"LIP_ca-ES-32bit.mlc"));
}

TEST(WindowsLPIActionTest, GetPackageNameVista)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsVista));
	EXPECT_THAT(lipAction.GetPackageName(), HasSubstr(L"lip_ca-es.mlc"));
}

#define WINDOWS_SP_MAJORNUM_SP1 1

TEST(WindowsLPIActionTest, GetPackageNameXPSP1)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, GetServicePackVersion()).WillRepeatedly(Return(MAKELONG(0,WINDOWS_SP_MAJORNUM_SP1)));
	EXPECT_THAT(lipAction.GetPackageName(), HasSubstr(L"Build2006Professional/LIPsetup.msi"));
}

#define WINDOWS_SP_MAJORNUM_SP2 2

TEST(WindowsLPIActionTest, GetPackageNameXPSP2)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, GetServicePackVersion()).WillRepeatedly(Return(MAKELONG(0,WINDOWS_SP_MAJORNUM_SP2)));
	EXPECT_THAT(lipAction.GetPackageName(), HasSubstr(L"XP2orlater/LIPsetup.msi"));
}
