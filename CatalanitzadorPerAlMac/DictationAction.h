/*
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

class DictationAction: public Action
{
public:
	DictationAction();
	
	virtual const char* GetName() {return "Configura el català com a llengua de dictat";}
	
	virtual const char* GetDescription()  {return "Configura el català com a llengua de dictat.";}
	
	virtual ActionID GetID() const {return MacSpellCheckerActionID;} // TODO: to get new ID
	
	virtual bool IsNeed();
	virtual void Execute();
	virtual const char* GetVersion();
	virtual void CheckPrerequirements(Action * action);
	
private:
	
	void _setDefault(NSArray* arguments);
	
	bool _isDictationEnabled();
	bool _isUseOnlyOffilineEnabled();
	bool _isLocaleSet();
	
	bool m_isDictationEnabled;
	bool m_isUseOnlyOffilineEnabled;
	bool m_isLocaleSet;
	string m_version;
};

