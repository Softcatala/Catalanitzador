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

#include "Action.h"

Action::Action()
{
	status = Selected;
}

void Action::Serialize(ostream* stream)
{
	char szText[1024];
	string version;

	sprintf(szText, "\t\t<action id='%u' version='%s' result='%u'/>\n",
		GetID(), GetVersion(), status);

	*stream << szText;
}

void Action::_setStatusNotInstalled()
{
    m_cannotBeApplied = "Aquest programa no està instal·lat o la versió que teniu és desconeguda.";
	SetStatus(NotInstalled);
}

const char*  Action::GetCannotNotBeApplied()
{
    if (GetStatus() == AlreadyApplied)
    {
        return "Aquesta acció ja està aplicada.";
    }
    return m_cannotBeApplied.c_str();
}




