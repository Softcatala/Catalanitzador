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
#include "StringConversion.h"

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

/*
		<id>12</id>
		We are only interested in the text from the next text node
*/

	
void XmlParser::_parseNodeGetText(MSXML2::IXMLDOMNode *pIDOMNode, XmlNode& node)
{	
	MSXML2::IXMLDOMNode* ppFirstChild;
	BSTR bstrNodeType;

	pIDOMNode->get_firstChild(&ppFirstChild);
	
	if (ppFirstChild == NULL)
		return;
	
	ppFirstChild->get_nodeTypeString(&bstrNodeType);

	if (lstrcmp((LPCTSTR)bstrNodeType, (LPCTSTR)L"text") == 0 ||
		(lstrcmp((LPCTSTR)bstrNodeType, (LPCTSTR)L"cdatasection") == 0))
	{
		BSTR bstrItemText;

		ppFirstChild->get_text(&bstrItemText);
		node.SetText(wstring(bstrItemText));
	}
}

void XmlParser::_parseNodeGetAttributes(MSXML2::IXMLDOMNode *pIDOMNode, XmlNode& node)
{
	MSXML2::IXMLDOMNamedNodeMapPtr attributes = pIDOMNode->attributes;
	BSTR attrName;

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

void XmlParser::_parseNode(MSXML2::IXMLDOMNode *pIDOMNode, XmlNode& node)
{	
	BSTR bstrItemNode;
	
	pIDOMNode->get_nodeName(&bstrItemNode);
	node.SetName(wstring(bstrItemNode));

	_parseNodeGetText(pIDOMNode, node);
	_parseNodeGetAttributes(pIDOMNode, node);
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

bool XmlParser::FindNode(wstring wfind, XmlNode& node)
{
	string find;
	
	StringConversion::ToMultiByte(wfind, find);
	MSXML2::IXMLDOMNodePtr pFindNode = m_domDocument->selectSingleNode(find.c_str());
	if (pFindNode == NULL)
		return false;

	_parseNode(pFindNode, node);
	node.SetIXMLDOMElementPtr(pFindNode);
	return true;
}

bool XmlParser::ReplaceNode(XmlNode nodeNew, XmlNode nodeOld)
{
	MSXML2::IXMLDOMNodePtr pParentNode;
			
	nodeOld.GetIXMLDOMElementPtr()->get_parentNode(&pParentNode);
	return pParentNode->replaceChild(nodeNew.GetIXMLDOMElementPtr(), nodeOld.GetIXMLDOMElementPtr());
}
