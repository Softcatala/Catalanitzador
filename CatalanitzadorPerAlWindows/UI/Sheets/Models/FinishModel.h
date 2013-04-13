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
#include <string>
#include <vector>

#include "Serializer.h"
#include "UploadStatisticsThread.h"
#include "HttpFormInet.h"

using namespace std;

class FinishModel
{
public:
		FinishModel();
		~FinishModel();

		void SetActions(vector <Action *> * value) {m_actions =  value; }
		void SetSerializer(Serializer* serializer) { m_serializer = serializer; }
		void SetSendStats(bool *pbSendStats) { m_pbSendStats = pbSendStats;}
		void SetShowSecDlg(bool *pbShowSecDlg) { m_pbShowSecDlg = pbShowSecDlg;}
		void SetSystemRestore(int *pSystemRestore) { m_pSystemRestore = pSystemRestore;}
		void SetDialectVariant(bool *dialectalVariant) { m_pbDialectalVariant = dialectalVariant;}

		double GetCompletionPercentage();
		bool HasErrors();
		bool IsRebootNeed();
		void Reboot();
		void SerializeOptionsSendStatistics();
		void WaitForStatisticsToCompleteOrTimeOut();
		void OpenTwitter();
		void OpenFacebook();
		void OpenGooglePlus();
		void OpenMailTo();

protected:
		virtual void _shellExecuteURL(wstring url);

private:

		void _sendStatistics();
		void _calculateIndicatorsForProgressBar();
		void _saveToDisk();
		void _serializeOptions();
		
		UploadStatisticsThread * m_uploadStatistics;
		vector <Action *> * m_actions;
		Serializer* m_serializer;
		HttpFormInet m_httpFormInet;

		bool* m_pbSendStats;
		wstring m_xmlFile;
		float m_completionPercentage;
		bool m_errors;

		int* m_pSystemRestore;
		bool* m_pbDialectalVariant;
		bool* m_pbShowSecDlg;
		bool m_sentStats;
		bool m_openTwitter;
		bool m_openFacebook;
		bool m_openGooglePlus;
};
