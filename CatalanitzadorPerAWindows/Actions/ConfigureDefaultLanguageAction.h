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

#include "Action.h"
#include "Runner.h"
#include "IRegistry.h"
#include "IOSVersion.h"
#include "OSVersion.h"

class _APICALL ConfigureDefaultLanguageAction : public Action
{
public:
		ConfigureDefaultLanguageAction(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner);
		~ConfigureDefaultLanguageAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() { return ConfigureDefaultLanguage;};
		virtual bool IsDownloadNeed() {return false;}
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual void CheckPrerequirements(Action * action);		
		virtual bool IsRebootNeed();

protected:

		bool _isCatalanKeyboardActive();
		bool _hasSpanishKeyboard();
		void _makeCatalanActiveKeyboard();

private:

		wchar_t szCfgFile[MAX_PATH];
		IRunner* m_runner;
		IRegistry* m_registry;
		IOSVersion* m_OSVersion;
};

