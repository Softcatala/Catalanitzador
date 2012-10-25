/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

using namespace std;

class ApplicationItem
{
	public:

		ApplicationItem()
		{
			m_isGroupName = m_isDisabled = false;
		}

		ApplicationItem(wstring name, void* data, bool isGroupName, bool isDisabled)
		{
			m_name = name;
			m_data = data;
			m_isGroupName = isGroupName;
			m_isDisabled = isDisabled;
		}

		wstring GetName() { return m_name;}
		void* GetData() { return m_data;}
		bool GetIsGroupName() { return m_isGroupName;}

		bool GetIsDisabled() { return m_isDisabled;}
		void SetIsDisabled(bool isDisabled) { m_isDisabled = isDisabled;}

		ImageIndex GetImageIndex() { return m_imageIndex;}
		void SetImageIndex(ImageIndex imageIndex) { m_imageIndex = imageIndex;}	

	private:
		wstring m_name;
		void* m_data; 
		bool m_isGroupName;
		bool m_isDisabled;
		ImageIndex m_imageIndex;		
};
