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

#include "stdafx.h"
#include "InstallPropertyPageUI.h"
#include "Actions.h"
#include "Action.h"
#include "PropertySheetUI.h"

#include <stdio.h>
#include <vector>
using namespace std;

#define TIMER_ID 1714

void InstallPropertyPageUI::_onInitDialog()
{
	hProgressBar = GetDlgItem (getHandle (), IDC_INSTALL_PROGRESS);
	hDescription = GetDlgItem (getHandle (), IDC_INSTALL_DESCRIPTION);
}

void iYield(void)
{
	MSG msg;
	
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void InstallPropertyPageUI::_onShowWindow()
{
	SetTimer (getHandle (), TIMER_ID, 500, NULL);
}

void InstallPropertyPageUI::_onTimer()
{
	KillTimer(getHandle (), TIMER_ID);

	int cnt = m_selectedActions->size();
	Action* action;

	SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, cnt * 2));
	SendMessage(hProgressBar, PBM_SETSTEP, 10, 0L); 

	for (int i = 0; i < cnt; i++)
	{
		wchar_t szMessage[255];

		action = m_selectedActions->at(i);

		// Download
		swprintf_s (szMessage, L"S'està baixant '%s'", action->GetName());
		SendMessage (hDescription, WM_SETTEXT, 0, (LPARAM) szMessage);
		iYield();
		action->Download ();
		SendMessage(hProgressBar, PBM_DELTAPOS, 1, 0);
		iYield();
		
		// Execute
		swprintf_s (szMessage, L"S'està executant '%s'", action->GetName());
		SendMessage (hDescription, WM_SETTEXT, 0, (LPARAM) szMessage);
		iYield();
		action->Execute();		

		while (true)
		{
			ActionResult result = action->Result();

			if (result == Successfull || result == FinishedWithError)
				break;

			iYield();
			Sleep(500);
			iYield();
		}

		SendMessage(hProgressBar, PBM_DELTAPOS, 1, 0);
		iYield();
	}

	SendMessage (hDescription, WM_SETTEXT, 0, (LPARAM) L"Procés completat");	
}

