﻿/* 
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
#include "XmlParser.h"
#include <windows.h>

#include <fstream>

using ::testing::StrCaseEq;

void _createXmlFile(wstring& file)
{
	char szBuff[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><parent><child1>value</child1><child2 attr=\"valattr\"></child2></parent>";
	wchar_t szFile[MAX_PATH];

	GetTempPath(MAX_PATH, szFile);
	wcscat_s(szFile, L"test.xml");
	file = szFile;

	ofstream of(szFile);
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();
}
bool ReadTagsCallback(XmlNode node, void *data)
{
	vector <wstring>* found;	

	found = (vector <wstring>*) data;	
	found->push_back(node.GetName());		
	return true;
}

TEST(XmlParserTest, ParseFile)
{	
	vector <wstring> found;
	wstring file;
	bool bRslt;

	_createXmlFile(file);

	XmlParser parser;
	bRslt = parser.Load(file);
	EXPECT_TRUE(bRslt);

	parser.Parse(ReadTagsCallback, &found);

	EXPECT_THAT(found.size(), 3);
	EXPECT_THAT(found[0], StrCaseEq(L"parent"));
	EXPECT_THAT(found[1], StrCaseEq(L"child1"));
	EXPECT_THAT(found[2], StrCaseEq(L"child2"));
	DeleteFile(file.c_str());
}