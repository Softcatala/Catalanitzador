/* 
 * Copyright (C) 2012 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

#include "ConfigurationRemote.h"
#include "TriBool.h"
#include "IOSVersion.h"
#include "OSVersion.h"
#include "Version.h"
#include "RemoteURLs.h"
#include "ApplicationVersion.h"

#define MIN_VRESOLUTION_FOR_AERO 768

class Configuration
{
	public:
			Configuration() :m_version(STRING_VERSION)
			{
				m_OSVersion = new OSVersion();
				m_downloadConfiguration = true;
				SetDownloadConfigurationUrl(REMOTE_CONFIGURATION_URL);
			}

			Configuration(IOSVersion* version) :m_version(STRING_VERSION)
			{
				m_OSVersion = version;
				m_downloadConfiguration = true;
				SetDownloadConfigurationUrl(REMOTE_CONFIGURATION_URL);
			}

			ConfigurationRemote& GetRemote() { return m_remote; }
			void SetRemote(ConfigurationRemote remote) {m_remote = remote;}

			// Application Configuration	
			void SetAeroEnabled(bool bUseAero) 
			{ 
				m_useAero = bUseAero; 
			}

			bool GetAeroEnabled()
			{
				if (m_useAero.IsUndefined())
				{
					if (m_OSVersion->GetVersion() == WindowsXP)
					{
						return false;
					}					
					return GetSystemMetricsYScreen() >= MIN_VRESOLUTION_FOR_AERO;
				}
				return m_useAero == true;
			}

			ApplicationVersion& GetVersion()
			{
				return m_version;
			}

			void SetVersion(ApplicationVersion version)
			{
				m_version = version;
			}

			bool GetDownloadConfiguration()
			{
				return m_downloadConfiguration;
			}

			void SetDownloadConfiguration(bool downloadConfiguration)
			{
				m_downloadConfiguration = downloadConfiguration;
			}

			wstring GetDownloadConfigurationUrl()
			{
				return m_downloadConfigurationUrl;
			}

			void SetDownloadConfigurationUrl(wstring downloadConfigurationUrl)
			{
				m_downloadConfigurationUrl = downloadConfigurationUrl;
			}

	protected:

			virtual int GetSystemMetricsYScreen() { return GetSystemMetrics(SM_CYSCREEN); }

	private:

			ConfigurationRemote m_remote;
			TriBool m_useAero;
			IOSVersion* m_OSVersion;
			ApplicationVersion m_version;
			bool m_downloadConfiguration;
			wstring m_downloadConfigurationUrl;
};
