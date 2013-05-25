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
public class SystemLanguageAction extends Action {
	
	private Activity _activity;
	private String _localeToSet;
	private String CatalanLocale1 = "ca";
	private String CatalanLocale2 = "ca_ES";
	private String LogString = "Catalanitzador";
	
	public static final int JELLY_BEAN_MR1 = 17;
	
	SystemLanguageAction(Activity activity)
	{
		_activity = activity;
		readCatalanLocaleAvailable();
	}	
	
	public boolean isNeeded()
	{
		boolean needed = false;
		
		if (isCatalanLocaleAvailable())
		{
			needed = IsCatalanCurrentDefaultLocale() == false;
			
			if (needed)
			{
				needed = m_cannotBeApplied.equals("");				
			}
		}
		
		Log.i(LogString, "SystemLanguageAction.isNeeded. Returns: " + needed);
		return needed;
	}	
	
	void CheckPrerequirements()
	{
		if (android.os.Build.VERSION.SDK_INT >= JELLY_BEAN_MR1) {
			m_cannotBeApplied = "Cal que configureu manualment el català en aquesta versió de l'Android";
		}	
	}	
	
	// There phones like HTC Explorer that provide Catalan locale but it has no translations
	// If this is the case, the language shown when set to Catalan locale is English
	boolean DoesCurrentLocaleProvidesCatalanLocalization() 
	{
		String str;
		
		str = Resources.getSystem().getString(android.R.string.cut);
		Log.i(LogString, "SystemLanguageAction.DoesCurrentLocaleProvidesCatalanLocalization. Read: " + str);
		return str.equals("Retalla") == true;
	}
	
	void Execute()
	{
		String locale = Locale.getDefault().getLanguage();
		setLocale(_localeToSet);
		
		// Disable for now, since it does not work
		if (DoesCurrentLocaleProvidesCatalanLocalization() == false)
		{
			//setLocale(locale);
			//Log.i(LogString, "SystemLanguageAction.Execute. Reverting locale change");
		}
	}
	
	private boolean IsCatalanCurrentDefaultLocale()
	{
		String locale;
		boolean isDefault;
		
		locale = Locale.getDefault().getLanguage();
		isDefault = locale.equals(CatalanLocale1) || locale.equals(CatalanLocale2); 
		Log.i(LogString, "SystemLanguageAction.IsCatalanCurrentDefaultLocale. Returns: " + isDefault);	
		return isDefault;
	}
	
	private void readCatalanLocaleAvailable()
	{		
		String[] locales = _activity.getAssets().getLocales();		
		
		_localeToSet = "";		
		for (int i = 0; i < locales.length; i++)
		{
			String locale = locales[i];
			Log.i(LogString, "SystemLanguageAction.readCatalanLocaleAvailable. Locale: " + locale);
			
			if (locale.equals(CatalanLocale1))
			{
				_localeToSet = CatalanLocale1;
				break;
			}
			
			if (locale.equals(CatalanLocale2))
			{
				_localeToSet = CatalanLocale2;
				break;
			}
		}
		
		Log.i(LogString, "SystemLanguageAction.readCatalanLocaleAvailable. Locale to set: " + _localeToSet);		
	}
	
	private boolean isCatalanLocaleAvailable()
	{	
		boolean available;
		
		available = _localeToSet.equals("") == false;
		Log.i(LogString, "SystemLanguageAction.isCatalanLocaleAvailable. Returns: " + available);
		return available;
	}
	
	// TODO: This may crash when the locale is two letters only?
	private void setLocale(String localeToSet)
	{
		try
		{			 
			Locale locale = new Locale(localeToSet);
		
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
		    Log.i(LogString, "SystemLanguageAction.setLocale. Locale: " + localeToSet);
		}
	        
		catch(Exception e)
		{
			Log.i(LogString, "SystemLanguageAction.setLocale. Exception: " + e);    
		}
	}

}
