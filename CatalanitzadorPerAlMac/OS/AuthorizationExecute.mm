/*
 * Copyright (C) 2017 Jordi Mas i Hernàndez <jmas@softcatala.org>
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


#include "AuthorizationExecute.h"

AuthorizationRef AuthorizationExecute::m_authorizationRef = nil;

AuthorizationExecute::AuthorizationExecute()
{
	
}

AuthorizationExecute::~AuthorizationExecute()
{
	// We are leaking m_authorizationRef once per execution since it is static
}

bool AuthorizationExecute::RunAsRoot(const char *pathToTool, char * __nonnull const * __nonnull arguments)
{
	OSStatus status;
	
	status = AuthorizationExecuteWithPrivileges(m_authorizationRef,
												[[[NSString stringWithUTF8String:pathToTool] stringByStandardizingPath] UTF8String],
												0, arguments, NULL);
	return status == errAuthorizationSuccess;
}

bool AuthorizationExecute::RequestPermissions()
{
	if (m_authorizationRef)
		return true;
	
	OSStatus status;
 
	status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &m_authorizationRef);
	
	if (status != errAuthorizationSuccess)
	{
		NSLog(@"AuthorizationExecute::_requestPermissions. AuthorizationCreate failed: %d", status);
		return false;
	}
	
	AuthorizationItem right = {kAuthorizationRightExecute, 0, NULL, 0};
	AuthorizationRights rights = {1, &right};
	AuthorizationFlags flags = kAuthorizationFlagDefaults | kAuthorizationFlagInteractionAllowed |
	kAuthorizationFlagPreAuthorize | kAuthorizationFlagExtendRights;
	
	status = AuthorizationCopyRights(m_authorizationRef, &rights, NULL, flags, NULL);
	if (status != errAuthorizationSuccess)
	{
		NSLog(@"AuthorizationExecute::_requestPermissions. AuthorizationCopyRights failed: %d", status);
		return false;
	}
	
	return true;
}
