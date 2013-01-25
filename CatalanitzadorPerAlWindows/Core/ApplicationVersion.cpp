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
#include "ApplicationVersion.h"

#define SEPARATOR L"."
#define SEPARATOR_LEN 1

vector <wstring> ApplicationVersion::GetComponents(wstring version)
{
	vector <wstring> components;
	int start = 0, end = 0, pos;	

	do
	{
		pos = version.find_first_of(SEPARATOR, end);
		start = end;
		end = pos;		
		components.push_back(version.substr(start, end - start));
		end = end + SEPARATOR_LEN;
		
	} while (pos != string::npos);

	return components;
}

vector <wstring> ApplicationVersion::GetComponents()
{
	if (m_components.size() == 0)
		m_components = GetComponents(m_version);

	return m_components;
}

bool ApplicationVersion::operator== (ApplicationVersion other)
{
	if (GetComponents().size() != other.GetComponents().size())
		return false;

	for (unsigned int i = 0; i < GetComponents().size(); i++)
	{
		if (GetComponents().at(i) != other.GetComponents().at(i))
			return false;
	}

	return true;
}

bool ApplicationVersion::operator != (ApplicationVersion other)
{
	return !(*this == other);
}

bool ApplicationVersion::operator > (ApplicationVersion other)
{
	if (GetComponents().size() != other.GetComponents().size())
	{
		return false;
	}

	for (unsigned int i = 0; i < GetComponents().size(); i++)
	{
		if (GetComponents().at(i) == other.GetComponents().at(i))
			continue;
		
		return GetComponents().at(i) > other.GetComponents().at(i);
	}
	return false;
}

bool ApplicationVersion::operator < (ApplicationVersion other)
{
	return !(*this >= other);
}

bool ApplicationVersion::operator >= (ApplicationVersion other)
{
	if (*this > other)
		return true;

	return *this == other;
}

bool ApplicationVersion::operator <= (ApplicationVersion other)
{
	if (*this < other)
		return true;

	return *this == other;
}