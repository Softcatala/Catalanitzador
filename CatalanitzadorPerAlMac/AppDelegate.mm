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


@implementation AppDelegate

SystemLanguageAction systemLanguageAction;
FirefoxAction firefoxAction;

void _showDialogBox(NSString* title, NSString* text)
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:title];
    [alert setInformativeText:text];
    [alert setAlertStyle:0];
    [alert runModal];
}


void redirectNSLogToDocumentFolder()
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
    
	NSString *documentsDirectory = [paths objectAtIndex:0];
    
	NSString *fileName = @"CatalanitzadorPerAlMac.log";
    
	NSString *logFilePath = [documentsDirectory stringByAppendingPathComponent:fileName];
    
	freopen([logFilePath cStringUsingEncoding:NSASCIIStringEncoding],"a+",stderr);
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    bool anyAction = false;
    
    redirectNSLogToDocumentFolder();
    
    NSLog(@"Built on: %s, %s", __DATE__, __TIME__);
    
    if (systemLanguageAction.IsNeed() == false)
        [_ConfigureLocale setEnabled:NO];
    else
        anyAction = true;
    
    if (firefoxAction.IsNeed() == false)
        [_Firefox setEnabled:NO];
    else
        anyAction = true;
    
    [NSApp activateIgnoringOtherApps:YES];
    
    if (anyAction == false)
    {
        [_DoChanges setEnabled:NO];
        
        _showDialogBox(@"Catalanitzador per al Mac",
                       @"Aquest ordinador ja es troba catalanitzat!");
        
    }
}

- (IBAction)Cancel:(id)sender {
    
    [NSApp terminate:self];
}
- (IBAction)SaveChanges:(id)sender {
    
    bool correct = true;
    
    if ([_ConfigureLocale state] != NSOffState)
    {
        systemLanguageAction.Execute();
        if (systemLanguageAction.GetStatus() != Successful)
            correct = false;
    }
    
    if ([_Firefox state] != NSOffState)
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
    
    [_DoChanges setEnabled:NO];
    
    if (correct)
    {
        _showDialogBox(@"Catalanitzador per al Mac",
                   @"Els canvis s'han aplicat correctament");
    }
    else
    {
        _showDialogBox(@"Catalanitzador per al Mac",
                       @"No tots els canvis s'han aplicat correctament");
    }
}
@end
