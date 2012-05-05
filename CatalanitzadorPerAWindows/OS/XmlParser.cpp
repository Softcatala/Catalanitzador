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
#include "XmlParser.h"

// Documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/ms753821%28v=vs.85%29.aspx

XmlParser::XmlParser()
{
	m_domDocument = NULL;
	_initialize();
}

XmlParser::~XmlParser()
{
	_uninitialize();
}

void XmlParser::_parseNode(MSXML2::IXMLDOMNode *pIDOMNode, XmlNode& node)
{
	MSXML2::IXMLDOMNamedNodeMapPtr attributes;
	BSTR bstrItemText, bstrItemNode, attrName;

	pIDOMNode->get_nodeName(&bstrItemNode);						 
	pIDOMNode->get_text(&bstrItemText);
	attributes = pIDOMNode->attributes;

	node.SetName(wstring(bstrItemNode));
	node.SetText(wstring(bstrItemText));
	
	for (int i = 0; i < attributes->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr atrr = attributes->Getitem(i);

		atrr->get_nodeName(&attrName);
		_variant_t val = atrr->GetnodeValue();
		
		wstring name(attrName);
		BSTR b = val.bstrVal;
		wstring value(b);

		XmlAttribute attribute(name, value);
		node.AddAttribute(attribute);
	}
}

void XmlParser::Parse(NodeCallback callback, void *data)
{
	MSXML2::IXMLDOMNodeListPtr NodeListPtr;
	BSTR strFindText  = L" ";
	MSXML2::IXMLDOMNode *pIDOMNode = NULL;
	BSTR bstrNodeType;	

	assert(m_domDocument != NULL);

	NodeListPtr = m_domDocument->getElementsByTagName(strFindText);	

	for(int i = 0; i < NodeListPtr->length; i++)
	{
		if (pIDOMNode)
			pIDOMNode->Release();

		NodeListPtr->get_item(i, &pIDOMNode);
		pIDOMNode->get_nodeTypeString(&bstrNodeType);

		if (lstrcmp((LPCTSTR)bstrNodeType, (LPCTSTR)L"element") != 0)
			continue;
		
		XmlNode node;
		_parseNode(pIDOMNode, node);
		
		if (callback(node, data) == false)
			break;
	}

	if (pIDOMNode)
			pIDOMNode->Release();	
}

void XmlParser::_processNode(XmlNode& node)
{
	MSXML2::IXMLDOMElementPtr item;
	vector <XmlNode>* children;

	item = m_domDocument->createElement(node.GetName().c_str());
	node.SetIXMLDOMElementPtr(item);

	children = node.GetChildren();

	for (unsigned i = 0; children->size(); i++)
	{		
		XmlNode child = children->at(i);
		_processNode(child);
		m_domDocument->documentElement->appendChild(item);
	}	
}

void XmlParser::AppendNode(XmlNode root)
{
	MSXML2::IXMLDOMElementPtr item;

	item = root.GetIXMLDOMElementPtr();

	MSXML2::IXMLDOMElementPtr docElement = m_domDocument->documentElement;

	if (docElement == NULL)
	{
		m_domDocument->appendChild(item);
	}
	else
	{
		m_domDocument->documentElement->appendChild(item);
	}
}

bool XmlParser::Load(wstring file)
{	  
	_variant_t varXml(file.c_str());
	_variant_t varResult((bool)TRUE);
      
	varResult = m_domDocument->load(varXml);
	return (bool)varResult;
}

bool XmlParser::Save(wstring file)
{
	_variant_t varXml(file.c_str());
	m_domDocument->save(varXml);	
	return true;
}

void XmlParser::_uninitialize()
{
	
}

void XmlParser::_initialize()
{
	if (m_domDocument != NULL)
		return;

	CoInitialize(NULL);
	 
	m_domDocument.CreateInstance(__uuidof(DOMDocument30));
}

/*
	XmlNode
*/

void XmlNode::_createElement()
{
	if (m_itemPtr != NULL)
		return;
	
	m_itemPtr = m_document->createElement(m_Name.c_str());

	if (GetText().size() > 0)
	{
		_variant_t varText(GetText().c_str());
		m_itemPtr->put_text(varText.bstrVal);
	}

	for (unsigned int i = 0; i < m_attributes.size(); i++)
	{
		m_itemPtr->setAttribute(m_attributes[i].GetName().c_str(),
			m_attributes[i].GetValue().c_str());				
	}	
}

void XmlNode::AddChildren(XmlNode child)
{
	MSXML2::IXMLDOMElementPtr childPtr;

	if (m_itemPtr == NULL)
	{	
		_createElement();
	}

	child._createElement();
	m_itemPtr->appendChild(child.GetIXMLDOMElementPtr());
	m_children.push_back(child);
}

void XmlNode::AddAttribute(XmlAttribute attribute)
{
	assert(m_itemPtr == NULL);
	m_attributes.push_back(attribute);
}

