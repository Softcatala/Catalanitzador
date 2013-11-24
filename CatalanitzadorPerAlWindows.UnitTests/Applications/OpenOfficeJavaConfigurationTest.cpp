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

#include "stdafx.h"
#include "Application.h"
#include "OpenOfficeTest.h"

using ::testing::StrCaseEq;

class OpenOfficeJavaConfigurationTest : public OpenOfficeJavaConfiguration
{
public:
		
		void _setSettingFile(wstring settingsFile)
		{
			m_settingsFile = settingsFile;
		}

		virtual wstring _getSettingFile()
		{
			if (m_settingsFile.empty())
				return OpenOfficeJavaConfiguration::_getSettingFile();

			wstring location;

			Application::GetExecutionLocation(location);
			location += L"OpenOffice.org\\";
			location += m_settingsFile;
			return location;
		}
		
private:
		wstring m_settingsFile;
};

TEST(OpenOfficeJavaConfigurationTest, _getSettingFile)
{	
	OpenOfficeJavaConfigurationTest openOfficeJava;	
	
	openOfficeJava.SetUserDirectory(L"directory\\");
	EXPECT_THAT(openOfficeJava._getSettingFile(), StrCaseEq(L"directory\\config\\javasettings_Windows_x86.xml"));
}

TEST(OpenOfficeJavaConfigurationTest, GetDefaultJavaVersion_Enabled16)
{	
	OpenOfficeJavaConfigurationTest openOfficeJava;	
	
	openOfficeJava._setSettingFile(L"javasettings_Windows_x86-EnabledJava16.xml");
	EXPECT_THAT(openOfficeJava.GetDefaultJavaVersion(), StrCaseEq(L"1.6.0_34"));
	EXPECT_TRUE(openOfficeJava.IsJavaEnabled());
}
