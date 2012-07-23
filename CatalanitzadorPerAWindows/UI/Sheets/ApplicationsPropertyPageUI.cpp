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
#include "ShowLicensesDlgUI.h"
#include "AppRunningDlgUI.h"
#include "AdobeReaderAction.h"

#define DEFAULT_SELECTEDITEM_INLISTVIEW 1

ApplicationsPropertyPageUI::ApplicationsPropertyPageUI()
{
	m_hFont = NULL;	
}

ApplicationsPropertyPageUI::~ApplicationsPropertyPageUI()
{
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}	
}

void ApplicationsPropertyPageUI::_getActionDisplayName(Action *action, wstring& name)
{	
	name = action->GetName();

	if (action->GetStatus() == Selected && action->HasLicense())
	{
		name+= L" *";
	}
}

void ApplicationsPropertyPageUI::_processDependantItem(Action* action)
{
	Action* dependant = action->AnotherActionDependsOnMe(m_availableActions);

	if (dependant == NULL)
		return;
	
	ActionStatus prevStatus = dependant->GetStatus();
	dependant->CheckPrerequirements(action);

	if (prevStatus == dependant->GetStatus())
		return;

	for (int i = 0; i < m_listview.Count(); i++)
	{
		Action* itemAction = (Action *) m_listview.GetItemData(i);
		if (itemAction == dependant)
		{
			m_listview.SetItemImage(i, itemAction->GetStatus());
			break;
		}
	}
}

void ApplicationsPropertyPageUI::ProcessClickOnItem(int nItem)
{
	Action* action = (Action *) m_listview.GetItemData(nItem);

	if (action == NULL)
		return;

	switch (action->GetStatus()) {
	case NotSelected:
		action->SetStatus(Selected);
		break;
	case Selected:
		action->SetStatus(NotSelected);
		break;
	default:
		return; // Non selectable item
	}

	_processDependantItem(action);

	wstring name;
	_getActionDisplayName(action, name);
	m_listview.SetItemImage(nItem, action->GetStatus());
	m_listview.SetItemText(nItem, name);
	
	_enableOrDisableLicenseControls();
}

void ApplicationsPropertyPageUI::_setBoldControls()
{
	int boldControls [] = {IDC_APPLICATION_DESCRIPTION_CAPTION, IDC_APPLICATION_CANNOTBEAPPLIED_CAPTION,
		IDC_APPLICATION_LEGEND_CAPTION, IDC_APPLICATION_LICENSES_CAPTION};

	m_hFont = Window::CreateBoldFont(getHandle());

	for (int b = 0; b < sizeof(boldControls) / sizeof(boldControls[0]); b++)
	{
		SendMessage(GetDlgItem (getHandle(), boldControls[b]),
			WM_SETFONT, (WPARAM) m_hFont, TRUE);
	}
}

void ApplicationsPropertyPageUI::_setLegendControl()
{
	ActionStatus statuses [] = {Selected, AlreadyApplied, CannotBeApplied};
	int resources [] = {IDS_LEGEND_SELECTED, IDS_LEGEND_ALREADYAPPLIED, IDS_LEGEND_CANNOT};

	m_listviewLegend.InitControl(GetDlgItem(getHandle(), IDC_APPLICATIONSLEGEND));
	
	for (int l = 0; l <sizeof(statuses) / sizeof(statuses[0]); l++)
	{
		wchar_t szString [MAX_LOADSTRING];
		
		LoadString(GetModuleHandle(NULL), resources[l], szString, MAX_LOADSTRING);
		m_listviewLegend.InsertItem(szString, NULL, statuses[l], l);
	}
}

void ApplicationsPropertyPageUI::_insertActioninListView(Action *action, int &itemID)
{
	wstring name;
	_getActionDisplayName(action, name);

	m_listview.InsertItem(name, (LPARAM) action, action->GetStatus(), itemID);
	itemID++;
}

// An index to ActionGroup
static const int groupNames [] = {IDS_GROUPNAME_NONE, IDS_GROUPNAME_WINDOWS, IDS_GROUPNAME_INTERNET, IDS_GROUPNAME_OFFICE};

