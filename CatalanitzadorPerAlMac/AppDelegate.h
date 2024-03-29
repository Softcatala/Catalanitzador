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
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSTableViewDataSource, NSTableViewDelegate>

- (NSInteger) numberOfRowsInTableView: (NSTableView*) tableView;
- (id) tableView:(NSTableView *) tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex;
- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)rowIndex;

@property (assign) IBOutlet NSButton *SendStats;
@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSButton *DoChanges;
@property (assign) IBOutlet NSButton *ConfigureLocale;
@property (assign) IBOutlet NSTextField *Results;

@property (assign) IBOutlet NSButton *Firefox;
@property (assign) IBOutlet NSTextField *CannotBeApplied;
@property (assign) IBOutlet NSButton *SpellChecker;
@property (assign) IBOutlet NSTextFieldCell *CannotBeAppliedReason;
@property (assign) IBOutlet NSButton *Chrome;
@property (assign) IBOutlet NSTableView *ApplicationsList;
@property (assign) IBOutlet NSTextFieldCell *ActionDescription;
@end

