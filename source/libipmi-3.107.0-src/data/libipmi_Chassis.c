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
* Filename: libipmi_Chassis.c
*
* Descriptions: Chassis Control Subfunctions Implementation.
*   For documentation see file libipmi_ChassisDevice.h
*
* Author: Chennakesava Rao Arnoori
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_ChassisDevice.h"

#include "string.h"
#include <dbgout.h>


/**
   \breif	Internel function to execute the raw IPMI Command
   @param	pSession		[in]Session handle
   @param	timeout			[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16	IPMICMD_ChassisControl( IPMI20_SESSION_T *pSession, int timeout, char cmd)
{

    uint32		dwResLen;
    uint16		wRet;
    ChassisControlReq_T ChassisReq;
    ChassisControlRes_T ChassisRes;
    
    ChassisReq.ChassisControl = cmd; 
    dwResLen = sizeof ( ChassisControlRes_T);


    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NETFNLUN_IPMI_CHASSIS, CMD_CHASSIS_CONTROL,
					    (uint8 *)&ChassisReq,sizeof(ChassisControlReq_T),
					    (uint8*)&ChassisRes, &dwResLen,
					    timeout);
    return wRet;
}


uint16 IPMICMD_GetSystemBootOptions(IPMI20_SESSION_T *pSession, GetBootOptionsReq_T* pReqGetBootOptions,
							 GetBootOptionsRes_T* pResGetBootOptions,int reslen,int timeout)
{
	uint16 wRet;
	uint32 dwResLen;

	dwResLen = reslen;
	
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_CHASSIS, CMD_GET_SYSTEM_BOOT_OPTIONS,
											(uint8*)pReqGetBootOptions, sizeof(GetBootOptionsReq_T),
											(uint8 *)pResGetBootOptions, &dwResLen,
											timeout);
	return wRet;
	
}


uint16 IPMICMD_SetSystemBootOptions(IPMI20_SESSION_T *pSession, SetBootOptionsReq_T* pReqSetBootOptions,
							 SetBootOptionsRes_T* pResSetBootOptions,int reqlen,int timeout)
{
	uint16 wRet;
	uint32 dwResLen;
	SetBootOptionsReq_T local_ReqSetBootOptions;
	SetBootOptionsRes_T local_ResSetBootOptions;



	
	local_ReqSetBootOptions.ParamValidCumParam = 1; //for set in progress
	local_ReqSetBootOptions.BootParam.SetInProgress = 1;
	dwResLen = sizeof(SetBootOptionsRes_T);
	//first do a set in progress
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_CHASSIS, CMD_SET_SYSTEM_BOOT_OPTIONS,
											(uint8*)&local_ReqSetBootOptions, 2,
											(uint8 *)&local_ResSetBootOptions, &dwResLen,
											timeout);

	if(wRet != 0)
	{
		TCRIT("Error while doing set in progress first before doing SetSystemBootOptions\n");
		return wRet;
	}


	dwResLen = sizeof(SetBootOptionsRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_CHASSIS, CMD_SET_SYSTEM_BOOT_OPTIONS,
											(uint8*)pReqSetBootOptions, reqlen,
											(uint8 *)pResSetBootOptions, &dwResLen,
											timeout);
	if(wRet != 0)
	{
		TCRIT("error doing actual set system boot options\n");
		return wRet;
	}

	//clear the set in progress
	local_ReqSetBootOptions.ParamValidCumParam = 1; //for set in progress
	local_ReqSetBootOptions.BootParam.SetInProgress = 0;
	dwResLen = sizeof(SetBootOptionsRes_T);
	//first do a set in progress
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
										NETFNLUN_IPMI_CHASSIS, CMD_SET_SYSTEM_BOOT_OPTIONS,
										(uint8*)&local_ReqSetBootOptions, 2,
										(uint8 *)&local_ResSetBootOptions, &dwResLen,
										timeout);

	if(wRet != 0)
	{
		TCRIT("Error while doing set in progress first before doing SetSystemBootOptions\n");
		return wRet;
	}

	return wRet;
	
}

uint16 IPMICMD_ChassisPOH(IPMI20_SESSION_T *pSession,int timeout, GetPOHCounterRes_T *pPOHRes)
{
	 uint32		dwResLen;
   	 uint16		wRet;
	 
	 dwResLen = sizeof (GetPOHCounterRes_T);
	 
	 wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NETFNLUN_IPMI_CHASSIS, CMD_GET_POH_COUNTER,
					    NULL, 0,
					    (uint8*)pPOHRes, &dwResLen,
					    timeout);
    					   
 
	return wRet;
}


uint16 IPMICMD_ChassisIdentify(IPMI20_SESSION_T *pSession,int timeout,ChassisIdentifyReq_T* ChassisIdReq)
{
	 uint32		dwResLen;
    	 uint16		wRet;
	 ChassisIdentifyRes_T ChassisIdRes = {0};
	 
	 dwResLen = sizeof (ChassisIdentifyRes_T);
	 
	 wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NETFNLUN_IPMI_CHASSIS, CMD_CHASSIS_IDENTIFY,
					    (uint8 *)ChassisIdReq,sizeof(ChassisIdentifyReq_T),
					    (uint8*)&ChassisIdRes, &dwResLen,
					    timeout);
					    
	return wRet;
}


uint16 LIBIPMI_HL_PowerOff( IPMI20_SESSION_T *pSession, int timeout )
{
    uint16	wRet;

    wRet = IPMICMD_ChassisControl( pSession, timeout, CHASSIS_POWEROFF);

    if (wRet == LIBIPMI_E_SUCCESS){
	TDBG("Succefully executed powerOFF  command.\n");
    }else
	TDBG("Error: PowerOFF Command Failed.\n");
	
    return wRet;
}

uint16 LIBIPMI_HL_PowerUp( IPMI20_SESSION_T *pSession, int timeout )
{
    uint16	wRet;

    wRet = IPMICMD_ChassisControl( pSession, timeout, CHASSIS_POWERUP);

    if (wRet == LIBIPMI_E_SUCCESS){
	TDBG("Succefully executed powerUP the command.\n");
    }else
	TDBG("Error: PowerUp Command Failed.\n");
	
    return wRet;
}

uint16 LIBIPMI_HL_PowerCycle( IPMI20_SESSION_T *pSession, int timeout )
{
    uint16	wRet;

    wRet = IPMICMD_ChassisControl( pSession, timeout, CHASSIS_POWERCYCLE);

    if (wRet == LIBIPMI_E_SUCCESS){
	TDBG("Succefully executed powerCycle the command.\n");
    }else
	TDBG("Error: PowerCycle Command Failed.\n");
	
    return wRet;
}

uint16 LIBIPMI_HL_HardReset( IPMI20_SESSION_T *pSession, int timeout )
{
    uint16	wRet;

    wRet = IPMICMD_ChassisControl( pSession, timeout, CHASSIS_HARDRESET);

    if (wRet == LIBIPMI_E_SUCCESS){
	TDBG("Succefully executed HardReset the command.\n");
    }else
	TDBG("Error: HardReset Command Failed.\n");
	
    return wRet;
}

uint16 LIBIPMI_HL_DiagInt( IPMI20_SESSION_T *pSession, int timeout )
{
    uint16	wRet;

    wRet = IPMICMD_ChassisControl( pSession, timeout, CHASSIS_DIAGINT);

    if (wRet == LIBIPMI_E_SUCCESS){
	TDBG("Succefully executed DiagInt command.\n");
    }else
	TDBG("Error: DiagInt Command Failed.\n");
	
    return wRet;
}

uint16 LIBIPMI_HL_SoftOff( IPMI20_SESSION_T *pSession, int timeout )
{
    uint16	wRet;

    wRet = IPMICMD_ChassisControl( pSession, timeout, CHASSIS_SOFTOFF);

    if (wRet == LIBIPMI_E_SUCCESS){
	TDBG("Succefully executed SoftOff command.\n");
    }else
	TDBG("Error: SoftOff Command Failed.\n");
	
    return wRet;
}

uint16	LIBIPMI_HL_GetChassisStatus( IPMI20_SESSION_T *pSession, int timeout, chassis_status_T *status)
{

    uint32		dwResLen;
    uint16		wRet;
    uint8		ChassisStatusReq;
    GetChassisStatusRes_T ChassisStatusRes;
    uint8		powerState = -1;

    dwResLen = sizeof(GetChassisStatusRes_T);
    
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NETFNLUN_IPMI_CHASSIS, CMD_GET_CHASSIS_STATUS,
					    (uint8 *)&ChassisStatusReq, 0 ,
					    (uint8*)&ChassisStatusRes, &dwResLen,
					    timeout);
    
    powerState = ChassisStatusRes.ChassisPowerState.PowerState ;
    
//    TINFO("PowerState is %d \n", powerState);
    
    status->power_state = powerState & 0x01;

    status->last_power_event = ChassisStatusRes.ChassisPowerState.LastPowerEvent;


    if(ChassisStatusRes.ChassisPowerState.MiscChassisState & CHASSIS_IDENTITY_STATE_INFO_SUPPORTED)
    {
	status->chassis_indetify_supported = 1;
    }
    else
    {
	status->chassis_indetify_supported = 0;
    }


    if(ChassisStatusRes.ChassisPowerState.MiscChassisState & CHASSIS_IDENTITY_INDEFINITE_ON)
    {
	status->chassis_identify_state = CHASSIS_ID_INDEFINITE_ON;
    }
    else if(ChassisStatusRes.ChassisPowerState.MiscChassisState & CHASSIS_IDENTITY_TIMED_ON)
    {
        status->chassis_identify_state = CHASSIS_ID_TIMED_ON;
    }
    else
    {
	status->chassis_identify_state = CHASSIS_ID_OFF;
    }
    
    return wRet;
}


//will turn chassis identify on or off
//BlinkTime - Set to TURN_OFF for turning off indefinitely
//	      Set to Interval in secs for chassis identify to be on for that much time.
// 	      for indefinite interval set ForceOn to 1.
// ForceOn - overrides BlinkTime settings if set to 1.Set to 0 if controlling by interval
uint16 LIBIPMI_HL_ChassisIdentify(IPMI20_SESSION_T *pSession, int timeout,INT8U BlinkTimeSecs,FORCE_CHASSIS_ID ForceOn)
{
	uint16 wRet;
	
	ChassisIdentifyReq_T ChassisIdReq = {0,0};
	
	//set interval to blinktimesecs. If caller set it to 0 so be it.
	//The caller should always give a time to this function . He can Use SPEC_DEFAULT to get default timeout behavior.
	ChassisIdReq.IdentifyInterval = BlinkTimeSecs;
	ChassisIdReq.ForceIdentify = ForceOn;
	
	TDBG("id interval and  force in libipmi are %d %d \n",ChassisIdReq.IdentifyInterval,ChassisIdReq.ForceIdentify);
	
	wRet = IPMICMD_ChassisIdentify(pSession,timeout,&ChassisIdReq);
	
	if(wRet == LIBIPMI_E_SUCCESS)
	{
		TDBG("Chassis identify command success\n");
	}
	else
	{	
		TCRIT("Chassis Identify failed for the following params : %d %d",BlinkTimeSecs,ForceOn);
	}
	
	return wRet;
}

uint16 LIBIPMI_HL_ChassisPOH(IPMI20_SESSION_T *pSession, GetPOHCounterRes_T *pPOH, int timeout)
{
    uint16 wRet;

    memset((char *)pPOH, 0, sizeof(GetPOHCounterRes_T));

    wRet = IPMICMD_ChassisPOH(pSession, timeout, pPOH);

    if(wRet == LIBIPMI_E_SUCCESS)
    {
        TDBG("Chassis POH success\n");
    }
    else
    {
        TCRIT("Chassis POH failed\n");
    }

    return wRet;
}


uint16	LIBIPMI_HL_GetSystemBootOptions_BootFlags( IPMI20_SESSION_T *pSession, BootFlags_T* pBootFlags,int timeout)
{
	uint16 wRet;
	GetBootOptionsReq_T ReqGetBootOptions;
	GetBootOptionsRes_T ResGetBootOptions;
	int reslen;

	//form the request
	ReqGetBootOptions.ParamSel = 5;
	ReqGetBootOptions.BlockSel = 0;
	ReqGetBootOptions.SetSel = 0;

	//calculate the response length
	//we want the common response header + the sizeof bootflags
	reslen = 3 + sizeof(BootFlags_T);


	wRet = IPMICMD_GetSystemBootOptions(pSession,&ReqGetBootOptions,&ResGetBootOptions,reslen,timeout);

	if(wRet != 0)
	{
		TCRIT("Error getting system boot options when trying to get boot flags error code  is %x\n",wRet);
		return wRet;
	}

	//check the response
	if(ResGetBootOptions.ParameterValid & 0x80) 
	{
		TCRIT("Got a parameter invlaid/locked error when trying to get Boot flags\n");
    }

	//we have the boot flags
	memcpy(pBootFlags,&ResGetBootOptions.BootParams.BootFlags,sizeof(BootFlags_T));

	return 0;
}


uint16	LIBIPMI_HL_SetSystemBootOptions_BootFlags( IPMI20_SESSION_T *pSession, BootFlags_T* pBootFlags,int timeout)
{
	uint16 wRet;
	SetBootOptionsReq_T ReqSetBootOptions;
	SetBootOptionsRes_T ResSetBootOptions;
	int reqlen;


    
	//form the request
	ReqSetBootOptions.ParamValidCumParam = 5; //for bootflags
	memcpy((uint8*)&(ReqSetBootOptions.BootParam.BootFlags),pBootFlags,sizeof(BootFlags_T));
	
	
	
	//we want the common response header + the sizeof bootflags
	reqlen = 1 + sizeof(BootFlags_T);




	wRet = IPMICMD_SetSystemBootOptions(pSession,&ReqSetBootOptions,&ResSetBootOptions,reqlen,timeout);

	if(wRet != 0)
	{
		TCRIT("Error setting system boot options when trying to set boot flags error code  is %x\n",wRet);
		return wRet;
	}


	return 0;
}
