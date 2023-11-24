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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#import "AppDelegate.h"
#import "SystemLanguageAction.h"
#import "FirefoxAction.h"
#import "SpellCheckerAction.h"
#import "ChromeAction.h"
#import "OSVersion.h"
#import "Serializer.h"
#import "HttpFormInet.h"
#include "Version.h"
#include "RemoteURLs.h"
#include "Reboot.h"
#include "DictationAction.h"
#include "DictSynonymsAction.h"
#include "LibreOfficeAction.h"

// Dictation action is still in development. Enable this define to enable it
//#define DICTATION_ACTION 1


@implementation AppDelegate

vector <Action *> actions;
SystemLanguageAction systemLanguageAction;
FirefoxAction firefoxAction;
SpellCheckerAction spellCheckerAction;
ChromeAction chromeAction;
DictSynonymsAction dictSynonymsAction;
LibreOfficeAction libreOfficeAction;

#if DICTATION_ACTION
DictationAction dictationAction;
#endif


NSString *statsFilename = nil;
NSString *alertTitle = @"Catalanitzador per al Mac";

-(NSInteger) numberOfRowsInTableView: (NSTableView *) tableView
{
	return actions.size();
}


- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	NSButtonCell *buttonCell;
	buttonCell = [aTableColumn dataCell];
	Action* action = actions.at(rowIndex);
	
	if (action->GetStatus() == Selected)
	{
		action->SetStatus(NotSelected);
	}
	else if (action->GetStatus() == NotSelected)
	{
		action->SetStatus(Selected);
	}
}


-(id) tableView:(NSTableView *) tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex
{
	NSButtonCell *buttonCell;
	NSString* text;
	buttonCell = [tableColumn dataCell];
	Action* action = actions.at(rowIndex);
	
	text = [NSString stringWithUTF8String: action->GetName()];
	
	// Action already executed show it disabled
	if (action->GetStatus() == Successful || action->GetStatus() == FinishedWithError)
	{
		[buttonCell setEnabled:NO];
	}
	else
	{
		if (action->IsNeed())
		{
			[buttonCell setEnabled:YES];
		}
		else
		{
			[buttonCell setEnabled:NO];
		}
	}
	
	[buttonCell setTitle:text];
	
	ActionStatus status = action->GetStatus();
 
	if (status == Selected || status == AlreadyApplied  ||
		status == FinishedWithError || status == Successful)
	{
		return [NSNumber numberWithBool:YES];
	}
	else
		return [NSNumber numberWithBool:NO];
}

- (void)updateDescriptionForRow:(NSInteger)rowIndex
{
	NSString* text;
	Action* action = actions.at(rowIndex);
	text = [NSString stringWithUTF8String: action->GetDescription()];
	[_ActionDescription setTitle:text];
	
	const char* cannotBeAppliedText = action->GetCannotNotBeApplied();
	text = [NSString stringWithUTF8String: cannotBeAppliedText];
	[_CannotBeAppliedReason setTitle:text];
	
	if (strlen(cannotBeAppliedText) > 0)
	{
		[_CannotBeApplied setHidden:NO];
	}
	else
	{
		[_CannotBeApplied setHidden:YES];
	}
}

- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)rowIndex
{
	[self updateDescriptionForRow:rowIndex];
	return TRUE;
}

void _showDialogBox(NSString* title, NSString* text)
{
	NSAlert *alert = [[NSAlert alloc] init];
	[alert setMessageText:title];
	[alert setInformativeText:text];
	[alert setAlertStyle:NSAlertStyleWarning];
	[alert runModal];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

NSString * _getApplicationSupportFolderForFile(NSString *fileName)
{
	NSError *error;
	NSFileManager *manager = [NSFileManager defaultManager];
	NSURL *applicationSupport = [manager URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:false error:&error];
	NSString *identifier = [[NSBundle mainBundle] bundleIdentifier];
	NSURL *folder = [applicationSupport URLByAppendingPathComponent:identifier];
	[manager createDirectoryAtURL:folder withIntermediateDirectories:true attributes:nil error:&error];

	NSURL *url = [folder URLByAppendingPathComponent:fileName];
	NSString *logFilePath = [url path];
	return logFilePath;
}

void redirectNSLogToApplicationSupportFolder()
{
	NSString * logFilePath = _getApplicationSupportFolderForFile(@"CatalanitzadorPerAlMac.log");
	freopen([logFilePath cStringUsingEncoding:NSASCIIStringEncoding],"a+",stderr);
}

- (void)setDefaultRow:(int)row
{
	NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:row];
	[_ApplicationsList selectRowIndexes:indexSet byExtendingSelection:NO];
	[self updateDescriptionForRow:row];
}

void _initLog()
{
	OSVersion version;
	
	NSLog(@"Catalanitzador per al Mac version %s built on: %s, %s", STRING_VERSION_RESOURCES,  __DATE__, __TIME__);
	NSLog(@"Mac OS version: %u.%u.%u", version.GetMajorVersion(),
		  version.GetMinorVersion(), version.GetBugFix());
}

void _serialize(Serializer& serializer)
{
	serializer.OpenHeader();
	
	serializer.StartAction();
	for (int i = 0; i < actions.size(); i++)
	{
		actions.at(i)->Serialize(serializer.GetStream());
	}
	serializer.EndAction();
	serializer.CloseHeader();
	
	NSString * statsFilename = _getApplicationSupportFolderForFile(@"statistics.xml");
	serializer.SaveToFile([statsFilename UTF8String]);
}


