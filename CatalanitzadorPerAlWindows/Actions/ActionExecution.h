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

#include "ExecutionProcess.h"
#include "x64BitsProcess.h"

#include <vector>

using namespace std;

//
// This abstract class defines the action interface for actions that may have executing processes
//
class ActionExecution
{
public:
		// Some actions cannot be applied (e.g. changes in the configuration) if the application is running
		// This method returns true is there is any process that should be terminated for the action to be able to run
		virtual ExecutionProcess GetExecutingProcess();

		// Finish the execution of that should be terminated for the action to be able to run
		virtual void FinishExecution(ExecutionProcess process) = 0;

protected:

		void _addExecutionProcess(ExecutionProcess process);
		vector <DWORD> _getProcessIDs(wstring processName, bool canBe64Bits);

private:
		vector <ExecutionProcess> m_processes;
		ExecutionProcess m_processEmpty;
		x64BitsProcess m_x64BitsProcess;
};
