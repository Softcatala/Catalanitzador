/* 
 * Copyright (C) 2014-2016 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

extern void _setMockForNoJava(OSVersionMock& osVersionMock, RegistryMock& registryMockobj, bool is64bits);
extern void _setMockForJava(OSVersionMock& osVersionMock, RegistryMock& registryMockobj, const wchar_t* version, bool is64bits);


class LangToolLibreOfficeActionTest : public LangToolLibreOfficeAction
{
public:
	
	LangToolLibreOfficeActionTest(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner, IOpenOffice* libreOffice, IOpenOffice* apacheOpenOffice, DownloadManager* downloadManager)
		: LangToolLibreOfficeAction(OSVersion, registry, runner, libreOffice, apacheOpenOffice, downloadManager){};

	public:

		void SetInstallingOpenOffice(IOpenOffice* openOffice) {m_installingOffice = openOffice; }
		void SetShouldInstallJava(bool houldInstallJava) {m_shouldInstallJava = houldInstallJava; }

		using LangToolLibreOfficeAction::_doesJavaNeedsConfiguration;
		using LangToolLibreOfficeAction::_refreshPathEnviromentVariable;
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

TEST(LangToolLibreOfficeActionTest, CheckPrerequirements_ShouldConfigureJava_CannotBeApplied)
{
	CreateLTOOAction;

	EXPECT_CALL(libreOfficeMock, IsInstalled()).WillRepeatedly(Return(true));
	EXPECT_CALL(libreOfficeMock, Is64Bits()).WillRepeatedly(Return(true));
	EXPECT_CALL(apacheOpenOfficeMock, IsInstalled()).WillRepeatedly(Return(false));

	EXPECT_CALL(libreOfficeMock, IsExtensionInstalled(StrCaseEq(EXTENSION_NAME))).WillRepeatedly(Return(false));
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L"1.0"));
	_setMockForJava(osVersionMock, registryMock, L"1.7", false);

	action.CheckPrerequirements(NULL);
	EXPECT_THAT(action.GetStatus(), CannotBeApplied);
}

TEST(LangToolLibreOfficeActionTest, CheckPrerequirements_OpenOffice64_WithJava32_CannotBeApplied)
{
	CreateLTOOAction;

	EXPECT_CALL(libreOfficeMock, IsInstalled()).WillRepeatedly(Return(true));
	EXPECT_CALL(libreOfficeMock, Is64Bits()).WillRepeatedly(Return(true));
	EXPECT_CALL(apacheOpenOfficeMock, IsInstalled()).WillRepeatedly(Return(false));

	EXPECT_CALL(libreOfficeMock, IsExtensionInstalled(StrCaseEq(EXTENSION_NAME))).WillRepeatedly(Return(false));
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L""));
	_setMockForJava(osVersionMock, registryMock, L"1.7", false);

	action.CheckPrerequirements(NULL);
	EXPECT_THAT(action.GetStatus(), CannotBeApplied);
}


TEST(LangToolLibreOfficeActionTest, CheckPrerequirements_OpenOffice32_WithJava64_CannotBeApplied)
{
	CreateLTOOAction;

	EXPECT_CALL(libreOfficeMock, IsInstalled()).WillRepeatedly(Return(true));
	EXPECT_CALL(libreOfficeMock, Is64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(apacheOpenOfficeMock, IsInstalled()).WillRepeatedly(Return(false));

	EXPECT_CALL(libreOfficeMock, IsExtensionInstalled(StrCaseEq(EXTENSION_NAME))).WillRepeatedly(Return(false));
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L""));
	_setMockForJava(osVersionMock, registryMock, L"1.7", true);

	action.CheckPrerequirements(NULL);
	EXPECT_THAT(action.GetStatus(), CannotBeApplied);
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

	_setMockForNoJava(osVersionMock, registryMock, false);	
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L""));
	action.SetInstallingOpenOffice(&libreOfficeMock);
	EXPECT_FALSE(action._doesJavaNeedsConfiguration());
}

TEST(LangToolLibreOfficeActionTest, _doesJavaNeedsConfiguration_Java18_NoJavaConfig)
{
	CreateLTOOAction;

	_setMockForJava(osVersionMock, registryMock, L"1.8", false);
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L""));
	action.SetInstallingOpenOffice(&libreOfficeMock);
	EXPECT_FALSE(action._doesJavaNeedsConfiguration());
}

TEST(LangToolLibreOfficeActionTest, _doesJavaNeedsConfiguration_Java18_Java17Config)
{
	CreateLTOOAction;

	_setMockForJava(osVersionMock, registryMock, L"1.8", false);
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L"1.8"));
	action.SetInstallingOpenOffice(&libreOfficeMock);
	EXPECT_FALSE(action._doesJavaNeedsConfiguration());
}

TEST(LangToolLibreOfficeActionTest, _doesJavaNeedsConfiguration_Java17_Java16Config)
{
	CreateLTOOAction;

	_setMockForJava(osVersionMock, registryMock, L"1.7", false);
	EXPECT_CALL(libreOfficeMock, GetJavaConfiguredVersion()).WillRepeatedly(Return(L"1.6"));
	action.SetInstallingOpenOffice(&libreOfficeMock);
	EXPECT_TRUE(action._doesJavaNeedsConfiguration());
}

#define ENVIRONMENT_KEY L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"

TEST(LangToolLibreOfficeActionTest, __refreshPathEnviromentVariable_64bits)
{
	CreateLTOOAction;
	wchar_t* PATH = L"TestPath";
	wchar_t szPreviousPath[4096], szResult[4096];
	wchar_t* VARIABLE_NAME = L"Path";

	GetEnvironmentVariable(VARIABLE_NAME, szPreviousPath, sizeof(szPreviousPath) * sizeof(wchar_t));
	EXPECT_CALL(registryMock, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(ENVIRONMENT_KEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMock, GetString(StrCaseEq(L"Path"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(PATH), Return(true)));
	
	action.SetShouldInstallJava(true);
	action._refreshPathEnviromentVariable(true);

	GetEnvironmentVariable(VARIABLE_NAME, szResult, sizeof(szResult) * sizeof(wchar_t));
	SetEnvironmentVariable(VARIABLE_NAME, szPreviousPath);	
	EXPECT_THAT(szResult, StrCaseEq(PATH));
}

TEST(LangToolLibreOfficeActionTest, __refreshPathEnviromentVariable_32bits)
{
	CreateLTOOAction;
	wchar_t* PATH = L"TestPath";
	wchar_t szPreviousPath[4096], szResult[4096];
	wchar_t* VARIABLE_NAME = L"Path";

	GetEnvironmentVariable(VARIABLE_NAME, szPreviousPath, sizeof(szPreviousPath) * sizeof(wchar_t));
	EXPECT_CALL(registryMock, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(ENVIRONMENT_KEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMock, GetString(StrCaseEq(L"Path"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(PATH), Return(true)));
	
	action.SetShouldInstallJava(true);
	action._refreshPathEnviromentVariable(false);

	GetEnvironmentVariable(VARIABLE_NAME, szResult, sizeof(szResult) * sizeof(wchar_t));
	SetEnvironmentVariable(VARIABLE_NAME, szPreviousPath);	
	EXPECT_THAT(szResult, StrCaseEq(PATH));
}
