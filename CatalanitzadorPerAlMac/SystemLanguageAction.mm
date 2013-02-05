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


#include "SystemLanguageAction.h"

void SystemLanguageAction::_setLocale()
{
    NSTask* task = [[NSTask alloc] init];
    
    NSString* arg1 = @"write";
    NSString* arg2 = @"NSGlobalDomain";
    NSString* arg3 = @"AppleLanguages";
    NSString* arg4 = @"-array";
    NSString* arg5 = @"\"ca\"";

    
    NSArray* arguments = [NSArray arrayWithObjects: arg1, arg2, arg3, arg4, arg5, nil];
    
    [task setLaunchPath:@"/usr/bin/defaults"];
    [task setArguments: arguments];
    [task launch];
}

bool SystemLanguageAction::_isCurrentLocaleOk()
{
    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSDictionary* globalDomain = [defs persistentDomainForName:@"NSGlobalDomain"];
    NSArray* languages = [globalDomain objectForKey:@"AppleLanguages"];
    NSString* language = [languages objectAtIndex:0];
    
    if ([language isEqualToString:@"ca"])
        return true;
    else
        return false;
}

void SystemLanguageAction::Execute()
{
    _setLocale();
}

// See: http://cocoadev.com/wiki/DeterminingOSVersion
void getSystemVersionMajor(SInt32& versionMajor, SInt32& versionMinor)
{
    SInt32 versionBugFix;
    versionMajor = versionMinor = versionBugFix = 0;
    
    Gestalt(gestaltSystemVersionMajor, &versionMajor);
    Gestalt(gestaltSystemVersionMinor, &versionMinor);
    Gestalt(gestaltSystemVersionBugFix, &versionBugFix);
    
    NSLog(@"Mac OS version: %u.%u.%u", versionMajor, versionMinor, versionBugFix);
}

bool SystemLanguageAction::IsNeed()
{
    bool isNeed = false;
    SInt32 versionMajor, versionMinor;
    
    getSystemVersionMajor(versionMajor, versionMinor);
    
    if (versionMajor > 10 || (versionMajor > 10 && versionMinor >= 7))
    {
        isNeed = _isCurrentLocaleOk() == false;
    }
    NSLog(@"SystemLanguageAction::IsNeed: %d", isNeed);
    return isNeed;
}
