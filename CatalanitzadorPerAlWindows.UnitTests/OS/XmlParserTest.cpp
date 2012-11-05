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


using ::testing::StrCaseEq;

void _createXmlFile(TempFile& tempfile)
{
	char szBuff[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><parent><child1>value</child1><child2 attr=\"valattr\">text_child2</child2></parent>";

	ofstream of(tempfile.GetFileName().c_str());
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();
}
bool _readTagsCallback(XmlNode node, void *data)
{
	vector <XmlNode>* found;

	found = (vector <XmlNode>*) data;
	found->push_back(node);
	return true;
}

TEST(XmlParserTest, ParseFile)
{	
	vector <XmlNode> found;
	TempFile tempfile;
	bool bRslt;

	_createXmlFile(tempfile);

	XmlParser parser;
	bRslt = parser.Load(tempfile.GetFileName());
	EXPECT_TRUE(bRslt);

	parser.Parse(_readTagsCallback, &found);

	EXPECT_THAT(found.size(), 3);
	EXPECT_THAT(found[0].GetName(), StrCaseEq(L"parent"));
	EXPECT_THAT(found[1].GetName(), StrCaseEq(L"child1"));
	EXPECT_THAT(found[2].GetName(), StrCaseEq(L"child2"));
}

TEST(XmlParserTest, ParseFileText)
{	
	vector <XmlNode> found;
	TempFile tempfile;
	bool bRslt;

	_createXmlFile(tempfile);

	XmlParser parser;
	bRslt = parser.Load(tempfile.GetFileName());
	EXPECT_TRUE(bRslt);

	parser.Parse(_readTagsCallback, &found);
	EXPECT_TRUE(found[0].GetText().empty());
	EXPECT_THAT(found[1].GetText(), StrCaseEq(L"value"));
}

TEST(XmlParserTest, ParseFileAttributes)
{	
	vector <XmlNode> found;
	TempFile tempfile;
	bool bRslt;

	_createXmlFile(tempfile);

	XmlParser parser;
	bRslt = parser.Load(tempfile.GetFileName());
	EXPECT_TRUE(bRslt);

	parser.Parse(_readTagsCallback, &found);
	
	EXPECT_THAT(found[2].GetName(), StrCaseEq(L"child2"));
	EXPECT_THAT(found[2].GetAttributes()->size(), 1);
	EXPECT_THAT(found[2].GetAttributes()->at(0).GetName(), StrCaseEq(L"attr"));
	EXPECT_THAT(found[2].GetAttributes()->at(0).GetValue(), StrCaseEq(L"valattr"));
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
	vector <XmlNode> found;
	TempFile tempfile;
	bool bRslt;

	XmlParser parser;
	_generateXmlNodes(parser, tempfile);

	XmlParser parserRead;

	bRslt = parserRead.Load(tempfile.GetFileName());
	parserRead.Parse(_readTagsCallback, &found);
	EXPECT_TRUE(bRslt);

	EXPECT_THAT(found.size(), 3);
	EXPECT_THAT(found[0].GetName(), StrCaseEq(L"item"));
	EXPECT_THAT(found[1].GetName(), StrCaseEq(L"prop"));
	EXPECT_THAT(found[2].GetName(), StrCaseEq(L"value"));
}

