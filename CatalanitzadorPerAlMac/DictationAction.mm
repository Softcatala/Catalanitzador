/*
 * Copyright (C) 2014 Jordi Mas i Hern‡ndez <jmas@softcatala.org>
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


#include "DictationAction.h"
#include "OSVersion.h"


static NSString *const g_dictationIMUserDefaultPersistentDomain = @"com.apple.speech.recognition.AppleSpeechRecognition.prefs";
static NSString *const g_dictationIMMasterDictationEnabled = @"DictationIMMasterDictationEnabled";
static NSString *const g_dictationIMUseOnlyOfflineDictation = @"DictationIMUseOnlyOfflineDictation";
static NSString *const g_dictationIMLocaleIdentifier = @"DictationIMLocaleIdentifier";
static NSString *const g_localeIdentifierToSet = @"ca_ES";

DictationAction::DictationAction()
{
	m_isDictationEnabled = false;
	m_isUseOnlyOffilineEnabled = false;
	m_isLocaleSet = false;
}

const char* DictationAction::GetVersion()
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

bool DictationAction::IsNeed()
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
	
	NSLog(@"DictationAction::IsNeed. %u", bNeed);
	return bNeed;
}

void DictationAction::_setDefault(NSArray* arguments)
{
	NSTask* task = [[NSTask alloc] init];
	
	[task setLaunchPath:@"/usr/bin/defaults"];
	[task setArguments: arguments];
	[task launch];
	[task waitUntilExit];
}

void DictationAction::Execute()
{
	NSArray* arguments;
	
	if (m_isDictationEnabled == false)
	{
		arguments = [NSArray arrayWithObjects: @"write", g_dictationIMUserDefaultPersistentDomain,
					 g_dictationIMMasterDictationEnabled, @"-bool", @"true" , nil];
		
		_setDefault(arguments);
	}
	
	if (m_isUseOnlyOffilineEnabled == false)
	{
		arguments = [NSArray arrayWithObjects: @"write", g_dictationIMUserDefaultPersistentDomain,
					 g_dictationIMUseOnlyOfflineDictation, @"-bool", @"true" , nil];
		
		_setDefault(arguments);
	}
	
	if (m_isLocaleSet == false)
	{
		arguments = [NSArray arrayWithObjects: @"write", g_dictationIMUserDefaultPersistentDomain,
					 g_dictationIMLocaleIdentifier, @"-string", g_localeIdentifierToSet, nil];
		
		_setDefault(arguments);
	}
	
	bool isOk = _isDictationEnabled() && _isUseOnlyOffilineEnabled() && _isLocaleSet();
	SetStatus(isOk ? Successful : FinishedWithError);
	NSLog(@"DictationAction::Execute. Result %u", isOk);
}


bool DictationAction::_isDictationEnabled()
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	NSDictionary* globalDomain = [defs persistentDomainForName:g_dictationIMUserDefaultPersistentDomain];
	bool enabled = [[globalDomain objectForKey:g_dictationIMMasterDictationEnabled] boolValue];
	
	NSLog(@"DictationAction::_isDictationEnabled. Result %u", enabled);
	return enabled;
}

bool DictationAction::_isUseOnlyOffilineEnabled()
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	NSDictionary* globalDomain = [defs persistentDomainForName:g_dictationIMUserDefaultPersistentDomain];
	bool enabled = [[globalDomain objectForKey:g_dictationIMUseOnlyOfflineDictation] boolValue];
	NSLog(@"DictationAction::_isUseOnlyOffilineEnabled. Result %u", enabled);
	return enabled;
}

bool DictationAction::_isLocaleSet()
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	NSDictionary* globalDomain = [defs persistentDomainForName:g_dictationIMUserDefaultPersistentDomain];
	NSString* locale = [globalDomain objectForKey:g_dictationIMLocaleIdentifier];
	
	bool isSet = [locale isEqualToString:g_localeIdentifierToSet];
	NSLog(@"DictationAction::_isLocaleSet. Result %u", isSet);
	return isSet;
}

void DictationAction::CheckPrerequirements(Action * action)
{
	OSVersion version;
	
	if (version.GetMajorVersion() > 10 ||
		(version.GetMajorVersion() == 10 && version.GetMinorVersion() >= 10))
	{
		m_isDictationEnabled = _isDictationEnabled();
		m_isUseOnlyOffilineEnabled = _isUseOnlyOffilineEnabled();
		m_isLocaleSet = _isLocaleSet();
		
		if (m_isDictationEnabled && m_isUseOnlyOffilineEnabled && m_isLocaleSet)
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



