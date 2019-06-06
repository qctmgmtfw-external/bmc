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
* Filename: libipmi_Misc.c
*	Author: Bojanki Ravi
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbgout.h"
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "std_macros.h"
#include "libipmi_Misc.h"

uint16
IPMICMD_GetSyslogPort ( IPMI20_SESSION_T *pSession,
				GetSyslogPortRes_T  *pGetSyslogPortRes,
                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetSyslogPortRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_SCORPIO << 2 ) + 0x00 ), CMD_GET_SYSLOG_PORT ,
                                           NULL ,0,
                                           (uint8 *)pGetSyslogPortRes, &dwResLen, timeout );
    return( wRet );

}

uint16
IPMICMD_SetSyslogPort ( IPMI20_SESSION_T *pSession,
		SetSyslogPortReq_T  *pSetSyslogPortReq ,
		SetSyslogPortRes_T  *pSetSyslogPortRes,
                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( SetSyslogPortRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_SCORPIO << 2 ) + 0x00 ), CMD_SET_SYSLOG_PORT ,
                                           (uint8 *)pSetSyslogPortReq ,sizeof (SetSyslogPortReq_T),
                                           (uint8 *)pSetSyslogPortRes, &dwResLen, timeout );
    return( wRet );

}

uint16
IPMICMD_GetSyslogHostname ( IPMI20_SESSION_T *pSession,
		GetSyslogHostnameRes_T  *pGetSyslogHostnameRes,
                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetSyslogHostnameRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_SCORPIO << 2 ) + 0x00 ), CMD_GET_SYSLOG_HOSTNAME ,
                                           NULL ,0,
                                           (uint8 *)pGetSyslogHostnameRes, &dwResLen, timeout );
    return( wRet );

}

uint16
IPMICMD_SetSyslogHostname ( IPMI20_SESSION_T *pSession,
		SetSyslogHostnameReq_T  *pSetSyslogHostnameReq ,
		SetSyslogHostnameRes_T  *pSetSyslogHostnameRes,
                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof( SetSyslogHostnameRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_SCORPIO << 2 ) + 0x00 ), CMD_SET_SYSLOG_HOSTNAME ,
                                           (uint8 *)pSetSyslogHostnameReq ,sizeof (SetSyslogHostnameReq_T),
                                           (uint8 *)pSetSyslogHostnameRes, &dwResLen, timeout );
    return( wRet );

}

uint16
IPMICMD_GetSyslogEnable ( IPMI20_SESSION_T *pSession,
		GetSyslogEnableRes_T  *pGetSyslogEnableRes,
                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( SetSyslogEnableRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_SCORPIO << 2 ) + 0x00 ), CMD_GET_SYSLOG_ENABLE ,
                                           NULL ,0,
                                           (uint8 *)pGetSyslogEnableRes, &dwResLen, timeout );
    return( wRet );

}

uint16
IPMICMD_SetSyslogEnable ( IPMI20_SESSION_T *pSession,
		SetSyslogEnableReq_T  *pSetSyslogEnableReq ,
		SetSyslogEnableRes_T  *pSetSyslogEnableRes,
                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( SetSyslogEnableRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_SCORPIO << 2 ) + 0x00 ), CMD_SET_SYSLOG_ENABLE ,
                                           (uint8 *)pSetSyslogEnableReq ,sizeof (SetSyslogEnableReq_T),
                                           (uint8 *)pSetSyslogEnableRes, &dwResLen, timeout );
    return( wRet );

}

uint16
IPMICMD_GetSnmpPort( IPMI20_SESSION_T *pSession,
		GetSnmpPortRes_T *pGetSnmpPortRes,
				int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetSnmpPortRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					   ( (NETFN_SCORPIO << 2 ) + 0x00 ), CMD_GET_SNMP_PORT ,
					   NULL, 0,
					   (uint8 *)pGetSnmpPortRes, &dwResLen, timeout );

    return( wRet );
}

uint16
IPMICMD_SetSnmpPort( IPMI20_SESSION_T *pSession,
		SetSnmpPortReq_T *pSetSnmpPortReq,
		SetSnmpPortRes_T *pSetSnmpPortRes,
				int timeout )
{
	uint16 wRet;
	uint32 dwResLen;
	
	dwResLen = sizeof( SetSnmpPortRes_T );
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					       ( (NETFN_SCORPIO << 2 ) + 0x00 ), CMD_SET_SNMP_PORT ,
					       (uint8 *)pSetSnmpPortReq, sizeof (SetSnmpPortReq_T),
					       (uint8 *)pSetSnmpPortRes, &dwResLen, timeout );

	return( wRet );
}

uint16
IPMICMD_GetSysTemp( IPMI20_SESSION_T *pSession,
		GetSysTempRes_T *pGetSysTempRes,
				int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetSysTempRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					   ( (NETFN_SCORPIO << 2 ) + 0x00 ), CMD_GET_SYSTEM_TEMP ,
					   NULL, 0,
					   (uint8 *)pGetSysTempRes, &dwResLen, timeout );

    return( wRet );
}


uint16
IPMICMD_GetSmashLiteActiveSessCnt( IPMI20_SESSION_T *pSession,
		GetSmashLiteActiveSessCntRes_T *pGetSmashLiteActiveSessCntRes,
				int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof( GetSmashLiteActiveSessCntRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					   ( (NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_SMASHLITE_ACTIVE_SESS_CNT ,
					   NULL, 0,
					   (uint8 *)pGetSmashLiteActiveSessCntRes, &dwResLen, timeout );

    return( wRet );
}

uint16
IPMICMD_SetSmashLiteActiveSessCnt( IPMI20_SESSION_T *pSession,
		SetSmashLiteActiveSessCntReq_T *pSetSmashLiteActiveSessCntReq,
		SetSmashLiteActiveSessCntRes_T *pSetSmashLiteActiveSessCntRes,
				int timeout )
{
	uint16 wRet;
	uint32 dwResLen;
	
	dwResLen = sizeof( SetSmashLiteActiveSessCntRes_T );
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					       ( (NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_SMASHLITE_ACTIVE_SESS_CNT ,
					       (uint8 *)pSetSmashLiteActiveSessCntReq, sizeof (SetSmashLiteActiveSessCntReq_T),
					       (uint8 *)pSetSmashLiteActiveSessCntRes, &dwResLen, timeout );

	return( wRet );
}

uint16
IPMICMD_GetSysHealthFirmwareANDPowerCycle( IPMI20_SESSION_T *pSession,
		GetSysHealthFirmwareANDPowerCycleRes_T *pGetSysHealthFirmwareANDPowerCycleRes,
				int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof( GetSysHealthFirmwareANDPowerCycleRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					   ( (NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SYSTEM_FIRMWARE_HEALTH_POWERCYCLE ,
					   NULL, 0,
					   (uint8 *)pGetSysHealthFirmwareANDPowerCycleRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_SetPowerCycleInterval(IPMI20_SESSION_T *pSession, 
		SetPowerCycleIntervalReq_T *pSetPowerCycleIntervalReq,
		SetPowerCycleIntervalRes_T *pSetPowerCycleIntervalRes,
		int timeout)
{
	uint16 wRet;
	uint32 dwResLen;
	
	dwResLen = sizeof( SetPowerCycleIntervalRes_T );
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
							NETFNLUN_IPMI_CHASSIS, CMD_SET_POWER_CYCLE_INTERVAL ,
					       (uint8 *)pSetPowerCycleIntervalReq, sizeof (SetPowerCycleIntervalReq_T),
					       (uint8 *)pSetPowerCycleIntervalRes, &dwResLen, timeout );

	return( wRet );
}
