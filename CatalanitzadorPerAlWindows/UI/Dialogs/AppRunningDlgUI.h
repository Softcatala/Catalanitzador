/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "DlgUI.h"
#include <Action.h>

class AppRunningDlgUI : public DlgUI
{
public:
		AppRunningDlgUI(wstring action_name, wstring processName, bool canClose);		
		virtual LPCTSTR GetResourceTemplate() {return MAKEINTRESOURCE(IDD_APPRUNNING);}
		virtual	void _onInitDialog();
		
private:

		void _cannotCloseApp();

		wstring m_action_name;
		wstring m_processName;
		bool m_canClose;
};
