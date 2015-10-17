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


#pragma once

#include <iostream>
#import <Cocoa/Cocoa.h>
#include "Action.h"

class SpellCheckerAction: public Action
{
public:
	SpellCheckerAction();
	~SpellCheckerAction();
	
	virtual const char* GetName() {return "Instal·la el corrector ortogràfic català al sistema";}
	
	virtual const char* GetDescription()  {return "Afegeix el corretor ortogràfic de Softcatalà al sistema en aplicacions com ara Mail i Pages.";}
	
	virtual ActionID GetID() const {return MacSpellCheckerActionID;}
	
	virtual bool IsNeed();
	virtual void Execute();
	virtual const char* GetVersion();
	virtual void CheckPrerequirements(Action * action);
	virtual bool IsRebootNeed() const;
	
private:
	
	bool _requestPermissions();
	NSString* _getBundlePath(CFStringRef file, CFStringRef extension);
	bool _copyfile(NSString* src, NSString* trg);
	bool _isDictionaryInstalled();
	void _createDirectoryIfDoesNotExists(NSString* directory);
	
	string m_version;
	AuthorizationRef m_authorizationRef;
};

