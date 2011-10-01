// CatalanitzadorPerAWindows.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CatalanitzadorPerAWindows.h"
#include "PropertyPageUI.h"
#include "PropertySheetUI.h"
#include "ApplicationsPropertyPageUI.h"

void CreateWizard (HINSTANCE hInstance);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CreateWizard (hInstance);
	
	return TRUE;
}

void CreateWizard (HINSTANCE hInstance)
{
	PropertySheetUI sheet;
	PropertyPageUI welcome;
	ApplicationsPropertyPageUI applications;
	PropertyPageUI finish;
	PropertyPageUI install;
	
	welcome.createPage(hInstance, IDD_WELCOME, NULL);
	sheet.addPage(&welcome);

	applications.createPage(hInstance, IDD_APPLICATIONS, NULL);
	sheet.addPage(&applications);

	install.createPage(hInstance, IDD_INSTALL, NULL);
	sheet.addPage(&install);

	finish.createPage(hInstance, IDD_FINISH, NULL);	
	sheet.addPage(&finish);

	sheet.runModal (hInstance, NULL, NULL);
}

