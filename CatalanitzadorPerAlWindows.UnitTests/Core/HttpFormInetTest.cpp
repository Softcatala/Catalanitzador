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
#include "HttpFormInet.h"

using ::testing::StrCaseEq;

TEST(HttpFormInetTest, UrlFormEncode_String)
{
	HttpFormInet form;
	string input, expected, encoded;
	vector <string> variables;
	vector <string> values;

	input = "<application MajorVersion='0' MinorVersion='9' Revision='6' />";
	expected = "xml=%3Capplication+MajorVersion%3D%270%27+MinorVersion%3D%279%27+Revision%3D%276%27+%2F%3E";

	variables.push_back("xml");
	values.push_back(input);
	form.UrlFormEncode(variables, values, encoded);

	EXPECT_THAT(encoded, StrCaseEq(expected));
}