void ApplicationsPropertyPageUI::_insertGroupNameListView(ActionGroup group, int &itemID)
{
	wchar_t szString[MAX_LOADSTRING];

	LoadString(GetModuleHandle(NULL), groupNames[(int)group], szString, MAX_LOADSTRING);
	m_listview.InsertItem(szString, itemID);
	itemID++;
}

void ApplicationsPropertyPageUI::_onClickItemEvent(int nItem, void* data)
{
	ApplicationsPropertyPageUI* pThis = (ApplicationsPropertyPageUI*) data;
	return pThis->ProcessClickOnItem(nItem);
}

void ApplicationsPropertyPageUI::_onInitDialog()
{
	HWND hListWnd;
	int nItemId = 0;
	map <Action *, bool>::iterator mapped_item;	

	if (m_availableActions->size() == 0)
		return;

	hListWnd = GetDlgItem(getHandle(), IDC_APPLICATIONSLIST);
	m_listview.InitControl(hListWnd);
	m_listview.SetClickItem(_onClickItemEvent, this);	
		
	for (int g = 0; g < ActionGroupLast; g++)
	{
		bool bFirstHit = false;

		for (unsigned int i = 0; i < m_availableActions->size (); i++)
		{		
			Action* action = m_availableActions->at(i);

			if (action->GetGroup() != (ActionGroup)g)
				continue;

			bool needed = action->IsNeed();

			if (bFirstHit == false)
			{
				_insertGroupNameListView((ActionGroup)g, nItemId);
				bFirstHit = true;
			}

			m_disabledActions.insert(ActionBool_Pair(action, needed));

			if (needed)
				action->SetStatus(Selected);
			
			_insertActioninListView(action, nItemId);
			_processDependantItem(action);
		}
	}

	m_listview.SelectItem(DEFAULT_SELECTEDITEM_INLISTVIEW);
	_setBoldControls();
	_setLegendControl();
	_enableOrDisableLicenseControls();
}

NotificationResult ApplicationsPropertyPageUI::_onNotify(LPNMHDR hdr, int iCtrlID)
{
	if(hdr->idFrom != IDC_APPLICATIONSLIST)
		return ReturnFalse;	
	if (hdr->code == NM_CUSTOMDRAW)
	{
		LPNMLVCUSTOMDRAW lpNMLVCD = (LPNMLVCUSTOMDRAW)hdr;

		switch (lpNMLVCD->nmcd.dwDrawStage)
		{
			case CDDS_PREPAINT:	
				SetWindowLong(getHandle(), DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
				return ReturnTrue;

			case CDDS_ITEMPREPAINT:
			{
				Action* action = (Action *)  lpNMLVCD->nmcd.lItemlParam;
				bool disabled = false;

				if (action != NULL)
				{
					map <Action *, bool>::iterator item;
					item = m_disabledActions.find((Action * const &)action);

					if (item->second == false)				
						disabled = true;				
				}

				m_listview.PreItemPaint(lpNMLVCD, disabled);
				SetWindowLong(getHandle(), DWLP_MSGRESULT, CDRF_NOTIFYPOSTPAINT);
				return ReturnTrue;
			}

			case CDDS_ITEMPOSTPAINT:		
				m_listview.PostItemPaint(lpNMLVCD, lpNMLVCD->nmcd.lItemlParam == NULL);
				return ReturnTrue;
			default:
				return CallDefProc;
		}
	}

	NMLISTVIEW *pListView = (NMLISTVIEW *)hdr;
	if (pListView->hdr.code != LVN_ITEMCHANGED)
		return ReturnFalse;

	_updateActionDescriptionAndReq((Action *)  pListView->lParam);
	return ReturnTrue;
}

void ApplicationsPropertyPageUI::_updateActionDescriptionAndReq(Action* action)
{
	int show;
	bool isHeader = action == NULL;

	SendDlgItemMessage (getHandle(), IDC_APPLICATION_DESCRIPTION,
		WM_SETTEXT, (WPARAM) 0, 
		isHeader ? NULL : (LPARAM) action->GetDescription());

	SendDlgItemMessage (getHandle(), IDC_APPLICATION_MISSINGREQ,
		WM_SETTEXT, (WPARAM) 0, 
		isHeader ? NULL : (LPARAM) action->GetCannotNotBeApplied());

	show = isHeader ? FALSE : *action->GetCannotNotBeApplied() != NULL ? SW_SHOWNORMAL: SW_HIDE;
	ShowWindow(GetDlgItem(getHandle(), IDC_APPLICATION_CANNOTBEAPPLIED_CAPTION), show);
	ShowWindow(GetDlgItem(getHandle(), IDC_APPLICATION_MISSINGREQ), show);	
}

 void ApplicationsPropertyPageUI::_enableOrDisableLicenseControls()
 {
	bool bEnabled;
		 
	bEnabled = _licensesNeedToBeAccepted();

	int controls [] = {IDC_LICENSES_ASTERISCTEXT, IDC_SHOWLICENSES_BUTTON, IDC_ACCEPTLICENSES_CHECKBOX, IDC_APPLICATION_LICENSES_CAPTION};
	for (int i = 0; i < sizeof(controls) / sizeof(controls[0]); i++)
	{
		EnableWindow(GetDlgItem (getHandle(), controls[i]), bEnabled ? TRUE : FALSE);
	}
 }

bool ApplicationsPropertyPageUI::_licensesNeedToBeAccepted()
{	
	for (unsigned int i = 0; i < m_availableActions->size (); i++)
	{
		Action* action = m_availableActions->at(i);

		if (action->GetStatus() != Selected)
			continue;

		if (action->HasLicense())
			return true;	
	}
	return false;	
}

bool ApplicationsPropertyPageUI::_licenseAccepted()
{
	if (_licensesNeedToBeAccepted() == false)
		return true;

	if (IsDlgButtonChecked(getHandle(),IDC_ACCEPTLICENSES_CHECKBOX) != BST_CHECKED)
	{
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];

		LoadString(GetModuleHandle(NULL), IDS_REQUIRES_ACCEPTLICENSES, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

		MessageBox(getHandle(), szMessage, szCaption, MB_ICONWARNING | MB_OK);
		return false;
	}
	return true;
}

void ApplicationsPropertyPageUI::_onCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_SHOWLICENSES_BUTTON)
	{
		ShowLicensesDlgUI licencesDlg;
		licencesDlg.SetActions(m_availableActions);
		licencesDlg.Run(hWnd);
	}
}

