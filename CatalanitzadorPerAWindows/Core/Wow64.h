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

class Wow64
{
	public:
			Wow64();
			BOOL DisableFsRedirection(PVOID *OldValue);
			BOOL RevertRedirection(PVOID OldValue);
	private:
			typedef BOOL (WINAPI *FN_Wow64DisableWow64FsRedirection)( __out PVOID *OldValue);
			typedef BOOL (WINAPI *FN_Wow64RevertWow64FsRedirection)( __in  PVOID OldValue);

			FN_Wow64DisableWow64FsRedirection m_fnDisable;
			FN_Wow64RevertWow64FsRedirection m_fnRevert;

		
};