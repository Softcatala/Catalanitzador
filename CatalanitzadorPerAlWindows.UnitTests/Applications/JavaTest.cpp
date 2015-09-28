/* 
 * Copyright (C) 2015 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Java.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define CreateJava \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	RegistryMock registryMock; \
	Java java(&osVersionExMock, &registryMock, &runnerMock);

void _setMockForNoJava(RegistryMock& registryMockobj)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\JavaSoft\\Java Runtime Environment"), false)).WillRepeatedly(Return(false));	
}

void _setMockForJava(RegistryMock& registryMockobj, const wchar_t* version)
{	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\JavaSoft\\Java Runtime Environment"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"CurrentVersion"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(version), Return(true)));
}

TEST(LangToolLibreOfficeActionTest, _shouldInstallJava_Yes)
{
	CreateJava;

	_setMockForJava(registryMock, L"1.6");
	EXPECT_TRUE(java.ShouldInstall(L"1.7"));
}

TEST(LangToolLibreOfficeActionTest, _shouldInstallJava_No)
{
	CreateJava;

	_setMockForJava(registryMock, L"1.7");
	EXPECT_FALSE(java.ShouldInstall(L"1.7"));
}

TEST(LangToolLibreOfficeActionTest, _readVersion)
{
	CreateJava;
	const wchar_t* VERSION = L"1.7";

	_setMockForJava(registryMock, VERSION);
	EXPECT_THAT(java.GetVersion(), StrCaseEq(VERSION));
}

