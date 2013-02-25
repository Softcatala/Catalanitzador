/*
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "FirefoxAcceptLanguages.h"

class FirefoxAction: public Action
{
public:
    
    FirefoxAction();
    ~FirefoxAction();
    
    virtual const char* GetName() {return "Configura la llengua de navegació del Firefox";}
    
    virtual const char* GetDescription()  {return "La llengua de navegació dóna a conèixer la nostra preferència lingüística al món. Quan un lloc web està disponible en diverses llengües, la llengua de navegació determina en quina llengua es presentarà a l'usuari.";}

    virtual ActionID GetID() const {return Firefox;}
    
    virtual bool IsNeed();
    virtual void Execute();
    virtual const char* GetVersion();
    
    bool IsApplicationRunning();
    
private:

    bool _isInstalled();
    string _getInstalledLang();
    FirefoxAcceptLanguages* _getAcceptLanguages();
    
    FirefoxAcceptLanguages* m_acceptLanguages;
    string m_version;
};

