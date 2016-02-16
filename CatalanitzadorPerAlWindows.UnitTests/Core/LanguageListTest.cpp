/* 
 * Copyright (C) 2016 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "LanguageList.h"

using ::testing::StrCaseEq;
using ::testing::StrCaseNe;


TEST(LanguageListTest, GetFirstLanguage)
{
	LanguageList languagelist(L"es, ca, de");
	EXPECT_THAT(languagelist.GetFirstLanguage(), StrCaseEq(L"es"));
}

TEST(LanguageListTest, GetWithCatalanAdded)
{
	LanguageList languagelist(L"es, de, fr-FR");
	wstring lang = languagelist.GetWithCatalanAdded().substr(0, 2);
	EXPECT_THAT(lang, StrCaseEq(L"ca"));
}
