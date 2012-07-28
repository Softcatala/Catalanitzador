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

#include "Configuration.h"
#include "ConfigurationEmbedded.h"

static bool g_bInit = false;
static Configuration g_configuration;

class _APICALL ConfigurationInstance
{
public:	
		static void Init()
		{
			if (g_bInit)
				return;

			ConfigurationEmbedded configurationEmbedeed;
			configurationEmbedeed.Load();
			g_configuration = configurationEmbedeed.GetConfiguration();
			g_bInit = true;
		}
public:

		static Configuration Get() { return g_configuration;}
		static void Set(Configuration configuration) {g_configuration = configuration;}
};