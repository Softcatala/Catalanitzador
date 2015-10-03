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
#include "OpenOfficeMock.h"
#include "LangToolLibreOfficeAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define EXTENSION_NAME L"org.languagetool.openoffice.Main"

#define CreateLTOOAction \
	OSVersionMock osVersionMock; \
	RunnerMock runnerMock; \
	RegistryMock registryMock; \
	OpenOfficeMock libreOfficeMock; \
	OpenOfficeMock apacheOpenOfficeMock; \
	LangToolLibreOfficeActionTest action(&osVersionMock, &registryMock, &runnerMock, &libreOfficeMock, &apacheOpenOfficeMock, &DownloadManager());

extern void _setMockForJava(RegistryMock& registryMockobj, const wchar_t* version);
extern void _setMockForNoJava(RegistryMock& registryMockobj);


class LangToolLibreOfficeActionTest : public LangToolLibreOfficeAction
{
public:
	
	LangToolLibreOfficeActionTest(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner, IOpenOffice* libreOffice, IOpenOffice* apacheOpenOffice, DownloadManager* downloadManager)
		: LangToolLibreOfficeAction(OSVersion, registry, runner, libreOffice, apacheOpenOffice, downloadManager){};

	public:

		void _setInstallingOpenOffice(IOpenOffice* openOffice) {m_installingOffice = openOffice; }

		using LangToolLibreOfficeAction::_doesJavaNeedsConfiguration;
};


TEST(LangToolLibreOfficeActionTest, CheckPrerequirements_NotInstalled)
{
	CreateLTOOAction;

	EXPECT_CALL(libreOfficeMock, IsInstalled()).WillRepeatedly(Return(false));
	EXPECT_CALL(apacheOpenOfficeMock, IsInstalled()).WillRepeatedly(Return(false));

	action.CheckPrerequirements(NULL);
	EXPECT_THAT(action.GetStatus(), NotInstalled);
}

TEST(LangToolLibreOfficeActionTest, CheckPrerequirements_LibreOffice_AlreadyApplied)
{
	CreateLTOOAction;

	EXPECT_CALL(libreOfficeMock, IsInstalled()).WillRepeatedly(Return(true));
	EXPECT_CALL(apacheOpenOfficeMock, IsInstalled()).WillRepeatedly(Return(false));

	EXPECT_CALL(libreOfficeMock, IsExtensionInstalled(StrCaseEq(EXTENSION_NAME))).WillRepeatedly(Return(true));
	EXPECT_CALL(apacheOpenOfficeMock, IsExtensionInstalled(StrCaseEq(EXTENSION_NAME))).WillRepeatedly(Return(false));

	action.CheckPrerequirements(NULL);
	EXPECT_THAT(action.GetStatus(), AlreadyApplied);
}

TEST(LangToolLibreOfficeActionTest, CheckPrerequirements_OpenOffice_AlreadyApplied)
{
	CreateLTOOAction;

	EXPECT_CALL(libreOfficeMock, IsInstalled()).WillRepeatedly(Return(false));
	EXPECT_CALL(apacheOpenOfficeMock, IsInstalled()).WillRepeatedly(Return(true));

	EXPECT_CALL(libreOfficeMock, IsExtensionInstalled(StrCaseEq(EXTENSION_NAME))).WillRepeatedly(Return(false));
	EXPECT_CALL(apacheOpenOfficeMock, IsExtensionInstalled(StrCaseEq(EXTENSION_NAME))).WillRepeatedly(Return(true));

	action.CheckPrerequirements(NULL);
	EXPECT_THAT(action.GetStatus(), AlreadyApplied);
}

TEST(LangToolLibreOfficeActionTest, _doesJavaNeedsConfiguration_NoJava_NoJavaConfig)
{
	CreateLTOOAction;

	_setMockForNoJava(registryMock);	
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L""));
	action._setInstallingOpenOffice(&libreOfficeMock);
	EXPECT_FALSE(action._doesJavaNeedsConfiguration());
}

TEST(LangToolLibreOfficeActionTest, _doesJavaNeedsConfiguration_Java17_NoJavaConfig)
{
	CreateLTOOAction;

	_setMockForJava(registryMock, L"1.7");	
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L""));
	action._setInstallingOpenOffice(&libreOfficeMock);
	EXPECT_FALSE(action._doesJavaNeedsConfiguration());
}

TEST(LangToolLibreOfficeActionTest, _doesJavaNeedsConfiguration_Java17_Java17Config)
{
	CreateLTOOAction;

	_setMockForJava(registryMock, L"1.7");
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L"1.7"));
	action._setInstallingOpenOffice(&libreOfficeMock);
	EXPECT_FALSE(action._doesJavaNeedsConfiguration());
}

TEST(LangToolLibreOfficeActionTest, _doesJavaNeedsConfiguration_Java17_Java16Config)
{
	CreateLTOOAction;

	_setMockForJava(registryMock, L"1.7");	
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L"1.6"));
	action._setInstallingOpenOffice(&libreOfficeMock);
	EXPECT_TRUE(action._doesJavaNeedsConfiguration());
}