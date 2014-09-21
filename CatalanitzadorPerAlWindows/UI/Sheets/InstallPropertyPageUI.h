/* 
 * Copyright (C) 2011-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "InstallModel.h"
#include "TriBool.h"

#include <vector>
using namespace std;

class InstallPropertyPageUI: public PropertyPageUI
{
public:	
		InstallPropertyPageUI(InstallModel* installModel) { m_model = installModel;}
				
		void StartSlideShowUnpack() { m_slideshow.Start();}	
		
private:
		virtual void _onInitDialog();
		virtual	void _onShowWindow();
		virtual	void _onTimer();		

		static bool _downloadStatus(int total, int current, void *data);
		static void _notifyExecutionCompleted(NotifyActionData* notifyActionData);
		static void _notifyExecutionStarts(NotifyActionData* notifyActionData);
		static void _notifyDownloadCompleted(NotifyActionData* notifyActionData);
		static bool _notifyDownloadError(NotifyActionData* notifyActionData);
		
		void _completed();
		void _setTaskMarqueeMode(bool enable);
		void _openURLInIE();
		void _systemRestore();
		void _setTotalProgressBar();

		vector <Action *> * m_actions;
		HWND m_hTotalProgressBar;
		HWND m_hTaskProgressBar;
		HWND m_hDescription;
		bool m_showWindowOnce;
		SlideshowThread m_slideshow;
		InstallModel* m_model;
		TriBool m_marqueeMode;
};
