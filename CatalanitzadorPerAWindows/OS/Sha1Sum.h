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

using namespace std;

#define BUFSIZE 4096
#define SHA1LEN 40

class Sha1Sum
{
	public:	
		    Sha1Sum(){}
			Sha1Sum(wstring file);
			void SetFile(wstring file) { m_file = file;}
			wstring GetSum() const { return m_sum;}
			wstring ComputeforFile();
			wstring ReadFromFile();
			
			bool operator== (Sha1Sum& other);
			bool operator!= (Sha1Sum& other) {return !(*this == other);};
						
	private:
			void _compute();
			wstring m_sum;
			wstring m_file;
};
