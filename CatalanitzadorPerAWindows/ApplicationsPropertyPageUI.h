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
#include <vector>
using namespace std;

class ApplicationsPropertyPageUI: public PropertyPageUI
{
public:	
		
		virtual void _onInitDialog();
		virtual void _onNotify(LPNMHDR /*hdr*/, int /*iCtrlID*/);
		virtual	void _onNext();

		vector <Action *> * GetSelectedActions () { return m_selectedActions;}
		void SetSelectedActions (vector <Action *> * value) {  m_selectedActions =  value;}
private:
		HWND hList;
		vector <Action *> m_actions;
		vector <Action *> * m_selectedActions;
};
