/* 
 * Copyright (C) 2011 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

#include "stdafx.h"
#include "Action.h"

#include <string>
using namespace std;

class CarregadorPerAlWindows
{
	
public:
		CarregadorPerAlWindows(HINSTANCE hInstance);
		~CarregadorPerAlWindows();

		int Run(wstring commandLine);		

private:
		void _updateCatalanitzadorAction(Action* catalanitzadorAction);
		void _initLog();		
		bool _isAlreadyRunning();		
		
		HINSTANCE m_hInstance;
		HANDLE m_hEvent;
};
