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

#include <string>
#include <vector>
using namespace std;


class IRunner
{
public:
		virtual bool Execute(wchar_t* program, wchar_t* params, bool b64bits = false) = 0;
		virtual bool IsRunning() const = 0;
		virtual void WaitUntilFinished() = 0;
		virtual vector <DWORD> GetProcessID(wstring name) const = 0;
		virtual bool RequestQuitToProcessID(DWORD processID) = 0;
};