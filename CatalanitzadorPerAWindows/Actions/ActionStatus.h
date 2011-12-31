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

enum ActionStatus
{
	NotSelected,		// The user did not select the action
	Selected,			// Selected but no started
	CannotBeApplied,	// The action cannot be applied (e.g. Windows ES needed US found or application not found)
	AlreadyApplied,		// Action not needed
	InProgress,			// Selected and in progress
	Successful,			// End up OK
	FinishedWithError	// End up with error
};