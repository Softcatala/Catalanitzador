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
#include "XmlAttribute.h"

#include <string>
#import <msxml3.dll>
#include <vector>

class XmlNode
{
public:

	XmlNode()
	{
		m_document = NULL;
	}

	XmlNode(MSXML2::IXMLDOMDocumentPtr document)
	{
		m_document = document;
	}

	void SetName(wstring name) 
	{
		assert(m_itemPtr == NULL);
		m_Name = name;
	}
	wstring GetName() const {return m_Name; }

	void SetText(wstring text) 
	{
		assert(m_itemPtr == NULL);
		m_Text = text;
	}
	wstring GetText() const {return m_Text; }

	MSXML2::IXMLDOMElementPtr GetIXMLDOMElementPtr() const { return m_itemPtr;}
	void SetIXMLDOMElementPtr(MSXML2::IXMLDOMElementPtr itemPtr) { m_itemPtr = itemPtr;}
	
	vector <XmlNode>* GetChildren() {return &m_children; }
	vector <XmlAttribute>* GetAttributes() {return &m_attributes;}

	void AddChildren(XmlNode child);
	void AddAttribute(XmlAttribute attribute);
	void CreateElement();
	
private:	

	vector <XmlAttribute> m_attributes;
	vector <XmlNode> m_children;
	wstring m_Name;
	wstring m_Text;
	MSXML2::IXMLDOMElementPtr m_itemPtr;
	MSXML2::IXMLDOMDocumentPtr m_document;
};


