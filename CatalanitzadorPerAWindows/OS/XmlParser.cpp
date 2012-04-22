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

void XmlParser::_parseNode(MSXML2::IXMLDOMNode *pIDOMNode, XMLNode& node)
{
	MSXML2::IXMLDOMNamedNodeMapPtr attributes;
	BSTR bstrItemText, bstrItemNode, attrName;

	pIDOMNode->get_nodeName(&bstrItemNode);						 
	pIDOMNode->get_text(&bstrItemText);
	attributes = pIDOMNode->attributes;

	node.SetName(wstring(bstrItemNode));
	node.SetText(bstrItemText);

	int len = attributes->Getlength();
	for (int i = 0; i < len; i++)
	{
		MSXML2::IXMLDOMNodePtr atrr = attributes->Getitem(i);

		atrr->get_nodeName(&attrName);
		_variant_t  val = atrr->GetnodeValue();
		
		wstring name(attrName);
		BSTR b = val.bstrVal;
		wstring value(b);

		XMLAttribute attribute(name, value);
		node.AddAtrribute(attribute);
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

	for(int i = 0; i < (NodeListPtr->length); i++)
	{
		if (pIDOMNode)
			pIDOMNode->Release();

		NodeListPtr->get_item(i, &pIDOMNode);
		pIDOMNode->get_nodeTypeString(&bstrNodeType);

		if (lstrcmp((LPCTSTR)bstrNodeType, (LPCTSTR)L"element") != 0)
			continue;
		
		XMLNode node;
		_parseNode(pIDOMNode, node);
		
		if (callback(node, data) == false)
			break;
	}

	if (pIDOMNode)
			pIDOMNode->Release();	
}

void XmlParser::_processNode(XMLNode& node)
{
	MSXML2::IXMLDOMElementPtr item;
	vector <XMLNode>* children;

	item = m_domDocument->createElement(node.GetName().c_str());
	node.SetIXMLDOMElementPtr(item);

	children = node.GetChildren();

	for (unsigned i = 0; children->size(); i++)
	{		
		XMLNode child = children->at(i);
		_processNode(child);
		m_domDocument->documentElement->appendChild(item);
	}	
}

void XmlParser::AppendNode(XMLNode node)
{
	MSXML2::IXMLDOMElementPtr item;
	vector <XMLNode>* children;

	item = m_domDocument->createElement(node.GetName().c_str());
	node.SetIXMLDOMElementPtr(item);

	children = node.GetChildren();

	for (unsigned i = 0; children->size(); i++)
	{		
		XMLNode child = children->at(i);
		_processNode(child);
		item->appendChild(child.GetIXMLDOMElementPtr());
	}
	m_domDocument->documentElement->appendChild(item);
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
	_variant_t varXml2(file.c_str());
	m_domDocument->save(varXml2);
	//TODO: error detection
	return true;
}


void XmlParser::_uninitialize()
{
	CoUninitialize();
}

void XmlParser::_initialize()
{
	if (m_domDocument != NULL)
		return;

	CoInitialize(NULL);
	 
	m_domDocument.CreateInstance(__uuidof(DOMDocument30));
}