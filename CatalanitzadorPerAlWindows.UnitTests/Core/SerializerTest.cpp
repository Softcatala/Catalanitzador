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
#include "Serializer.h"
#include "XmlParser.h"

using ::testing::StrCaseEq;

bool SerializerReadTagsCallback(XmlNode node, void *data)
{
	vector <wstring>* found;	

	found = (vector <wstring>*) data;	
	found->push_back(node.GetName());		
	return true;
}

TEST(VersionTest, SaveToString)
{
	vector <wstring> found;	
	bool bRslt;
	TempFile tempFile;
	Serializer serializer;

	serializer.OpenHeader();
	serializer.CloseHeader();
	serializer.SaveToFile(tempFile.GetFileName());

	XmlParser parser;
	bRslt = parser.Load(tempFile.GetFileName());
	EXPECT_TRUE(bRslt);

	parser.Parse(SerializerReadTagsCallback, &found);
	
	EXPECT_THAT(found.size(), 3);
	EXPECT_THAT(found[0], StrCaseEq(L"execution"));
	EXPECT_THAT(found[1], StrCaseEq(L"application"));
	EXPECT_THAT(found[2], StrCaseEq(L"operating"));
}
