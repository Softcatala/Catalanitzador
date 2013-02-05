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

#ifndef __CatalanitzadorPerAlMac__SystemLanguageAction__
#define __CatalanitzadorPerAlMac__SystemLanguageAction__

#include <iostream>
#import <Cocoa/Cocoa.h>
#include "Action.h"

class SystemLanguageAction: Action
{
public:
        SystemLanguageAction() : Action() {};
    
        virtual bool IsNeed();
        virtual void Execute();

private:
        NSArray* _getCurrentLanguages();
        void _setLocale();
        bool _isCurrentLocaleOk();
};

#endif /* defined(a__CatalanitzadorPerAlMac__SystemLanguageAction__) */
