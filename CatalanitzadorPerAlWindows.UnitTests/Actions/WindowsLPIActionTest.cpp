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
#include "IWin32I18N.h"
#include "WindowsLPIAction.h"
#include "ActionStatus.h"
#include "RegistryMock.h"
#include "OSVersionMock.h"
#include "Win32I18NMock.h"
#include "RunnerMock.h"
#include "ConfigurationRemote.h"
#include "ConfigurationInstance.h"
#include "ConfigurationFileActionDownload.h"
#include "ConfigurationFileActionDownloads.h"


using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;

#define SPANISH_LOCALE 0x0c0a
#define FRENCH_LOCALE 0x040c
#define US_LOCALE 0x0409


class WindowsLPIActionTest : public WindowsLPIAction
{
public:
	
	WindowsLPIActionTest::WindowsLPIActionTest(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner)
		: WindowsLPIAction(OSVersion, registry, win32I18N, runner) {};

	virtual void TearDown()
	{
		ConfigurationInstance::Reset();
	}

	public: using WindowsLPIAction::_isLangPackInstalled;
	public: using WindowsLPIAction::_isDefaultLanguage;
	public: using WindowsLPIAction::_getDownloadID;
	public: using WindowsLPIAction::_isValidOperatingSystem;
	public: using WindowsLPIAction::_isASupportedSystemLanguage;
	public: using WindowsLPIAction::_isDownloadAvailable;

	bool virtual _isWindowsValidated()	{ return true;}
};

class WindowsLPIActionTestDefaultLanguage : public WindowsLPIActionTest
{
public:

	WindowsLPIActionTestDefaultLanguage::WindowsLPIActionTestDefaultLanguage(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner)
		: WindowsLPIActionTest(OSVersion, registry, win32I18N, runner) {};
	
	virtual bool _isValidOperatingSystem() { return true; }
	virtual bool _isASupportedSystemLanguage() { return true; }
	virtual bool _isDownloadAvailable() { return true; }
};

#define CreateWindowsLIPAction \
	RegistryMock registryMockobj; \
	Win32I18NMock win32I18NMockobj; \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	WindowsLPIActionTest lipAction(&osVersionExMock, &registryMockobj, &win32I18NMockobj, &runnerMock);

#define CreateWindowsLPIActionTestDefaultLanguage \
	RegistryMock registryMockobj; \
	Win32I18NMock win32I18NMockobj; \
	OSVersionMock osVersionExMock; \
	RunnerMock runnerMock; \
	WindowsLPIActionTestDefaultLanguage lipAction(&osVersionExMock, &registryMockobj, &win32I18NMockobj, &runnerMock);

TEST(WindowsLPIActionTest, CheckPrerequirements_WindowsSpanish)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(win32I18NMockobj, GetSystemDefaultUILanguage()).Times(1).WillRepeatedly(Return(SPANISH_LOCALE));
	
	EXPECT_TRUE(lipAction._isASupportedSystemLanguage());
}

TEST(WindowsLPIActionTest, CheckPrerequirements_WindowsXPFrench)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(win32I18NMockobj, GetSystemDefaultUILanguage()).Times(1).WillRepeatedly(Return(FRENCH_LOCALE));

	EXPECT_FALSE(lipAction._isASupportedSystemLanguage());
}

TEST(WindowsLPIActionTest, CheckPrerequirements_Windows7French)
{
	CreateWindowsLIPAction;
	vector <LANGID> ids;

	ids.push_back(US_LOCALE);
	ids.push_back(FRENCH_LOCALE);
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(win32I18NMockobj, EnumUILanguages()).Times(1).WillRepeatedly(Return(ids));
	
	EXPECT_TRUE(lipAction._isASupportedSystemLanguage());
}

TEST(WindowsLPIActionTest, CheckPrerequirements_WindowsEnglish)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(win32I18NMockobj, GetSystemDefaultUILanguage()).Times(1).WillRepeatedly(Return(US_LOCALE));
	
	EXPECT_FALSE(lipAction._isASupportedSystemLanguage());
}

TEST(WindowsLPIActionTest, CheckPrerequirementsWindows2008)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows2008));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	
	EXPECT_FALSE(lipAction._isValidOperatingSystem());
}

TEST(WindowsLPIActionTest, CheckPrerequirementsWindowsXP_32bits)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	
	EXPECT_TRUE(lipAction._isValidOperatingSystem());
}

TEST(WindowsLPIActionTest, CheckPrerequirementsWindowsXP_64bits)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(true));
	
	EXPECT_FALSE(lipAction._isValidOperatingSystem());
}

TEST(WindowsLPIActionTest, _isLangPackInstalled_XPFalse)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\"), false)).WillRepeatedly(Return(false));

	EXPECT_FALSE(lipAction._isLangPackInstalled());
}

TEST(WindowsLPIActionTest, _isLangPackInstalled_XPTrue)
{
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"MUILanguagePending"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"0403"), Return(true)));	
	
	EXPECT_TRUE(lipAction._isLangPackInstalled());
}

TEST(WindowsLPIActionTest, _isLangPackInstalled_7True)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(true));

	EXPECT_TRUE(lipAction._isLangPackInstalled());
}

TEST(WindowsLPIActionTest, _isLangPackInstalledPending_7True)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-ES"), false)).WillRepeatedly(Return(true));

	EXPECT_TRUE(lipAction._isLangPackInstalled());
}

TEST(WindowsLPIActionTest, _isLangPackInstalled_7False)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-ES"), false)).WillRepeatedly(Return(false));

	EXPECT_FALSE(lipAction._isLangPackInstalled());
}

