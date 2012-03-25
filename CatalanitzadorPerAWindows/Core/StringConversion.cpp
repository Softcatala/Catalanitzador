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
#include "StringConversion.h"

void StringConversion::ToMultiByte(wstring src, string& target)
{
	char* multiByte;
	int size;
	
	size = 4 * (src.size() + 1);
	multiByte = new char[size];
	WideCharToMultiByte(CP_ACP, 0, src.c_str(), src.size() + 1, multiByte, size, NULL, NULL);
	target = multiByte;
	delete multiByte;
}

void StringConversion::ToWriteChar(string src, string& target)
{
	
}			