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

void CheckedListView::InsertItem(wstring text)
{	
	InsertItem(text, (LPARAM) NULL, ImageIndexNone);
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


void CheckedListView::_createButtonCheckImage(HDC dc, LPRECT r, bool bChecked, CheckedColor color)
{
    RECT myr, bar;

    _makeSquareRect(r, &myr);
    FillRect(dc, &myr, GetSysColorBrush(COLOR_WINDOW));
	_createFrameBox(dc, r);

    if (bChecked)
    {        
        HBRUSH hBrush = CreateSolidBrush(color);

        /* draw 7 bars, with h=3w to form the check */
        bar.left = myr.left + 3;
        bar.top = myr.top + 5;
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
                                    
	int x = GetSystemMetrics(SM_CXSMICON);
	himl = ImageList_Create(x, GetSystemMetrics(SM_CYSMICON), ILC_COLOR16 | ILC_MASK, 2, 2);
	hdc_wnd = GetDC(hWnd);
	hdc = CreateCompatibleDC(hdc_wnd);
	hbm_im = CreateCompatibleBitmap(hdc_wnd, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));	
	hbm_mask = CreateBitmap(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 1, 1, NULL);
	ReleaseDC(hWnd, hdc_wnd);
                                                                      
	rc.left = rc.top = 0;
	rc.right = GetSystemMetrics(SM_CXSMICON);
	rc.bottom = GetSystemMetrics(SM_CYSMICON);

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


#define PIXELS_TO_INDENT_ACTIONS 10

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

void CheckedListView::PreItemPaint(LPNMLVCUSTOMDRAW lpNMLVCD, bool disabled)
{
	if (disabled)
	{
		DWORD color = GetSysColor(COLOR_GRAYTEXT);
		lpNMLVCD->clrText = color;
	}		

	RECT rect;
	ListView_GetItemRect(lpNMLVCD->nmcd.hdr.hwndFrom, lpNMLVCD->nmcd.dwItemSpec, &rect, LVIR_SELECTBOUNDS);
	FillRect(lpNMLVCD->nmcd.hdc, &rect, GetSysColorBrush(COLOR_WINDOW));
}

void CheckedListView::PostItemPaint(LPNMLVCUSTOMDRAW lpNMLVCD, bool groupName)
{
	RECT rect;

	if (groupName)
	{
		int icon_size = GetSystemMetrics(SM_CXSMICON);
		ListView_GetItemRect(lpNMLVCD->nmcd.hdr.hwndFrom, lpNMLVCD->nmcd.dwItemSpec, &rect, LVIR_SELECTBOUNDS);
		ScrollDC(lpNMLVCD->nmcd.hdc, -icon_size, 0, &rect, NULL, NULL, NULL);		
	}
	else
	{		
		ListView_GetItemRect(lpNMLVCD->nmcd.hdr.hwndFrom, lpNMLVCD->nmcd.dwItemSpec, &rect, LVIR_SELECTBOUNDS);
		ScrollDC(lpNMLVCD->nmcd.hdc, PIXELS_TO_INDENT_ACTIONS, 0, &rect, NULL, NULL, NULL);

		rect.right = rect.left + PIXELS_TO_INDENT_ACTIONS;
		FillRect(lpNMLVCD->nmcd.hdc, &rect, GetSysColorBrush(COLOR_WINDOW));
	}
}

