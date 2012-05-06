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

#include <SRRestorePtAPI.h>
#include <accctrl.h>
#include <aclapi.h>

typedef BOOL (WINAPI *PFN_SETRESTOREPTW) (PRESTOREPOINTINFOW, PSTATEMGRSTATUS);

class SystemRestore
{
public:

	bool Init();		
	bool Start();
	bool End();
		
private:

	bool _initializeCOMSecurity();
	void _fillExplicitAccessStructure(EXPLICIT_ACCESS& ea, LPTSTR name);

	HMODULE m_hLibrary;
	PFN_SETRESTOREPTW m_SetRestorePointW;
	INT64 m_llSequenceNumber;
};