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
#include "RegistryMock.h"
#include "InternetExplorerVersion.h"

using ::testing::StrCaseEq;
using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;


#define CreateIEVersion \
	RegistryMock registryMockobj; \
	InternetExplorerVersion IEVersion(&registryMockobj);


TEST(InternetExplorerVersionTest, _readIEVersion)
{
	CreateIEVersion;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Microsoft\\Internet Explorer"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Version"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"8.0.6001.19272"), Return(true)));
	
	EXPECT_THAT(IEVersion.GetVersion(), InternetExplorerVersion::IE8);
}

TEST(InternetExplorerVersionTest, _readIEVersion_IEUnknown)
{
	CreateIEVersion;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Microsoft\\Internet Explorer"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Version"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"5.0"), Return(true)));

	EXPECT_THAT(IEVersion.GetVersion(), InternetExplorerVersion::IEUnknown);
}
