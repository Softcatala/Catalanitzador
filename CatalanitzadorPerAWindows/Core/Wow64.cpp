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
#include "Wow64.h"

Wow64::Wow64()
{
	m_fnDisable = NULL;
	m_fnRevert = NULL;
}

BOOL Wow64::DisableFsRedirection(PVOID *OldValue)
{
	BOOL bRslt;

	if (m_fnDisable == NULL)
	{
		m_fnDisable = (FN_Wow64DisableWow64FsRedirection) GetProcAddress(
			GetModuleHandle (L"kernel32"), "Wow64DisableWow64FsRedirection");
	}

	if (m_fnDisable)
	{
		bRslt = m_fnDisable(OldValue);

		if (bRslt == FALSE)
			g_log.Log(L"Wow64::DisableFsRedirection. Error calling Wow64DisableWow64FsRedirection.");

		return bRslt;
	}
	else
	{
		g_log.Log(L"Wow64::DisableFsRedirection. Unable to find Wow64DisableWow64FsRedirection API entry point");	
	}
	return FALSE;
}

BOOL Wow64::RevertRedirection(PVOID OldValue)
{
	if (m_fnRevert == NULL)
	{
		m_fnRevert = (FN_Wow64RevertWow64FsRedirection) GetProcAddress(
			GetModuleHandle (L"kernel32"), "Wow64RevertWow64FsRedirection");
	}

	if (m_fnRevert)
	{
		BOOL bRslt;

		bRslt = m_fnRevert(OldValue);

		if (bRslt == FALSE)
			g_log.Log(L"Wow64::RevertRedirection. Error calling Wow64RevertWow64FsRedirection.");

		return bRslt;
	}
	else
	{
		g_log.Log(L"Wow64::RevertRedirection. Unable to find Wow64RevertWow64FsRedirection API entry point");
	}
	return FALSE;
}
