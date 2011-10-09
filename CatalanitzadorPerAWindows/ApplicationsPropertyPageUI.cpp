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
#include "ApplicationsPropertyPageUI.h"
#include "Actions.h"
#include "Action.h"

#include <vector>
using namespace std;

void ApplicationsPropertyPageUI::_onInitDialog()
{
	hList = GetDlgItem(getHandle(), IDC_APPLICATIONSLIST);

	ListView_SetExtendedListViewStyle (hList, LVS_EX_CHECKBOXES);

	LVITEM item;
	memset(&item,0,sizeof(item));
	item.mask=LVIF_TEXT | LVIF_PARAM;

	Actions actions;
	m_actions = actions.GetActions ();

	if (m_actions.size () == 0)
		return;

	for (unsigned int i = 0; i < m_actions.size (); i++)
	{
		Action* action = m_actions.at(i);

		if (action->IsNeed () == false)
			continue;

		item.iItem=i;
		item.pszText= action->GetName ();
		item.lParam = (LPARAM) action;		
		ListView_InsertItem (hList, &item);
		ListView_SetCheckState (hList, 0, true);
	}

	ListView_SetItemState (hList, 0, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
}

void ApplicationsPropertyPageUI::_onNotify(LPNMHDR hdr, int iCtrlID)
{
	if(hdr->idFrom != IDC_APPLICATIONSLIST)   
		return;

	NMLISTVIEW *pListView = (NMLISTVIEW *)hdr;
    
	if(pListView->hdr.code != LVN_ITEMCHANGED)
		return;

	Action* action = (Action *)  pListView->lParam;

	SendDlgItemMessage (getHandle(), IDC_APPLICATION_DESCRIPTION,
		WM_SETTEXT, (WPARAM) 0, 
		(LPARAM) action->GetDescription());
}

void ApplicationsPropertyPageUI::_onNext()
{
	int items = ListView_GetItemCount (hList);

	for (int i = 0; i < items; ++i)
	{
		LVITEM item;

		item.iSubItem = 0;
		item.iItem = i;
		item.mask = LVIF_PARAM;
		item.pszText = 0;
		item.cchTextMax = 0;

		ListView_GetItem(hList, &item);
		m_selectedActions->push_back((Action *) item.lParam);
	}	
}