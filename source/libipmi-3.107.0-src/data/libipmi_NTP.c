/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
* 
* Filename: libipmi_NTP.c
*
******************************************************************/
#include "libipmi_NTP.h"
#include "OemDefs.h"

#include <string.h>

LIBIPMI_API uint16	IPMICMD_SetTimeZone( IPMI20_SESSION_T *pSession, uint8* pTimeZone, int timeout )
{
	uint8		Res [20];
	uint32		dwResLen;
	uint16		wRet;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											(NETFN_GENERIC_VANILLA_OEM << 2), CMD_OEM_SET_TIMEZONE,
											pTimeZone, 1,
											Res, &dwResLen,
											timeout);
	return wRet;
}


