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

enum MSOfficeVersion
{
	MSOfficeUnKnown,
	NoMSOffice,
	MSOffice2003,
	MSOffice2007,
	MSOffice2010
};

class MSOfficeAction : public Action
{
public:
		MSOfficeAction ();
		~MSOfficeAction ();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual int GetID() { return 2;};
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute(ProgressStatus progress, void *data);
		virtual ActionStatus GetStatus();

private:

		void _getVersionInstalledWithNoLangPack ();
		bool _isVersionInstalled(wchar_t* version);
		bool _isLangPackForVersionInstalled(wchar_t* version);
		bool _isVersionInstalledWithNoLangPack(wchar_t* version);
		bool _wasInstalledCorrectly();
		bool _extractCabFile(wchar_t * file, wchar_t * path);
		void _setDefaultLanguage();
		static VOID CALLBACK _timerProc(HWND hwnd, UINT uMsg,  UINT_PTR idEvent, DWORD dwTime);		
		wchar_t* _getPackageName();

		wchar_t filename[MAX_PATH];
		wchar_t szTempPath[MAX_PATH];		
		Runner runner;
		UINT hTimerID;
		MSOfficeVersion m_MSVersion;		
};

