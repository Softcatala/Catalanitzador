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
#include "MSOfficeAction.h"
#include "OSVersionMock.h"
#include "RunnerMock.h"
#include "RegistryMock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;

class MSOfficeActionTest : public MSOfficeAction
{
public:

	MSOfficeActionTest::MSOfficeActionTest(IRegistry* registry, IRunner* runner)
		: MSOfficeAction(registry, runner) {};
	
	public: using MSOfficeAction::_getVersionInstalled;

};

#define CreateMSOfficeAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	MSOfficeActionTest officeAction(&registryMockobj, &runnerMock);

TEST(MSOfficeActionTest, _isVersionInstalled_2003)
{
	CreateMSOfficeAction;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\LanguageResources\\ParentFallback"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\LanguageResources\\ParentFallback"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\LanguageResources\\ParentFallback"), false)).WillRepeatedly(Return(false));
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"SomePath"), Return(true)));
	
	EXPECT_EQ(officeAction._getVersionInstalled(), MSOffice2003);
}

TEST(MSOfficeActionTest, _isVersionInstalled_2007)
{
	CreateMSOfficeAction;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"SomePath"), Return(true)));
	
	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2007);
}

TEST(MSOfficeActionTest, _isVersionInstalled_2010)
{
	CreateMSOfficeAction;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(false));
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"SomePath"), Return(true)));
	
	EXPECT_THAT(officeAction._getVersionInstalled(), MSOffice2010);	
}
