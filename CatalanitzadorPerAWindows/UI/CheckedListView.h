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

enum CheckedColor
{
	CheckedColorBlack = RGB(0,0,0),
	CheckedColorNone = -1,
	CheckedColorGrey = RGB(0xad,0xa8,0x91),
	CheckedColorGreen = RGB(0x22, 0x8b, 0x22),
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
		HIMAGELIST CreateCheckBoxImageList(HWND hWnd);

		static ImageIndex GetImageIndex(ActionStatus status);
		
private:
		int _makeSquareRect(LPRECT src, LPRECT dst);
		void _createFrameBox(HDC dc, LPRECT r);
		void _createButtonCheckImage(HDC dc, LPRECT r, bool bChecked, CheckedColor color);
};
