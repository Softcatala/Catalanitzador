/* 
 * Copyright (C) 2012 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

#include "CheckedListView.h"
#include <string>

class ApplicationLegendItem
{
	public:

		ApplicationLegendItem(wstring name, ImageIndex imageIndex)
		{
			m_name = name;
			m_imageIndex = imageIndex;
		}

		wstring GetName() { return m_name;}
		
		ImageIndex GetImageIndex() { return m_imageIndex;}
		void SetImageIndex(ImageIndex imageIndex) { m_imageIndex = imageIndex;}	

	private:
		wstring m_name;
		ImageIndex m_imageIndex;		
};