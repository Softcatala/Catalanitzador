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

class ExecutionProcess
{
	public:

		ExecutionProcess() {}

		ExecutionProcess(wstring name, wstring printableName, bool canClose)
		{
			m_name = name;
			m_printableName = printableName;
			m_canClose = canClose;
			m_is64Bits = false;
		}

		wstring GetPrintableName() { return m_printableName;}
		wstring GetName() { return m_name;}
		bool CanClose() { return m_canClose;}
		bool Is64Bits() { return m_is64Bits;}
		void SetIs64Bits(bool is64Bits) { m_is64Bits = is64Bits;}

		bool IsEmpty()
		{
			return m_name.empty() && m_printableName.empty();
		}

	private:
		wstring m_name;
		wstring m_printableName;
		bool m_canClose;
		bool m_is64Bits;
};