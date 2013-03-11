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

@implementation AppDelegate

vector <Action *> actions;
SystemLanguageAction systemLanguageAction;
FirefoxAction firefoxAction;
SpellCheckerAction spellCheckerAction;
ChromeAction chromeAction;
NSString *statsFilename = nil;


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
    [alert setAlertStyle:0];
    [alert runModal];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

void redirectNSLogToDocumentFolder()
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *fileName = @"CatalanitzadorPerAlMac.log";
	NSString *logFilePath = [documentsDirectory stringByAppendingPathComponent:fileName];
    
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
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *fileName = @"statistics.xml";
	statsFilename = [documentsDirectory stringByAppendingPathComponent:fileName];
    statsFilename = [statsFilename copy];
    serializer.SaveToFile([statsFilename UTF8String]);
}

//#define DEVELOPMENT_VERSION 1

#if DEVELOPMENT_VERSION
    #define UPLOAD_URL "http://dev-catalanitzador.softcatala.org/parser.php"
#else
    #define UPLOAD_URL "http://catalanitzador.softcatala.org/parser.php"
#endif

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

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    bool anyAction = false;
    
    redirectNSLogToDocumentFolder();
    _initLog();
    
    actions.push_back(&systemLanguageAction);
    actions.push_back(&firefoxAction);
    actions.push_back(&spellCheckerAction);
    actions.push_back(&chromeAction);
    
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
        [self sendStatistics];
    }
    
    [_ApplicationsList setDataSource:self];
    [_ApplicationsList setDelegate:self];
    [self setDefaultRow:0];
}

- (IBAction)Cancel:(id)sender {
    
    [NSApp terminate:self];
}
- (IBAction)SaveChanges:(id)sender {

    bool correct = true;
    bool chromeSelected, firefoxSelected;
    
    chromeSelected = chromeAction.IsNeed() && chromeAction.GetStatus() == Selected;
    firefoxSelected = firefoxAction.IsNeed() && firefoxAction.GetStatus() == Selected;
    
    if (chromeSelected)
    {
        if (chromeAction.IsApplicationRunning())
        {
            _showDialogBox(@"Catalanitzador per al Mac",
                           @"Cal que tanqueu el navegador Chrome abans de continuar. No es pot canviar la llengua de navegació si està obert.");
            return;
        }
    }
    
    if (firefoxSelected)
    {
        if (firefoxAction.IsApplicationRunning())
        {
            _showDialogBox(@"Catalanitzador per al Mac",
                           @"Cal que tanqueu el navegador Firefox abans de continuar. No es pot canviar la llengua de navegació si està obert.");
            return;
        }
    }
    
    if (chromeSelected)
    {
        chromeAction.Execute();
        if (chromeAction.GetStatus() != Successful)
            correct = false;
    }
    
    if (firefoxSelected)
    {
        firefoxAction.Execute();
        if (firefoxAction.GetStatus() != Successful)
            correct = false;
    }
    
    if (systemLanguageAction.IsNeed() && systemLanguageAction.GetStatus() == Selected)
    {
        systemLanguageAction.Execute();
        if (systemLanguageAction.GetStatus() != Successful)
            correct = false;
    }
        
    if (spellCheckerAction.IsNeed() && spellCheckerAction.GetStatus() == Selected)
    {
        spellCheckerAction.Execute();
        if (spellCheckerAction.GetStatus() != Successful)
            correct = false;
    }
    
    [_DoChanges setEnabled:NO];
    
    if (correct)
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
@end
