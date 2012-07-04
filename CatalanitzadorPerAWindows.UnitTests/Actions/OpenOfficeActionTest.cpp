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
#include "OpenOfficeAction.h"
#include "RegistryMock.h"
#include "RunnerMock.h"
#include "Application.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define ENUM_REG_INDEX0 0
#define ENUM_REG_INDEX1 1

class OpenOfficeActionTest : public OpenOfficeAction
{
public:
	
	OpenOfficeActionTest::OpenOfficeActionTest(IRegistry* registry, IRunner* runner)
		: OpenOfficeAction(registry, runner) {};
	
	public: 
			
		using OpenOfficeAction::_readVersionInstalled;
		using OpenOfficeAction::_isDefaultLanguage;
		using OpenOfficeAction::_setDefaultLanguage;

		void SetFilename(wstring file) { m_filename = file; }

	protected:

		virtual void _getPreferencesFile(wstring& location)
		{
			assert(m_filename.size() > 0);

			Application::GetExecutionLocation(location);
			location += L"OpenOffice.org\\";
			location += m_filename;
		}

	private:

		wstring m_filename;
};


#define CreateOpenOfficeAction \
	RegistryMock registryMockobj; \
	RunnerMock runnerMock; \
	OpenOfficeActionTest openofficeAction(&registryMockobj, &runnerMock);

void SetOpenOfficeVersion(RegistryMock& registryMockobj, wstring version)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(OPENOFFICCE_PROGRAM_REGKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX0,_)).WillRepeatedly(DoAll(SetArgWStringPar2(version), Return(true)));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX1,_)).WillRepeatedly(Return(false));	
}

TEST(OpenOfficeActionTest, _readVersionInstalled)
{
	const wchar_t* OPENOFFICE_VERSION = L"3.3";
	CreateOpenOfficeAction;
	
	SetOpenOfficeVersion(registryMockobj, OPENOFFICE_VERSION);	
	openofficeAction._readVersionInstalled();
	EXPECT_THAT(openofficeAction.GetVersion(), StrCaseEq(OPENOFFICE_VERSION));	
}

TEST(OpenOfficeActionTest, CheckPrerequirements_NonSupportedVersion)
{
	const wchar_t* OPENOFFICE_VERSION = L"3.1";
	CreateOpenOfficeAction;
	
	SetOpenOfficeVersion(registryMockobj, OPENOFFICE_VERSION);	
	openofficeAction.CheckPrerequirements(NULL);
	EXPECT_THAT(openofficeAction.GetStatus(), CannotBeApplied);
}

TEST(OpenOfficeActionTest, _isDefaultLanguage_No)
{
	CreateOpenOfficeAction;

	openofficeAction.SetFilename(L"registrymodifications_default_no.xcu");
	EXPECT_FALSE(openofficeAction._isDefaultLanguage());
}

TEST(OpenOfficeActionTest, _isDefaultLanguage_Yes)
{
	CreateOpenOfficeAction;

	openofficeAction.SetFilename(L"registrymodifications_default_yes.xcu");
	EXPECT_TRUE(openofficeAction._isDefaultLanguage());
}

TEST(OpenOfficeActionTest, _setDefaultLanguage)
{
	wstring src, trg;
	CreateOpenOfficeAction;
	const wchar_t* TARGET_FILE = L"registrymodifications_modified.xcu";
	
	Application::GetExecutionLocation(src);
	src += L"OpenOffice.org\\";
	trg = src;

	src += L"registrymodifications_default_no.xcu";
	trg += TARGET_FILE;

	CopyFile(src.c_str(), trg.c_str(), FALSE);
	openofficeAction.SetFilename(TARGET_FILE);
	openofficeAction._setDefaultLanguage();
	EXPECT_TRUE(openofficeAction._isDefaultLanguage());
	DeleteFile(trg.c_str());
}

