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


#include "DictSynonymsAction.h"
#include "OSVersion.h"

#define SYSTEM_DICTPATH @"/Library/Dictionaries"
#define DICTIONARY_NAME @"Diccionari de sinònims - Albert Jané.dictionary"
#define RESOURCE_NAME "Diccionari de sinònims - Albert Jané"
#define RESOURCE_EXTENSION "dictionary"


DictSynonymsAction::DictSynonymsAction() : Action()
{
	
}

DictSynonymsAction::~DictSynonymsAction()
{
	
}

NSString* DictSynonymsAction::_getBundlePath(CFStringRef file, CFStringRef extension)
{
	CFURLRef url;
	CFBundleRef mainBundle;
	
	mainBundle = CFBundleGetMainBundle();
	url = CFBundleCopyResourceURL(mainBundle, file, extension, NULL);
	CFStringRef path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	return (NSString *)path;
}

bool DictSynonymsAction::_copyfile(NSString* src, NSString* trg)
{
	NSString * cpUtilityPath = @"/bin/cp";
	
	char * args[4] = {
		[0] = (char *)"-r",
		[1] = (char *)[[src stringByStandardizingPath] UTF8String],
		[2] = (char *)[[trg stringByStandardizingPath] UTF8String],
		[3] = NULL
	};
	
	return m_authorizationExecute.RunAsRoot([[cpUtilityPath stringByStandardizingPath] UTF8String], args);
}


const char* DictSynonymsAction::GetVersion()
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

void DictSynonymsAction::_createDirectoryIfDoesNotExists(NSString* directory)
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

void DictSynonymsAction::_setDictionary()
{
	NSTask* task = [[NSTask alloc] init];
	NSArray* arguments = [NSArray arrayWithObjects: @"write", @"com.apple.DictionaryServices",
						  @"DCSActiveDictionaries", @"-array-add", _getDictionaryPath() , nil];
	
	[task setLaunchPath:@"/usr/bin/defaults"];
	[task setArguments: arguments];
	[task launch];
	[task waitUntilExit];
}

void DictSynonymsAction::Execute()
{
	NSString* srcDir;
	bool isOk;
	
	if (m_authorizationExecute.RequestPermissions())
	{
		_createDirectoryIfDoesNotExists(SYSTEM_DICTPATH);
		
		srcDir = _getBundlePath(CFSTR(RESOURCE_NAME), CFSTR(RESOURCE_EXTENSION));
		
		_copyfile(srcDir, _getDictionaryPath());
		_setDictionary();
		
		isOk = _isDictionaryInstalled();
		SetStatus(isOk ? Successful : FinishedWithError);
	}
	else
	{
		// If the user has not authorized the action we assume that reported as not selected since he does not want to do it
		SetStatus(NotSelected);
		isOk = false;
	}
	
	NSLog(@"DictSynonymsAction::Execute. Result %u", isOk);
}

NSString* DictSynonymsAction::_getDictionaryPath()
{
	NSArray *components = [NSArray arrayWithObjects:SYSTEM_DICTPATH, DICTIONARY_NAME, nil];
	return [NSString pathWithComponents:components];
}

bool DictSynonymsAction::_isDictionaryInstalled()
{
	
	bool installed;
	installed = [[NSFileManager defaultManager] fileExistsAtPath:_getDictionaryPath()];
	NSLog(@"DictSynonymsAction::_isDictionaryInstalled %u", installed);
	return installed;
}

bool DictSynonymsAction::IsNeed()
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
	
	NSLog(@"DictSynonymsAction::IsNeed. %u", bNeed);
	return bNeed;
}

void DictSynonymsAction::CheckPrerequirements(Action * action)
{
	if (_isDictionaryInstalled())
	{
		SetStatus(AlreadyApplied);
	}
}
