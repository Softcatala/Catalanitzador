﻿/* 
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
#include "Action.h"

#include <vector>
using namespace std;

class ShowLicensesDlgUI : public DlgUI
{
public:	
		ShowLicensesDlgUI();
		~ShowLicensesDlgUI();

		virtual LPCTSTR GetResourceTemplate() {return MAKEINTRESOURCE(IDD_SHOWLICENSES);}
		virtual	void _onInitDialog();
		virtual void _onCommand(WPARAM wParam, LPARAM lParam);
		void SetActions(vector <Action *> * value) { m_actions =  value;}

private:
		void _setLicenseTextForItem(int index);		
		void _fillActions();

		HMODULE m_handle;
		vector <Action *> * m_actions;
};
