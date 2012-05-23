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

#include <windows.h>
#include "Serializable.h"
#include "InspectorID.h"
#include "StringConversion.h"

#include <vector>

using namespace std;

class _APICALL InspectorKeyValue
{

public:

	InspectorKeyValue() {}

	InspectorKeyValue(wstring key, wstring value)
	{
		m_key = key;
		m_value = value;
	}

	wstring GetKey() {return m_key;}
	wstring GetValue() {return m_value;}

private:

	wstring m_key;
	wstring m_value;
};

class _APICALL Inspector : public Serializable
{
public:
		Inspector() {};
		virtual ~Inspector() {};
		virtual InspectorID GetID() const = 0;
		virtual vector <InspectorKeyValue> GetKeyValues() {return m_KeyValues;}
		virtual void Execute() = 0;
		virtual void Serialize(ostream* stream);
		
protected:

		 vector <InspectorKeyValue> m_KeyValues;
};