bool ApplicationsPropertyPageUI::_anyActionNeedsInternetConnection()
{
	bool needInet = false;

	for (int i = 0; i < m_listview.Count(); ++i)
	{				
		Action* action = (Action *) m_listview.GetItemData(i);

		if (action == NULL)
			continue;
		
		bool bSelected;

		bSelected = action->GetStatus() == Selected;
		g_log.Log(L"ApplicationsPropertyPageUI::_onNext. Action '%s', selected %u", action->GetName(), (wchar_t *)bSelected);

		if (bSelected && action->IsDownloadNeed())		
			needInet = true;
	}
	return needInet;
}

bool ApplicationsPropertyPageUI::_onNext()
{
	if (_licenseAccepted() == false)
		return false;

	if (_checkRunningApps() == true)
		return false;

	if (_anyActionNeedsInternetConnection() && Inet::IsThereConnection() == false)
	{
		_showNoInternetConnectionDialog();
		return FALSE;
	}
	return TRUE;
}

void ApplicationsPropertyPageUI::_showNoInternetConnectionDialog()
{
	wchar_t szMessage [MAX_LOADSTRING];
	wchar_t szCaption [MAX_LOADSTRING];

	LoadString(GetModuleHandle(NULL), IDS_NOINETACCESS, szMessage, MAX_LOADSTRING);
	LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

	MessageBox(getHandle(), szMessage, szCaption, MB_ICONWARNING | MB_OK);	
}

bool ApplicationsPropertyPageUI::_checkRunningApps()
{
	for (unsigned int i = 0; i < m_availableActions->size(); i++)
	{
		Action* action = m_availableActions->at(i);

		if (action->GetStatus() != Selected)
			continue;

		if (action->GetID() == AcrobatReader)
		{
			AdobeReaderAction* readerAction = (AdobeReaderAction*) action;
			if (readerAction->IsIERunning() == true)
				return true;
		}
		
		if (action->IsExecuting())
		{		
			AppRunningDlgUI dlg (action->GetName());
			if (dlg.Run(getHandle()) == IDOK)
			{
				action->FinishExecution();
			}
			return true;
		}
	}
	return false;
}
