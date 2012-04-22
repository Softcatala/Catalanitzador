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
 
#pragma once

#include "Defines.h"
#include "StringConversion.h"

#include <Windows.h>
#include <WinInet.h>
#include <string>
#import <msxml3.dll>
#include <vector>

using namespace std;
using namespace MSXML2;


class _APICALL XMLAttribute
{
public:

	XMLAttribute() {};

	XMLAttribute(wstring name, wstring value)
	{
		m_name = name;
		m_value = value;
	}

	wstring GetName() {return m_name; }
	wstring GetValue() {return m_value; }

private:

	wstring m_name;
	wstring m_value;
};

class _APICALL XMLNode
{
public:

	void SetName(wstring name) { m_Name = name;}
	wstring GetName() {return m_Name; }

	void SetText(wstring text) { m_Text = text;}
	wstring GetText() {return m_Text; }

	MSXML2::IXMLDOMElementPtr GetIXMLDOMElementPtr() { return m_itemPtr;}
	void SetIXMLDOMElementPtr(MSXML2::IXMLDOMElementPtr itemPtr) { m_itemPtr = itemPtr;}
	
	vector <XMLNode>* GetChildren() {return &m_children; }
	void AddChildren(XMLNode node)
	{
		m_children.push_back(node);
	}

	vector <XMLAttribute>* GetAtrributes() {return &m_attributes;}
	void AddAtrribute(XMLAttribute attribute)
	{
		m_attributes.push_back(attribute);
	}

private:
	vector <XMLAttribute> m_attributes;
	vector <XMLNode> m_children;
	wstring m_Name;
	wstring m_Text;
	MSXML2::IXMLDOMElementPtr m_itemPtr;
};


typedef bool (*NodeCallback)(XMLNode node, void *data);

class _APICALL XmlParser
{
public:
		XmlParser();
		~XmlParser();
		void Parse(NodeCallback callback, void *data);
		bool Load(wstring file);
		bool Save(wstring file);
		void AppendNode(XMLNode node);
		
private:
		void _initialize();
		void _uninitialize();
		void _parseNode(MSXML2::IXMLDOMNode *pIDOMNode, XMLNode& node);
		void _processNode(XMLNode& node);

		MSXML2::IXMLDOMDocumentPtr m_domDocument;
};