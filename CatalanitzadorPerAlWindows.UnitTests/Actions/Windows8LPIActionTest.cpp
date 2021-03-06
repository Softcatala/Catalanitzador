﻿/* 
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
#include "Windows8LPIAction.h"
#include "ActionStatus.h"
#include "RegistryMock.h"
#include "OSVersionMock.h"
#include "RunnerMock.h"
#include "ConfigurationInstance.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::HasSubstr;
using ::testing::DoAll;

#define SPANISH_LOCALE 0x0c0a
#define FRENCH_LOCALE 0x040c
#define US_LOCALE 0x0409
#define PT_LOCALE 0x0816
#define WIN10_APRIL_2018 17134
#define SCRIPT_ADDPACKAGE L"add-package.ps1"

class Windows8LPIActionTest: public testing::Test
{
protected:

	virtual void TearDown()
	{
		ConfigurationInstance::Reset();
	}
};

class Windows8LPIActionForTest : public Windows8LPIAction
{
public:
	
	Windows8LPIActionForTest::Windows8LPIActionForTest(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner, DownloadManager* downloadManager)
		: Windows8LPIAction(OSVersion, registry, win32I18N, runner, downloadManager) {};

	public:
			using Windows8LPIAction::_isLangPackInstalled;
			using Windows8LPIAction::_getDownloadID;
			using Windows8LPIAction::_setLanguagePanel;
			using Windows8LPIAction::_getScriptSetLang;
			using Windows8LPIAction::_isLanguagePanelFirstForLanguage;
};

#define CreateWindowsLIPAction \
	RegistryMock registryMockobj; \
	Win32I18NMock win32I18NMockobj; \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	Windows8LPIActionForTest lipAction(&osVersionExMock, &registryMockobj, &win32I18NMockobj, &runnerMock, &DownloadManager());

void SetLangPackInstalled(RegistryMock& registryMockobj, bool enabled)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(enabled));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-ES"), false)).WillRepeatedly(Return(enabled));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-es-valencia"), false)).WillRepeatedly(Return(enabled));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-es-valencia"), false)).WillRepeatedly(Return(enabled));
}

void SetPanelLanguage(RegistryMock& registryMockobj, const wchar_t* language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\International\\User Profile"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Languages"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(false)));
}

void setWindowsMockActionDownload(wstring version)
{
	ConfigurationRemote remote;
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload;

	fileActionDownload.AddUrl(L"http://url");
	fileActionDownload.SetVersion(version);
	fileActionDownloads.SetActionID(WindowsLPIActionID);
	fileActionDownloads.AddFileActionDownload(fileActionDownload);
	remote.AddFileActionDownloads(fileActionDownloads);
	ConfigurationInstance::Get().SetRemote(remote);
}

TEST_F(Windows8LPIActionTest, _isLangPackInstalled_True)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-es-valencia"), false)).WillRepeatedly(Return(true));

	EXPECT_TRUE(lipAction._isLangPackInstalled());
}

TEST_F(Windows8LPIActionTest, _isLangPackInstalledPending_True)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-ES"), false)).WillRepeatedly(Return(true));

	EXPECT_TRUE(lipAction._isLangPackInstalled());
}

TEST_F(Windows8LPIActionTest, _isLangPackInstalled_False)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-ES"), false)).WillRepeatedly(Return(false));
		EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-es-valencia"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-es-valencia"), false)).WillRepeatedly(Return(false));

	EXPECT_FALSE(lipAction._isLangPackInstalled());
}

TEST_F(Windows8LPIActionTest, ExecuteWindows_Win81_CA)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	SetLangPackInstalled(registryMockobj, false);

	lipAction._getDownloadID();
	lipAction.SetStatus(Selected);
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"lpksetup.exe"), HasSubstr(L"/i ca-ES /r /s /p"), false)).Times(1).WillRepeatedly(Return(true));

	lipAction.Execute();
}

TEST_F(Windows8LPIActionTest, ExecuteWindows_Win81_VA)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	SetLangPackInstalled(registryMockobj, false);

	lipAction.SetUseDialectalVariant(true);
	lipAction._getDownloadID();
	lipAction.SetStatus(Selected);
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"lpksetup.exe"), HasSubstr(L"/i ca-es-valencia /r /s /p"), false)).Times(1).WillRepeatedly(Return(true));

	lipAction.Execute();
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win80_ca_32)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win8_ca_32"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win80_ca_64)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win8_ca_64"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win80_va_32)
{	
	CreateWindowsLIPAction;

	lipAction.SetUseDialectalVariant(true);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win8_va_32"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win80_va_64)
{	
	CreateWindowsLIPAction;

	lipAction.SetUseDialectalVariant(true);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win8_va_64"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win81_ca_32)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows81));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win81_ca_32"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win81_ca_64)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows81));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win81_ca_64"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win81_va_32)
{	
	CreateWindowsLIPAction;

	lipAction.SetUseDialectalVariant(true);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows81));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win81_va_32"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win81_va_64)
{	
	CreateWindowsLIPAction;

	lipAction.SetUseDialectalVariant(true);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows81));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win81_va_64"));
}

#define WINDOWS_10_BUILD 10500

TEST_F(Windows8LPIActionTest, _getDownloadID_Win10_ca_32)
{
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows10));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(osVersionExMock, GetBuildNumber()).WillRepeatedly(Return(WINDOWS_10_BUILD));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win10_ca_32"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win10_ca_64)
{
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows10));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	EXPECT_CALL(osVersionExMock, GetBuildNumber()).WillRepeatedly(Return(WINDOWS_10_BUILD));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win10_ca_64"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win10_va_32)
{
	CreateWindowsLIPAction;
	
	lipAction.SetUseDialectalVariant(true);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows10));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(osVersionExMock, GetBuildNumber()).WillRepeatedly(Return(WINDOWS_10_BUILD));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win10_va_32"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win10_va_64)
{
	CreateWindowsLIPAction;

	lipAction.SetUseDialectalVariant(true);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows10));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	EXPECT_CALL(osVersionExMock, GetBuildNumber()).WillRepeatedly(Return(WINDOWS_10_BUILD));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win10_va_64"));
}


#define WINDOWS_10_NOV2015_BUILD 10586

TEST_F(Windows8LPIActionTest, _getDownloadID_Win10_10586_ca_32)
{
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows10));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(osVersionExMock, GetBuildNumber()).WillRepeatedly(Return(WINDOWS_10_NOV2015_BUILD));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win10_10586_ca_32"));
}

TEST_F(Windows8LPIActionTest, _getDownloadID_Win10_10586_ca_64)
{
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows10));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	EXPECT_CALL(osVersionExMock, GetBuildNumber()).WillRepeatedly(Return(WINDOWS_10_NOV2015_BUILD));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win10_10586_ca_64"));
}

TEST_F(Windows8LPIActionTest, _setLanguagePanel_Catalan)
{
	CreateWindowsLIPAction;
	string content;

	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"powershell.exe"), HasSubstr(L"-ExecutionPolicy remotesigned"), false)).Times(1).WillRepeatedly(Return(true));
	lipAction._setLanguagePanel();
	
	ifstream reader(lipAction._getScriptSetLang().c_str());
	content = string((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	reader.close();

	EXPECT_THAT(content, HasSubstr("New-WinUserLanguageList ca"));
}

TEST_F(Windows8LPIActionTest, _setLanguagePanel_Valencian)
{
	CreateWindowsLIPAction;
	string content;

	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"powershell.exe"), HasSubstr(L"-ExecutionPolicy remotesigned"), false)).Times(1).WillRepeatedly(Return(true));	
	lipAction.SetUseDialectalVariant(true);
	lipAction._setLanguagePanel();

	ifstream reader(lipAction._getScriptSetLang().c_str());
	content = string((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	reader.close();

	EXPECT_THAT(content, HasSubstr("New-WinUserLanguageList ca-es-valencia"));
	EXPECT_THAT(content, HasSubstr("$1 += \"ca\""));
}

TEST_F(Windows8LPIActionTest, CheckPrerequirements_French)
{
	CreateWindowsLIPAction;
	vector <LANGID> ids;

	ids.push_back(US_LOCALE);
	ids.push_back(FRENCH_LOCALE);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(win32I18NMockobj, EnumUILanguages()).Times(1).WillRepeatedly(Return(ids));
	setWindowsMockActionDownload(L"Win8_ca_32");
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_NE(CannotBeApplied, lipAction.GetStatus());
}

TEST_F(Windows8LPIActionTest, CheckPrerequirements_Portuguese)
{
	CreateWindowsLIPAction;
	vector <LANGID> ids;

	ids.push_back(PT_LOCALE);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(win32I18NMockobj, EnumUILanguages()).Times(1).WillRepeatedly(Return(ids));	
	
	lipAction.CheckPrerequirements(NULL);
	EXPECT_THAT(CannotBeApplied, lipAction.GetStatus());
}

TEST_F(Windows8LPIActionTest, _isLanguagePanelFirstForLanguage_Catalan)
{
	CreateWindowsLIPAction;
	const wchar_t* LANGUAGE_CODE = L"ca";

	SetPanelLanguage(registryMockobj, LANGUAGE_CODE);	
	EXPECT_TRUE(lipAction._isLanguagePanelFirstForLanguage(LANGUAGE_CODE));
}

TEST_F(Windows8LPIActionTest, _isLanguagePanelFirstForLanguage_Spanish)
{
	CreateWindowsLIPAction;	

	SetPanelLanguage(registryMockobj, L"es");
	EXPECT_FALSE(lipAction._isLanguagePanelFirstForLanguage(L"ca"));
}

TEST_F(Windows8LPIActionTest, ExecuteWindows_Win10_17134)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows10));
	EXPECT_CALL(osVersionExMock, GetBuildNumber()).WillRepeatedly(Return(WIN10_APRIL_2018));	

	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	SetLangPackInstalled(registryMockobj, false);
	
	lipAction._getDownloadID();
	lipAction.SetStatus(Selected);
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"powershell.exe"), HasSubstr(SCRIPT_ADDPACKAGE), false)).Times(1).WillRepeatedly(Return(true));

	lipAction.Execute();
}
