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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */
 
#pragma once

#include "Action.h"

class ActionMock : public Action
{
public:

		MOCK_METHOD1(SetStatus, void(ActionStatus));
		MOCK_METHOD0(GetStatus, ActionStatus());
		MOCK_METHOD0(GetName, wchar_t*());
		MOCK_METHOD0(GetDescription, wchar_t*());
		MOCK_CONST_METHOD0(GetID, ActionID());
		MOCK_METHOD0(IsNeed, bool());
		MOCK_METHOD0(Execute, void());
		
};
