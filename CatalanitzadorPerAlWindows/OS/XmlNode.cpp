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
#include "XmlNode.h"

void XmlNode::CreateElement()
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
		CreateElement();
	}

	child.CreateElement();
	m_itemPtr->appendChild(child.GetIXMLDOMElementPtr());
	m_children.push_back(child);
}

void XmlNode::AddAttribute(XmlAttribute attribute)
{
	assert(m_itemPtr == NULL);
	m_attributes.push_back(attribute);
}

