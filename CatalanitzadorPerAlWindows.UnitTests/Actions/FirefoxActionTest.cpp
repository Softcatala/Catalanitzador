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
#include "FirefoxAction.h"
#include "Serializer.h"
#include "XmlParser.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

class FirefoxActionForTest : public FirefoxAction
{
	public:

		FirefoxActionForTest(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : FirefoxAction(registry, runner, downloadManager) {};

		using FirefoxAction::_readVersionAndLocale;
		using FirefoxAction::_getLocale;
		using FirefoxAction::_readInstallPath;
};

void _setMockForLocale(RegistryMock& registryMockobj, const wchar_t* locale)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Mozilla\\Mozilla Firefox"), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"CurrentVersion"),_ ,_)).	WillRepeatedly(DoAll(SetArgCharStringPar2(locale), Return(true)));
}

void _setMockForInstalldir(RegistryMock& registryMockobj, const wchar_t* locale, const wchar_t* installdir)
{
	wchar_t szKeyName[2048];

	swprintf_s(szKeyName, L"SOFTWARE\\Mozilla\\Mozilla Firefox\\%s\\Main", locale);
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(szKeyName), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Install Directory"),_ ,_)).WillRepeatedly(DoAll(SetArgCharStringPar2(installdir), Return(true)));
}

bool FirefoxSerializerReadActionsIDs(XmlNode node, void *data)
{
	vector <XmlAttribute>* attributes;
	int *cnt = (int *) data;
	
	if (node.GetName() != L"action")
		return true;
	
	attributes = node.GetAttributes();
	for (unsigned int i = 0; i < attributes->size(); i++)
	{
		XmlAttribute attribute = attributes->at(i);

		if (attribute.GetName() == L"id")
		{
			int id = _wtoi(attribute.GetValue().c_str());

			if (id == FirefoxActionID || id == FirefoxLangPackActionID)
				*cnt = *cnt + 1;
		}
	}	
	return true;
} 

TEST(FirefoxActionTest, _readVersionAndLocale)
{
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());

	_setMockForLocale(registryMockobj, L"12.0 (ca)");

	firefoxAction._readVersionAndLocale();
	EXPECT_THAT(firefoxAction.GetVersion(), StrCaseEq(L"12.0"));
	EXPECT_THAT(firefoxAction._getLocale(), StrCaseEq(L"ca"));	
}

TEST(FirefoxActionTest, _readInstallPath)
{
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"12.0 (ca)";
	wstring path;	

	_setMockForLocale(registryMockobj, VERSION);
	_setMockForInstalldir(registryMockobj, VERSION, PATH);

	firefoxAction._readInstallPath(path);
	EXPECT_THAT(path.c_str(), StrCaseEq(PATH));
}

TEST(FirefoxActionTest, _Serialize)
{
	bool bRslt;
	TempFile tempFile;
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	Serializer serializer;
	int cnt = 0;
	const wchar_t* VERSION = L"12.0 (ca)";

	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());
	_setMockForLocale(registryMockobj, VERSION);
	_setMockForInstalldir(registryMockobj, VERSION, L"");

	serializer.OpenHeader();
	firefoxAction.Serialize(serializer.GetStream());
	serializer.CloseHeader();
	serializer.SaveToFile(tempFile.GetFileName());

	XmlParser parser;
	bRslt = parser.Load(tempFile.GetFileName());
	EXPECT_TRUE(bRslt);

	parser.Parse(FirefoxSerializerReadActionsIDs, &cnt);
	EXPECT_THAT(cnt, 2);
}
