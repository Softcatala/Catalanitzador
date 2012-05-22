﻿/* 
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

class _APICALL TriBool
{

private:
	enum _triBool
	{
		True,
		False,
		Undefined
	};

	_triBool value;

public:

	TriBool() {value = Undefined;}
	
	bool IsUndefined() const { return value == Undefined;}
	void SetUndefined() { value = Undefined;}
	bool operator == (bool other);
	void operator = (bool other);
	bool operator != (bool other);
	wchar_t* ToString();
};

