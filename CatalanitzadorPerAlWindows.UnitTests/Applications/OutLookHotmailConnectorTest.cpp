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
#include "RegistryMock.h"
#include "OutLookHotmailConnector.h"
#include "RunnerMock.h"

using ::testing::StrCaseEq;
using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::HasSubstr;

class OutLookHotmailConnectorTest : public OutLookHotmailConnector
{
public:

	OutLookHotmailConnectorTest::OutLookHotmailConnectorTest(bool MSOffice2013OrHigher, IRegistry* registry)
		: OutLookHotmailConnector(MSOffice2013OrHigher, registry) {};

	virtual wstring _getOutLookPathToCatalanLibrary() { return m_outlookPath; }
	void _SetOutLookPathToCatalanLibrary(wstring path) { m_outlookPath = path; }

private:

	wstring m_outlookPath;
};

#define CreateAOutLookHotmailConnector(MSOffice2013OrHigher) \
	RegistryMock registryMockobj; \
	OutLookHotmailConnectorTest outLookHotmailConnector(MSOffice2013OrHigher, &registryMockobj);

#define CONNECTOR_REGKEY L"SOFTWARE\\Microsoft\\Office\\Outlook\\Addins\\MSNCON.Addin.1"


TEST(OutLookHotmailConnectorTest, IsNeed_False_NotInstalled)
{
	CreateAOutLookHotmailConnector(true);

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CONNECTOR_REGKEY), false)).WillRepeatedly(Return(false));
 	EXPECT_FALSE(outLookHotmailConnector.IsNeed());
}

TEST(OutLookHotmailConnectorTest, IsNeed_False_CatalanConnectorInstalled)
{
	CreateAOutLookHotmailConnector(false);
	TempFile tempFile;
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CONNECTOR_REGKEY), false)).WillRepeatedly(Return(true));

	HANDLE hFile = CreateFile(tempFile.GetFileName().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	CloseHandle(hFile);

	outLookHotmailConnector._SetOutLookPathToCatalanLibrary(tempFile.GetFileName());
 	EXPECT_FALSE(outLookHotmailConnector.IsNeed());
}

TEST(OutLookHotmailConnectorTest, IsNeed_True_RegKeyButNoDll)
{
	CreateAOutLookHotmailConnector(false);
	TempFile tempFile;
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CONNECTOR_REGKEY), false)).WillRepeatedly(Return(true));

	outLookHotmailConnector._SetOutLookPathToCatalanLibrary(tempFile.GetFileName());
 	EXPECT_TRUE(outLookHotmailConnector.IsNeed());
}

TEST(OutLookHotmailConnectorTest, Execute)
{
	CreateAOutLookHotmailConnector(false);
	TempFile tempFile;
	RunnerMock runnerMock;
	
	// Is need true
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(CONNECTOR_REGKEY), false)).WillRepeatedly(Return(true));
	outLookHotmailConnector._SetOutLookPathToCatalanLibrary(tempFile.GetFileName());

	EXPECT_CALL(runnerMock, Execute(_, HasSubstr(L"/quiet"), false)).Times(1).WillRepeatedly(Return(true));
	EXPECT_TRUE(outLookHotmailConnector.Execute(&runnerMock));
}

	
