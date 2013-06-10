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
#include "CheckedListView.h"

CheckedListView::CheckedListView()
{
	m_itemID = 0;
}

void CheckedListView::InsertSingleColumnAllWidth()
{
	LVCOLUMN listCol;

	memset(&listCol, 0, sizeof(LVCOLUMN));
	listCol.mask = LVCF_WIDTH |LVCF_SUBITEM;

	// The total size is the internal rect - size of the scroll bar
	// Things to test if this changes
	// 
	//  1. The Aero and non-area versions are shown properly 
	//  2. Check with 120 dpi in both versions
	//  3. Check with scroll bar at the right in both versions
	//
	RECT rect;
	GetClientRect(m_hWnd, &rect);

	const int scroll_size = GetSystemMetrics(SM_CXVSCROLL);
	listCol.cx = rect.right - rect.left - scroll_size;
	SendMessage(m_hWnd,LVM_INSERTCOLUMN,0,(LPARAM)&listCol);	
}

void CheckedListView::InitControl(HWND hWnd)
{
	m_hWnd = hWnd;
	m_hImageList = CreateCheckBoxImageList(m_hWnd);
	ListView_SetImageList(m_hWnd, m_hImageList, LVSIL_SMALL);

	PreviousProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR) _listViewSubclassProc);		
	SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG) this);
}

LPARAM CheckedListView::GetItemData(int nItem)
{			
	LVITEM item;
	memset(&item,0,sizeof(item));
	item.iItem = nItem;
	item.mask = LVIF_PARAM;

	ListView_GetItem(m_hWnd, &item);
	return item.lParam;
}

void CheckedListView::SetItemText(int nItem, wstring text)
{
	LVITEM item;
	memset(&item,0,sizeof(item));
	item.iItem = nItem;
	item.mask = LVIF_TEXT;
	item.pszText = (LPWSTR) text.c_str();
	ListView_SetItem(m_hWnd, &item);
}

void CheckedListView::SetItemImage(int nItem, ImageIndex imageIndex)
{
	LVITEM item;
	memset(&item,0,sizeof(item));
	item.iItem = nItem;
	item.mask = LVIF_IMAGE;
	item.iImage = imageIndex;
	ListView_SetItem(m_hWnd, &item);
}

void CheckedListView::InsertItem(wstring text, LPARAM parameter, ImageIndex imageIndex)
{
	wstring name;
	LVITEM item;
	memset(&item,0,sizeof(item));
	item.mask=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	
	item.iItem = m_itemID;
	item.pszText = (LPWSTR) text.c_str();
	item.lParam = (LPARAM) parameter;
	item.iImage = imageIndex;
	ListView_InsertItem(m_hWnd, &item);
	m_itemID++;
}

int CheckedListView::_makeSquareRect(LPRECT src, LPRECT dst)
{
    int Width  = src->right - src->left;
    int Height = src->bottom - src->top;
    int SmallDiam = Width > Height ? Height : Width;

    *dst = *src;
    
    if (Width < Height)
    {
        dst->top += (Height-Width)/2;
        dst->bottom = dst->top + SmallDiam;
    }
    else if(Width > Height)
    {
        dst->left += (Width-Height)/2;
        dst->right = dst->left + SmallDiam;
    }

   return SmallDiam;
}


void CheckedListView::_createFrameBox(HDC dc, LPRECT r)
{	
	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0,0));
	HPEN hPenOld = (HPEN) SelectObject(dc,hPen);

	// top
	MoveToEx(dc, r->left, r->top, NULL);
	LineTo(dc, r->right, r->top);

	// bottom
	MoveToEx(dc, r->left, r->bottom, NULL);
	LineTo(dc, r->right, r->bottom);

	// left
	MoveToEx(dc, r->left, r->top, NULL);
	LineTo(dc, r->left, r->bottom);

	// right
	MoveToEx(dc, r->right, r->top, NULL);
	LineTo(dc, r->right, r->bottom);

	SelectObject(dc, hPenOld);
	DeleteObject(hPen);
}

#define ICON_DRAW_WIDTH 16
#define ICON_DRAW_HEIGHT 16

void CheckedListView::_createButtonCheckImage(HDC dc, LPRECT r, bool bChecked, CheckedColor color)
{
    RECT myr, bar;

    _makeSquareRect(r, &myr);
    FillRect(dc, &myr, GetSysColorBrush(COLOR_WINDOW));
	_createFrameBox(dc, r);

    if (bChecked)
    {
        HBRUSH hBrush = CreateSolidBrush(color);
		const int offset_x = (myr.right - ICON_DRAW_WIDTH) / 2;
		const int offset_y = (myr.bottom - ICON_DRAW_HEIGHT) / 2;

        /* draw 7 bars, with h=3w to form the check */
        bar.left = myr.left + 3 + offset_x;
        bar.top = myr.top + 5 + offset_y;
        for (int k = 0; k < 7; k++) 
		{
            bar.left = bar.left + 1;
            bar.top = (k < 3) ? bar.top + 1 : bar.top - 1;
            bar.bottom = bar.top + 3;
            bar.right = bar.left + 1;
			FillRect(dc, &bar, hBrush);
        }
		DeleteObject(hBrush);
    }    
}

