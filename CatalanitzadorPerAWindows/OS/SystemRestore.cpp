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
#include "SystemRestore.h"

bool SystemRestore::Init()
{
	return false;
	m_hLibrary = LoadLibraryW(L"srclient.dll");

	if (m_hLibrary == NULL)
	{
		g_log.Log(L"SystemRestore::Init. Cannot find srclient.dll");
		return false;
	}

	// If the library is loaded, find the entry point
	m_SetRestorePointW = (PFN_SETRESTOREPTW) GetProcAddress(m_hLibrary, "SRSetRestorePointW");

	if (m_SetRestorePointW == NULL)
	{
		g_log.Log(L"SystemRestore::Init. Cannot find SRSetRestorePointW entry point");
		return false;
	}
	return true;
}

bool SystemRestore::Start(wstring name)
{
	BOOL bRslt;
	RESTOREPOINTINFO restorePoint;		
	STATEMGRSTATUS status;
	
	restorePoint.dwEventType = BEGIN_NESTED_SYSTEM_CHANGE;
	restorePoint.dwRestorePtType = APPLICATION_INSTALL;
	restorePoint.llSequenceNumber = 0;
	wcscpy_s(restorePoint.szDescription, name.c_str());

	bRslt = m_SetRestorePointW(&restorePoint, &status);
	m_llSequenceNumber = status.llSequenceNumber;
	g_log.Log(L"SystemRestore::Start. Created point '%s' with result %u", (wchar_t*) name.c_str(), (wchar_t*)bRslt);
	return bRslt == TRUE;
}

bool SystemRestore::End()
{
	BOOL bRslt;
	RESTOREPOINTINFO restorePoint;		
	STATEMGRSTATUS status;
	
	restorePoint.dwEventType = END_NESTED_SYSTEM_CHANGE;
	restorePoint.dwRestorePtType = APPLICATION_INSTALL;
	restorePoint.llSequenceNumber = m_llSequenceNumber;
	restorePoint.szDescription[0] = NULL;

	bRslt = m_SetRestorePointW(&restorePoint, &status);	
	g_log.Log(L"SystemRestore::End. Called with result %u", (wchar_t*)bRslt);
	return bRslt == TRUE;	
}
