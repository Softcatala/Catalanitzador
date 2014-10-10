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

#include "stdafx.h"
#include "ApplicationExecutor.h"
#include "WindowsXPDialogCanceler.h"
#include "Inspectors.h"
#include "ConfigurationInstance.h"

#define SEPARATOR L"."
#define SEPARATOR_LEN 1

ApplicationExecutor::ApplicationExecutor() : m_actions(&m_downloadManager)
{
	m_serializer = NULL;
	m_uploadStatistics = NULL;
	m_sentStats = false;
	m_errors = false;
	m_dialectVariantSet = false;
	m_selActions = 0;
	m_downloads = 0;
	m_selActions = 0;

#if _DEBUG
	m_doSystemRestore = false;
	m_sendStats = false;
#else
	m_doSystemRestore = true;
	m_sendStats = true;
#endif

}

ApplicationExecutor::~ApplicationExecutor()
{
	if (m_uploadStatistics != NULL)
	{
		delete m_uploadStatistics;
		m_uploadStatistics = NULL;
	}

#if !DEVELOPMENT_VERSION
	if (m_xmlFile.size() > 0)
	{
		DeleteFile(m_xmlFile.c_str());
	}
#endif
}

void ApplicationExecutor::_systemRestoreStarts(NotifyExecutionCompleted notifyExecutionCompleted, void* data)
{
	if (m_selActions > 0 && m_doSystemRestore)
	{
		m_systemRestore.Start();
		m_systemRestore.Wait();
		
		NotifyActionData notifyActionData;
		notifyActionData.pThis = data;
		notifyActionData.action = NULL;
		notifyExecutionCompleted(&notifyActionData);
	}
}

void ApplicationExecutor::_systemRestoreEnds()
{
	if (m_selActions > 0 && m_doSystemRestore)
	{
		m_systemRestore.End();
	}
}

#define SLEEP_TIME 50 // miliseconds
#define TIME_TO_READ_COUNTER 10 * 1000 / SLEEP_TIME // 10 seconds
#define MAX_WAIT_TIME 45 * 60 // 45 minutes

void ApplicationExecutor::_waitExecutionComplete(Action* action)
{
	LARGE_INTEGER li;
	INT64 start;
	int cnt = 0;
	double frequency = 0.0;

	start = GetTickCount();
	
	if (QueryPerformanceFrequency(&li))
	{
		 frequency = double(li.QuadPart);
		 QueryPerformanceCounter(&li);
		 start = li.QuadPart;
	}
	else
	{
		g_log.Log(L"ApplicationExecutor::_waitExecutionComplete. QueryPerformanceFrequency failed");
	}

	while (true)
	{
		ActionStatus status = action->GetStatus();

		if (status == Successful || status == FinishedWithError)
			break;

		Window::ProcessMessages();
		Sleep(SLEEP_TIME);
		Window::ProcessMessages();
		cnt++;

		if (frequency > 0 && cnt > TIME_TO_READ_COUNTER)
		{
			double diff;
			
			QueryPerformanceCounter(&li);
			diff = double(li.QuadPart - start) / frequency;

			if (diff > MAX_WAIT_TIME)
			{
				g_log.Log(L"ApplicationExecutor::_waitExecutionComplete. Timeout");
				break;
			}
			cnt = 0;
		}
	}
}

bool ApplicationExecutor::_download(Action* action, ProgressStatus _downloadStatus, NotifyDownloadError notifyDownloadError,
			NotifyDownloadCompleted notifyDownloadCompleted, NotifyActionData* data)
{
	if (action->IsDownloadNeed() == false)
		return true;

	bool bDownload = true;
	bool bError = false;
	
	while (bDownload)
	{
		if (action->Download(_downloadStatus, data) == false)
		{
			bool skipDownload = notifyDownloadError(data);

			if (skipDownload)
			{
				bDownload = false;
				bError = true;
			}
		}
		else
		{
			bDownload = false;
		}
	}
	
	notifyDownloadCompleted(data);
	return bError == false;
}


void ApplicationExecutor::_serializeInspectors()
{
	Inspectors inspectors;
	inspectors.Execute();
	inspectors.Serialize(m_serializer->GetStream());
}

