/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "MSOfficeFactory.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::Eq;

void MockOfficeInstalled(OSVersionMock& osVersionMock, RegistryMock& registryMockobj, MSOfficeVersion version)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2003));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2007));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2010));

	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2010_64));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2013));
	
	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\InstallRoot"), false)).
		WillRepeatedly(Return(version == MSOffice2013_64));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"SomePath"), Return(true)));

	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(version == MSOffice2013_64 || MSOffice2010_64));
}

TEST(MSOfficeFactoryTest, _isVersionInstalled_2003)
{
	RegistryMock registryMockobj;
	OSVersionMock osVersionMock;
	
	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2003);
	EXPECT_TRUE(MSOfficeFactory::_isVersionInstalled(&osVersionMock, &registryMockobj, MSOffice2003));
}

TEST(MSOfficeFactoryTest, _isVersionInstalled_2007)
{
	RegistryMock registryMockobj;
	OSVersionMock osVersionMock;
	
	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2007);
	EXPECT_TRUE(MSOfficeFactory::_isVersionInstalled(&osVersionMock, &registryMockobj, MSOffice2007));	
}

TEST(MSOfficeFactoryTest, _isVersionInstalled_2010)
{
	RegistryMock registryMockobj;
	OSVersionMock osVersionMock;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2010);
	EXPECT_TRUE(MSOfficeFactory::_isVersionInstalled(&osVersionMock, &registryMockobj, MSOffice2010));	
}

TEST(MSOfficeFactoryTest, _isVersionInstalled_2010_64)
{
	RegistryMock registryMockobj;
	OSVersionMock osVersionMock;

	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2010_64);
	EXPECT_TRUE(MSOfficeFactory::_isVersionInstalled(&osVersionMock, &registryMockobj, MSOffice2010_64));	
}

TEST(MSOfficeFactoryTest, _isVersionInstalled_2013_64)
{
	RegistryMock registryMockobj;
	OSVersionMock osVersionMock;
	
	MockOfficeInstalled(osVersionMock, registryMockobj, MSOffice2013_64);
	EXPECT_TRUE(MSOfficeFactory::_isVersionInstalled(&osVersionMock, &registryMockobj, MSOffice2013_64));	
}
