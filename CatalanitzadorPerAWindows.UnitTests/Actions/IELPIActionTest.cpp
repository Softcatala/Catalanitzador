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
#include "IELPIAction.h"
#include "WindowsLPIAction.h"
#include "ActionStatus.h"
#include "RegistryMock.h"
#include "OSVersionMock.h"
#include "RunnerMock.h"
#include "Win32I18NMock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

class IELPIActionTest : public IELPIAction
{
public:

	IELPIActionTest(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner) :
		  IELPIAction(OSVersion, registry, runner) {}	
		
	public: using IELPIAction::_readIEVersion;
	public: using IELPIAction::_getIEVersion;
	public: using IELPIAction::_setIEVersion;

};

#define CreateIELPIAction \
	RegistryMock registryMockobj; \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	IELPIActionTest lipAction(&osVersionExMock, &registryMockobj, &runnerMock);

#define CreateWindowsLIPAction \
	RegistryMock registryMockobjWin; \
	Win32I18NMock win32I18NMockobjWin; \
	OSVersionMock osVersionExMockWin; \
	RunnerMock runnerMockWin; \
	WindowsLPIAction winLIPAction(&osVersionExMockWin, &registryMockobjWin, &win32I18NMockobjWin, &runnerMockWin);

TEST(IELPIActionTest, _readIEVersion)
{
	CreateIELPIAction;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Microsoft\\Internet Explorer"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Version"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"8.0"), Return(true)));

	lipAction._readIEVersion();
	EXPECT_THAT(lipAction._getIEVersion(), IE8);
}

TEST(IELPIActionTest, CheckPrerequirementsDependand_IE6_WindowsXP)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	lipAction._setIEVersion(IE6);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), AppliedInWinLPI);
}


TEST(IELPIActionTest, CheckPrerequirementsDependand_IE7_WindowsXP)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	lipAction._setIEVersion(IE7);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), NeedsWinLPI);
}


TEST(IELPIActionTest, CheckPrerequirementsDependand_IE7_WindowsVista)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsVista));
	lipAction._setIEVersion(IE7);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), AppliedInWinLPI);
}

TEST(IELPIActionTest, CheckPrerequirementsDependand_IE8_WindowsXP)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	lipAction._setIEVersion(IE8);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), NeedsWinLPI);
}

TEST(IELPIActionTest, CheckPrerequirementsDependand_IE8_WindowsVista)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsVista));
	lipAction._setIEVersion(IE8);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), NeedsWinLPI);
}

TEST(IELPIActionTest, CheckPrerequirementsDependand_IE8_Windows7)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	lipAction._setIEVersion(IE8);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), AppliedInWinLPI);
}

TEST(IELPIActionTest, CheckPrerequirementsDependand_IE9_WindowsVista)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsVista));
	lipAction._setIEVersion(IE9);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), NeedsWinLPI);
}

TEST(IELPIActionTest, CheckPrerequirementsDependand_IE9_Windows7)
{
	CreateIELPIAction;
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	lipAction._setIEVersion(IE9);

	EXPECT_THAT(lipAction.CheckPrerequirementsDependand(&winLIPAction), NeedsWinLPI);
}

TEST(IELPIActionTest, CheckPrerequirements_UnknownIEVersion)
{
	CreateIELPIAction;

	lipAction._setIEVersion(IEUnknown);
	EXPECT_THAT(lipAction.CheckPrerequirements(), UnknownIEVersion);
}

TEST(IELPIActionTest, CheckPrerequirements_Windows7_64bits_IE8)
{
	CreateIELPIAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));

	lipAction._setIEVersion(IE8);
	EXPECT_THAT(lipAction.CheckPrerequirements(), PrerequirementsOk);
}

TEST(IELPIActionTest, CheckPrerequirements_Windows7_64bits_IE9)
{
	CreateIELPIAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));

	lipAction._setIEVersion(IE9);
	EXPECT_THAT(lipAction.CheckPrerequirements(), PrerequirementsOk);
}