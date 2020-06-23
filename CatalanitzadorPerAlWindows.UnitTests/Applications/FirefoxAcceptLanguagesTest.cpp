/* 
 * Copyright (C) 2012-2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "FirefoxAcceptLanguages.h"
#include "StringConversion.h"
#include "Application.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;


class FirefoxAcceptLanguagesTest : public FirefoxAcceptLanguages
{
	public:

		FirefoxAcceptLanguagesTest(wstring profileRootDir, wstring locale) :
		  FirefoxAcceptLanguages(profileRootDir, locale) { m_bNoRootLocation =  false;};

		void SetNoRootLocation(bool bNoRootLocation) {m_bNoRootLocation = bNoRootLocation;}

		using FirefoxAcceptLanguages::_getLocale;
		using FirefoxAcceptLanguages::_getLanguages;
	
	private:

		bool m_bNoRootLocation;
};

#define CreateFirefoxAction \
	RegistryMock registryMockobj; \
	FirefoxActionForTest firefoxAction(&registryMockobj);


void getPrefsJSLocation(wstring &location)
{
	Application::GetExecutionLocation(location);
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

wstring _getProfileRootDir()
{
	wstring location;

	Application::GetExecutionLocation(location);
	location += L"Firefox\\";
	return location;			
}

#define USER_PREF L"user_pref(\"intl.accept_languages\", \""

void CreateAcceptLanguage(wstring lang, wstring& accept)
{
	accept = USER_PREF;
	accept+= lang;
	accept+= L"\");";
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_CatalanValencianLocale_EmptyAccept)
{
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"ca-valencia");
	GeneratePrefsJS(wstring());

	EXPECT_FALSE(acceptLanguages.IsNeed());
}


TEST(FirefoxAcceptLanguagesTest, IsNeed_CatalanLocale_EmptyAccept)
{
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"ca");
	GeneratePrefsJS(wstring());

	EXPECT_FALSE(acceptLanguages.IsNeed());	
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_CatalanLocale_DEAccept)
{
	wstring accept;
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"ca");
	
	CreateAcceptLanguage(wstring(L"de"), accept);
	GeneratePrefsJS(wstring(accept));
	
	EXPECT_TRUE(acceptLanguages.IsNeed());
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_FrenchLocale_Empty)
{
	wstring accept;
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"fr");
	
	GeneratePrefsJS(wstring(accept));	
	EXPECT_TRUE(acceptLanguages.IsNeed());	
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_FrenchLocale_Catalan)
{
	wstring accept;
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"fr");
	
	CreateAcceptLanguage(wstring(L"ca"), accept);
	GeneratePrefsJS(wstring(accept));
	
	EXPECT_FALSE(acceptLanguages.IsNeed());	
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_FrenchLocale_CatalanSpain)
{
	wstring accept;
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"fr");
	
	CreateAcceptLanguage(wstring(L"ca-ES"), accept);
	GeneratePrefsJS(wstring(accept));
	
	EXPECT_FALSE(acceptLanguages.IsNeed());	
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_FrenchLocale_CatalanValencian)
{
	wstring accept;
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"fr");
	
	CreateAcceptLanguage(wstring(L"ca-valencia"), accept);
	GeneratePrefsJS(wstring(accept));

	EXPECT_FALSE(acceptLanguages.IsNeed());	
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_FrenchLocale_CatalanAndorra)
{
	wstring accept;
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"fr");
	
	CreateAcceptLanguage(wstring(L"ca-AD"), accept);
	GeneratePrefsJS(wstring(accept));

	EXPECT_FALSE(acceptLanguages.IsNeed());	
}

TEST(FirefoxAcceptLanguagesTest, IsNeed_NotInstalled)
{
	FirefoxAcceptLanguagesTest acceptLanguages(L"", L"");
	EXPECT_FALSE(acceptLanguages.IsNeed());
}


// Execute

TEST(FirefoxAcceptLanguagesTest, Execute_CatalanLocale_DEAccept)
{
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"ca");
	wstring accept, langcode;
	vector <wstring> * languages;
	
	CreateAcceptLanguage(wstring(L"de"), accept);
	GeneratePrefsJS(wstring(accept));
	
	acceptLanguages.Execute();	
	languages = acceptLanguages._getLanguages();

	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"ca"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"de"));	
}


TEST(FirefoxAcceptLanguagesTest, Execute_FrenchLocale_Empty)
{
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"fr");
	vector <wstring> * languages;
	
	GeneratePrefsJS(wstring());
	acceptLanguages.Execute();
	languages = acceptLanguages._getLanguages();

	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"ca"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"fr"));	
}

TEST(FirefoxAcceptLanguagesTest, Execute_SpanishLocale_English)
{
	FirefoxAcceptLanguagesTest acceptLanguages(_getProfileRootDir(), L"es");
	wstring accept;
	vector <wstring> * languages;
	
	CreateAcceptLanguage(wstring(L"en"), accept);
	GeneratePrefsJS(wstring(accept));
	acceptLanguages.Execute();
	languages = acceptLanguages._getLanguages();

	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"ca"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"en"));	
}

