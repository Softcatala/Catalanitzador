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

#pragma once

enum _APICALL ActionStatus
{
	// Do not modify these since they are used in the server side to identify 
	// action status in a unique manner
	NotSelected = 0,		// The user did not select the action
	Selected = 1,			// Selected but no started
	CannotBeApplied = 2,	// The action cannot be applied (e.g. Windows ES needed US found or application not found)
	AlreadyApplied = 3,		// Action not needed
	InProgress = 4,			// Selected and in progress
	Successful = 5,			// End up OK
	FinishedWithError = 6	// End up with error
};