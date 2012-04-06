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
#include "Inspector.h"
#include "StringConversion.h"

#include <fstream>
#include <iostream>

void Inspector::Serialize(ostream* stream)
{
	wchar_t szText[1024];

	for (unsigned i = 0; i < m_KeyValues.size(); i++)
	{
		InspectorKeyValue keyValue;

		keyValue = m_KeyValues[i];

		swprintf_s(szText, L"\t\t<inspector id='%u' key='%s' value='%s'/>\n", 
			GetID(), keyValue.GetKey().c_str(), keyValue.GetValue().c_str());

		string text;
		StringConversion::ToMultiByte(wstring(szText), text);
		*stream << text;
	}	
}
