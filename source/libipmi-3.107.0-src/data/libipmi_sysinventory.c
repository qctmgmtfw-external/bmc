/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2015, American Megatrends Inc.         ***
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
* Filename: libipmi_sysinventory.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if ( _WIN64 || _WIN32 ) 
#include<ws2tcpip.h>
#define snprintf sprintf_s
#endif
#include "dbgout.h"
#include "libipmi_AMIOEM.h"
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_struct.h"

uint16 LIBIPMI_HL_GetSysInventory(IPMI20_SESSION_T *pSession, GetInventoryReq_T *pGetSysInvenotryReq, GetInventoryRes_T *pGetSysInvenotryRes,int timeout)
{
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( GetInventoryReq_T ) ;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_INVENTORY,
                        (uint8 *)pGetSysInvenotryReq, ReqLen,
                        (uint8 *)pGetSysInvenotryRes, &ResLen,
                        timeout);
    return wRet;
}

