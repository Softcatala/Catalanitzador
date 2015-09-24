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

#include <stdafx.h>
 
bool TriBool::operator== (bool other)
{
	if (other == true && m_value == True)
		return true;

	if (other == false && m_value == False)
		return true;

	return false;		
}

void TriBool::operator= (bool other)
{
	if (other == true)
		m_value = True;
	else
		if (other == false)
			m_value = False;
		else
			m_value = Undefined;
}

bool TriBool::operator != (bool other)
{
	if (other == true && m_value != True)
		return true;

	if (other == false && m_value != False)
		return true;

	return false;		
}

wchar_t* TriBool::ToString()
{
	switch(m_value)
	{
		case True:
			return L"true";
		case False:
			return L"false";
		case Undefined:
			return L"undefined";
		default:				
			return L"";
	}
}


