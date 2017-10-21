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

#include "stdafx.h"
#include "Defines.h"
#include "FirefoxTest.h"
#include "OSVersionMock.h"

using ::testing::StrCaseEq;

#define CreateFirefoxAction \
	RegistryMock registryMockobj; \
	OSVersionMock osVersionMock; \
	FirefoxTest firefox(&registryMockobj, &osVersionMock);

TEST(FirefoxTest, _readVersionAndLocale)
{
	CreateFirefoxAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	firefox._setMockForLocale(registryMockobj, L"12.0 (ca)");

	firefox._readVersionAndLocale();
	EXPECT_THAT(firefox.GetVersion(), StrCaseEq(L"12.0"));
	EXPECT_THAT(firefox.GetLocale(), StrCaseEq(L"ca"));
	EXPECT_FALSE(firefox.Is64Bits());
}

TEST(FirefoxTest, _readVersionAndLocale_x86)
{
	CreateFirefoxAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	firefox._setMockForLocale(registryMockobj, L"31.0 (x86 ca)");

	firefox._readVersionAndLocale();
	EXPECT_THAT(firefox.GetVersion(), StrCaseEq(L"31.0"));
	EXPECT_THAT(firefox.GetLocale(), StrCaseEq(L"ca"));	
}

TEST(FirefoxTest, _readVersionAndLocale_empty)
{
	CreateFirefoxAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(false));

	firefox._setMockForLocale(registryMockobj, L"");

	firefox._readVersionAndLocale();
	EXPECT_THAT(firefox.GetVersion(), StrCaseEq(L""));
	EXPECT_THAT(firefox.GetLocale(), StrCaseEq(L""));
}

TEST(FirefoxTest, _readInstallPath_32bits)
{
	CreateFirefoxAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"12.0 (ca)";
	wstring path;

	firefox._setMockForLocale(registryMockobj, VERSION);
	firefox._setMockForInstalldir(registryMockobj, VERSION, PATH);
	
	EXPECT_THAT(firefox.GetInstallPath(), StrCaseEq(PATH));
}

TEST(FirefoxTest, _readInstallPath_64bits)
{
	CreateFirefoxAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"12.0 (ca)";
	wstring path;

	firefox._setMockForLocale64(registryMockobj, VERSION);
	firefox._setMockForInstalldir64(registryMockobj, VERSION, PATH);

	EXPECT_THAT(firefox.GetInstallPath(), StrCaseEq(PATH));
}

TEST(FirefoxTest, _readVersionAndLocale_with64bits)
{
	CreateFirefoxAction;
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	firefox._setMockForLocale64(registryMockobj, L"31.0 (x64 ca)");

	firefox._readVersionAndLocale();
	EXPECT_THAT(firefox.GetVersion(), StrCaseEq(L"31.0"));
	EXPECT_THAT(firefox.GetLocale(), StrCaseEq(L"ca"));
	EXPECT_TRUE(firefox.Is64Bits());
}