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
	HBRUSH hbr_white = (HBRUSH) GetStockObject(GRAY_BRUSH);
	HBRUSH hbr_black = (HBRUSH) GetStockObject(WHITE_BRUSH);
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

ImageIndex CheckedListView::GetImageIndex(ActionStatus status)
{
	switch (status)
	{
	case NotSelected:
		return ImageIndexNone;
	case Selected:
		return ImageIndexBlack;
	case CannotBeApplied:
		return ImageIndexGrey;
	case AlreadyApplied:
		return ImageIndexGreen;
	default:
		return ImageIndexNone;
	}
}
