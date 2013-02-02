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
#include "FirefoxAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

class FirefoxActionForTest : public FirefoxAction
{
	public:

		FirefoxActionForTest(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : FirefoxAction(registry, runner, downloadManager) {};

		using FirefoxAction::_readVersionAndLocale;
		using FirefoxAction::_getLocale;
};

#define SetLocale(registryMockobj, locale) \
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Mozilla\\Mozilla Firefox"), false)).WillRepeatedly(Return(true));  \
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"CurrentVersion"),_ ,_)).  \
	WillRepeatedly(DoAll(SetArgCharStringPar2(locale), Return(true)));


TEST(FirefoxActionTest, _readVersionAndLocale)
{
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());

	SetLocale(registryMockobj, L"12.0 (ca)");

	firefoxAction._readVersionAndLocale();
	EXPECT_THAT(firefoxAction.GetVersion(), StrCaseEq(L"12.0"));
	EXPECT_THAT(firefoxAction._getLocale(), StrCaseEq(L"ca"));
}
