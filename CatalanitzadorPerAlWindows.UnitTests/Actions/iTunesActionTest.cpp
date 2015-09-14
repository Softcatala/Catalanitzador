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
#include "iTunesAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::ReturnRefOfCopy;
using ::testing::StrEq;
using ::testing::AnyNumber;
using ::testing::ReturnRef;

#define ITUNES_REGKEY L"Software\\Apple Computer, Inc.\\iTunes"
#define CATALAN_LANGCODE L"1027"

class iTunesActionTest : public iTunesAction
{
public:
	
	iTunesActionTest(IRegistry* registry, IFileVersionInfo* fileVersionInfo, IOSVersion* OSVersion)
		: iTunesAction(registry, fileVersionInfo, OSVersion) 
	{
		*m_szFolder = NULL;
	};
	
	public:

		wchar_t m_szFolder[MAX_PATH];

		using iTunesAction::_isDefaultLanguageForUser;
		using iTunesAction::_isDefaultLanguage;
		using iTunesAction::_getProgramLocation;
		using iTunesAction::_setDefaultLanguageForUser;

		void SetGetFolderPath(wchar_t* folder) { wcscpy_s (m_szFolder, folder); }

		virtual void _getSHGetFolderPath(wstring& folder)
		{
			folder = m_szFolder;
		}
};


#define CreateiTunesAction \
	RegistryMock registryMockobj; \
	FileVersionInfoMock fileVersionInfoMock; \
	OSVersionMock osVersionMock; \
	iTunesActionTest iTunes(&registryMockobj, &fileVersionInfoMock, &osVersionMock);

#define CATALAN_LANGCODE L"1027"
#define SPANISH_LANGCODE L"1034"

void _setMockForUserLanguage(RegistryMock& registryMockobj, const wchar_t* language)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"LangID"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
}

void _setMockForUserLanguageFalse(RegistryMock& registryMockobj)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"LangID"),_ ,_)).WillRepeatedly(Return(false));
}

void _setMockForMachineLanguage(RegistryMock& registryMockobj, OSVersionMock& osVersionMock, const wchar_t* language, bool is64bits)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"InstalledLangID"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(language), Return(true)));
	EXPECT_CALL(osVersionMock, IsWindows64Bits()).WillRepeatedly(Return(is64bits));
}

void _setMockForProgramFolder32bits(RegistryMock& registryMockobj, OSVersionMock& osVersionMockobj, const wchar_t* folder)
{
	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"ProgramFolder"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(folder), Return(true)));
	EXPECT_CALL(osVersionMockobj, IsWindows64Bits()).WillRepeatedly(Return(false));
}

void _setMockForProgramFolder64bits(RegistryMock& registryMockobj, OSVersionMock& osVersionMockobj, const wchar_t* folder)
{
	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"InstallDir"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(folder), Return(true)));
	EXPECT_CALL(osVersionMockobj, IsWindows64Bits()).WillRepeatedly(Return(true));
}

void _setiTunesVersion(FileVersionInfoMock& fileVersionInfoMock, const wchar_t* version)
{
	wstring s(version);
	EXPECT_CALL(fileVersionInfoMock, GetVersion()).WillRepeatedly(ReturnRefOfCopy(s));
}

void _setFileVersion(FileVersionInfoMock &fileVersionInfoMockobj, wstring& versionString)
{
	EXPECT_CALL(fileVersionInfoMockobj, SetFilename(_)).Times(AnyNumber());
	EXPECT_CALL(fileVersionInfoMockobj, GetVersion()).WillRepeatedly(ReturnRef(versionString));	
}

