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

import android.app.Activity;
import android.content.res.Configuration;
import android.util.Log;
import android.content.res.Resources;

// References
// 	http://grepcode.com/file/repository.grepcode.com/java/ext/com.google.android/android-apps/1.6_r2/com/android/settings/LocalePicker.java
//
//
public class SystemLanguageAction {	
	
	private Activity _activity;
	private String _locateToSet;
	private String CatalanLocale1 = "ca";
	private String CatalanLocale2 = "ca_ES";
	private String LogString = "Catalanitzador";

	SystemLanguageAction(Activity activity)
	{
		_activity = activity;
		readCatalanLocaleAvailable();
	}	
	
	boolean isNeeded()
	{
		boolean needed = false;
		
		if (isCatalanLocaleAvailable())
		{
			needed = IsCatalanCurrentDefaultLocale() == false;
		}		
		
		Log.i(LogString, "SystemLanguageAction.isNeeded. Returns: " + needed);
		return needed;
	}	
	
	void Execute()
	{
		setLocale();		
	}
	
	private boolean IsCatalanCurrentDefaultLocale()
	{
		String locale;
		boolean isDefault;
		
		locale = Locale.getDefault().getLanguage();
		isDefault = locale.equals(CatalanLocale1) || locale.equals(CatalanLocale2); 
		Log.i(LogString, "SystemLanguageAction.IsCatalanCurrenttDefaultLocale. Returns: " + isDefault);	
		return isDefault;
	}
	
	private void readCatalanLocaleAvailable()
	{		
		String[] locales = _activity.getAssets().getLocales();		
		
		_locateToSet = "";		
		for (int i = 0; i < locales.length; i++)
		{
			String locale = locales[i];
			Log.i(LogString, "SystemLanguageAction.readCatalanLocaleAvailable. Locale: " + locale);
			
			if (locale.equals(CatalanLocale1))
			{
				_locateToSet = CatalanLocale1;
				break;
			}
			
			if (locale.equals(CatalanLocale2))
			{
				_locateToSet = CatalanLocale2;
				break;
			}
		}
		
		Log.i(LogString, "SystemLanguageAction.readCatalanLocaleAvailable. Locale to set: " + _locateToSet);		
	}
	
	private boolean isCatalanLocaleAvailable()
	{	
		boolean available;
		
		available = _locateToSet.equals("") == false;
		Log.i(LogString, "SystemLanguageAction.isCatalanLocaleAvailable. Returns: " + available);
		return available;
	}
	
	private void setLocale()
	{
		try
		{			 
			Locale locale = new Locale(_locateToSet);
		
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
		    Log.i(LogString, "SystemLanguageAction.setLocale. Locale: " + _locateToSet);
		}
	        
		catch(Exception e)
		{
			Log.i(LogString, "SystemLanguageAction.setLocale. Exception: " + e);    
		}
	}

}
