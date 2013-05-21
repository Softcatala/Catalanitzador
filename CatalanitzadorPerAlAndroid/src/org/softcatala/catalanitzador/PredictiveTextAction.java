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

import android.provider.UserDictionary;
import android.app.Activity;

public class PredictiveTextAction extends Action {
	
	private Activity _activity;
	
	PredictiveTextAction(Activity activity)
	{
		_activity = activity;
	}
	
	public boolean isNeeded()
	{
		return true;
	}	

	// TODO: Can we ask for prediction to Android for a Catalan words to determine if prediction is installed?
	void CheckPrerequirements()
	{

	}
	
	void Execute()
	{
		addWords();
	}
	
	private void addWords()
	{
		  UserDictionary.Words.addWord(_activity , "siusplau", 1, UserDictionary.Words.LOCALE_TYPE_ALL);
		  UserDictionary.Words.addWord(_activity , "feréstec", 1, UserDictionary.Words.LOCALE_TYPE_ALL);
		  UserDictionary.Words.addWord(_activity , "reguitzell", 1, UserDictionary.Words.LOCALE_TYPE_ALL);
		  UserDictionary.Words.addWord(_activity , "aixopluc", 1, UserDictionary.Words.LOCALE_TYPE_ALL);
		  UserDictionary.Words.addWord(_activity , "lletraferit", 1, UserDictionary.Words.LOCALE_TYPE_ALL);
		  UserDictionary.Words.addWord(_activity , "somiseremcatalans", 1, UserDictionary.Words.LOCALE_TYPE_ALL);
	}
}