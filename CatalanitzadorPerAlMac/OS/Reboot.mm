/*
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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


#include "Reboot.h"

OSStatus Reboot::RestartNow()
{
	const AEEventID EventToSend = kAERestart;
	AEAddressDesc targetDesc;
	static const ProcessSerialNumber kPSNOfSystemProcess = { 0, kSystemProcess };
	AppleEvent eventReply = {typeNull, NULL};
	AppleEvent appleEventToSend = {typeNull, NULL};
	
	OSStatus error = noErr;
	
	error = AECreateDesc(typeProcessSerialNumber, &kPSNOfSystemProcess,
						 sizeof(kPSNOfSystemProcess), &targetDesc);
	
	if (error != noErr)
	{
		return(error);
	}
	
	error = AECreateAppleEvent(kCoreEventClass, EventToSend, &targetDesc,
							   kAutoGenerateReturnID, kAnyTransactionID, &appleEventToSend);
	
	AEDisposeDesc(&targetDesc);
	if (error != noErr)
	{
		return(error);
	}
	
	error = AESend(&appleEventToSend, &eventReply, kAENoReply,
				   kAENormalPriority, kAEDefaultTimeout, NULL, NULL);
	
	AEDisposeDesc(&appleEventToSend);
	if (error != noErr)
	{
		return(error);
	}
	
	AEDisposeDesc(&eventReply);
	
	return(error);
	
}

