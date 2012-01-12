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

#include "Action.h"
#include "Runner.h"
#include "OSVersion.h"

enum IEVersion
{
	IEUnknown = 0,
	IE6 = 6,
	IE7 = 7,
	IE8 = 8,
	IE9 = 9,
};


class IELPIAction : public Action
{
public:
		IELPIAction ();
		~IELPIAction ();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual int GetID() { return IELPI;};
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();		
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual void CheckPrerequirements(Action * action);
		virtual ActionID DependsOn() { return WindowsLPI;};

private:

		wchar_t* _getPackageName();
		wchar_t* _getPackageNameIE7();
		wchar_t* _getPackageNameIE8();
		wchar_t* _getPackageNameIE9();
		IEVersion _getVersion();
		bool _isLangPackInstalled();

		wchar_t m_filename[MAX_PATH];
		Runner runner;
		IEVersion m_version;
		OSVersion m_osVersion;
};

