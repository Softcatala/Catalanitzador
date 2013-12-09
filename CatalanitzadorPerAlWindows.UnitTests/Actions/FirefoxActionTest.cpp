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
#include "FirefoxTest.h"
#include "Serializer.h"
#include "XmlParser.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;


class FirefoxActionForTest : public FirefoxAction
{
	public:

		FirefoxActionForTest(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : FirefoxAction(registry, runner, downloadManager) 
		{
			_firefoxAcceptLanguagesAction = NULL;
			_firefoxLangPackAction = NULL;
		};

		FirefoxLangPackAction* _firefoxLangPackAction;
		FirefoxAcceptLanguagesAction* _firefoxAcceptLanguagesAction;

		virtual FirefoxLangPackAction * _getLangPackAction() 
		{
			if (_firefoxLangPackAction)
				return _firefoxLangPackAction;

			return FirefoxAction::_getLangPackAction();
		}

		virtual FirefoxAcceptLanguagesAction * _getAcceptLanguagesAction()
		{
			if (_firefoxAcceptLanguagesAction)
				return _firefoxAcceptLanguagesAction;

			return FirefoxAction::_getAcceptLanguagesAction();
		}

		void SetFirefoxLangPackAction(FirefoxLangPackAction* action) {_firefoxLangPackAction = action;}
		void SetFirefoxAcceptLanguagesAction(FirefoxAcceptLanguagesAction* action) {_firefoxAcceptLanguagesAction = action;}

		void SetSubActionsStatus(ActionMock& firefoxLangPackAction, ActionMock& firefoxAcceptLanguagesAction, 
			ActionStatus firefoxLangPackStatus, ActionStatus firefoxAcceptLanguagesStatus)
		{			
			EXPECT_CALL(firefoxLangPackAction, CheckPrerequirements(_));
			EXPECT_CALL(firefoxAcceptLanguagesAction, CheckPrerequirements(_));
			EXPECT_CALL(firefoxLangPackAction, GetStatus()).WillRepeatedly(Return(firefoxLangPackStatus));
			EXPECT_CALL(firefoxAcceptLanguagesAction, GetStatus()).WillRepeatedly(Return(firefoxAcceptLanguagesStatus));

			SetFirefoxLangPackAction((FirefoxLangPackAction*) &firefoxLangPackAction);
			SetFirefoxAcceptLanguagesAction((FirefoxAcceptLanguagesAction*) &firefoxAcceptLanguagesAction);
		}
};


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
	FirefoxTest::_setMockForLocale(registryMockobj, VERSION);
	FirefoxTest::_setMockForInstalldir(registryMockobj, VERSION, L"");

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

TEST(FirefoxActionTest, CheckPrerequirements_NotInstalled)
{	
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"";

	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());	

	FirefoxTest::_setMockForLocale(registryMockobj, VERSION);
	FirefoxTest::_setMockForInstalldir(registryMockobj, VERSION, PATH);

	firefoxAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxAction.GetStatus(), NotInstalled);
}

TEST(FirefoxActionTest, CheckPrerequirements_BothSubAtionsStatusAlready)
{	
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"12.0 (ca)";
	ActionMock firefoxLangPackAction, firefoxAcceptLanguagesAction;

	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());	

	FirefoxTest::_setMockForLocale(registryMockobj, VERSION);
	FirefoxTest::_setMockForInstalldir(registryMockobj, VERSION, PATH);

	firefoxAction.SetSubActionsStatus(firefoxLangPackAction, firefoxAcceptLanguagesAction, AlreadyApplied, AlreadyApplied);
	firefoxAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxAction.GetStatus(), AlreadyApplied);
}

TEST(FirefoxActionTest, CheckPrerequirements_SubAtionsCannotBeAppliedAndStatusAlready)
{	
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"12.0 (ca)";
	ActionMock firefoxLangPackAction, firefoxAcceptLanguagesAction;

	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());	
	FirefoxTest::_setMockForLocale(registryMockobj, VERSION);
	FirefoxTest::_setMockForInstalldir(registryMockobj, VERSION, PATH);

	firefoxAction.SetSubActionsStatus(firefoxLangPackAction, firefoxAcceptLanguagesAction, CannotBeApplied, AlreadyApplied);
	firefoxAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxAction.GetStatus(), AlreadyApplied);
}

TEST(FirefoxActionTest, CheckPrerequirements_SubAtionsStatusAlreadyAndCannotBeApplied)
{	
	RegistryMock registryMockobj;
	RunnerMock runnerMockobj;
	const wchar_t* PATH = L"MyPath";
	const wchar_t* VERSION = L"12.0 (ca)";
	ActionMock firefoxLangPackAction, firefoxAcceptLanguagesAction;

	FirefoxActionForTest firefoxAction(&registryMockobj, (IRunner *)&runnerMockobj, &DownloadManager());	

	FirefoxTest::_setMockForLocale(registryMockobj, VERSION);
	FirefoxTest::_setMockForInstalldir(registryMockobj, VERSION, PATH);

	firefoxAction.SetSubActionsStatus(firefoxLangPackAction, firefoxAcceptLanguagesAction, AlreadyApplied, CannotBeApplied);
	firefoxAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxAction.GetStatus(), AlreadyApplied);
}