HIMAGELIST CheckedListView::CreateCheckBoxImageList(HWND hWnd)
{
	HDC hdc_wnd, hdc;
	HBITMAP hbm_im, hbm_mask, hbm_orig;
	RECT rc;
	HIMAGELIST himl;
	const int cx_icon = GetSystemMetrics(SM_CXSMICON);
	const int cy_icon = GetSystemMetrics(SM_CYSMICON);
	
	himl = ImageList_Create(cx_icon, cy_icon, ILC_COLOR16 | ILC_MASK, 2, 2);
	hdc_wnd = GetDC(hWnd);
	hdc = CreateCompatibleDC(hdc_wnd);
	hbm_im = CreateCompatibleBitmap(hdc_wnd, cx_icon, cy_icon);
	hbm_mask = CreateBitmap(cx_icon, cy_icon, 1, 1, NULL);
	ReleaseDC(hWnd, hdc_wnd);
                                                                      
	rc.left = rc.top = 0;
	rc.right = cx_icon;
	rc.bottom = cy_icon;

	hbm_orig = (HBITMAP) SelectObject(hdc, hbm_mask);
	SelectObject(hdc, hbm_im);	

	// Create normal image
	_createButtonCheckImage(hdc, &rc, false, CheckedColorNone);
	SelectObject(hdc, hbm_orig);
	ImageList_Add(himl, hbm_im, hbm_mask);
	SelectObject(hdc, hbm_im);

	// Create color checked images
	CheckedColor colors[] = {CheckedColorBlack, CheckedColorGrey, CheckedColorYellow, CheckedColorGreen, CheckedColorNone};
	CheckedColor* color = colors;

	while (*color != CheckedColorNone)
	{	
		_createButtonCheckImage(hdc, &rc, true, *color);
		SelectObject(hdc, hbm_orig);
		ImageList_Add(himl, hbm_im, hbm_mask);
		SelectObject(hdc, hbm_im);
		color++;
	}

	DeleteObject(hbm_mask);
	DeleteObject(hbm_im);
	DeleteDC(hdc);
	return himl;
}


#define PIXELS_TO_INDENT_ACTIONS 5

LRESULT CheckedListView::_listViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CheckedListView *pThis = (CheckedListView *)GetWindowLong(hWnd,GWL_USERDATA);

	switch (uMsg)
	{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		{
			LVHITTESTINFO lvHitTestInfo;
			lvHitTestInfo.pt.x = LOWORD(lParam) - PIXELS_TO_INDENT_ACTIONS;
			lvHitTestInfo.pt.y = HIWORD(lParam);
			ListView_HitTest(hWnd, &lvHitTestInfo);

			if (lvHitTestInfo.flags & LVHT_ONITEMICON)
			{
				pThis->m_onClickItem(lvHitTestInfo.iItem, pThis->m_clickData);
			}
			break;
		}
		
		case WM_KEYDOWN:
		{
			if (wParam == VK_SPACE)
			{
				pThis->m_onClickItem(ListView_GetSelectionMark(hWnd), pThis->m_clickData);
			}
			break;
		}
		default:
			break;
	}
	return CallWindowProc(pThis->PreviousProc, hWnd, uMsg, wParam, lParam);
}

void CheckedListView::_getItem(int itemID, LVITEMW& lvItem)
{	
	lvItem.mask =  LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvItem.stateMask = LVIS_SELECTED;
	lvItem.iItem = itemID;
	lvItem.iSubItem = 0;
	ListView_GetItem(m_hWnd, &lvItem);
}

#define UNDEFINED_COLOR 0xffffffff

void CheckedListView::PostItemPaint(LPNMLVCUSTOMDRAW lpNMLVCD, wstring text, bool isGroupName, bool isDisabled)
{
	RECT rect;
	LVITEM lvItem;

	_getItem(lpNMLVCD->nmcd.dwItemSpec, lvItem);	
	ListView_GetItemRect(lpNMLVCD->nmcd.hdr.hwndFrom, lpNMLVCD->nmcd.dwItemSpec, &rect, LVIR_SELECTBOUNDS);
	FillRect(lpNMLVCD->nmcd.hdc, &rect, GetSysColorBrush(COLOR_WINDOW));

	const int icon_size = GetSystemMetrics(SM_CXSMICON);
	COLORREF color = UNDEFINED_COLOR, oldColor = UNDEFINED_COLOR, bkColor = UNDEFINED_COLOR, oldBkColor = UNDEFINED_COLOR;

	SetBkMode(lpNMLVCD->nmcd.hdc, OPAQUE);
	FillRect(lpNMLVCD->nmcd.hdc, &rect, GetSysColorBrush(COLOR_WINDOW));

	if (isGroupName == false)
	{		
		ImageList_DrawEx(m_hImageList, lvItem.iImage, lpNMLVCD->nmcd.hdc,  rect.left, rect.top, icon_size, icon_size, lpNMLVCD->clrTextBk, CLR_DEFAULT, ILD_NORMAL);
		rect.left += (icon_size + PIXELS_TO_INDENT_ACTIONS);
	}
	 
	if (lvItem.state & LVIS_SELECTED)
	{	
		bkColor = GetSysColor(COLOR_HIGHLIGHT);
		color = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else if (isDisabled && isGroupName == false)
	{
		bkColor = GetSysColor(COLOR_WINDOW);
		color = GetSysColor(COLOR_GRAYTEXT);
	}
	else
	{
		bkColor = GetSysColor(COLOR_WINDOW);
		color = GetSysColor(COLOR_WINDOWTEXT);
	}

	if (color != UNDEFINED_COLOR)
		oldColor = SetTextColor(lpNMLVCD->nmcd.hdc, color);	

	if (bkColor != UNDEFINED_COLOR)
		oldBkColor = SetBkColor(lpNMLVCD->nmcd.hdc, bkColor);
	
	DrawText(lpNMLVCD->nmcd.hdc, text.c_str(), text.length(), &rect, LVCFMT_LEFT);

	if (oldColor != UNDEFINED_COLOR)
		SetTextColor(lpNMLVCD->nmcd.hdc, oldColor);

	if (oldBkColor != UNDEFINED_COLOR)
		SetBkColor(lpNMLVCD->nmcd.hdc, oldBkColor);
}

