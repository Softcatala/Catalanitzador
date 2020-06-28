/*
 * Copyright (C) 2018 Jordi Mas i Hernàndez <jmas@softcatala.org>
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


#include "LibreOfficeAction.h"
#include "OSVersion.h"
#include "AuthorizationExecute.h"

NSString* LIBREOFFICE_APP = @"/Applications/LibreOffice.app";
NSString* LIBREOFFICE_VANILLA_APP = @"/Applications/LibreOffice Vanilla.app";
NSString* LANGPACK_VOLUME = @"/Volumes/LibreOffice Language Pack/";

// References
//	https://github.com/caskroom/homebrew-cask/blob/master/Casks/libreoffice-language-pack.rb
//	https://github.com/Softcatala/Catalanitzador/blob/navega/CatalanitzadorPerAlWindows/Actions/OpenOfficeAction.cpp

LibreOfficeAction::LibreOfficeAction() : Action()
{
	m_installDirectory = LibreOfficeTypeUnknown;
}

LibreOfficeAction::~LibreOfficeAction()
{
	
}

const char* LibreOfficeAction::GetVersion()
{
	if (m_version.empty())
	{
		NSBundle *bundle = [NSBundle bundleWithPath:LIBREOFFICE_APP];
		NSString *version = [bundle objectForInfoDictionaryKey:@"CFBundleGetInfoString"];
		m_version = version.UTF8String;
	}
	return m_version.c_str();
}

bool LibreOfficeAction::IsNeed()
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
	
	NSLog(@"LibreOfficeAction::IsNeed. %u", bNeed);
	return bNeed;
}

bool LibreOfficeAction::IsApplicationRunning()
{
	NSDictionary *info;
	bool foundApp = false;
	OSErr err;
	ProcessSerialNumber psn = {0, kNoProcess};
	
	while (!foundApp)
	{
		err = GetNextProcess(&psn);
		
		if (!err)
		{
			info = (NSDictionary *)ProcessInformationCopyDictionary(&psn, kProcessDictionaryIncludeAllInformationMask);
			foundApp = [@"org.libreoffice.script" isEqual:[info objectForKey:(NSString *)kCFBundleIdentifierKey]];
		}
		else
		{
			break;
		}
	}

	NSLog(@"LibreOfficeAction::IsApplicationRunning. Result %u", foundApp);
	return foundApp;
}

string LibreOfficeAction::_getVersionForDownload()
{
	string versionForDownload;
	NSString* version = [NSString stringWithUTF8String:GetVersion()];
	
	NSError *error = NULL;
	NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"(.*)(\\d+.\\d+.\\d+).(\\d+)(.*)"
		options:NSRegularExpressionCaseInsensitive error:&error];
	
	NSTextCheckingResult *match = [regex firstMatchInString:version options:0 range:NSMakeRange(0, [version length])];
	
	if (!match || match.numberOfRanges < 5) {
		NSLog(@"LibreOfficeAction. Version without enought elements: %u", (unsigned int) match.numberOfRanges);
		return versionForDownload;
	}
	
	NSString* smallVersion = [version substringWithRange:[match rangeAtIndex:2]];
	versionForDownload = smallVersion.UTF8String;
	NSLog(@"LibreOfficeAction. Version for download: %s", versionForDownload.c_str());
	return versionForDownload;
}

bool LibreOfficeAction::_download()
{
	string version = _getVersionForDownload();
	
	if (version.size() == 0)
		return false;
	
	// Downloading directly from an https mirror since https://download.documentfoundation.org/ does redirect to an HTTP
	NSString* url = [NSString stringWithFormat:@"https://mirror.uni-konstanz.de/tdf/libreoffice/stable/%s/mac/x86_64/LibreOffice_%s_MacOS_x86-64_langpack_ca.dmg", version.c_str(),version.c_str()];
	
	NSLog(@"LibreOfficeAction. URL: %@", url);

	NSURL *yourURL = [NSURL URLWithString:url];
	NSURLRequest *request = [NSURLRequest requestWithURL:yourURL];
	
	NSURLResponse *response = nil;
	NSError *error = nil;
	NSData *data = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
	if (data == nil) {
		NSInteger errorCode = [((NSHTTPURLResponse *)response) statusCode];
		NSLog(@"LibreOfficeAction. Download Status :%d", (int) errorCode);
		NSLog(@"LibreOfficeAction. Download Error :%@", error);
		return false;	
	}
	
	// find Documents directory and append your local filename
	NSURL *documentsURL = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
	documentsURL = [documentsURL URLByAppendingPathComponent:@"LibreOffice_MacOS_x86-64_langpack_ca.dmg"];
	m_filename = documentsURL.path;
	[data writeToURL:documentsURL atomically:YES];
	NSLog(@"LibreOfficeAction. Download completed");
	return true;
}

void LibreOfficeAction::_executeProgram(NSString* program, NSArray* arguments)
{
	NSTask* task = [[NSTask alloc] init];
	
	[task setLaunchPath:program];
	[task setArguments: arguments];
	[task launch];
	[task waitUntilExit];
}

void LibreOfficeAction::_attach()
{
	NSString* program = @"/usr/bin/hdiutil";
	NSArray* arguments = [NSArray arrayWithObjects: @"attach", m_filename, nil];
	
	_executeProgram(program, arguments);
	NSLog(@"LibreOfficeAction hdiutil attach executed");
}

void LibreOfficeAction::_tar()
{
	NSString* program = @"/usr/bin/tar";
	NSString* tarball = [LANGPACK_VOLUME stringByAppendingString:@"LibreOffice Language Pack.app/Contents/tarball.tar.bz2"];
	NSArray* arguments = [NSArray arrayWithObjects: @"-xjf", tarball, @"-C", @"/Applications/LibreOffice.app", nil];
	
	_executeProgram(program, arguments);
	NSLog(@"LibreOfficeAction tar executed");
}

void LibreOfficeAction::_deattach()
{
	NSString* program = @"/usr/bin/hdiutil";
	NSArray* arguments = [NSArray arrayWithObjects: @"detach", LANGPACK_VOLUME, nil];
	_executeProgram(program, arguments);
	NSLog(@"LibreOfficeAction hdiutil detach executed");
}

void LibreOfficeAction::Execute()
{
	if (_download())
	{
		_attach();
		_tar();
		_deattach();

		NSError* error = nil;
		[[NSFileManager defaultManager] removeItemAtPath:m_filename error:&error];
		NSLog(@"LibreOfficeAction. Deleting %@", m_filename);
		if (error) {
			NSLog(@"LibreOfficeAction. Error deleting file:%@", error);
		}
	}
	SetStatus(_isLanguagePackInstalled() ? Successful : FinishedWithError);
}

bool LibreOfficeAction::_isLanguagePackInstalled()
{
	NSString* CA_FILE = [NSString stringWithFormat:@"%@%@", _getInstallDirectory(), @"/Contents/Resources/resource/ca/LC_MESSAGES"];
	bool installed;
	installed = [[NSFileManager defaultManager] fileExistsAtPath:CA_FILE];
	NSLog(@"LibreOfficeAction::isLanguagePackInstalled %u", installed);
	return installed;
}

bool LibreOfficeAction::_isInstalled()
{
	bool installed = LibreOfficeAction::_getInstallDirectory() != NULL;
	NSLog(@"LibreOfficeAction::_isInstalled %u", installed);
	return installed;
}

LibreOfficeType LibreOfficeAction::_getLibreOfficeType()
{
	if ([[NSFileManager defaultManager] fileExistsAtPath:LIBREOFFICE_APP])
		return LibreOffice;
	
	if ([[NSFileManager defaultManager] fileExistsAtPath:LIBREOFFICE_VANILLA_APP])
		return LibreOfficeVanilla;
	
	return LibreOfficeNotInstalled;
}

NSString* LibreOfficeAction::_getInstallDirectory()
{
	if (m_installDirectory == LibreOfficeTypeUnknown)
		m_installDirectory = _getLibreOfficeType();
	
	switch (m_installDirectory) {
		case LibreOffice:
			return LIBREOFFICE_APP;
		case LibreOfficeVanilla:
			return LIBREOFFICE_VANILLA_APP;
		default:
			return NULL;
	}
}

void LibreOfficeAction::CheckPrerequirements(Action * action)
{
	if (!_isInstalled())
	{
		SetStatus(NotInstalled);
		return;
	}
	
	if (_isLanguagePackInstalled())
	{
		SetStatus(AlreadyApplied);
	}
}
