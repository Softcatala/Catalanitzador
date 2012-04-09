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


bool XmlParser::Parse(wstring file, NodeCallback callback)
{
	_initialize();
	if (_load(file) == false)
	{
		_uninitialize();
		return false;
	}

	MSXML2::IXMLDOMNodeListPtr NodeListPtr;
	BSTR strFindText  = L" ";
	MSXML2::IXMLDOMNode *pIDOMNode = NULL;
	BSTR bstrItemText, bstrItemNode,bstrNodeType;	

	NodeListPtr = m_domDocument->getElementsByTagName(strFindText);

	for(int i = 0; i < (NodeListPtr->length); i++)
	{
		if (pIDOMNode)
			pIDOMNode->Release();

		NodeListPtr->get_item(i, &pIDOMNode);

		pIDOMNode->get_nodeName(&bstrItemNode);									 
		pIDOMNode->get_text(&bstrItemText);
		pIDOMNode->get_nodeTypeString(&bstrNodeType);		

		if (lstrcmp((LPCTSTR)bstrNodeType, (LPCTSTR)L"element") != 0)
			continue;

		callback(wstring (bstrItemNode), wstring(bstrItemText));
	}

	if (pIDOMNode)
			pIDOMNode->Release();

	_uninitialize();
	return true;
}

bool XmlParser::_load(wstring file)
{	  
	_variant_t varXml(file.c_str());
	_variant_t varResult((bool)TRUE);
      
	varResult = m_domDocument->load(varXml);

	return (bool)varResult;
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