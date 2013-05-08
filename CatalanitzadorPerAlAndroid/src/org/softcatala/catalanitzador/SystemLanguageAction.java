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


package org.softcatala.catalanitzador;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Locale;
import android.content.res.Configuration;

public class SystemLanguageAction {	
	
	boolean isNeeded()
	{
		return true;
	}	
	
	// TODO: We need to figure out how to see if the Catalan locale is installed
	void CheckPrerequirements()
	{
	
	}
	
	void Execute()
	{
		setLocale();
	}
	
	 private void setLocale()
	 {
	        try
	        {
	        	// TODO: Is this the right locale?
		        Locale locale = new Locale("ca_ES");
	
		        Class amnClass = Class.forName("android.app.ActivityManagerNative");
		        Object amn = null;
		        Configuration config = null;	
		        
		        Method methodGetDefault = amnClass.getMethod("getDefault");
		        methodGetDefault.setAccessible(true);
		        amn = methodGetDefault.invoke(amnClass);	

		        Method methodGetConfiguration = amnClass.getMethod("getConfiguration");
		        methodGetConfiguration.setAccessible(true);
		        config = (Configuration) methodGetConfiguration.invoke(amn);	
 
		        Class configClass = config.getClass();
		        Field f = configClass.getField("userSetLocale");
		        f.setBoolean(config, true);
 
		        config.locale = locale;
		        Method methodUpdateConfiguration = amnClass.getMethod("updateConfiguration", Configuration.class);
		        methodUpdateConfiguration.setAccessible(true);
		        methodUpdateConfiguration.invoke(amn, config);
	        }
	        
	        catch(Exception e)
	        {
	            
	        }
	    }


}
