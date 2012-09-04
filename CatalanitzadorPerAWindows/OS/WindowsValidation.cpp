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
#include "WindowsValidation.h"

WindowsValidation::WindowsValidation(IOSVersion* OSVersion)
{
	m_OSVersion = OSVersion;	
}

#define FUNCTION_ID 0x1

// Strategy: only says that is validated if we can really confirm it
bool WindowsValidation::_isWindowsXPValidated()
{
	CLSID lcsid;
	IDispatch* disp;
	VARIANT dispRes;
	EXCEPINFO *pExcepInfo = NULL;
	unsigned int *puArgErr = NULL;
	bool bRslt;	

	CoInitialize(NULL);
	
	if (!SUCCEEDED(CLSIDFromString(L"{17492023-C23A-453E-A040-C7C580BBF700}", &lcsid)))
	{
		g_log.Log(L"WindowsValidation::IsWindowsValidated. CLSIDFromString failed, passed: 0");
		return false;
	}
	 
    if (!SUCCEEDED(CoCreateInstance(lcsid, NULL, CLSCTX_INPROC_SERVER, __uuidof(IDispatch), (void**)&disp)))
	{
		g_log.Log(L"WindowsValidation::IsWindowsValidated. CreateInstance failed, passed: 0");
		return false;
	}

	DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};

	disp->Invoke(FUNCTION_ID, IID_NULL,
		LOCALE_SYSTEM_DEFAULT,
		DISPATCH_METHOD,
		&dispparamsNoArgs, &dispRes,
		pExcepInfo, puArgErr);

	disp->Release();

	bRslt = wcscmp(dispRes.bstrVal, L"0") == 0;
	g_log.Log(L"WindowsValidation::IsWindowsValidated. Result: '%s', passed %u", dispRes.bstrVal,  (wchar_t *)bRslt);
	return bRslt;
}

bool WindowsValidation::IsWindowsValidated() 
{		
	if (m_OSVersion->GetVersion() == WindowsXP)
	{
		DWORD dwservicePack;

		dwservicePack = m_OSVersion->GetServicePackVersion();
	
		if (HIWORD(dwservicePack) < 2)
		{
			g_log.Log(L"WindowsValidation::IsWindowsValidated. Old XP");
			return true;
		}
		return _isWindowsXPValidated();
	}
	else
	{
		g_log.Log(L"WindowsValidation::IsWindowsValidated. No XP");
		return true;
	}
}