void ApplicationExecutor::Start(NotifyExecutionStarts notifyExecutionStarts,
						 ProgressStatus downloadStatus,
						 NotifyDownloadError notifyDownloadError,
						 NotifyDownloadCompleted notifyDownloadCompleted,
						 NotifyExecutionCompleted notifyExecutionCompleted,
						 void* data)
{	
	Action* action;
	WindowsXPDialogCanceler dialogCanceler;

	_systemRestoreStarts(notifyExecutionCompleted, data);
	dialogCanceler.Start();	
	m_serializer->StartAction();

	for (unsigned int i = 0; i < GetActions()->size(); i++)
	{
		action = GetActions()->at(i);

		if (action->GetStatus() != Selected)
		{
			m_serializer->Serialize(action);
			continue;
		}

		NotifyActionData notifyActionData;
		notifyActionData.pThis = data;
		notifyActionData.action = action;

		if (_download(action, downloadStatus, notifyDownloadError, notifyDownloadCompleted, &notifyActionData) == true)
		{
			notifyExecutionStarts(&notifyActionData);
			action->Execute();
			_waitExecutionComplete(action);
			notifyExecutionCompleted(&notifyActionData);
		}
		else
		{
			action->SetStatus(NotSelected);
		}
		
		m_serializer->Serialize(action);
	}

	dialogCanceler.Stop();
	m_serializer->EndAction();
	_serializeInspectors();	
	_systemRestoreEnds();
}

void ApplicationExecutor::PrepareStart()
{
	_updateSelectedActionsCounts();
}


void ApplicationExecutor::_updateSelectedActionsCounts()
{
	Action* action;

	m_selActions = m_downloads = 0;

	for (unsigned int i = 0; i < GetActions()->size(); i++)
	{
		action = GetActions()->at(i);
		if (action->GetStatus() == Selected)
		{
			m_selActions++;
			if (action->IsDownloadNeed())
			{
				m_downloads++;
			}
		}
	}

	m_totalSteps = m_downloads + m_selActions;

	if (m_selActions > 0 && m_doSystemRestore)
		m_totalSteps++;
}

ActionStatus ApplicationExecutor::_getDefaultStatusForAction(ActionID actionID)
{
	vector <ConfigurationFileActionDownloads> configurationFileDownloads = ConfigurationInstance::Get().GetRemote().GetFileActionsDownloads();
	ActionStatus defaultStatus = Selected;

	for (unsigned int i = 0; i < configurationFileDownloads.size(); i++)
	{
		if (configurationFileDownloads.at(i).GetActionID() == actionID)
		{
			if (configurationFileDownloads.at(i).GetActionDefaultStatusHasValue())
			{
				defaultStatus = configurationFileDownloads.at(i).GetActionDefaultStatus();
				break;
			}
		}
	}

	return defaultStatus;
}


void ApplicationExecutor::SelectDefaultActions()
{
	for (unsigned int i = 0; i < GetActions()->size(); i++)
	{		
		Action* action = GetActions()->at(i);

		if (action->IsVisible() == false)
			continue;

		bool needed = action->IsNeed();
		
		if (needed)
		{				
			action->SetStatus(_getDefaultStatusForAction(action->GetID()));
		}
	}

	//_processDependantItems(); // TODO: Do it
}


void ApplicationExecutor::_saveToDisk()
{
	wchar_t szXML[MAX_PATH];

	GetTempPath(MAX_PATH, szXML);	
	wcscat_s(szXML, L"statistics.xml");
	m_xmlFile = szXML;
	m_serializer->SaveToFile(m_xmlFile);
}

void ApplicationExecutor::_serializeOptions()
{
	SetOption(Option(OptionSystemRestore, m_doSystemRestore));
	SetOption(Option(OptionDialect, m_dialectVariantSet));

	m_options.Serialize(m_serializer->GetStream());
	m_serializer->CloseHeader();
}


void ApplicationExecutor::_sendStatistics()
{
	if (m_sendStats)
	{
		m_uploadStatistics = new UploadStatisticsThread(&m_httpFormInet, m_serializer, m_errors);
		m_uploadStatistics->Start();
	}

	_saveToDisk();
}

void ApplicationExecutor::SerializeOptionsSendStatistics()
{
	if (m_sentStats == true)
		return;

	_serializeOptions();
	_sendStatistics();
	m_sentStats = true;
}


void ApplicationExecutor::WaitForStatisticsToCompleteOrTimeOut()
{
	if (m_uploadStatistics!= NULL)
		m_uploadStatistics->Wait();
}

void ApplicationExecutor::SetOption(Option option)
{	
	m_options.Set(option);
}


void ApplicationExecutor::SetDialectVariant(bool dialectVariant)
{
	m_dialectVariantSet = dialectVariant;
	for (unsigned int i = 0; i < GetActions()->size (); i++)
	{
		Action* action = GetActions()->at(i);
		action->SetUseDialectalVariant(dialectVariant);
	}
}

