/* 
 * Copyright (C) 2012-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "TempFile.h"
#include <vector>
using namespace std;


class x64BitsProcess
{
public:
		x64BitsProcess();
		vector <wstring> GetRunningProcessesNames();
		
private:

		void _extractExecutable();
		bool _execute(wchar_t* program, wchar_t* params);
	
		TempFile m_executable;
		bool m_extracted;
};