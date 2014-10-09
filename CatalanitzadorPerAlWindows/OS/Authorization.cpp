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
 
#include "stdafx.h"
#include "Authorization.h"

bool Authorization::UserHasAdminPermissions()
{	
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;

	if (!AllocateAndInitializeSid(&NtAuthority, 2,
		SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0, &AdministratorsGroup))
	{
		return false;
	}
	
	BOOL IsInAdminGroup = FALSE;
	if(!CheckTokenMembership(NULL, AdministratorsGroup, &IsInAdminGroup))
	{	
		IsInAdminGroup = FALSE;
	}	
	FreeSid(AdministratorsGroup);
	return IsInAdminGroup == TRUE ? true : false;
}

bool Authorization::RequestShutdownPrivileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
		CloseHandle(hToken);
		return true;
	}

	return false;
}

#define MAX_NAME 256

// See:
//		http://msdn.microsoft.com/en-us/library/windows/desktop/aa446670%28v=vs.85%29.aspx
//		http://src.chromium.org/svn/branches/1312/src/rlz/win/lib/process_info.cc

PTOKEN_USER Authorization::_allocateToken(HANDLE token)
{
	DWORD token_size = 0;
	PTOKEN_USER token_user = NULL;
	if (!GetTokenInformation(token, TokenUser, NULL, 0, &token_size))
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			g_log.Log(L"Authorization::_allocateToken. GetTokenInformation error %x", (wchar_t*) GetLastError());
			return NULL;
		}
	}

	token_user = (PTOKEN_USER) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, token_size);
	
	if (token_user == NULL)
	{
		g_log.Log(L"Authorization::_allocateToken. HeapAlloc error %x", (wchar_t*) GetLastError());
		return NULL;
	}

  	DWORD token_size2;
	if (!GetTokenInformation(token, TokenUser, token_user, token_size, &token_size2))
	{
		g_log.Log(L"Authorization::_allocateToken. GetTokenInformation error %x", (wchar_t*) GetLastError());
		HeapFree(GetProcessHeap(), 0, (LPVOID)token_user);
		return NULL;
	}

	return token_user;
}

bool Authorization::IsRunningElevatedWithOtherUser(IOSVersion* osVersion)
{
	wstring logged, current;
	bool result = false;
	
	// Support only for operaring systems major version > 5
	if (osVersion->GetVersion() == WindowsXP ||
		osVersion->GetVersion() == WindowsXP64_2003 ||
		osVersion->GetVersion() == Windows2000)
		return false;

	current = GetCurrentUser();
	logged = GetLoggedUser();

	result = (logged != current);
	g_log.Log(L"Authorization::RunningElevatedWithOtherUser. Returns %u, current: %s, logged: %s", (wchar_t*) result,
		(wchar_t*)current.c_str(), (wchar_t*)logged.c_str());

	return result;
}

//
// Gets the current user. If the process was run by user A and then elevated using the credentials user B
// this will return user B
//
wstring Authorization::GetCurrentUser()
{
	return _getUserFromProcessHandle(GetCurrentProcess());
}

//
// Gets the current logged user. If the process was run by user A and then elevated using the credentials user B
// this will return user A. 
//
// It is just not possible to know what was the original user that lunch the process. We need to look at was the
// user that for example launched Progman
//
wstring Authorization::GetLoggedUser()
{
	wstring user;
	DWORD processId = 0;

	HWND shell = FindWindowEx(NULL, NULL, __T("Progman"), NULL);

	if (shell != NULL)
		GetWindowThreadProcessId(shell, &processId);
	else
		g_log.Log(L"Authorization::GetCurrentUser. GetWindowThreadProcessId NULL shell");

	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
	user = _getUserFromProcessHandle(processHandle);
	CloseHandle(processHandle);
	return user;
}


wstring Authorization::_getUserFromProcessHandle(HANDLE processHandle)
{
	wstring user;
	HANDLE token;
	PTOKEN_USER token_user;

	if (!OpenProcessToken(processHandle, TOKEN_QUERY, &token))
	{
		g_log.Log(L"Authorization::GetCurrentUser. OpenProcessToken error %x", (wchar_t*) GetLastError());
		return user;
	}

	token_user = _allocateToken(token);
	if (token_user == NULL)
		return user;
	
	WCHAR username[MAX_NAME + 1];
	DWORD username_size = MAX_NAME + 1;
	WCHAR domain[MAX_NAME + 1];
	DWORD domain_size = MAX_NAME + 1;
	SID_NAME_USE sid_type;
	PSID user_sid = token_user->User.Sid;

	if (LookupAccountSid(NULL, user_sid, username, &username_size, domain, &domain_size, &sid_type))
	{
		user = username;
	}
	else
	{
		g_log.Log(L"Authorization::GetCurrentUser. LookupAccountSid error %x", (wchar_t*) GetLastError());
	}

	HeapFree(GetProcessHeap(), 0, (LPVOID)token_user);
	return user;
}

//
// If the process was run by user A and then elevated using the credentials user B,
// this method allows to become again user A. 
//
// It is possible that RevertToSelf returns to User B again. If does not work,
// we can try using the same mecanism that to impersonate to user A.
//
// Not tested yet with production code
// 
void Authorization::Impersonate()
{
	wstring user;
	DWORD processId = 0;

	HWND shell = FindWindowEx(NULL, NULL, __T("Progman"), NULL);

	if (shell != NULL)
		GetWindowThreadProcessId(shell, &processId);
	else
	g_log.Log(L"Authorization::Impersonate. GetWindowThreadProcessId NULL shell");

	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
	HANDLE token;	

	if (OpenProcessToken(processHandle, TOKEN_QUERY | TOKEN_DUPLICATE, &token))
	{
		ImpersonateLoggedOnUser(token);
		CloseHandle(processHandle);
	}
}


const TOKEN_ELEVATION_TYPE TOKEN_ELEVATION_TYPE_NULL = (TOKEN_ELEVATION_TYPE) 0;


// TODO: Only Vista or better
// Seems to be always return 2 (TokenElevationTypeFull) for Catalanitzador
TOKEN_ELEVATION_TYPE Authorization::GetElevationType() 
{	
	HANDLE token;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
	{
		g_log.Log(L"Authorization::GetCurrentUser. OpenProcessToken error %x", (wchar_t*) GetLastError());
		return TOKEN_ELEVATION_TYPE_NULL;
	}

	DWORD size;
	TOKEN_ELEVATION_TYPE elevation_type;
	if (!GetTokenInformation(token, TokenElevationType, &elevation_type, sizeof(elevation_type), &size))
	{
	    return TOKEN_ELEVATION_TYPE_NULL;
	}

	return elevation_type;
}

