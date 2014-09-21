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
 
#pragma once

#include "Action.h"
#include <string>
#include <vector>

#include "Serializer.h"
#include "ApplicationExecutor.h"

using namespace std;

class FinishModel
{
public:			
		FinishModel();
		FinishModel(ApplicationExecutor* applicationExecutor);

		void SetActionsForUT(vector <Action *> * value) {m_actionsForUT =  value; }
		vector <Action *> * GetActions();

		void SerializeOptionsSendStatistics();
		void WaitForStatisticsToCompleteOrTimeOut() { m_applicationExecutor->WaitForStatisticsToCompleteOrTimeOut(); }

		double GetCompletionPercentage();
		bool HasErrors();
		bool IsRebootNeed();
		void Reboot();
		void OpenTwitter();
		void OpenFacebook();
		void OpenGooglePlus();
		void OpenMailTo();

protected:
		virtual void _shellExecuteURL(wstring url);

		bool m_openTwitter;
		bool m_openFacebook;
		bool m_openGooglePlus;

private:

		void _commonConstructor();
		void _calculateIndicatorsForProgressBar();
		void _setSocialOptions();
		
		ApplicationExecutor* m_applicationExecutor;
		vector <Action *> * m_actionsForUT;
		
		float m_completionPercentage;
		bool m_errors;
};
