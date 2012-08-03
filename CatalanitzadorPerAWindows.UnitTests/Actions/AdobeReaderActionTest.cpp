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
#include "AdobeReaderAction.h"
#include "RegistryMock.h"
#include "RunnerMock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define ENUM_REG_INDEX0 0
#define ENUM_REG_INDEX1 1

class AdobeReaderActionTest : public AdobeReaderAction
{
public:
	
	AdobeReaderActionTest::AdobeReaderActionTest(IRegistry* registry, IRunner* runner)
		: AdobeReaderAction(registry, runner) {};
	
	public: 
			using AdobeReaderAction::_enumVersions;	
			using AdobeReaderAction::_readInstalledLang;

			wstring GetLanguage() {return m_lang;}
};


#define CreateAdobeReaderAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	AdobeReaderActionTest adobeAction(&registryMockobj, &runnerMock);


TEST(AdobeReaderActionTest, _enumVersions_95)
{
	const wchar_t* ADOBEREADER_VERSION = L"9.5";
	CreateAdobeReaderAction;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX0,_)).WillRepeatedly(DoAll(SetArgWStringPar2(wstring(ADOBEREADER_VERSION)), Return(true)));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX1,_)).WillRepeatedly(Return(false));

	vector <wstring> versions;
	adobeAction._enumVersions(versions);

	EXPECT_EQ(1, versions.size());
	EXPECT_THAT(versions.at(0), StrCaseEq(ADOBEREADER_VERSION));
}

TEST(AdobeReaderActionTest, _readInstalledLang_CAT)
{
	CreateAdobeReaderAction;
	const wchar_t* ADOBEREADER_VERSION = L"9.5";
	const wchar_t* LANGCODE = L"CAT";
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader\\9.5\\Language"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"UI"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(LANGCODE), Return(true)));

	adobeAction._readInstalledLang(wstring(ADOBEREADER_VERSION));
	
	EXPECT_THAT(adobeAction.GetLanguage(), StrCaseEq(LANGCODE));
}

TEST(AdobeReaderActionTest, IsNeed_No_AlreadyApplied)
{
	CreateAdobeReaderAction;
	const wchar_t* ADOBEREADER_VERSION = L"9.5";
	const wchar_t* LANGCODE = L"CAT";

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX0,_)).WillRepeatedly(DoAll(SetArgWStringPar2(wstring(ADOBEREADER_VERSION)), Return(true)));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX1,_)).WillRepeatedly(Return(false));
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader\\9.5\\Language"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"UI"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(LANGCODE), Return(true)));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader\\9.5\\Installer"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"ENU_GUID"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(L""), Return(false)));
	
	adobeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(adobeAction.GetStatus(), AlreadyApplied);
	EXPECT_FALSE(adobeAction.IsNeed());
}

TEST(AdobeReaderActionTest, IsNeed_No_NotInstalled)
{
	CreateAdobeReaderAction;
	const wchar_t* ADOBEREADER_VERSION = L"9.5";	

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader"), false)).WillRepeatedly(Return(false));
	
	adobeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(adobeAction.GetStatus(), NotInstalled);
	EXPECT_FALSE(adobeAction.IsNeed());
}

TEST(AdobeReaderActionTest, IsNeed_No_CannotBeApplied)
{
	CreateAdobeReaderAction;
	const wchar_t* ADOBEREADER_VERSION = L"8.0";

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX0,_)).WillRepeatedly(DoAll(SetArgWStringPar2(wstring(ADOBEREADER_VERSION)), Return(true)));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX1,_)).WillRepeatedly(Return(false));

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader\\8.0\\Language"), false)).WillRepeatedly(Return(false));
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"Software\\Adobe\\Acrobat Reader\\8.0\\Installer"), false)).WillRepeatedly(Return(false));
	
	adobeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(adobeAction.GetStatus(), CannotBeApplied);
	EXPECT_FALSE(adobeAction.IsNeed());
}
