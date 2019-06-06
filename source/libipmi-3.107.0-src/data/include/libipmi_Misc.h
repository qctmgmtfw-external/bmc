#ifndef LIBIPMI_MISC_H_
#define LIBIPMI_MISC_H_

#include "Misc.h"
#include "Types.h"
#include "IPMI_ChassisDevice.h"
#include "IPMI_Chassis.h"


#ifdef __cplusplus
extern "C" {
#endif

LIBIPMI_API  uint16 IPMICMD_GetSyslogPort ( IPMI20_SESSION_T *pSession, GetSyslogPortRes_T  *pGetSyslogPortRes, int timeout );

LIBIPMI_API  uint16 IPMICMD_SetSyslogPort ( IPMI20_SESSION_T *pSession,
		SetSyslogPortReq_T  *pSetSyslogPortReq ,
		SetSyslogPortRes_T  *pSetSyslogPortRes,
                                     int timeout );
LIBIPMI_API  uint16 IPMICMD_GetSyslogHostname ( IPMI20_SESSION_T *pSession,
		GetSyslogHostnameRes_T  *pGetSyslogHostnameRes,
                                     int timeout );
LIBIPMI_API  uint16 IPMICMD_SetSyslogHostname ( IPMI20_SESSION_T *pSession,
		SetSyslogHostnameReq_T  *pSetSyslogHostnameReq ,
		SetSyslogHostnameRes_T  *pSetSyslogHostnameRes,
                                     int timeout );
LIBIPMI_API  uint16 IPMICMD_GetSyslogEnable ( IPMI20_SESSION_T *pSession,
		GetSyslogEnableRes_T  *pGetSyslogEnableRes,
                                     int timeout );
LIBIPMI_API  uint16 IPMICMD_SetSyslogEnable ( IPMI20_SESSION_T *pSession,
		SetSyslogEnableReq_T  *pSetSyslogEnableReq ,
		SetSyslogEnableRes_T  *pSetSyslogEnableRes,
                                     int timeout );
LIBIPMI_API  uint16 IPMICMD_GetSnmpPort( IPMI20_SESSION_T *pSession,
		GetSnmpPortRes_T *pGetSnmpPortRes,
				int timeout );
LIBIPMI_API  uint16 IPMICMD_SetSnmpPort( IPMI20_SESSION_T *pSession,
		SetSnmpPortReq_T *pSetSnmpPortReq,
		SetSnmpPortRes_T *pSetSnmpPortRes,
				int timeout );
LIBIPMI_API  uint16 IPMICMD_GetSysTemp( IPMI20_SESSION_T *pSession,
		GetSysTempRes_T *pGetSysTempRes,
				int timeout );
LIBIPMI_API uint16 IPMICMD_SetPowerCycleInterval(IPMI20_SESSION_T *pSession, SetPowerCycleIntervalReq_T *pSetPowerCycleIntervalReq,SetPowerCycleIntervalRes_T *pSetPowerCycleIntervalRes,int timeout);

LIBIPMI_API uint16 IPMICMD_GetSmashLiteActiveSessCnt( IPMI20_SESSION_T *pSession,
		GetSmashLiteActiveSessCntRes_T *pGetSmashLiteActiveSessCntRes,
				int timeout );
LIBIPMI_API uint16 IPMICMD_SetSmashLiteActiveSessCnt( IPMI20_SESSION_T *pSession,
		SetSmashLiteActiveSessCntReq_T *pSetSmashLiteActiveSessCntReq,
		SetSmashLiteActiveSessCntRes_T *pSetSmashLiteActiveSessCntRes,
				int timeout );
LIBIPMI_API uint16 IPMICMD_GetSysHealthFirmwareANDPowerCycle( IPMI20_SESSION_T *pSession,
		GetSysHealthFirmwareANDPowerCycleRes_T *pGetSysHealthFirmwareANDPowerCycleRes,
				int timeout );


#ifdef __cplusplus
}
#endif

#endif
