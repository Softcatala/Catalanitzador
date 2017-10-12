/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "Defines.h"
#include "RegistryMock.h"
#include "Firefox.h"

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::StrCaseEq;

#pragma once

class FirefoxTest : public Firefox
{
	public:

		FirefoxTest(IRegistry* registry, IOSVersion* OSVersion) : Firefox(registry, OSVersion)
		{
			
		}

		using Firefox::_readVersionAndLocale;
				
		static void _setMockForInstalldir(RegistryMock& registryMockobj, const wchar_t* locale, const wchar_t* installdir)
		{
			wchar_t szKeyName[2048];

			swprintf_s(szKeyName, L"SOFTWARE\\Mozilla\\Mozilla Firefox\\%s\\Main", locale);
			EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(szKeyName), false)).WillRepeatedly(Return(true));
			EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Install Directory"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(installdir), Return(true)));
		}

		static void _setMockForLocale(RegistryMock& registryMockobj, const wchar_t* locale)
		{
			EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Mozilla\\Mozilla Firefox"), false)).WillRepeatedly(Return(true));
			EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"CurrentVersion"),_ ,_)).	WillRepeatedly(DoAll(SetArgCharStringPar2(locale), Return(true)));
		}

		static void _setMockForLocale64(RegistryMock& registryMockobj, const wchar_t* locale)
		{
			EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Mozilla\\Mozilla Firefox"), false)).WillRepeatedly(Return(false));
			EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Mozilla\\Mozilla Firefox"), false)).WillRepeatedly(Return(true));
			EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"CurrentVersion"),_ ,_)).	WillRepeatedly(DoAll(SetArgCharStringPar2(locale), Return(true)));
		}
};
