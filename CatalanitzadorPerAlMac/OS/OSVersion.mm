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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


#include "OSVersion.h"

OSVersion::OSVersion()
{
    _readVersions();
}

// See: http://cocoadev.com/wiki/DeterminingOSVersion
void OSVersion::_readVersions()
{
    Gestalt(gestaltSystemVersionMajor, &m_major);
    Gestalt(gestaltSystemVersionMinor, &m_minor);
    Gestalt(gestaltSystemVersionBugFix, &m_bugfix);
}

#define MAC_SYSTEM 1
#define MACHINE_BITS 64

void OSVersion::Serialize(ostream* stream)
{
    char szText[2048];
    char szAsciiName[2048];
    
    snprintf(szAsciiName, sizeof(szAsciiName), "Mac OS %u.%u.%u", m_major, m_minor, m_bugfix);
    snprintf(szText, sizeof(szText), "\t<operating OSMajorVersion='%u' OSMinorVersion='%u' SPMajorVersion='%u' SPMinorVersion='%u' SuiteMask='%u' ProductType='%u' Bits='%u' Name='%s' System='%u'/>\r\n",
            m_major,
            m_minor,
            m_bugfix,
            0,
            0,
            0,
            MACHINE_BITS,
            szAsciiName,
            MAC_SYSTEM);
    
    *stream << szText;
}
