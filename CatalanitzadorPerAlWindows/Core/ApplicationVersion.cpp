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

vector <int> ApplicationVersion::GetComponents(wstring version)
{
	vector <int> components;
	int start = 0, end = 0, pos, num;
	wstring number;

	// Do not use it with strings
	for (unsigned int i = 0; i < version.size(); i++)
		assert(iswdigit(version[i]) || version[i] == L'.');

	do
	{
		pos = version.find_first_of(SEPARATOR, end);
		start = end;
		end = pos;
		number = version.substr(start, end - start);
		num = _wtoi(number.c_str());
		components.push_back(num);
		end = end + SEPARATOR_LEN;
		
	} while (pos != string::npos);

	return components;
}

vector <int> ApplicationVersion::GetComponents()
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

// If version A is 10.0.2 and B 10.2 compare starting by the left only the MIN of both
// If both the MIN number of the digits are equal, return as major the one with more digits
bool ApplicationVersion::_majorDiferentSizes (ApplicationVersion other)
{
	unsigned int size;
	
	size = GetComponents().size() < other.GetComponents().size() ? GetComponents().size() : other.GetComponents().size();

	for (unsigned int i = 0; i < size; i++)
	{
		if (GetComponents().at(i) == other.GetComponents().at(i))
			continue;

		return GetComponents().at(i) > other.GetComponents().at(i);
	}

	return GetComponents().size() > other.GetComponents().size();
}

bool ApplicationVersion::operator > (ApplicationVersion other)
{
	if (GetComponents().size() != other.GetComponents().size())
	{
		return _majorDiferentSizes(other);
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