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
#include <CommCtrl.h>
#include "ActionStatus.h"

#include <string>
using namespace std;

enum CheckedColor
{
	CheckedColorBlack = RGB(0,0,0),
	CheckedColorNone = -1,
	CheckedColorGrey = RGB(0xad,0xa8,0x91),
	CheckedColorGreen = RGB(0x0, 0xc0, 0x0),
	CheckedColorYellow = RGB(0xff, 0xd7, 0)
};

enum ImageIndex
{
	ImageIndexNone = 0,
	ImageIndexBlack = 1,
	ImageIndexGrey = 2,
	ImageIndexYellow = 3,
	ImageIndexGreen = 4
};

class CheckedListView
{
public:
		CheckedListView();

		void InitControl(HWND hWnd);
		int Count() {return ListView_GetItemCount(m_hWnd); }
		LPARAM GetItemData(int nItem);
		void SetItemImage(int nItem, ActionStatus status);
		void SetItemText(int nItem, wstring text);
		void SelectItem(int nItem) { ListView_SetItemState(m_hWnd, nItem, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);}
		void InsertItem(wstring text);
		void InsertItem(wstring text, LPARAM parameter, ActionStatus image);

		void PreItemPaint(LPNMLVCUSTOMDRAW lpNMLVCD, bool disabled);
		void PostItemPaint(LPNMLVCUSTOMDRAW lpNMLVCD, bool groupName);

		typedef void (*OnClickItem)(int nItem, void* data);
		void SetClickItem(OnClickItem item, void* data) { m_onClickItem = item; m_clickData = data;}
		void Invalidate() const {InvalidateRect(m_hWnd, NULL, TRUE);}
				
private:

		HIMAGELIST CreateCheckBoxImageList(HWND hWnd);
		int _makeSquareRect(LPRECT src, LPRECT dst);
		void _createFrameBox(HDC dc, LPRECT r);
		void _createButtonCheckImage(HDC dc, LPRECT r, bool bChecked, CheckedColor color);
		ImageIndex GetImageIndex(ActionStatus status);
		static LRESULT _listViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		HWND m_hWnd;
		HIMAGELIST m_hImageList;
		WNDPROC PreviousProc;
		int m_itemID;

		OnClickItem m_onClickItem;
		void *m_clickData;
};
