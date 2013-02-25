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
    NSDictionary *info;
    bool foundApp = false;
    OSErr err;
    ProcessSerialNumber psn = {0, kNoProcess};

    while (!foundApp)
    {
        err = GetNextProcess(&psn);
    
        if (!err)
        {
            info = (NSDictionary *)ProcessInformationCopyDictionary(&psn,   kProcessDictionaryIncludeAllInformationMask);
            foundApp = [@"org.mozilla.firefox" isEqual:[info objectForKey:(NSString *)kCFBundleIdentifierKey]];
        }
        else
        {
            break;
        }
    }
   
    NSLog(@"FirefoxAction::IsApplicationRunning. Result %u", foundApp);
    return foundApp;
}

bool FirefoxAction::_isInstalled()
{
    string APP_FILE = "/Applications/Firefox.app/Contents/MacOS/firefox";
    bool installed = false;
    fstream reader;
    
    reader.open(APP_FILE.c_str());
    
    if (reader.is_open())
    {
        reader.close();
        installed = true;
    }
    
    NSLog(@"FirefoxAction::_isInstalled. Installed %u", installed);
    return installed;
}

string FirefoxAction::_getInstalledLang()
{
    string CATALAN_RESOURCE_FILE = "/Applications/Firefox.app/Contents/Resources/ca.lproj/InfoPlist.strings";
    string language;
    fstream reader;
    
    reader.open(CATALAN_RESOURCE_FILE.c_str());
    
    if (reader.is_open())
    {
        reader.close();
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

bool FirefoxAction::IsNeed()
{
    bool isNeed;
    
    isNeed = _isInstalled() && _getAcceptLanguages()->IsNeed();
    NSLog(@"FirefoxAction::IsNeed. Result %u", isNeed);
    return isNeed;
}

void FirefoxAction::Execute()
{
    bool isOk;
    
    _getAcceptLanguages()->Execute();
    isOk = _getAcceptLanguages()->IsNeed() == false;
    SetStatus(isOk ? Successful : FinishedWithError);
    
    NSLog(@"FirefoxAction::Execute. Result %u", isOk);
}

