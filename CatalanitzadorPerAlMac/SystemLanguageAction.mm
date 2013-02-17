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
#include "OSVersion.h"

NSArray* SystemLanguageAction::_getCurrentLanguages()
{
    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSDictionary* globalDomain = [defs persistentDomainForName:@"NSGlobalDomain"];
    NSArray* languages = [globalDomain objectForKey:@"AppleLanguages"];

    return languages;
}

void SystemLanguageAction::_setLocale()
{
    NSTask* task = [[NSTask alloc] init];
    NSArray* prevLanguages = _getCurrentLanguages();
    NSArray* arguments = [NSArray arrayWithObjects: @"write", @"NSGlobalDomain",
                          @"AppleLanguages", @"-array", @"ca" , nil];
    
    unsigned long cnt = [prevLanguages count] + [arguments count];
    NSMutableArray *myArray = [NSMutableArray arrayWithCapacity:cnt];
    
    [myArray addObjectsFromArray:arguments];
    
    // Copy all previous languages except Catalan that is already first one
    for (NSString* language in prevLanguages)
    {
        if ([language isEqualToString:@"ca"] ==false)
            [myArray addObject:language];
    }
        
    [task setLaunchPath:@"/usr/bin/defaults"];
    [task setArguments: myArray];
    [task launch];
    [task waitUntilExit];
}

bool SystemLanguageAction::_isCurrentLocaleOk()
{
    NSArray* languages = _getCurrentLanguages();
    NSString* language = [languages objectAtIndex:0];
    
    if ([language isEqualToString:@"ca"])
        return true;
    else
        return false;
}

const char* SystemLanguageAction::GetVersion()
{
    if (m_version.empty())
    {
        char szVersion[1024];
        OSVersion version;
        
        sprintf(szVersion, "%u.%u.%u", version.GetMajorVersion(), version.GetMinorVersion(), version.GetBugFix());
        m_version = szVersion;
    }
    
    return m_version.c_str();
}

void SystemLanguageAction::Execute()
{
    bool isOk;
    
    _setLocale();
    isOk = _isCurrentLocaleOk();
    SetStatus(isOk ? Successful : FinishedWithError);
    
    NSLog(@"SystemLanguageAction::Execute. Result %u", isOk);
}

bool SystemLanguageAction::IsNeed()
{
    bool isNeed = false;
    OSVersion version;
    
    if (version.GetMajorVersion() > 10 ||
        (version.GetMajorVersion() == 10 && version.GetMinorVersion() >= 7))
    {
        isNeed = _isCurrentLocaleOk() == false;
    }
    NSLog(@"SystemLanguageAction::IsNeed: %d", isNeed);
    return isNeed;
}
