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

#include "stdafx.h"
#include "ActionExecution.h"
#include "Runner.h"
#include <algorithm>

void ActionExecution::_addExecutionProcess(ExecutionProcess process)
{
	m_processes.push_back(process);
}

vector <DWORD> ActionExecution::_getProcessIDs(wstring processName, bool is64Bits)
{
	Runner runner;
	return runner.GetProcessID(processName);
}

ExecutionProcess ActionExecution::GetExecutingProcess()
{
	for (unsigned int i = 0; i < m_processes.size(); i++)
	{
		ExecutionProcess process = m_processes.at(i);
		
		if (process.Is64Bits())
		{
			wstring name = process.GetName();
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);

			vector <wstring> names = m_x64BitsProcess.GetRunningProcessesNames();
			for (unsigned int i = 0; i < names.size(); i++)
			{
				wstring current = names[i];
				std::transform(current.begin(), current.end(), current.begin(), ::tolower);				
				if (current.compare(name) == 0)
				{
					return process;
				}
			}
		}
		else
		{
			if (_getProcessIDs(process.GetName(), process.Is64Bits()).size() != 0)
				return process;
		}
	}

	return m_processEmpty;
}