void _upload(Serializer& serializer)
{
	string serialize;
	vector <string> variables;
	vector <string> values;
	
	serializer.SaveToString(serialize);
	
	variables.push_back("xml");
	values.push_back(serialize);
	
	HttpFormInet access;
	access.PostForm(UPLOAD_URL, variables, values);
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	if (statsFilename != nil)
	{
		[[NSFileManager defaultManager] removeItemAtPath:statsFilename error:nil];
	}
}

- (void)sendStatistics
{
	if ([_SendStats state] != NSOffState)
	{
		Serializer serializer;
		_serialize(serializer);
		_upload(serializer);
	}
}

- (bool)IsRebootNeeded
{
	bool rebootNeed = false;
	
	for (int i = 0; i < actions.size(); i++)
	{
		if (actions.at(i)->IsRebootNeed())
		{
			rebootNeed = true;
			break;
		}
	}
	
	return rebootNeed;
}

- (void)AskIfUserWantsToReboot
{
	NSAlert *alert = [[NSAlert alloc] init];
	[alert setMessageText:alertTitle];
	[alert setInformativeText:@"Els canvis fets a la configuració requereixen reiniciar l'ordinador. Podeu fer-ho ara o més endavant. Voleu reiniciar-lo ara mateix?"];
	[alert setAlertStyle:NSAlertStyleWarning];
	[alert addButtonWithTitle:@"Sí"];
	[alert addButtonWithTitle:@"Cancel·lar"];
	
	if ([alert runModal] == NSAlertFirstButtonReturn)
	{
		Reboot reboot;
		reboot.RestartNow();
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	bool anyAction = false;
	
	redirectNSLogToApplicationSupportFolder();
	_initLog();
	
	actions.push_back(&systemLanguageAction);
	actions.push_back(&spellCheckerAction);
	actions.push_back(&dictSynonymsAction);
	actions.push_back(&chromeAction);
	actions.push_back(&firefoxAction);
	actions.push_back(&libreOfficeAction);
	
#if DICTATION_ACTION
	actions.push_back(&dictationAction);
#endif
	
	[NSApp activateIgnoringOtherApps:YES];
	[_DoChanges setKeyEquivalent:@"\r"];
	
	for (int i = 0; i < actions.size(); i++)
	{
		actions.at(i)->CheckPrerequirements(NULL);
		if (actions.at(i)->IsNeed() == true)
		{
			anyAction = true;
		}
	}
	
	if (anyAction == false)
	{
		[_DoChanges setEnabled:NO];
		[_Results setStringValue:@"Aquest ordinador ja es troba catalanitzat!"];
	}
	
	[_ApplicationsList setDataSource:self];
	[_ApplicationsList setDelegate:self];
	[self setDefaultRow:0];
}

- (IBAction)Cancel:(id)sender {
	
	[NSApp terminate:self];
}

- (bool)AskUserToCloseOpenApps
{
	
	if (chromeAction.IsNeed() && chromeAction.GetStatus() == Selected)
	{
		if (chromeAction.IsApplicationRunning())
		{
			_showDialogBox(alertTitle,
						   @"Cal que tanqueu el navegador Chrome abans de continuar. No es pot canviar la llengua de navegació si està obert.");
			return true;
		}
	}
	
	if (firefoxAction.IsNeed() && firefoxAction.GetStatus() == Selected)
	{
		if (firefoxAction.IsApplicationRunning())
		{
			_showDialogBox(alertTitle,
						   @"Cal que tanqueu el navegador Firefox abans de continuar. No es pot canviar la llengua de navegació si està obert.");
			return true;
		}
	}
	
	if (libreOfficeAction.IsNeed() && libreOfficeAction.GetStatus() == Selected)
	{
		if (libreOfficeAction.IsApplicationRunning())
		{
			_showDialogBox(alertTitle,
						   @"Cal que tanqueu el LibreOffice abans de continuar. No es pot instal·lar el paquet d'idioma si està obert.");
			return true;
		}
	}
	return false;
}


- (bool)ExecuteActions
{
	bool correct = true;
	
	for (int i = 0; i < actions.size(); i++)
	{
		Action* action = actions.at(i);
		
		if (action->IsNeed() && action->GetStatus() == Selected)
		{
			action->Execute();
			if (action->GetStatus() == FinishedWithError)
				correct = false;
		}
	}
	return correct;
}

- (IBAction)SaveChanges:(id)sender
{
	if ([self AskUserToCloseOpenApps] == true)
		return;
	
	[_DoChanges setEnabled:NO];
	
	if ([self ExecuteActions])
	{
		[_Results setStringValue:@"Els canvis s'han aplicat correctament"];
	}
	else
	{
		[_Results setStringValue:@"No tots els canvis s'han aplicat correctament"];
	}
	
	[_ApplicationsList reloadData];
	[self sendStatistics];
	
}
- (IBAction)ApplicationListSelector:(id)sender {
}
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
	
	if ([self IsRebootNeeded])
		[self AskIfUserWantsToReboot];
	
	return NSTerminateNow;
}

@end
