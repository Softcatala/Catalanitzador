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
	
	// TODO: Is COM security needed?
	//return _initializeCOMSecurity();
	return true;
}

bool SystemRestore::Start()
{
	BOOL bRslt;
	RESTOREPOINTINFO restorePoint;		
	STATEMGRSTATUS status;
	
	restorePoint.dwEventType = BEGIN_SYSTEM_CHANGE;
	restorePoint.dwRestorePtType = APPLICATION_INSTALL;
	restorePoint.llSequenceNumber = 0;
	wcscpy_s(restorePoint.szDescription, L"Catalanitzador per a Windows");

	bRslt = m_SetRestorePointW(&restorePoint, &status);
	m_llSequenceNumber = status.llSequenceNumber;
	return bRslt == TRUE;
}

bool SystemRestore::End()
{
	BOOL bRslt;
	RESTOREPOINTINFO restorePoint;		
	STATEMGRSTATUS status;
	
	restorePoint.dwEventType = END_SYSTEM_CHANGE;
	restorePoint.dwRestorePtType = APPLICATION_INSTALL;
	restorePoint.llSequenceNumber = m_llSequenceNumber;
	restorePoint.szDescription[0] = NULL;

	bRslt = m_SetRestorePointW(&restorePoint, &status);	
	return bRslt == TRUE;	
}

void SystemRestore::_fillExplicitAccessStructure(EXPLICIT_ACCESS& ea, LPTSTR name)
{
	ea.grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
    ea.grfAccessMode = SET_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.pMultipleTrustee = NULL;
    ea.Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea.Trustee.ptstrName = name;
}

bool SystemRestore::_initializeCOMSecurity()
{   
	SECURITY_DESCRIPTOR securityDesc = {0};
	EXPLICIT_ACCESS   ea[5] = {0};
	ACL        *pAcl = NULL;
	ULONGLONG  rgSidBA[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={0};
	ULONGLONG  rgSidLS[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={0};
	ULONGLONG  rgSidNS[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={0};
	ULONGLONG  rgSidPS[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={0};
	ULONGLONG  rgSidSY[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={0};
	DWORD      cbSid = 0;
	BOOL       fRet = FALSE;
	DWORD      dwRet = ERROR_SUCCESS;
	HRESULT    hrRet = S_OK;

    //
    // This creates a security descriptor that is equivalent to the following 
    // security descriptor definition language (SDDL) string:
    //
    //   O:BAG:BAD:(A;;0x1;;;LS)(A;;0x1;;;NS)(A;;0x1;;;PS)(A;;0x1;;;SY)(A;;0x1;;;BA)
    //
 
    // Initialize the security descriptor.
    fRet = ::InitializeSecurityDescriptor( &securityDesc, SECURITY_DESCRIPTOR_REVISION );
    if( !fRet )
    {
        goto exit;
    }

    // Create an administrator group security identifier (SID).
    cbSid = sizeof( rgSidBA );
    fRet = ::CreateWellKnownSid( WinBuiltinAdministratorsSid, NULL, rgSidBA, &cbSid );
    if( !fRet )
    {
        goto exit;
    }

    // Create a local service security identifier (SID).
    cbSid = sizeof( rgSidLS );
    fRet = ::CreateWellKnownSid( WinLocalServiceSid, NULL, rgSidLS, &cbSid );
    if( !fRet )
    {
        goto exit;
    }

    // Create a network service security identifier (SID).
    cbSid = sizeof( rgSidNS );
    fRet = ::CreateWellKnownSid( WinNetworkServiceSid, NULL, rgSidNS, &cbSid );
    if( !fRet )
    {
        goto exit;
    }

    // Create a personal account security identifier (SID).
    cbSid = sizeof( rgSidPS );
    fRet = ::CreateWellKnownSid( WinSelfSid, NULL, rgSidPS, &cbSid );
    if( !fRet )
    {
        goto exit;
    }

    // Create a local service security identifier (SID).
    cbSid = sizeof( rgSidSY );
    fRet = ::CreateWellKnownSid( WinLocalSystemSid, NULL, rgSidSY, &cbSid );
    if( !fRet )
    {
        goto exit;
    }

	_fillExplicitAccessStructure(ea[0], (LPTSTR)rgSidBA);
	_fillExplicitAccessStructure(ea[1], (LPTSTR)rgSidLS);
	_fillExplicitAccessStructure(ea[2], (LPTSTR)rgSidNS);
	_fillExplicitAccessStructure(ea[3], (LPTSTR)rgSidPS);
	_fillExplicitAccessStructure(ea[4], (LPTSTR)rgSidSY);

    // Create an access control list (ACL) using this ACE list.
    dwRet = ::SetEntriesInAcl( ARRAYSIZE( ea ), ea, NULL, &pAcl );
    if( dwRet != ERROR_SUCCESS || pAcl == NULL )
    {
        fRet = FALSE;
        goto exit;
    }

    // Set the security descriptor owner to Administrators.
    fRet = ::SetSecurityDescriptorOwner( &securityDesc, rgSidBA, FALSE );
    if( !fRet )
    {
        goto exit;
    }

    // Set the security descriptor group to Administrators.
    fRet = ::SetSecurityDescriptorGroup( &securityDesc, rgSidBA, FALSE );
    if( !fRet )
    {
        goto exit;
    }

    // Set the discretionary access control list (DACL) to the ACL.
    fRet = ::SetSecurityDescriptorDacl( &securityDesc, TRUE, pAcl, FALSE );
    if( !fRet )
    {
        goto exit;
    }

    // Initialize COM. You may need to modify the parameters of
    // CoInitializeSecurity() for your application. Note that an
    // explicit security descriptor is being passed down.
 
    hrRet = ::CoInitializeSecurity( &securityDesc,
                                    -1,
                                    NULL,
                                    NULL,
                                    RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
                                    RPC_C_IMP_LEVEL_IDENTIFY,
                                    NULL,
                                    EOAC_DISABLE_AAA | EOAC_NO_CUSTOM_MARSHAL,
                                    NULL );
    if( FAILED( hrRet ) )
    {
        fRet = FALSE;
        goto exit;
    }

    fRet = TRUE;

 exit:

    ::LocalFree( pAcl );

    return fRet == TRUE;
}

