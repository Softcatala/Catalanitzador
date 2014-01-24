/* 
 * Copyright (C) 2011-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "MSOfficeLPIAction.h"
#include "MSOfficeFactory.h"
#include "OSVersion.h"

MSOfficeLPIAction::MSOfficeLPIAction(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) :
	Action(downloadManager), m_multipleDownloads(downloadManager)
{
	m_OSVersion = OSVersion;
	m_registry = registry;
	m_runner = runner;
	m_executionStep = ExecutionStepNone;	
	m_executionStepIndex = -1;
	m_OutLookHotmailConnector = NULL;
}

MSOfficeLPIAction::~MSOfficeLPIAction()
{
	if (m_OutLookHotmailConnector)
		delete m_OutLookHotmailConnector;
}

wchar_t* MSOfficeLPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_MSOFFICEACTION_NAME, szName);
}

wchar_t* MSOfficeLPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_MSOFFICEACTION_DESCRIPTION, szDescription);
}

LPCWSTR MSOfficeLPIAction::GetLicenseID()
{	
	if (m_MSOffices.size() > 0)
	{
		switch (m_MSOffices.at(0).GetVersionEnum())
		{
			case MSOffice2003:
				return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2003);
			case MSOffice2007:
				return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2007);
			case MSOffice2010:
			case MSOffice2010_64:
				return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2010);
			case MSOffice2013:
			case MSOffice2013_64:
				return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2013);
			default:
				break;
		}
	}
	return NULL;
}

const wchar_t* MSOfficeLPIAction::GetVersion()
{
	// Not expected to be called since we do our own serialization
	assert(false);
	return NULL;
}

bool MSOfficeLPIAction::IsNeed()
{
	bool bNeed;

	switch(GetStatus())
	{		
		case NotInstalled:
		case AlreadyApplied:
		case CannotBeApplied:
			bNeed = false;
			break;
		default:
			bNeed = true;
			break;
	}
	g_log.Log(L"MSOfficeLPIAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}


bool MSOfficeLPIAction::Download(ProgressStatus progress, void *data)
{
	for (unsigned int i = 0; i < m_MSOffices.size(); i++)
	{
		if (m_MSOffices[i].IsLangPackInstalled() == false)
			m_MSOffices.at(i).AddDownloads(m_multipleDownloads);

		// When download is requested the user has started the installation, let's set dialect at this point
		m_MSOffices.at(i).SetUseDialectalVariant(GetUseDialectalVariant());
	}

	if (_getOutLookHotmailConnector()->IsNeed())
	{
		ConfigurationFileActionDownload downloadVersion;
		wstring tempFile;

		_getOutLookHotmailConnector()->GetDownloadConfigurationAndTempFile(downloadVersion, tempFile);
		m_multipleDownloads.AddDownload(downloadVersion, tempFile);
	}	
	
	return m_multipleDownloads.Download(progress, data);
}

void MSOfficeLPIAction::Execute()
{
	if (m_executionStep == ExecutionStepNone)
	{
		m_executionStep = ExecutionStepMSOffice;
		m_executionStepIndex = 0;
	}
	else
	{
		m_executionStepIndex++;
	}

	if (m_executionStepIndex < m_MSOffices.size())
	{
		if (m_MSOffices[m_executionStepIndex].IsLangPackInstalled() == false)
		{
			m_MSOffices[m_executionStepIndex].Execute();
		}
	}
	else
	{
		m_executionStep = ExecutionStepOutLookConnector;
	}
	
	SetStatus(InProgress);
}

void MSOfficeLPIAction::Serialize(ostream* stream)
{
	for (unsigned int i = 0; i < m_MSOffices.size(); i++)
	{
		char szText[1024];
		string version;

		StringConversion::ToMultiByte(wstring(m_MSOffices.at(i).GetVersion()), version);
		sprintf_s(szText, "\t\t<action id='%u' version='%s' result='%u'/>\n",
			GetID(), version.c_str(), status);

		*stream << szText;
	}
}

ActionStatus MSOfficeLPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		switch (m_executionStep)
		{
			case ExecutionStepNone:
				assert(false);
				break;
			case ExecutionStepMSOffice:
			{
				Execute(); // Continue executing other instances of MSOffice installations
				return InProgress;
			}
			case ExecutionStepOutLookConnector:
				m_executionStep = ExecutionCompleted;
				if (_getOutLookHotmailConnector()->IsNeed())
				{
					_getOutLookHotmailConnector()->Execute(m_runner);
				}
				return InProgress;
			case ExecutionCompleted:
				break;
			default:
				assert(false);
				break;
		}

		ActionStatus allStatus = Successful;		
		for (unsigned int i = 0; i < m_MSOffices.size(); i++)
		{
			ActionStatus versionStatus = Successful;
			if (m_MSOffices.at(i).IsLangPackInstalled())
			{
				m_MSOffices.at(i).SetDefaultLanguage();
			}
			else
			{
				versionStatus = allStatus = FinishedWithError;
				m_MSOffices.at(i).DumpLogOnError();
			}		
			g_log.Log(L"MSOfficeLPIAction::GetStatus (%s) is '%s'", (wchar_t *) m_MSOffices.at(i).GetVersion(),
				versionStatus == Successful ? L"Successful" : L"FinishedWithError");
		}
		SetStatus(allStatus);
	}
	return status;
}


OutLookHotmailConnector* MSOfficeLPIAction::_getOutLookHotmailConnector()
{
	if (m_OutLookHotmailConnector == NULL)
	{
		bool MSOffice2010OrPrevious = false;

		for (unsigned int i = 0; i < m_MSOffices.size(); i++)
		{
			MSOfficeVersion version = m_MSOffices.at(i).GetVersionEnum();

			if (version== MSOffice2010_64 || version == MSOffice2010 || version == 2007 ||  version == 2003)
			{
				MSOffice2010OrPrevious = true;
				break;
			}
		}		
		m_OutLookHotmailConnector = new OutLookHotmailConnector(MSOffice2010OrPrevious, m_registry);
	}

	return m_OutLookHotmailConnector;
}

void MSOfficeLPIAction::CheckPrerequirements(Action * action) 
{
	m_MSOffices = MSOfficeFactory::GetInstalledOfficeInstances(m_OSVersion, m_registry, m_runner);

	if (m_MSOffices.size() == 0)
	{
		_setStatusNotInstalled();
		return;
	}

	bool alreadyApplied = true;
	for (unsigned int i = 0; i < m_MSOffices.size(); i++)
	{
		g_log.Log(L"MSOfficeLPIAction::CheckPrerequirements. MSOfficce %s application installed", (wchar_t *) m_MSOffices[i].GetVersion());
		if (m_MSOffices[i].IsLangPackInstalled() == false || m_MSOffices[i].IsDefaultLanguage() == false)
		{
			alreadyApplied = false;
		}
	}

	if (alreadyApplied)
	{
		SetStatus(AlreadyApplied);
	}
}
