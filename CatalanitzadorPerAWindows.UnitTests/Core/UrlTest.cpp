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
#include "Url.h"

using ::testing::StrCaseEq;

TEST(UrlTest, UrlFormEncode)
{	
	wstring input, expected, encoded;

	input = L"Jo ja he catalanitzat el meu ordinador! Tu també pots amb el Catalanitzador de Softcatalà #catalanitzador";
	expected = L"Jo%20ja%20he%20catalanitzat%20el%20meu%20ordinador%21%20Tu%20tamb%C3%A9%20pots%20amb%20el%20Catalanitzador%20de%20Softcatal%C3%A0%20%23catalanitzador";
	
	Url::EncodeParameter(input, encoded);
	
	EXPECT_THAT(encoded, StrCaseEq(expected));
}

TEST(UrlTest, GetFileName)
{
	Url url (L"http://www.softcatala.org/pub/softcatala/catalanitzador/office2010/LanguageInterfacePack-x86-ca-es.exe");

	EXPECT_THAT(url.GetFileName(), StrCaseEq(L"LanguageInterfacePack-x86-ca-es.exe"));
}

TEST(UrlTest, GetPathAndFileName)
{
	Url url (L"http://www.softcatala.org/pub/softcatala/catalanitzador/office2010/LanguageInterfacePack-x86-ca-es.exe");

	EXPECT_THAT(url.GetPathAndFileName(), StrCaseEq(L"/pub/softcatala/catalanitzador/office2010/LanguageInterfacePack-x86-ca-es.exe"));
}

TEST(UrlTest, GetHostname)
{
	Url url (L"http://www.softcatala.org/pub/softcatala/catalanitzador/office2010/LanguageInterfacePack-x86-ca-es.exe");

	EXPECT_THAT(url.GetHostname(), StrCaseEq(L"www.softcatala.org"));

}