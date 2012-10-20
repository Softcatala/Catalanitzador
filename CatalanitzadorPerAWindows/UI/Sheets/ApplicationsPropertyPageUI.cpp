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
#include "ActionExecution.h"

#define DEFAULT_SELECTEDITEM_INLISTVIEW 1

ApplicationsPropertyPageUI::ApplicationsPropertyPageUI(ApplicationsModel* model)
{
	m_hFont = NULL;
	m_model = model;
}

ApplicationsPropertyPageUI::~ApplicationsPropertyPageUI()
{
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}	
}

void ApplicationsPropertyPageUI::UpdateItem(ApplicationItem itemToUpdate)
{
	for (unsigned int i = 0; i < m_model->GetItems().size(); i++)
	{
		ApplicationItem item = m_model->GetItems().at(i);

		if (item.GetData() == itemToUpdate.GetData())
		{
			m_listview.SetItemImage(i, item.GetImageIndex());
			m_listview.Invalidate();
			break;
		}
	}	
}

void ApplicationsPropertyPageUI::ProcessClickOnItem(int nItem)
{
	Action* action = (Action *) m_listview.GetItemData(nItem);

	if (action != NULL)
	{	
		for (unsigned int i = 0; i < m_model->GetItems().size(); i++)
		{
			ApplicationItem item = m_model->GetItems().at(i);

			if ((Action *)item.GetData() == action)
			{
				m_model->ProcessClickOnItem(item);				
				break;
			}
		}
	}

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
	vector <ApplicationLegendItem> applicationLegendItems =  m_model->GetLegendItems();

	m_listviewLegend.InitControl(GetDlgItem(getHandle(), IDC_APPLICATIONSLEGEND));	
	for (unsigned int i = 0; i < applicationLegendItems.size(); i++)
	{
		m_listviewLegend.InsertItem(applicationLegendItems[i].GetName(), NULL, 
			applicationLegendItems[i].GetImageIndex());
	}
}

void ApplicationsPropertyPageUI::_onClickItemEvent(int nItem, void* data)
{
	ApplicationsPropertyPageUI* pThis = (ApplicationsPropertyPageUI*) data;
	return pThis->ProcessClickOnItem(nItem);
}

void ApplicationsPropertyPageUI::_onInitDialog()
{
	HWND hListWnd;	

	hListWnd = GetDlgItem(getHandle(), IDC_APPLICATIONSLIST);
	m_listview.InitControl(hListWnd);
	m_listview.SetClickItem(_onClickItemEvent, this);	
	
	m_model->BuildListOfItems();

	// TODO: Move to a PopulateMethod
	for (unsigned int i = 0; i < m_model->GetItems().size(); i++)
	{
		ApplicationItem item = m_model->GetItems().at(i);

		if (item.GetIsGroupName())
		{
			m_listview.InsertItem(item.GetName());
		}
		else
		{
			m_listview.InsertItem(item.GetName(), (LPARAM) item.GetData(), item.GetImageIndex());
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
					for (unsigned int i = 0; i < m_model->GetItems().size(); i++)
					{
						ApplicationItem item = m_model->GetItems().at(i);

						if ((Action *)item.GetData() == action)
						{
							disabled = item.GetIsDisabled();
							break;
						}
					}
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

// TODO: Refactor to use the model
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

//TODO: Move to the model
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
	} else if (wParam == IDC_DIALECTVARIANT_CHECKBOX)
	{
		bool variant;

		variant = IsDlgButtonChecked(getHandle(),IDC_DIALECTVARIANT_CHECKBOX)==BST_CHECKED;
		*m_pbDialectalVariant = variant;

		//TODO: Move to the model
		for (unsigned int i = 0; i < m_availableActions->size (); i++)
		{
			Action* action = m_availableActions->at(i);
			action->SetUseDialectalVariant(variant);
		}
	}
}

bool ApplicationsPropertyPageUI::_onNext()
{
	if (_licenseAccepted() == false)
		return false;

	if (_checkRunningApps() == true)
		return false;

	if (m_model->ShouldShowNoInternetConnectionDialog())
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

//TODO: Move to the model
bool ApplicationsPropertyPageUI::_checkRunningApps()
{
	for (unsigned int i = 0; i < m_availableActions->size(); i++)
	{
		Action* action = m_availableActions->at(i);

		if (action->GetStatus() != Selected)
			continue;
		
		ActionExecution* execution = dynamic_cast<ActionExecution*>(action);

		if (execution == NULL)
			continue;

		ExecutionProcess process = execution->GetExecutingProcess();

		if (process.IsEmpty())
			continue;
		
		AppRunningDlgUI dlg(action->GetName(), process.GetPrintableName(), process.CanClose());
		if (dlg.Run(getHandle()) == IDOK)
		{
			execution->FinishExecution(process);
		}
		return true;		
	}
	return false;
}
