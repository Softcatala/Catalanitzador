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
	InstallPropertyPageUI install;
	FinishPropertyPageUI finish;	
	vector <Action *> selectedActions;
	
	welcome.setParent (&sheet);
	welcome.setPageButtons (NextButton);
	welcome.createPage(hInstance, IDD_WELCOME, NULL);
	sheet.addPage(&welcome);

	applications.createPage(hInstance, IDD_APPLICATIONS, NULL);
	applications.setParent (&sheet);
	applications.setPageButtons (NextBackButtons);
	applications.SetSelectedActions (&selectedActions);
	sheet.addPage(&applications);

	install.setParent (&sheet);
	install.setParent (&sheet);
	install.setPageButtons (NextButton);
	install.SetSelectedActions (&selectedActions);
	install.createPage(hInstance, IDD_INSTALL, NULL);
	sheet.addPage(&install);

	finish.setParent (&sheet);
	finish.setPageButtons (FinishButton);
	finish.createPage(hInstance, IDD_FINISH, NULL);	
	sheet.addPage(&finish);

	sheet.runModal (hInstance, NULL, NULL);
}

