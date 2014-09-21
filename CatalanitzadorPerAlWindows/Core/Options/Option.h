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

#include "OptionID.h"
#include "Serializable.h"
#include "StringConversion.h"

#include <string>
#include <sstream>

using namespace std;


class Option : public Serializable
{
	public:

		Option(OptionID optionID, wstring value)
		{
			m_optionID = optionID;
			m_value = value;
		}

		Option(OptionID optionID, bool value)
		{
			m_optionID = optionID;
			m_value = (value == true ? L"1" : L"0");
		}

		Option(OptionID optionID, int value)
		{
			wostringstream convert;
			
			m_optionID = optionID;

			convert << value;
			m_value = convert.str();
		}

		virtual void Serialize(ostream* stream)
		{
			wchar_t szText[1024];
			swprintf_s(szText, L"\t\t<option id='%u' value='%s'/>\n", m_optionID, m_value.c_str());

			string text;
			StringConversion::ToMultiByte(wstring(szText), text);
			*stream << text;			
		}

		OptionID GetOptionId() {return m_optionID;}
		wstring GetValue() {return m_value;}

	private:

		OptionID m_optionID;
		wstring m_value;		
};
