/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

/******************************************************************
*
* Filename: defaulthost.c
*
******************************************************************/
#include <stdio.h>
#include <string.h>
#include <dbgout.h>
#include <hostname.h>
#include "featuredef.h"

int main()
{
	int retval = 0;
	char CurrentHostName[MAX_HOSTNAME_LEN];
	char HostNameWithMACZero[MAX_HOSTNAME_LEN];
	int HostSetting = 0;
	
    memset(CurrentHostName, '\0', MAX_HOSTNAME_LEN);
    memset(HostNameWithMACZero, '\0', MAX_HOSTNAME_LEN);
	if((retval = GetHostName(CurrentHostName)))
	{
		TCRIT("GetHostName() failed in hostname process.");
		return retval;
	}


	GetMacrodefine_string("CONFIG_SPX_FEATURE_GLOBAL_HOST_PREFIX",HostNameWithMACZero);
	strcat(HostNameWithMACZero,"000000000000");

	// Check hostname settings file. whether host name is configured Automatic/Manual. 
	if(GetHostNameConf(CurrentHostName, &HostSetting) < 0)
		HostSetting = 1;

		
	//if HostSetting is '1' means, hostname obtained automatically. so need to update once MAC address changed.
	if(!strcmp(CurrentHostName,"localhost") || !strcmp(CurrentHostName,HostNameWithMACZero) || (HostSetting == 1))
	{
		if((retval = SetDefaultHostName()))
		{
			TCRIT("SetDefaultHostName() failed in hostname process.");
			return retval;
		}
	}
	return 0;
}
