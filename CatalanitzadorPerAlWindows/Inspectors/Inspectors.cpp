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
#include "Inspectors.h"
#include "IRegistry.h"
#include "Registry.h"
#include "LibreOfficeInspector.h"
#include "PDFCreatorInspector.h"
#include "WinRARInspector.h"


Inspectors::Inspectors()
{
	_buildListOfInspectors();
}

Inspectors::~Inspectors()
{
	for (unsigned int i = 0; i < m_Inspectors.size(); i++)
	{		
		Inspector* inspector = m_Inspectors.at(i);
		delete inspector;
	}

	for (unsigned int i = 0; i < m_objectsToDelete.size(); i++)
	{
		void* object = m_objectsToDelete.at(i);
		delete object;
	}
}

IRegistry* Inspectors::_getNewRegistry()
{
	IRegistry* registry;
	registry = (IRegistry *)new Registry();
	m_objectsToDelete.push_back(registry);
	return registry;
}

void Inspectors::_buildListOfInspectors()
{
	m_Inspectors.push_back(new LibreOfficeInspector(_getNewRegistry()));
	m_Inspectors.push_back(new PDFCreatorInspector(_getNewRegistry()));
	m_Inspectors.push_back(new WinRARInspector(_getNewRegistry()));	
}

void Inspectors::Execute()
{
	for (unsigned int i = 0; i < m_Inspectors.size(); i++)
	{
		Inspector* inspector = m_Inspectors.at(i);
		inspector->Execute();
	}
}

void Inspectors::Serialize(ostream* stream)
{
	*stream << "\t<inspectors>\n";

	for (unsigned int i = 0; i < m_Inspectors.size(); i++)
	{
		Inspector* inspector = m_Inspectors.at(i);
		inspector->Serialize(stream);
	}

	*stream << "\t</inspectors>\n";
}