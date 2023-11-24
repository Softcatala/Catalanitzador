/*
 * Copyright (C) 2013 Jordi Mas i Hern‡ndez <jmas@softcatala.org>
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

#include "FirefoxAction.h"
#include <fstream>

FirefoxAction::FirefoxAction() : Action()
{
	m_acceptLanguages = NULL;
}

FirefoxAction::~FirefoxAction()
{
	if (m_acceptLanguages)
		delete m_acceptLanguages;
}

bool FirefoxAction::IsApplicationRunning()
{
	NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
	NSArray *runningApps = [workspace runningApplications];
	NSString *bundleIdentifierToFind = @"org.mozilla.firefox";
	bool foundApp = false;

	for (NSRunningApplication *app in runningApps)
	{
		NSString * bundleIdentifier = app.bundleIdentifier;
		if ([bundleIdentifierToFind isEqualToString:bundleIdentifier])
		{
		    foundApp = true;
		    break;
		}
	}

	NSLog(@"FirefoxAction::IsApplicationRunning. Result %u", foundApp);
	return foundApp;
}

bool FirefoxAction::_isInstalled()
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSString *APP_FILE = @"/Applications/Firefox.app/Contents/MacOS/firefox";

	bool installed = [fileManager fileExistsAtPath:APP_FILE];
	
	NSLog(@"FirefoxAction::_isInstalled. Installed %u", installed);
	return installed;
}

string FirefoxAction::_getInstalledLang()
{
	string language;
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSString *CATALAN_RESOURCE_FILE = @"/Applications/Firefox.app/Contents/Resources/ca.lproj/InfoPlist.strings";

	bool installed = [fileManager fileExistsAtPath:CATALAN_RESOURCE_FILE];
	
	if (installed)
	{
		language = "ca";
	}
	else
		language = "xx";
	
	NSLog(@"FirefoxAction::_getInstalledLang. Language %s", language.c_str());
	return language;
}

FirefoxAcceptLanguages * FirefoxAction::_getAcceptLanguages()
{
	if (m_acceptLanguages == NULL)
	{
		string fullPath, installLang;
		NSString* homeDir = NSHomeDirectory();
		
		fullPath = [homeDir UTF8String];
		fullPath += "/Library/Application Support/Firefox/";
		installLang = _getInstalledLang();
		
		m_acceptLanguages = new FirefoxAcceptLanguages(fullPath, installLang);
	}
	return m_acceptLanguages;
}


const char* FirefoxAction::GetVersion()
{
	if (m_version.empty())
	{
		NSBundle* bundle;
		
		bundle = [[NSBundle alloc] initWithPath: @"/Applications/Firefox.app/"];
		NSDictionary* infoDict = [bundle infoDictionary];
		NSString* version = [infoDict objectForKey:@"CFBundleShortVersionString"];
		
		if (version != NULL)
		{
			m_version = [version UTF8String];
		}
	}
	
	return m_version.c_str();
}


bool FirefoxAction::IsNeed()
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
	
	NSLog(@"FirefoxAction::IsNeed. %u", bNeed);
	return bNeed;
}

void FirefoxAction::Execute()
{
	bool isOk;
	
	_getAcceptLanguages()->Execute();
	isOk = _getAcceptLanguages()->IsNeed() == false;
	SetStatus(isOk ? Successful : FinishedWithError);
	
	NSLog(@"FirefoxAction::Execute. Result %u", isOk);
}

void FirefoxAction::CheckPrerequirements(Action * action)
{
	if (_isInstalled())
	{
		if (_getAcceptLanguages()->IsNeed() == false)
		{
			SetStatus(AlreadyApplied);
		}
	}
	else
	{
		_setStatusNotInstalled();
	}
}

