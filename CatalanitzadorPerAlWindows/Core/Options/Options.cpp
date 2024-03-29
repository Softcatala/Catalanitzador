﻿/* 
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


#include "stdafx.h"
#include "Options.h"

void Options::Serialize(ostream* stream)
{
	*stream << "\t<options>\n";

	for (unsigned int i = 0; i < m_options.size(); i++)
	{
		m_options.at(i).Serialize(stream);
	}

	*stream << "\t</options>\n";
}

void Options::Set(Option option)
{
	for (unsigned int i= 0; i < m_options.size(); i++)
	{		
		if (m_options[i].GetOptionId() == option.GetOptionId())
		{
			Option newOption(option.GetOptionId(), option.GetValue());
			m_options[i] = option;
			return;
		}
	}

	m_options.push_back(option);
}