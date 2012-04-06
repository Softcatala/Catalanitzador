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

class IEAcceptLanguagesActionTest : public IEAcceptLanguagesAction
{
public:

	IEAcceptLanguagesActionTest::IEAcceptLanguagesActionTest(IRegistry* registry)
		: IEAcceptLanguagesAction(registry) {};

	public: using IEAcceptLanguagesAction::_parseLanguage;
	public: using IEAcceptLanguagesAction::_createRegistryString;
	public: using IEAcceptLanguagesAction::_addCatalanToArrayAndRemoveOldIfExists;
	public: using IEAcceptLanguagesAction::_getLanguages;

};


#define CreateIEAcceptLanguagesAction \
	RegistryMock registryMockobj; \
	IEAcceptLanguagesActionTest IEAction(&registryMockobj);


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

TEST(IEAcceptLanguagesActionTest, _parseLanguagesSpanishEnglishCatalan)
{
	CreateIEAcceptLanguagesAction;	
	vector <wstring> * languages;

	IEAction._parseLanguage(L"en-US,es-ES;q=0.7,ca-ES;q=0.3");
	languages = IEAction._getLanguages();

	EXPECT_EQ(3, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"en-US"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
	EXPECT_THAT(languages->at(2), StrCaseEq(L"ca-ES"));
}

TEST(IEAcceptLanguagesActionTest, _parseLanguagesSpanishEnglish)
{
	CreateIEAcceptLanguagesAction;	
	vector <wstring> * languages;

	IEAction._parseLanguage(L"en-US,es-ES;q=0.5");
	languages = IEAction._getLanguages();
		
	EXPECT_EQ(2, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"en-US"));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
}

TEST(IEAcceptLanguagesActionTest, _parseLanguagesSpanish)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;

	IEAction._parseLanguage(L"es-ES");
	languages = IEAction._getLanguages();

	EXPECT_EQ(1, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(L"es-ES"));
}

TEST(IEAcceptLanguagesActionTest, _parseLanguagesNone)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;

	IEAction._parseLanguage(L"");
	languages = IEAction._getLanguages();

	EXPECT_EQ(0, languages->size());	
}

TEST(IEAcceptLanguagesActionTest, _createRegistryStringCatalan)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction._getLanguages();
	languages->push_back(L"ca-ES");
	
	IEAction._createRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES"));
}

TEST(IEAcceptLanguagesActionTest, _createRegistryStringCatalanSpanish)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction._getLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	
	IEAction._createRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.5"));
}

TEST(IEAcceptLanguagesActionTest, _createRegistryStringCatalanSpanishEnglish)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction._getLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");

	IEAction._createRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.7,en-US;q=0.3"));
}

TEST(IEAcceptLanguagesActionTest, _createRegistryStringCatalanSpanishEnglishGerman)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction._getLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");
	languages->push_back(L"de-DE");

	IEAction._createRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.8,en-US;q=0.5,de-DE;q=0.3"));
}

TEST(IEAcceptLanguagesActionTest, _createRegistryStringCatalanSpanishEnglishGermanEuskera)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;
	wstring regvalue;

	languages = IEAction._getLanguages();
	languages->push_back(L"ca-ES");
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");
	languages->push_back(L"de-DE");
	languages->push_back(L"eu-ES");

	IEAction._createRegistryString(regvalue);

	EXPECT_THAT(regvalue, StrCaseEq(L"ca-ES,es-ES;q=0.8,en-US;q=0.6,de-DE;q=0.4,eu-ES;q=0.2"));
}

TEST(IEAcceptLanguagesActionTest, _addCatalanToArrayAndRemoveOldIfExistsNoCatalan)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;	

	languages = IEAction._getLanguages();
	languages->push_back(L"es-ES");
	languages->push_back(L"en-US");	

	IEAction._addCatalanToArrayAndRemoveOldIfExists();

	EXPECT_EQ(3, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(LANGCODE_SET));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
	EXPECT_THAT(languages->at(2), StrCaseEq(L"en-US"));
}

TEST(IEAcceptLanguagesActionTest, _addCatalanToArrayAndRemoveOldIfExistsCatalan)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;	

	languages = IEAction._getLanguages();
	languages->push_back(L"es-ES");
	languages->push_back(L"ca-ES");
	languages->push_back(L"en-US");	

	IEAction._addCatalanToArrayAndRemoveOldIfExists();

	EXPECT_EQ(3, languages->size());
	EXPECT_THAT(languages->at(0), StrCaseEq(LANGCODE_SET));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es-ES"));
	EXPECT_THAT(languages->at(2), StrCaseEq(L"en-US"));
}

TEST(IEAcceptLanguagesActionTest, AddCatalanToArrayAndRemoveOldIE6)
{
	CreateIEAcceptLanguagesAction;
	vector <wstring> * languages;	

	languages = IEAction._getLanguages();
	languages->push_back(L"es");
	languages->push_back(L"ca");	

	IEAction._addCatalanToArrayAndRemoveOldIfExists();

	EXPECT_EQ(2, languages->size());	
	EXPECT_THAT(languages->at(0), StrCaseEq(LANGCODE_SET));
	EXPECT_THAT(languages->at(1), StrCaseEq(L"es"));
}