void _setMockNotInstalled(RegistryMock& registryMockobj, OSVersionMock& osVersionMockobj, bool is64bits)
{
	EXPECT_CALL(registryMockobj, OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(ITUNES_REGKEY), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(osVersionMockobj, IsWindows64Bits()).WillRepeatedly(Return(is64bits));
}

TEST(iTunesActionTest, _isDefaultLanguageForUser_True)
{
	CreateiTunesAction;

	_setMockForUserLanguage(registryMockobj, CATALAN_LANGCODE);	
	EXPECT_TRUE(iTunes._isDefaultLanguageForUser() == true);
}

TEST(iTunesActionTest, _isDefaultLanguageForUser_False)
{
	CreateiTunesAction;

	_setMockForUserLanguage(registryMockobj, SPANISH_LANGCODE);	
	EXPECT_FALSE(iTunes._isDefaultLanguageForUser() == true);
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineSpanish_UserCatalan_True_32AndOld64bits)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, osVersionMock, SPANISH_LANGCODE, false);
	_setMockForUserLanguage(registryMockobj, CATALAN_LANGCODE);
	EXPECT_TRUE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineCatalan_UserSpanish_False_32AndOld64bits)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, osVersionMock, CATALAN_LANGCODE, false);
	_setMockForUserLanguage(registryMockobj, SPANISH_LANGCODE);
	EXPECT_FALSE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineCatalan_UserNone_True_32AndOld64bits)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, osVersionMock, CATALAN_LANGCODE, false);
	_setMockForUserLanguageFalse(registryMockobj);
	EXPECT_TRUE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineSpanish_UserCatalan_True_New64bits)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, osVersionMock, SPANISH_LANGCODE, true);
	_setMockForUserLanguage(registryMockobj, CATALAN_LANGCODE);
	EXPECT_TRUE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineCatalan_UserSpanish_False_New64bits)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, osVersionMock, CATALAN_LANGCODE, true);
	_setMockForUserLanguage(registryMockobj, SPANISH_LANGCODE);
	EXPECT_FALSE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, _isDefaultLanguage_MachineCatalan_UserNone_True_New64bits)
{
	CreateiTunesAction;

	_setMockForMachineLanguage(registryMockobj, osVersionMock, CATALAN_LANGCODE, true);
	_setMockForUserLanguageFalse(registryMockobj);
	EXPECT_TRUE(iTunes._isDefaultLanguage());
}

TEST(iTunesActionTest, CheckPrerequirements_OldVersion_CannotBeApplied)
{
	CreateiTunesAction;
	wstring VERSION = L"10.6.2.0";

	_setMockForProgramFolder32bits(registryMockobj, osVersionMock,  L"folder\\");
	_setiTunesVersion(fileVersionInfoMock, VERSION.c_str());
	_setFileVersion(fileVersionInfoMock, VERSION);

	iTunes.CheckPrerequirements(NULL);
	EXPECT_THAT(iTunes.GetStatus(), CannotBeApplied);
}

TEST(iTunesActionTest, CheckPrerequirements_32bits_NotInstalled)
{
	CreateiTunesAction;

	_setMockNotInstalled(registryMockobj,osVersionMock, false);	
	iTunes.CheckPrerequirements(NULL);
	EXPECT_THAT(iTunes.GetStatus(), NotInstalled);
}

TEST(iTunesActionTest, CheckPrerequirements_64bits_NotInstalled)
{
	CreateiTunesAction;
	
	_setMockNotInstalled(registryMockobj,osVersionMock, true);
	iTunes.CheckPrerequirements(NULL);
	EXPECT_THAT(iTunes.GetStatus(), NotInstalled);
}

TEST(iTunesActionTest, CheckPrerequirements_AlreadyInstalled)
{
	CreateiTunesAction;
	wstring VERSION = L"12.1.1.1";

	_setMockForProgramFolder32bits(registryMockobj, osVersionMock,  L"folder\\");
	_setiTunesVersion(fileVersionInfoMock, VERSION.c_str());
	_setFileVersion(fileVersionInfoMock, VERSION);

	_setMockForMachineLanguage(registryMockobj, osVersionMock, SPANISH_LANGCODE, false);
	_setMockForUserLanguage(registryMockobj, CATALAN_LANGCODE);

	iTunes.CheckPrerequirements(NULL);
	EXPECT_THAT(iTunes.GetStatus(), AlreadyApplied);
}

TEST(iTunesActionTest, _setDefaultLanguageForUser)
{
	CreateiTunesAction;
	wchar_t lcid[1024];
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(ITUNES_REGKEY), true)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, SetString(StrCaseEq(L"LangID"),_)).WillRepeatedly(DoAll(ReadArgCharString(lcid), Return(true)));

	iTunes._setDefaultLanguageForUser();
	EXPECT_THAT(lcid, StrEq(CATALAN_LANGCODE));
}

TEST(iTunesActionTest, _getProgramLocation_x64bits_Old)
{
	CreateiTunesAction;
	wstring location;

	_setMockForProgramFolder32bits(registryMockobj, osVersionMock, L"iTunes\\");
	iTunes.SetGetFolderPath(L"c:\\Program Files (x86)");
	iTunes._getProgramLocation(location);
	EXPECT_THAT(location.c_str(), StrEq(L"c:\\Program Files (x86)\\iTunes\\iTunes.exe"));
}

TEST(iTunesActionTest, _getProgramLocation_x64bits_New)
{
	CreateiTunesAction;
	wstring location;

	_setMockForProgramFolder64bits(registryMockobj, osVersionMock, L"c:\\Program Files (x86)\\iTunes\\");
	iTunes._getProgramLocation(location);
	EXPECT_THAT(location.c_str(), StrEq(L"c:\\Program Files (x86)\\iTunes\\iTunes.exe"));
}


