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

@implementation AppDelegate

vector <Action *> actions;
SystemLanguageAction systemLanguageAction;
FirefoxAction firefoxAction;
SpellCheckerAction spellCheckerAction;
ChromeAction chromeAction;


-(NSInteger) numberOfRowsInTableView: (NSTableView *) tableView
{
    return actions.size();
}


- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    NSButtonCell *buttonCell;
    buttonCell = [aTableColumn dataCell];
    Action* action = actions.at(rowIndex);
    
    if (action->GetSelected())
    {
        action->SetSelected(false);
    }
    else
        action->SetSelected(true);

}


-(id) tableView:(NSTableView *) tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex
{
    NSButtonCell *buttonCell;
    NSString* text;
    buttonCell = [tableColumn dataCell];
    Action* action = actions.at(rowIndex);
    
    text = [NSString stringWithUTF8String: action->GetName()];
    
    if (action->IsNeed())
    {
         [buttonCell setEnabled:YES];
    }
    else
    {
         [buttonCell setEnabled:NO];
    }
    
    [buttonCell setTitle:text];
 
    if (action->GetSelected())
    {
        return [NSNumber numberWithBool:YES];
    }
    else
        return [NSNumber numberWithBool:NO];
}


- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)rowIndex
{
    NSString* text;
    Action* action = actions.at(rowIndex);
    text = [NSString stringWithUTF8String: action->GetDescription()];
    [_ActionDescription setTitle:text];
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
    NSString* text;
    NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:row];
    Action* action = actions.at(row);
    
    [_ApplicationsList selectRowIndexes:indexSet byExtendingSelection:NO];;
    text = [NSString stringWithUTF8String: action->GetDescription()];
    [_ActionDescription setTitle:text];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    bool anyAction = false;
    
    redirectNSLogToDocumentFolder();
    
    actions.push_back(&systemLanguageAction);
    actions.push_back(&firefoxAction);
    actions.push_back(&spellCheckerAction);
    actions.push_back(&chromeAction);
    
    NSLog(@"Catalanitzador per al Mac built on: %s, %s", __DATE__, __TIME__);
    
    [NSApp activateIgnoringOtherApps:YES];
    [_DoChanges setKeyEquivalent:@"\r"];
    
    for (int i = 0; i < actions.size(); i++)
    {
        if (actions.at(i)->IsNeed() == true)
        {
            anyAction = true;
            break;
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
- (IBAction)SaveChanges:(id)sender {
    
    bool correct = true;
    
    if (systemLanguageAction.IsNeed() && systemLanguageAction.GetSelected())
    {
        systemLanguageAction.Execute();
        if (systemLanguageAction.GetStatus() != Successful)
            correct = false;
    }
    
    if (firefoxAction.IsNeed() && firefoxAction.GetSelected())
    {
        if (firefoxAction.IsApplicationRunning())
        {
            _showDialogBox(@"Catalanitzador per al Mac",
                           @"Us suggerim tancar l'aplicació Firefox abans de continuar. Si no ho feu, els canvis no tindran efecte fins que torneu a obrir el Firefox.");
        }
        
        
        firefoxAction.Execute();
        if (firefoxAction.GetStatus() != Successful)
            correct = false;
    }
    
    if (chromeAction.IsNeed() && chromeAction.GetSelected())
    {
        if (chromeAction.IsApplicationRunning())
        {
            _showDialogBox(@"Catalanitzador per al Mac",
                           @"Us suggerim tancar l'aplicació Chrome abans de continuar. Si no ho feu, els canvis no tindran efecte fins que torneu a obrir el Chrome.");
        }
        
        
        chromeAction.Execute();
        if (chromeAction.GetStatus() != Successful)
            correct = false;
    }
    
    if (spellCheckerAction.IsNeed() && spellCheckerAction.GetSelected())
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
}
- (IBAction)ApplicationListSelector:(id)sender {
}
@end
