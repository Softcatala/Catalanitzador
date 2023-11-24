/*
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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


#include "SpellCheckerAction.h"
#include "OSVersion.h"

using namespace std;

SpellCheckerAction::SpellCheckerAction() : Action()
{

}

SpellCheckerAction::~SpellCheckerAction()
{
	
}


NSString* SpellCheckerAction::_getBundlePath(CFStringRef file, CFStringRef extension)
{
	CFURLRef url;
	CFBundleRef mainBundle;
	
	mainBundle = CFBundleGetMainBundle();
	url = CFBundleCopyResourceURL(mainBundle, file, extension, NULL);
	CFStringRef path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	return (NSString *)path;
}

bool SpellCheckerAction::_copyfile(NSString* src, NSString* trg)
{
	NSString * cpUtilityPath = @"/bin/cp";
	
	char * args[3] = {
		[0] = (char *)[[src stringByStandardizingPath] UTF8String],
		[1] = (char *)[[trg stringByStandardizingPath] UTF8String],
		[2] = NULL
	};
	
	return m_authorizationExecute.RunAsRoot([[cpUtilityPath stringByStandardizingPath] UTF8String], args);
}


const char* SpellCheckerAction::GetVersion()
{
	if (m_version.empty())
	{
		char szVersion[1024];
		OSVersion version;
		
		snprintf(szVersion, sizeof(szVersion), "%u.%u.%u", version.GetMajorVersion(), version.GetMinorVersion(), version.GetBugFix());
		m_version = szVersion;
	}
	
	return m_version.c_str();
}

void SpellCheckerAction::_createDirectoryIfDoesNotExists(NSString* directory)
{
	if([[NSFileManager defaultManager] fileExistsAtPath:directory])
		return;
	
	NSString * mkdirUtilityPath = @"/bin/mkdir";
	
	char * args[2] = {
		[0] = (char *)[[directory stringByStandardizingPath] UTF8String],
		[1] = NULL
	};
	
	// Using mkdir instead of createDirectoryAtPath:directory to change how we request permissions
	m_authorizationExecute.RunAsRoot([[mkdirUtilityPath stringByStandardizingPath] UTF8String], args);
}

void SpellCheckerAction::Execute()
{
	NSString* srcFile;
	bool isOk;
	
	if (m_authorizationExecute.RequestPermissions())
	{
		// Starting in OS X Capitan the directory is not created
		_createDirectoryIfDoesNotExists(@"/Library/Spelling");
	
		// NOTE: do not use special chars (like accents) in the target file name
		// We were not able to reproduce it but users reported problems with the dictionary
		srcFile = _getBundlePath(CFSTR("Català"), CFSTR("aff"));
		_copyfile(srcFile, @"/Library/Spelling/ca.aff");
	
		srcFile = _getBundlePath(CFSTR("Català"), CFSTR("dic"));
		_copyfile(srcFile, @"/Library/Spelling/ca.dic");
	
		isOk = _isDictionaryInstalled();
		SetStatus(isOk ? Successful : FinishedWithError);
	}
	else
	{
		// If the user has not authorized the action we assume that reported as not selected since he does not want to do it
		SetStatus(NotSelected);
		isOk = false;
	}
		
	NSLog(@"SpellCheckerAction::Execute. Result %u", isOk);
}

bool SpellCheckerAction::_isDictionaryInstalled()
{
	bool installed;
	NSString* DICTIONARY_FILE_MANUAL(@"/Library/Spelling/catala.aff");
	NSString* DICTIONARY_FILE_OLD(@"/Library/Spelling/Català.aff");
	NSString* DICTIONARY_FILE(@"/Library/Spelling/ca.aff");
	
	installed = [[NSFileManager defaultManager] fileExistsAtPath:DICTIONARY_FILE];

	if (installed == false)
	{
		installed = [[NSFileManager defaultManager] fileExistsAtPath:DICTIONARY_FILE_OLD];
	}

	if (installed == false)
	{
		installed = [[NSFileManager defaultManager] fileExistsAtPath:DICTIONARY_FILE_MANUAL];
	}
	
	NSLog(@"SpellCheckerAction::_isDictionaryInstalled %u", installed);
	return installed;
}

bool SpellCheckerAction::IsNeed()
{
	bool bNeed;
	
	switch(GetStatus())
	{
		case NotInstalled:
		case AlreadyApplied:
		case CannotBeApplied:
			bNeed = false;
			break;
		default:
			bNeed = true;
			break;
	}
	
	NSLog(@"SpellCheckerAction::IsNeed. %u", bNeed);
	return bNeed;
}

bool SpellCheckerAction::IsRebootNeed() const
{
	return status == Successful;
}

void SpellCheckerAction::CheckPrerequirements(Action * action)
{
	OSVersion version;
	
	if (version.GetMajorVersion() > 10 ||
		(version.GetMajorVersion() == 10 && version.GetMinorVersion() >= 7))
	{
		if (_isDictionaryInstalled())
		{
			SetStatus(AlreadyApplied);
		}
	}
	else
	{
		m_cannotBeApplied = "No es pot realitzar aquesta acció en aquesta versió del sistema.";
		SetStatus(CannotBeApplied);
	}
}
