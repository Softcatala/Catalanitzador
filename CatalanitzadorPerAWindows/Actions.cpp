/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "stdafx.h"
#include "Actions.h"
#include "WindowsLPIAction.h"
#include "IEAcceptLanguagesAction.h"
#include "ConfigureLocaleAction.h"

Actions::Actions ()
{
	BuildListOfActions ();
}

Actions::~Actions ()
{
	for (unsigned int i = 0; i < m_actions.size (); i++)
	{		
		Action* action = m_actions.at(i);
		delete action;
	}
	m_actions.clear();
}

void Actions::BuildListOfActions ()
{
	m_actions.push_back (new WindowsLPIAction ());
	m_actions.push_back (new IEAcceptedLanguagesAction ());
	m_actions.push_back (new ConfigureLocaleAction ());
}

		