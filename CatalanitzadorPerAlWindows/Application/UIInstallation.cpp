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

#include "UIInstallation.h"
#include "ApplicationSheetUI.h"
#include "PropertyPageUI.h"
#include "InstallPropertyPageUI.h"
#include "FinishPropertyPageUI.h"
#include "PropertySheetUI.h"
#include "ApplicationsPropertyPageUI.h"
#include "IApplicationsPropertyPageUI.h"
#include "WelcomePropertyPageUI.h"
#include "ApplicationsModel.h"
#include "FinishModel.h"
#include "InstallModel.h"
#include "WelcomeModel.h"


void UIInstallation::Run(HINSTANCE  hInstance, ApplicationExecutor& applicationExecutor)
{	
	ApplicationSheetUI sheet;
	WelcomeModel welcomeModel(&applicationExecutor);
	WelcomePropertyPageUI welcome(&welcomeModel);
	ApplicationsModel applicationModel(&applicationExecutor);
	ApplicationsPropertyPageUI applications(&applicationModel);
	InstallModel installModel(&applicationExecutor);
	InstallPropertyPageUI install(&installModel);
	FinishModel finishModel(&applicationExecutor);
	FinishPropertyPageUI finish(&finishModel);
		
	welcome.setParent(&sheet);
	welcome.setPageButtons(NextButton);
	welcome.createPage(hInstance, IDD_WELCOME, IDD_WELCOME_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_WELCOME));
	sheet.addPage(&welcome);
	
	applicationModel.SetView((IApplicationsPropertyPageUI *)&applications);	
	applications.createPage(hInstance, IDD_APPLICATIONS, IDD_APPLICATIONS_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_APPLICATIONS));
	applications.setParent(&sheet);
	applications.setPageButtons(NextBackButtons);
	sheet.addPage(&applications);

	install.setParent(&sheet);
	install.setPageButtons(CancelButtonOnly);
	install.StartSlideShowUnpack();
	install.createPage(hInstance, IDD_INSTALL, IDD_INSTALL_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_INSTALL));
	sheet.addPage(&install);

	finish.setParent(&sheet);
	finish.setPageButtons(FinishButtonOnly);
	finish.createPage(hInstance, IDD_FINISH, IDD_FINISH_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_FINISH));
	sheet.addPage(&finish);

	sheet.runModal(hInstance, NULL, (LPWSTR)IDS_WIZARD_TITLE);
}

