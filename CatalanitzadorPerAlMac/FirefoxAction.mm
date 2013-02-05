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

string FirefoxAction::_getInstalledLang()
{
    string CATALAN_RESOURCE_FILE = "/Applications/Firefox.app/Contents/Resources/ca.lproj/InfoPlist.strings";
    fstream reader;
    
    reader.open(CATALAN_RESOURCE_FILE);
    
    if (reader.is_open())
    {
        reader.close();
        return "ca";
    }
    else
        return "xx";
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
    return _getAcceptLanguages()->IsNeed();
}

void FirefoxAction::Execute()
{
    _getAcceptLanguages()->Execute();
}

