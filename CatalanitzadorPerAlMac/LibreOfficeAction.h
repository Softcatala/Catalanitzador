/*
 * Copyright (C) 2018 Jordi Mas i Hernàndez <jmas@softcatala.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#pragma once

#include "Action.h"

enum LibreOfficeType
{
	LibreOffice,
	LibreOfficeVanilla,
	LibreOfficeNotInstalled,
	LibreOfficeTypeUnknown,
};

class LibreOfficeAction: public Action
{
public:
	
	LibreOfficeAction();
	~LibreOfficeAction();
	
	virtual const char* GetName() {return "Instal·la paquet d'idioma i diccionaris al LibreOffice";}
	
	virtual const char* GetDescription()  {return "Paquet d'idioma i diccionaris en català per al LibreOffice";}
	
	virtual ActionID GetID() const {return LibreOfficeActionID;}
	
	virtual bool IsNeed();
	virtual void Execute();
	virtual const char* GetVersion();
	virtual void CheckPrerequirements(Action * action);

	bool IsApplicationRunning();

private:
	string m_version;
	NSString* m_filename;
	LibreOfficeType m_installDirectory;
	
	bool _download();
	void _attach();
	void _deattach();
	void _tar();
	string _getVersionForDownload();
	void _executeProgram(NSString* program, NSArray* arguments);
	bool _isInstalled();
	bool _isLanguagePackInstalled();
	LibreOfficeType _getLibreOfficeType();
	NSString* _getInstallDirectory();
};