TEST(WindowsLPIActionTest, ExecuteWindowsXP)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"msiexec.exe"), HasSubstr(L"/qn"), false)).Times(1).WillRepeatedly(Return(true));

	lipAction.Execute();
}

TEST(WindowsLPIActionTest, ExecuteWindows7)
{	
	CreateWindowsLIPAction;
		
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-ES"), false)).WillRepeatedly(Return(false));

	EXPECT_CALL(runnerMock, Execute(HasSubstr(L"lpksetup.exe"), HasSubstr(L"/i ca-ES /r /s /p"), false)).Times(1).WillRepeatedly(Return(true));

	lipAction.Execute();
}

TEST(WindowsLPIActionTest, GetDownloadID7)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Win7_32"));
}

TEST(WindowsLPIActionTest, GetDownloadIDVista)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsVista));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"Vista"));
}

#define WINDOWS_SP_MAJORNUM_SP1 1

TEST(WindowsLPIActionTest, GetDownloadIDXPSP1)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, GetServicePackVersion()).WillRepeatedly(Return(MAKELONG(0,WINDOWS_SP_MAJORNUM_SP1)));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"XP"));
}

#define WINDOWS_SP_MAJORNUM_SP2 2

TEST(WindowsLPIActionTest, GetDownloadIDXPSP2)
{	
	CreateWindowsLIPAction;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));
	EXPECT_CALL(osVersionExMock, GetServicePackVersion()).WillRepeatedly(Return(MAKELONG(0,WINDOWS_SP_MAJORNUM_SP2)));
	EXPECT_THAT(lipAction._getDownloadID(), StrCaseEq(L"XP2"));
}

TEST(WindowsLPIActionTest, _isDefaultLanguage_W7_PreferredUILanguagesYes)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\MuiCached"), false)).WillRepeatedly(Return(false));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguages"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"ca-ES"), Return(true)));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguagesPending"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L""), Return(true)));
	
	EXPECT_TRUE(lipAction._isDefaultLanguage());
}

TEST(WindowsLPIActionTest, _isDefaultLanguage_W7_PreferredUILanguagesPendingYes)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\MuiCached"), false)).WillRepeatedly(Return(false));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguages"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L""), Return(true)));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguagesPending"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"ca-ES"), Return(true)));

	EXPECT_TRUE(lipAction._isDefaultLanguage());
}

TEST(WindowsLPIActionTest, _isDefaultLanguage_W7_No)
{	
	CreateWindowsLIPAction;
	
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\MuiCached"), false)).WillRepeatedly(Return(false));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguages"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"es-ES"), Return(true)));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguagesPending"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"es-ES"), Return(true)));

	EXPECT_FALSE(lipAction._isDefaultLanguage());
}

TEST(WindowsLPIActionTest, IsNeed_isLangPackInstalledYes_isDefaultLanguageNo)
{	
	CreateWindowsLIPAction;

	//_isLangPackInstalled == true
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\MuiCached"), false)).WillRepeatedly(Return(false));

	// No selected
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguages"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"es-ES"), Return(true)));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguagesPending"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"es-ES"), Return(true)));

	EXPECT_FALSE(lipAction._isDefaultLanguage());
	EXPECT_FALSE(lipAction.IsDownloadNeed());
}

TEST(WindowsLPIActionTest, IsNeed_isLangPackInstalledYes_isDefaultLanguageYes)
{	
	CreateWindowsLPIActionTestDefaultLanguage;

	//_isLangPackInstalled == true
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\MuiCached"), false)).WillRepeatedly(Return(false));

	// _isDefaultLanguage = true
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguages"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"ca-ES"), Return(true)));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguagesPending"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"ca-ES"), Return(true)));

	lipAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(lipAction.IsNeed());
}

TEST(WindowsLPIActionTest, IsNeed_isLangPackInstalledYes_isDefaultLanguageYesMachine)
{	
	CreateWindowsLPIActionTestDefaultLanguage;

	//_isLangPackInstalled == true
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop\\MuiCached"), false)).WillRepeatedly(Return(true));

	// _isDefaultLanguage = true (machine only)
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Control Panel\\Desktop"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguages"),_ ,_)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"PreferredUILanguagesPending"),_ ,_)).WillRepeatedly(Return(false));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"MachinePreferredUILanguages"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"ca-ES"), Return(true)));

	lipAction.CheckPrerequirements(NULL);
	EXPECT_FALSE(lipAction.IsNeed());
	EXPECT_EQ(AlreadyApplied, lipAction.GetStatus());
}

TEST(WindowsLPIActionTest, _isDownloadAvailable_No)
{	
	CreateWindowsLIPAction;
	ConfigurationRemote remote;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));

	ConfigurationInstance::Get().SetRemote(remote);
	EXPECT_FALSE(lipAction._isDownloadAvailable());
}

TEST(WindowsLPIActionTest, _isDownloadAvailable_Yes)
{
	CreateWindowsLIPAction;
	ConfigurationRemote remote;
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload;

	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));
	EXPECT_CALL(osVersionExMock, IsWindows64Bits()).WillRepeatedly(Return(false));

	fileActionDownload.SetVersion(lipAction._getDownloadID());	
	fileActionDownloads.SetActionID(WindowsLPI);
	fileActionDownload.AddUrl(L"http://www.softcatala.org/");
	fileActionDownloads.AddFileActionDownload(fileActionDownload);
	remote.AddFileActionDownloads(fileActionDownloads);

	ConfigurationInstance::Get().SetRemote(remote);
	EXPECT_TRUE(lipAction._isDownloadAvailable());
}