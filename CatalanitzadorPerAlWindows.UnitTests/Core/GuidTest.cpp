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
#include "Guid.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define GUID_SAMPLE L"6eddb85f-ba6f-49ab-a0e8-76c6d67cd2f5"
#define GUID_LEN 36

TEST(GuidTest, readGuidFromRegistry)
{
	RegistryMock registryMockobj;
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CATALANITZADOR_REGKEY), false)).WillRepeatedly(Return(true));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(GUID_REGKEY),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(GUID_SAMPLE), Return(true)));

	Guid guidTest(&registryMockobj);
	EXPECT_THAT(guidTest.Get(), StrCaseEq(GUID_SAMPLE));
}

TEST(GuidTest, generateGuid)
{
	RegistryMock registryMockobj;
	Guid guidTest(&registryMockobj);	
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CATALANITZADOR_REGKEY), false)).WillRepeatedly(Return(true));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(GUID_REGKEY),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L""), Return(true)));

	EXPECT_TRUE(guidTest.Get().size() == GUID_LEN);
}

TEST(GuidTest, storeNewGuid)
{
	wchar_t guid[1024];
	RegistryMock registryMockobj;
	Guid guidTest(&registryMockobj);
	
	// Get
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CATALANITZADOR_REGKEY), false)).WillRepeatedly(Return(true));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(GUID_REGKEY),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L""), Return(true)));

	// Store
	EXPECT_CALL(registryMockobj, CreateKey(HKEY_LOCAL_MACHINE, StrCaseEq(CATALANITZADOR_REGKEY))).WillRepeatedly(Return(true));

	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(GUID_REGKEY),_)).
		WillRepeatedly(DoAll(ReadArgCharString(guid), Return(true)));

	guidTest.Get();
	guidTest.Store();
	EXPECT_TRUE(wcslen(guid) == GUID_LEN);
}