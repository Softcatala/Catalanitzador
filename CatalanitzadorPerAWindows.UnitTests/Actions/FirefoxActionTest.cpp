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
#include "RegistryMock.h"
#include "FirefoxAction.h"
#include <iostream>
#include <fstream>

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;

void getExecutionLocation(wstring &location);

#define SetLocale(registryMockobj, locale) \
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(L"SOFTWARE\\Mozilla\\Mozilla Firefox"), false)).WillRepeatedly(Return(true));  \
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"CurrentVersion"),_ ,_)).  \
	WillRepeatedly(DoAll(SetArgCharStringPar2(locale), Return(true)));

class FirefoxActionForTest : public FirefoxAction
{
public:

	FirefoxActionForTest (IRegistry* registry) : FirefoxAction(registry) {};	

	protected:
		virtual void _getProfileRootDir(wstring &location) 
		{
			getExecutionLocation(location);
			location += L"Firefox\\";
		}

	public: using FirefoxAction::_readVersionAndLocale;
	public: using FirefoxAction::_getLocale;
	public: using FirefoxAction::_getLanguages;

};

#define CreateFirefoxAction \
	RegistryMock registryMockobjWin; \
	FirefoxActionForTest firefoxAction(&registryMockobjWin);


void getExecutionLocation(wstring &location)
{
	HMODULE hModule;
	wchar_t name[2048];
	int i;

	hModule = GetModuleHandle(NULL);
	GetModuleFileName(hModule, name, sizeof(name));

	for (i = wcslen(name); i > 0 && name[i] != '\\' ; i--);

	name[i+1] = NULL;
	location = name;
}

void getPrefsJSLocation(wstring &location)
{
	getExecutionLocation(location);
	location+=L"Firefox\\Profiles\\0u5lxfv2.default\\prefs.js";
}

void GeneratePrefsJS(wstring wcontent)
{
	wstring location;
	string content;

	getPrefsJSLocation(location);

	StringConversion::ToMultiByte(wcontent, content);

	ofstream myfile;
	myfile.open(location.c_str());

	if (myfile.is_open())
	{
		myfile << content.c_str();
		myfile.close();
	}
}

#define USER_PREF L"user_pref(\"intl.accept_languages\", \""

void CreateAcceptLanguage(wstring lang, wstring& accept)
{
	accept = USER_PREF;
	accept+= lang;
	accept+= L"\");";
}

/*
	Tests
*/


TEST(FirefoxActionTest, _readVersionAndLocale)
{
	RegistryMock registryMockobj; 
	FirefoxActionForTest firefoxAction(&registryMockobj);

	SetLocale(registryMockobj, L"12.0 (ca)");

	firefoxAction._readVersionAndLocale();
	EXPECT_THAT(firefoxAction.GetVersion(), StrCaseEq("12.0"));
	EXPECT_THAT(firefoxAction._getLocale()->c_str(), StrCaseEq(L"ca"));
}

TEST(FirefoxActionTest, IsNeed_CatalanLocale_EmptyAccept)
{	
	RegistryMock registryMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj);

	SetLocale(registryMockobj, L"12.0 (ca)");	
	GeneratePrefsJS(wstring());

	EXPECT_FALSE(firefoxAction.IsNeed());
	EXPECT_EQ(firefoxAction.GetStatus(), AlreadyApplied);
}

TEST(FirefoxActionTest, IsNeed_CatalanLocale_DEAccept)
{
	wstring accept;
	RegistryMock registryMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj);

	SetLocale(registryMockobj, L"12.0 (ca)");
	CreateAcceptLanguage(wstring(L"de"), accept);
	GeneratePrefsJS(wstring(accept));

	EXPECT_TRUE(firefoxAction.IsNeed());
	EXPECT_NE(firefoxAction.GetStatus(), AlreadyApplied);
}

TEST(FirefoxActionTest, IsNeed_FrenchLocale_Empty)
{
	wstring accept;
	RegistryMock registryMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj);

	SetLocale(registryMockobj, L"12.0 (fr)");	
	GeneratePrefsJS(wstring(accept));

	EXPECT_TRUE(firefoxAction.IsNeed());
	EXPECT_NE(firefoxAction.GetStatus(), AlreadyApplied);
}

TEST(FirefoxActionTest, IsNeed_FrenchLocale_Catalan)
{
	wstring accept;
	RegistryMock registryMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj);

	SetLocale(registryMockobj, L"12.0 (fr)");
	CreateAcceptLanguage(wstring(L"ca"), accept);
	GeneratePrefsJS(wstring(accept));

	EXPECT_FALSE(firefoxAction.IsNeed());
	EXPECT_EQ(firefoxAction.GetStatus(), AlreadyApplied);
}

// Execute

TEST(FirefoxActionTest, Execute_CatalanLocale_DEAccept)
{
	wstring accept, langcode;
	RegistryMock registryMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj);
	vector <wstring> * languages;

	SetLocale(registryMockobj, L"12.0 (ca)");
	CreateAcceptLanguage(wstring(L"de"), accept);
	GeneratePrefsJS(wstring(accept));
	firefoxAction.Execute();	
	languages = firefoxAction._getLanguages();

	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"ca"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"de"));
	EXPECT_EQ(firefoxAction.GetStatus(), Successful);
}

TEST(FirefoxActionTest, Execute_FrenchLocale_Empty)
{	
	RegistryMock registryMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj);
	vector <wstring> * languages;

	SetLocale(registryMockobj, L"12.0 (fr)");
	GeneratePrefsJS(wstring());
	firefoxAction.Execute();
	languages = firefoxAction._getLanguages();

	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"ca"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"fr"));
	EXPECT_EQ(firefoxAction.GetStatus(), Successful);
}

TEST(FirefoxActionTest, Execute_SpanishLocale_English)
{	
	wstring accept;
	RegistryMock registryMockobj;
	FirefoxActionForTest firefoxAction(&registryMockobj);
	vector <wstring> * languages;

	SetLocale(registryMockobj, L"12.0 (es)");
	CreateAcceptLanguage(wstring(L"en"), accept);
	GeneratePrefsJS(wstring(accept));
	firefoxAction.Execute();
	languages = firefoxAction._getLanguages();

	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"ca"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"en"));
	EXPECT_EQ(firefoxAction.GetStatus(), Successful);
}
