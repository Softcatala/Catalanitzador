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

using ::testing::StrCaseEq;

TEST(FirefoxActionTest, _readVersionAndLocale)
{
	RegistryMock registryMockobj;	
	FirefoxTest firefox(&registryMockobj);

	firefox._setMockForLocale(registryMockobj, L"12.0 (ca)");

	firefox._readVersionAndLocale();
	EXPECT_THAT(firefox.GetVersion(), StrCaseEq(L"12.0"));
	EXPECT_THAT(firefox.GetLocale(), StrCaseEq(L"ca"));	
}

TEST(FirefoxActionTest, _readInstallPath)
{
	RegistryMock registryMockobj;	
	FirefoxTest firefox(&registryMockobj);
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"12.0 (ca)";
	wstring path;	

	firefox._setMockForLocale(registryMockobj, VERSION);
	firefox._setMockForInstalldir(registryMockobj, VERSION, PATH);
	
	EXPECT_THAT(firefox.GetInstallPath(), StrCaseEq(PATH));
}