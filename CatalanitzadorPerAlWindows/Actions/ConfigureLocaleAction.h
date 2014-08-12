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

#include "Action.h"
#include "IRunner.h"
#include "IRegistry.h"
#include "IOSVersion.h"

class ConfigureLocaleAction : public Action
{
public:
		ConfigureLocaleAction(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner);
		~ConfigureLocaleAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return ConfigureLocaleActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupWindows;}
		virtual bool IsDownloadNeed() {return false;}
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		bool IsCatalanLocaleActive();

protected:

		void _userLocaleFromLanguageProfileOptOut();

private:		

		wchar_t m_szCfgFile[MAX_PATH];
		IRegistry* m_registry;
		IRunner* m_runner;
		IOSVersion* m_OSVersion;
};

