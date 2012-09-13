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

#include <stdafx.h>
#include "Configuration.h"
#include "ConfigurationInstance.h"
#include "ConfigurationRemoteEmbedded.h"

static bool g_bInit = false;
static Configuration* g_pConfiguration;

void ConfigurationInstance::Init()
{
	g_pConfiguration = new Configuration();
	ConfigurationRemoteEmbedded configurationEmbedeed;
	configurationEmbedeed.Load();
	g_pConfiguration->SetRemote(configurationEmbedeed.GetConfiguration());
	g_bInit = true;
}

Configuration& ConfigurationInstance::Get()
{
	if (g_bInit == false)
		Init();

	return *g_pConfiguration;
}

void ConfigurationInstance::Reset()
{
	g_bInit = false;
}

void ConfigurationInstance::Set(Configuration configuration)
{
	g_pConfiguration = &configuration;
}

