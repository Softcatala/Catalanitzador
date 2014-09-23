/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "ApplicationExecutor.h"

using namespace std;

class InstallModel
{
public:
		InstallModel(ApplicationExecutor* applicationExecutor) {m_applicationExecutor = applicationExecutor; }

		void PrepareStart()
		{
			m_applicationExecutor->PrepareStart();
		}
		
		void Start(NotifyExecutionStarts notifyExecutionStarts, ProgressStatus downloadStatus, NotifyDownloadError notifyDownloadError, 
			NotifyDownloadCompleted notifyDownloadCompleted, NotifyExecutionCompleted notifyExecutionCompleted, void* data)
		{
			m_applicationExecutor->Start(notifyExecutionStarts, downloadStatus, notifyDownloadError, notifyDownloadCompleted, notifyExecutionCompleted, data);
		}

		int GetTotalSteps() { return m_applicationExecutor->GetTotalSteps(); }

private:

		ApplicationExecutor* m_applicationExecutor;
};
