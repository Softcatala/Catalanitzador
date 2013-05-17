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


import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.widget.TextView;

public class MainActivity extends Activity {
	
	TextView _textview;
	private String LogString = "Catalanitzador";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);		
		setContentView(R.layout.activity_main);
		_textview = (TextView) findViewById(R.id.mainTextView);
		init();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	void init (){
		
		Log.i(LogString, "Init start");
		SystemLanguageAction systemActionLanguage = new SystemLanguageAction(this);		 
		if (systemActionLanguage.isNeeded()) {
			_textview.append("SystemLanguageAction was needed");
			systemActionLanguage.Execute();
		}
		
		PredictiveTextAction predictiveAction = new PredictiveTextAction(this);
		if (predictiveAction.isNeeded()) {
			_textview.append("PredictiveTextAction was needed");
			predictiveAction.Execute();
		}
		Log.i(LogString, "Init end");
	}	
	
}
