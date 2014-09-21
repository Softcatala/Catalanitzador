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
#include "Options.h"
#include <sstream>
#include <string>

using ::testing::HasSubstr;
using ::testing::StrCaseEq;

TEST(OptionTest, Option_ValueString)
{
	const wchar_t* VALUE = L"123";
	ostringstream stream;

	Option option(OptionSystemRestore, wstring(VALUE));
	option.Serialize(&stream);
	
	EXPECT_THAT(stream.str(), HasSubstr("<option id='0' value='123'/>"));
}

TEST(OptionTest, Option_ValueInt)
{
	const int VALUE = 12345;
	ostringstream stream;

	Option option(OptionSystemRestore, VALUE);
	option.Serialize(&stream);
	
	EXPECT_THAT(stream.str(), HasSubstr("<option id='0' value='12345'/>"));
}

TEST(OptionTest, Option_ValueBool)
{
	const bool VALUE = true;
	ostringstream stream;

	Option option(OptionSystemRestore, VALUE);
	option.Serialize(&stream);
	
	EXPECT_THAT(stream.str(), HasSubstr("<option id='0' value='1'/>"));
}

TEST(OptionTest, Option_Getters)
{
	const wchar_t* VALUE = L"123";
	ostringstream stream;

	Option option(OptionSystemRestore, wstring(VALUE));
	option.Serialize(&stream);
	
	EXPECT_THAT(option.GetOptionId(), OptionSystemRestore);
	EXPECT_THAT(option.GetValue(), StrCaseEq(VALUE));
}

