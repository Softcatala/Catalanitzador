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

#include <string>
#include <vector>

#include "Actions.h"

using namespace std;

class CommandLine
{
	public:
			CommandLine(Actions* pActions);
			void Process(wstring commandLine);
			bool GetRunningCheck() const {return m_bRunningCheck;}
			bool GetSilent() const {return m_bSilent;}
			
	private:

			Actions* m_pActions;
			void _createCatalanitzadorUpdateAction(wstring version);
			bool _readCommandLineParameter(wchar_t** pcommandline, wstring& parameter);
			
			bool m_bRunningCheck;
			bool m_bSilent;
			int NORUNNING_PARAMETER_LEN;
			int VERSION_PARAMETER_LEN;
			int USEAEROLOOK_PARAMETER_LEN;
			int USECLASSICLOOK_PARAMETER_LEN;
			int NOCONFIGURATIONDOWNLOAD_PARAMETER_LEN;
			int CONFIGURATIONDOWNLOADURL_PARAMETER_LEN;
			int HELP_PARAMETER_LEN;
			int SILENT_PARAMETER_LEN;
};
