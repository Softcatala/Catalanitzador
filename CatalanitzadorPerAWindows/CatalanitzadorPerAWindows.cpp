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

#include "stdafx.h"
#include "CatalanitzadorPerAWindows.h"
#include "PropertyPageUI.h"
#include "InstallPropertyPageUI.h"
#include "FinishPropertyPageUI.h"
#include "PropertySheetUI.h"
#include "ApplicationsPropertyPageUI.h"
#include "WelcomePropertyPageUI.h"
#include "OSVersion.h"
#include "Actions.h"
#include "Serializer.h"

void CreateWizard(HINSTANCE hInstance);
bool SupportedOS();
void InitLog ();

LogFile g_log;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	InitLog ();
	if (SupportedOS() == true)
	{
		CreateWizard(hInstance);
	}
	g_log.Close();
	return TRUE;
}

void InitLog()
{	
	g_log.CreateLog(L"CatalanitzadorPerAWindows.log");
	
	wchar_t szOSInfo [2048];
	OSVersion::GetLogInfo (szOSInfo, sizeof (szOSInfo));
	g_log.Log (szOSInfo);
}

bool SupportedOS()
{
	if (OSVersion::GetVersion() == Windows2000 || OSVersion::IsWindows64Bits ())
	{
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];

		g_log.Log (L"Show unsupported OS dialog");
		LoadString(GetModuleHandle(NULL), IDS_NOTSUPPORTEDOS, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
		MessageBox(NULL, szMessage, szCaption, MB_OK | MB_ICONINFORMATION);
		return false;
	}
	return true;
}

void CreateWizard(HINSTANCE hInstance)
{
	PropertySheetUI sheet;
	WelcomePropertyPageUI welcome;
	ApplicationsPropertyPageUI applications;
	InstallPropertyPageUI install;
	FinishPropertyPageUI finish;
	Actions actions;
	Serializer serializer;
	BOOL bSendStats = TRUE;

	welcome.setParent(&sheet);
	welcome.setPageButtons(NextButton);
	welcome.createPage(hInstance, IDD_WELCOME, NULL);
	welcome.SetSendStats(&bSendStats);
	sheet.addPage(&welcome);

	applications.createPage(hInstance, IDD_APPLICATIONS, NULL);
	applications.setParent(&sheet);
	applications.setPageButtons(NextBackButtons);
	vector <Action *> acts = actions.GetActions();
	applications.SetActions(&acts);	
	sheet.addPage(&applications);

	install.setParent(&sheet);
	install.setPageButtons(CancelButton);
	install.SetActions(&acts);
	install.SetSerializer(&serializer);
	install.createPage(hInstance, IDD_INSTALL, NULL);
	sheet.addPage(&install);

	finish.SetSerializer(&serializer);
	finish.setParent(&sheet);
	finish.SetActions(&acts);
	finish.setPageButtons(FinishButton);
	finish.SetSendStats(&bSendStats);
	finish.createPage(hInstance, IDD_FINISH, NULL);	
	sheet.addPage(&finish);

	sheet.runModal(hInstance, NULL, NULL);
}
