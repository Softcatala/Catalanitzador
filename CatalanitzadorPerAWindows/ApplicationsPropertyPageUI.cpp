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

bool ApplicationsPropertyPageUI::_isActionNeeded(HWND hWnd, int nItem)
{	
	LVITEM lvitem;
	
	memset(&lvitem,0,sizeof(lvitem));
	lvitem.iItem = nItem;
	lvitem.mask = LVIF_PARAM;
	ListView_GetItem(hWnd, &lvitem);

	Action* action = (Action *) lvitem.lParam;
	map <Action *, bool>::iterator item;

	item = m_disabledActions.find((Action * const &)action);
	return item->second;
}

LRESULT ApplicationsPropertyPageUI::ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{	
	ApplicationsPropertyPageUI *pThis = (ApplicationsPropertyPageUI *)GetWindowLong(hWnd,GWL_USERDATA);

	switch (uMsg)
	{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		{
			LVHITTESTINFO lvHitTestInfo;
			lvHitTestInfo.pt.x = LOWORD(lParam);
			lvHitTestInfo.pt.y = HIWORD(lParam);
			int nItem = ListView_HitTest(hWnd, &lvHitTestInfo);

			if (lvHitTestInfo.flags & LVHT_ONITEMSTATEICON)
			{
				if (pThis->_isActionNeeded(hWnd, nItem) == false)
				{
					return 0;
				}
			}
		}
		
		case WM_KEYDOWN:
		{
			if (wParam == VK_SPACE)
			{
				int nItem = ListView_GetSelectionMark(hWnd);
				if (pThis->_isActionNeeded(hWnd, nItem) == false)
				{
					return 0;
				}
			}
		}

		default:
			return CallWindowProc(pThis->PreviousProc, hWnd, uMsg, wParam, lParam);
	}
} 

void ApplicationsPropertyPageUI::_onInitDialog()
{
	int nItemId = 0;
	hList = GetDlgItem(getHandle(), IDC_APPLICATIONSLIST);

	ListView_SetExtendedListViewStyle (hList, LVS_EX_CHECKBOXES);

	LVITEM item;
	memset(&item,0,sizeof(item));
	item.mask=LVIF_TEXT | LVIF_PARAM;

	if (m_availableActions->size () == 0)
		return;

	// Enabled items
	for (unsigned int i = 0; i < m_availableActions->size (); i++)
	{		
		Action* action = m_availableActions->at(i);
		bool needed = action->IsNeed();

		m_disabledActions.insert(ActionBool_Pair (action, needed));

		if (needed == false)
			continue;

		item.iItem=nItemId;
		item.pszText= action->GetName ();
		item.lParam = (LPARAM) action;		
		ListView_InsertItem (hList, &item);
		ListView_SetCheckState (hList, nItemId, true);
		nItemId++;
	}

	// Disabled items
	for (unsigned int i = 0; i < m_availableActions->size (); i++)
	{		
		Action* action = m_availableActions->at(i);
		map <Action *, bool>::iterator disabled_item;

		disabled_item = m_disabledActions.find((Action * const &)action);

		if (disabled_item->second == true)
			continue;
		
		item.iItem=nItemId;
		item.pszText= action->GetName ();
		item.lParam = (LPARAM) action;		
		ListView_InsertItem (hList, &item);
		ListView_SetCheckState (hList, nItemId, false);
		nItemId++;
	}

	ListView_SetItemState (hList, 0, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
	SetWindowLongPtr(hList, GWL_USERDATA, (LONG) this);
	PreviousProc = (WNDPROC)SetWindowLongPtr (hList, GWLP_WNDPROC, (LONG_PTR) ListViewSubclassProc);
}

NotificationResult ApplicationsPropertyPageUI::_onNotify(LPNMHDR hdr, int iCtrlID)
{
	if(hdr->idFrom != IDC_APPLICATIONSLIST)
		return ReturnFalse;

	if (hdr->code == NM_CUSTOMDRAW)
	{
		LPNMLVCUSTOMDRAW lpNMLVCD = (LPNMLVCUSTOMDRAW)hdr;

		if (lpNMLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		{
			SetWindowLong (getHandle (), DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
			return ReturnTrue;
		}

		if (lpNMLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		{
			Action* action = (Action *)  lpNMLVCD->nmcd.lItemlParam;
			map <Action *, bool>::iterator item;

			item = m_disabledActions.find((Action * const &)action);

			if (item->second == false)
			{
				DWORD color = GetSysColor(COLOR_GRAYTEXT);
				lpNMLVCD->clrText = color;
			}
			SetWindowLong (getHandle (), DWLP_MSGRESULT, CDRF_DODEFAULT);
			return ReturnTrue;
		}
		
		return CallDefProc;
	}

	NMLISTVIEW *pListView = (NMLISTVIEW *)hdr;
	    
	if(pListView->hdr.code != LVN_ITEMCHANGED)
		return ReturnFalse;

	Action* action = (Action *)  pListView->lParam;

	SendDlgItemMessage (getHandle(), IDC_APPLICATION_DESCRIPTION,
		WM_SETTEXT, (WPARAM) 0, 
		(LPARAM) action->GetDescription());

	return ReturnTrue;
}

bool ApplicationsPropertyPageUI::_onNext()
{
	int items = ListView_GetItemCount (hList);
	bool needInet = false;

	for (int i = 0; i < items; ++i)
	{
		bool selected = ListView_GetCheckState(hList, i) != FALSE;

		LVITEM item;
		memset(&item,0,sizeof(item));
		item.iItem = i;
		item.mask = LVIF_PARAM;

		ListView_GetItem(hList, &item);
		Action* action = (Action *) item.lParam;
		action->SetStatus (selected ? Selected : NotSelected);
		g_log.Log (L"ApplicationsPropertyPageUI::_onNext. Action '%s', selected %u", action->GetName(), (wchar_t *)selected);

		if (selected == true && action->IsDownloadNeed())
			needInet = true;
	}

	if (needInet && InternetAccess::IsThereConnection() == false)
	{		
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];

		LoadString(GetModuleHandle(NULL), IDS_NOINETACCESS, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

		MessageBox(getHandle(), szMessage, szCaption, MB_ICONWARNING | MB_OK);
		return FALSE;
	}
	return TRUE;
}