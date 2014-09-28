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
#include <functional>

#include "Actions.h"
#include "Serializer.h"
#include "SystemRestoreThread.h"
#include "UploadStatisticsThread.h"
#include "HttpFormInet.h"
#include "OptionID.h"
#include "Options.h"

using namespace std;


struct NotifyActionData
{
	void* pThis;
	Action* action;
};


typedef void (*NotifyExecutionStarts)(NotifyActionData *data);
typedef bool (*NotifyDownloadError)(NotifyActionData *data);
typedef void (*NotifyDownloadCompleted)(NotifyActionData *data);
typedef void (*NotifyExecutionCompleted)(NotifyActionData *data);


//
// This class encapsulates the whole work flow of the application
//
class ApplicationExecutor
{
	public:

			ApplicationExecutor();
			~ApplicationExecutor();

			/* Actions */
			void CheckPrerequirements() { m_actions.CheckPrerequirements();}
			void SelectDefaultActions();

			void PrepareStart();

			void Start(NotifyExecutionStarts notifyExecutionStarts, ProgressStatus downloadStatus, NotifyDownloadError notifyDownloadError, 
				NotifyDownloadCompleted notifyDownloadCompleted, NotifyExecutionCompleted notifyExecutionCompleted, void* data);

			void SerializeOptionsSendStatistics();
			void WaitForStatisticsToCompleteOrTimeOut();

			/* Getters */
			int GetTotalSteps() { return m_totalSteps; }
			vector <Action *> * GetActions() {return m_actions.GetActions(); }
			Actions* GetActionsObject() {return &m_actions; }
			bool GetSystemRestore() {return m_doSystemRestore; }
			bool GetSendStats() {return m_sendStats; }

			/* Setters */
			void SetOption(Option option);
			void SetSerializer(Serializer* serializer) { m_serializer = serializer; }
			void SetSendStats(bool sendStats) { m_sendStats = sendStats; }
			void SetSystemRestore(bool systemRestore) { m_doSystemRestore = systemRestore; }
			void SetDialectVariant(bool dialectVariant);

	private:

			ActionStatus _getDefaultStatusForAction(ActionID actionID);
			void _serializeOptions();

			bool _download(Action* action, ProgressStatus _downloadStatus, NotifyDownloadError notifyDownloadError,	
				NotifyDownloadCompleted notifyDownloadCompleted, NotifyActionData* data);

			void _updateSelectedActionsCounts();

			void _waitExecutionComplete(Action* action);
			void _serializeInspectors();
			void _systemRestoreStarts(NotifyExecutionCompleted notifyExecutionCompleted, void* data);
			void _systemRestoreEnds();
			
			void _sendStatistics();
			void _saveToDisk();

			SystemRestoreThread m_systemRestore;
			Serializer* m_serializer;
			Actions m_actions;
			DownloadManager m_downloadManager;
			UploadStatisticsThread * m_uploadStatistics;
			HttpFormInet m_httpFormInet;

			int m_selActions;
			bool m_doSystemRestore;
			int m_downloads;
			int m_totalSteps;

			wstring m_xmlFile;
			bool m_sentStats;
			bool m_sendStats;
			bool m_errors;
			bool m_dialectVariantSet;

			Options m_options;
};
