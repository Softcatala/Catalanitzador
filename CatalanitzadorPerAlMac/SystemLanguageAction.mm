//
//  SystemLanguageAction.cpp
//  CatalanitzadorPerAlMac
//
//  Created by Jordi Mas on 04/02/13.
//  Copyright (c) 2013 Softcatalà. All rights reserved.
//

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

bool SystemLanguageAction::IsNeed()
{
    return _isCurrentLocaleOk() == false;
}

