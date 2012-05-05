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
#include "XmlParser.h"
#include "TempFile.h"

#include <fstream>

using ::testing::StrCaseEq;

void _createXmlFile(TempFile& tempfile)
{
	char szBuff[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><parent><child1>value</child1><child2 attr=\"valattr\"></child2></parent>";

	ofstream of(tempfile.GetFileName().c_str());
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
	TempFile tempfile;
	bool bRslt;

	_createXmlFile(tempfile);

	XmlParser parser;
	bRslt = parser.Load(tempfile.GetFileName());
	EXPECT_TRUE(bRslt);

	parser.Parse(ReadTagsCallback, &found);

	EXPECT_THAT(found.size(), 3);
	EXPECT_THAT(found[0], StrCaseEq(L"parent"));
	EXPECT_THAT(found[1], StrCaseEq(L"child1"));
	EXPECT_THAT(found[2], StrCaseEq(L"child2"));	
}

#define DEFAULT_LANGUAGE L"ca"

void _generateXmlNodes(XmlParser& parser, TempFile& tempfile)
{
	XmlNode item(parser.getDocument()), prop(parser.getDocument()), value(parser.getDocument());
	value.SetName(wstring(L"value"));
	value.SetText(wstring(DEFAULT_LANGUAGE));
	
	prop.SetName(wstring(L"prop"));
	prop.AddAttribute(XmlAttribute(L"oor:name", L"UILocale"));
	prop.AddAttribute(XmlAttribute(L"oorop", L"fuse"));
	prop.AddChildren(value);

	item.SetName(wstring(L"item"));
	// We need to define the namespace for the XML file to be a valid XML
	item.AddAttribute(XmlAttribute(L"xmlns:oor", L"http://openoffice.org/2001/registry"));	
	item.AddAttribute(XmlAttribute(L"oor:path", L"/org.openoffice.Office.Linguistic/General"));	
	item.AddChildren(prop);

	parser.AppendNode(item);
	parser.Save(tempfile.GetFileName());
}

TEST(XmlParserTest, Save)
{
	vector <wstring> found;
	TempFile tempfile;
	bool bRslt;

	XmlParser parser;
	_generateXmlNodes(parser, tempfile);

	XmlParser parserRead;

	bRslt = parserRead.Load(tempfile.GetFileName());
	parserRead.Parse(ReadTagsCallback, &found);
	EXPECT_TRUE(bRslt);

	EXPECT_THAT(found.size(), 3);
	EXPECT_THAT(found[0], StrCaseEq(L"item"));
	EXPECT_THAT(found[1], StrCaseEq(L"prop"));
	EXPECT_THAT(found[2], StrCaseEq(L"value"));	
}

