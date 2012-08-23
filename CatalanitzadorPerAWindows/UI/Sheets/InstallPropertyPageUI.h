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

#include "PropertyPageUI.h"
#include "Action.h"
#include "Serializer.h"
#include "SlideshowThread.h"
#include "SystemRestoreThread.h"

#include <vector>
using namespace std;

class InstallPropertyPageUI: public PropertyPageUI
{
public:		
		void SetSerializer(Serializer* serializer) { m_serializer = serializer; }
		void SetActions(vector <Action *> * value) { m_actions = value;}
		void SetSystemRestore(BOOL *pbSystemRestore) { m_pbSystemRestore = pbSystemRestore;}
		void StartSlideShowUnpack() { m_slideshow.Start();}
		
private:
		virtual void _onInitDialog();
		virtual	void _onShowWindow();
		virtual	void _onTimer();
		static void _downloadStatus(int total, int current, void *data);

		void _execute(Action* action);
		bool _download(Action* action);
		void _completed();
		void _updateSelectedActionsCounts();
		void _windowsXPAsksCDWarning();
		void _setTaskMarqueeMode(bool enable);
		void _waitExecutionComplete(Action* action);
		void _openURLInIE();
		void _systemRestore(SystemRestoreThread& systemRestore);

		vector <Action *> * m_actions;
		HWND hTotalProgressBar;
		HWND hTaskProgressBar;
		HWND hDescription;
		BOOL ShowWindowOnce;
		Serializer* m_serializer;
		int m_selActions;
		int m_downloads;
		SlideshowThread m_slideshow;
		BOOL* m_pbSystemRestore;
};
