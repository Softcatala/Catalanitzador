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
#include "IEAcceptLanguagesAction.h"
#include "RegistryMock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define LANGCODE_SET L"ca"

#define CreateIEAcceptLanguagesAction \
	RegistryMock registryMockobj; \
	IEAcceptLanguagesAction IEAction(&registryMockobj);


TEST(IEAcceptLanguagesActionTest, IsNeeded_CatalanOnly)
{
	CreateIEAcceptLanguagesAction;	
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Internet Explorer\\International"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"AcceptLanguage"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"ca-ES"), Return(true)));

	EXPECT_FALSE(IEAction.IsNeed());
}

TEST(IEAcceptLanguagesActionTest, IsNeeded_CatalanAndSpanish)
{
	CreateIEAcceptLanguagesAction;	
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Internet Explorer\\International"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"AcceptLanguage"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"ca-ES,es-ES;q=0.5"), Return(true)));

	EXPECT_FALSE(IEAction.IsNeed());
}

TEST(IEAcceptLanguagesActionTest, IsNeeded_EnglishSpanishCatalan)
{
	CreateIEAcceptLanguagesAction;	
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Internet Explorer\\International"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"AcceptLanguage"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"en-US,es-ES;q=0.7,ca-ES;q=0.3"), Return(true)));

	EXPECT_TRUE(IEAction.IsNeed());
}

TEST(IEAcceptLanguagesActionTest, IsNeeded_SpanishOnly)
{
	CreateIEAcceptLanguagesAction;	
	
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_CURRENT_USER, StrCaseEq(L"Software\\Microsoft\\Internet Explorer\\International"), false)).WillRepeatedly(Return(true));	
	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"AcceptLanguage"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(L"es-ES"), Return(true)));

	EXPECT_TRUE(IEAction.IsNeed());
}

TEST(IEAcceptLanguagesActionTest, ParseLanguagesSpanishEnglishCatalan)
{
	CreateIEAcceptLanguagesAction;	
	vector <wstring> * languages;

	IEAction.ParseLanguage(L"en-US,es-ES;q=0.7,ca-ES;q=0.3");
	languages = IEAction.GetLanguages();

	EXPECT_EQ(3, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"en-US"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
	EXPECT_THAT(languages->at(2), StrCaseEq(L"ca-ES"));
}

TEST(IEAcceptLanguagesActionTest, ParseLanguagesSpanishEnglish)
{
	CreateIEAcceptLanguagesAction;	
	vector <wstring> * languages;

	IEAction.ParseLanguage(L"en-US,es-ES;q=0.5");
	languages = IEAction.GetLanguages();
		
	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"en-US"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
}

TEST(IEAcceptLanguagesActionTest, ParseLanguagesSpanish)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;

	IEAction.ParseLanguage(L"es-ES");
	languages = IEAction.GetLanguages();

	EXPECT_EQ(1, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"es-ES"));
}

TEST(IEAcceptLanguagesActionTest, ParseLanguagesNone)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;

	IEAction.ParseLanguage(L"");
	languages = IEAction.GetLanguages();

	EXPECT_EQ(0, languages->size());	
}

TEST(IEAcceptLanguagesActionTest, CreateRegistryStringCatalan)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction.GetLanguages();
	languages->push_back(L"ca-ES");
	
	IEAction.CreateRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES"));
}

TEST(IEAcceptLanguagesActionTest, CreateRegistryStringCatalanSpanish)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction.GetLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	
	IEAction.CreateRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.5"));
}

TEST(IEAcceptLanguagesActionTest, CreateRegistryStringCatalanSpanishEnglish)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction.GetLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");

	IEAction.CreateRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.7,en-US;q=0.3"));
}

TEST(IEAcceptLanguagesActionTest, CreateRegistryStringCatalanSpanishEnglishGerman)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction.GetLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");
	languages->push_back(L"de-DE");

	IEAction.CreateRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.8,en-US;q=0.5,de-DE;q=0.3"));
}

TEST(IEAcceptLanguagesActionTest, CreateRegistryStringCatalanSpanishEnglishGermanEuskera)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction.GetLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");
	languages->push_back(L"de-DE");
	languages->push_back(L"eu-ES");

	IEAction.CreateRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.8,en-US;q=0.6,de-DE;q=0.4,eu-ES;q=0.2"));
}

TEST(IEAcceptLanguagesActionTest, AddCatalanToArrayAndRemoveOldIfExistsNoCatalan)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;	

	languages = IEAction.GetLanguages();
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");	

	IEAction.AddCatalanToArrayAndRemoveOldIfExists();

	EXPECT_EQ(3, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(LANGCODE_SET));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
	EXPECT_THAT(languages->at(2), StrCaseEq(L"en-US"));
}

TEST(IEAcceptLanguagesActionTest, AddCatalanToArrayAndRemoveOldIfExistsCatalan)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;	

	languages = IEAction.GetLanguages();
	languages->push_back(L"es-ES");
	languages->push_back(L"ca-ES");
	languages->push_back(L"en-US");	

	IEAction.AddCatalanToArrayAndRemoveOldIfExists();

	EXPECT_EQ(3, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(LANGCODE_SET));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
	EXPECT_THAT(languages->at(2), StrCaseEq(L"en-US"));
}

TEST(IEAcceptLanguagesActionTest, AddCatalanToArrayAndRemoveOldIE6)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;	

	languages = IEAction.GetLanguages();
	languages->push_back(L"es");
	languages->push_back(L"ca");	

	IEAction.AddCatalanToArrayAndRemoveOldIfExists();

	EXPECT_EQ(2, languages->size());	
	EXPECT_THAT(languages->at(0), StrCaseEq(LANGCODE_SET));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es"));
}
