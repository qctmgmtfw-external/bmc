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
* Filename: libipmi_AMIFlash.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbgout.h"

#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_AMIOEM.h"
#include "std_macros.h"
#include "fmh.h"

static INT8U NetFn_OEM;

uint16 IPMICMD_AMIYAFUSwitchFlashDevice(IPMI20_SESSION_T *pSession,
	                    AMIYAFUSwitchFlashDeviceReq_T* pAMIYAFUSwitchFlashDeviceReq,
	                    AMIYAFUSwitchFlashDeviceRes_T* pAMIYAFUSwitchFlashDeviceRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUSwitchFlashDeviceRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_SWITCH_FLASH_DEVICE,
                                             (uint8 *)pAMIYAFUSwitchFlashDeviceReq,sizeof(AMIYAFUSwitchFlashDeviceReq_T),
                                             (uint8 *)pAMIYAFUSwitchFlashDeviceRes, &dwResLen, timeout );

    return( wRet );   
}


uint16 IPMICMD_AMIYAFUActivateFlashDevice(IPMI20_SESSION_T *pSession,
                            AMIYAFUActivateFlashDeviceReq_T* pAMIYAFUActivateFlashDeviceReq,
                            AMIYAFUActivateFlashDeviceRes_T* pAMIYAFUActivateFlashDeviceRes,
                            int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUActivateFlashDeviceRes_T);
    
    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_ACTIVATE_FLASH_DEVICE,
                                             (uint8 *)pAMIYAFUActivateFlashDeviceReq,sizeof(AMIYAFUActivateFlashDeviceReq_T),
                                             (uint8 *)pAMIYAFUActivateFlashDeviceRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMIGetFwVersion( IPMI20_SESSION_T *pSession, 
                       AMIGetFwVersionReq_T* pAMIGetFwVersionReq, INT8U ReqLen,
                       AMIGetFwVersionRes_T* pAMIGetFwVersionRes, int timeout )
{
    uint16 wRet;
    uint32 ResLen=sizeof(AMIGetFwVersionRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NetFn_OEM << 2) + 0x00), CMD_AMI_GET_FW_VERSION,
                                         (uint8 *)pAMIGetFwVersionReq, ReqLen,
                                         (uint8 *)pAMIGetFwVersionRes,& ResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFURestoreFlashDevice(IPMI20_SESSION_T *pSession,
	                    AMIYAFUSwitchFlashDeviceReq_T* pAMIYAFUSwitchFlashDeviceReq,
	                    AMIYAFUSwitchFlashDeviceRes_T* pAMIYAFUSwitchFlashDeviceRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUSwitchFlashDeviceRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_RESTORE_FLASH_DEVICE,
                                             (uint8 *)pAMIYAFUSwitchFlashDeviceReq,sizeof(AMIYAFUSwitchFlashDeviceReq_T),
                                             (uint8 *)pAMIYAFUSwitchFlashDeviceRes, &dwResLen, timeout );
    return( wRet );   
}

uint16 IPMICMD_AMIYAFUGetFlashInfo(IPMI20_SESSION_T *pSession,
	                    AMIYAFUGetFlashInfoReq_T* pAMIYAFUGetFlashInfoReq,
	                    AMIYAFUGetFlashInfoRes_T* pAMIYAFUGetFlashInfoRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUGetFlashInfoRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_FLASH_INFO,
                                             (uint8 *)pAMIYAFUGetFlashInfoReq,sizeof(AMIYAFUGetFlashInfoReq_T),
                                             (uint8 *)pAMIYAFUGetFlashInfoRes, &dwResLen, timeout );
    return( wRet );   
}

uint16 IPMICMD_AMIYAFUGetFirmwareInfo(IPMI20_SESSION_T *pSession,
			   AMIYAFUGetFirmwareInfoReq_T* pAMIYAFUGetFirmwareInfoReq, 
			   AMIYAFUGetFirmwareInfoRes_T* pAMIYAFUGetFirmwareInfoRes,
			   int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(AMIYAFUGetFirmwareInfoRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_FIRMWARE_INFO,
                                             (uint8 *)pAMIYAFUGetFirmwareInfoReq,sizeof(AMIYAFUGetFirmwareInfoReq_T),
                                             (uint8 *)pAMIYAFUGetFirmwareInfoRes, &dwResLen, timeout );
    return( wRet );   	
    	
}



uint16 IPMICMD_AMIYAFUGetFMHInfo(IPMI20_SESSION_T *pSession,
                         AMIYAFUGetFMHInfoReq_T* pAMIYAFUGetFMHInfoReq,               
                         AMIYAFUGetFMHInfoRes_T* pAMIYAFUGetFMHInfoRes,
                         int timeout)
{

    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUGetFMHInfoRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_FMH_INFO,
                                             (uint8 *)pAMIYAFUGetFMHInfoReq, sizeof(AMIYAFUGetFMHInfoReq_T),
                                             (uint8 *)pAMIYAFUGetFMHInfoRes, &dwResLen, timeout );
    return( wRet );
         
}

uint16 IPMICMD_AMIYAFUGetStatus(IPMI20_SESSION_T *pSession,
	                    AMIYAFUGetStatusReq_T* pAMIYAFUGetStatusReq,
	                    AMIYAFUGetStatusRes_T* pAMIYAFUGetStatusRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUGetStatusRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_STATUS,
                                             (uint8 *)pAMIYAFUGetStatusReq, sizeof(AMIYAFUGetStatusReq_T),
                                             (uint8 *)pAMIYAFUGetStatusRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUActivateFlashMode(IPMI20_SESSION_T *pSession,
	                    AMIYAFUActivateFlashModeReq_T* pAMIYAFUActivateFlashReq,
	                    AMIYAFUActivateFlashModeRes_T* pAMIYAFUActivateFlashRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    if(0)
    {
         timeout=timeout;   /*-Wextra: Flag added for strict compilation.*/
    }

    dwResLen = sizeof( AMIYAFUActivateFlashModeRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_ACTIVATE_FLASH,
                                             (uint8 *)pAMIYAFUActivateFlashReq, sizeof(AMIYAFUActivateFlashModeReq_T),
                                             (uint8 *)pAMIYAFUActivateFlashRes, &dwResLen, ACTIVATE_FLASH_TIMEOUT );
    return( wRet );   
}

uint16 IPMICMD_AMIYAFUAllocateMemory(IPMI20_SESSION_T *pSession,
	                    AMIYAFUAllocateMemoryReq_T* pAMIYAFUAllocateMemoryReq,
	                    AMIYAFUAllocateMemoryRes_T* pAMIYAFUAllocateMemoryRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUAllocateMemoryRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_ALLOCATE_MEMORY,
                                             (uint8 *)pAMIYAFUAllocateMemoryReq, sizeof(AMIYAFUAllocateMemoryReq_T),
                                             (uint8 *)pAMIYAFUAllocateMemoryRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUFreeMemory(IPMI20_SESSION_T *pSession,
	                    AMIYAFUFreeMemoryReq_T* pAMIYAFUFreeMemoryReq,
	                    AMIYAFUFreeMemoryRes_T* pAMIYAFUFreeMemoryRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUFreeMemoryRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_FREE_MEMORY,
                                             (uint8 *)pAMIYAFUFreeMemoryReq, sizeof(AMIYAFUFreeMemoryReq_T),
                                             (uint8 *)pAMIYAFUFreeMemoryRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUReadFlash(IPMI20_SESSION_T *pSession,
	                AMIYAFUReadFlashReq_T* pAMIYAFUReadFlashReq,
	                AMIYAFUReadFlashRes_T* pAMIYAFUReadFlashRes,
	                int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUReadFlashRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_READ_FLASH,
                                             (uint8 *)pAMIYAFUReadFlashReq, sizeof(AMIYAFUReadFlashReq_T),
                                             (uint8 *)pAMIYAFUReadFlashRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUWriteFlash(IPMI20_SESSION_T *pSession,
	                AMIYAFUWriteFlashReq_T* pAMIYAFUWriteFlashReq,
	                AMIYAFUWriteFlashRes_T* pAMIYAFUWriteFlashRes,
	                int timeout)
{
    uint16 wRet;
    uint32 dwResLen,dwReqLen;

    dwResLen = sizeof( AMIYAFUWriteFlashRes_T );
    dwReqLen = sizeof(AMIYAFUWriteFlashReq_T)+pAMIYAFUWriteFlashReq->WriteFlashReq.Datalen;

    pAMIYAFUWriteFlashReq->WriteFlashReq.Datalen+=5;

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_WRITE_FLASH,
                                             (uint8 *)pAMIYAFUWriteFlashReq, dwReqLen,
                                             (uint8 *)pAMIYAFUWriteFlashRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUEraseFlash(IPMI20_SESSION_T *pSession,
	                AMIYAFUErashFlashReq_T* pAMIYAFUEraseFlashReq,
	                AMIYAFUErashFlashRes_T* pAMIYAFUEraseFlashRes,
	                int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUErashFlashRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_ERASE_FLASH,
                                             (uint8 *)pAMIYAFUEraseFlashReq, sizeof(AMIYAFUErashFlashReq_T),
                                             (uint8 *)pAMIYAFUEraseFlashRes, &dwResLen, timeout );
    return( wRet ); 
}

uint16 IPMICMD_AMIYAFUProtectFlash(IPMI20_SESSION_T *pSession,
	               AMIYAFUProtectFlashReq_T* pAMIYAFUProtectFlashReq,
	               AMIYAFUProtectFlashRes_T* pAMIYAFUProtectFlashRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUProtectFlashRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_PROTECT_FLASH,
                                             (uint8 *)pAMIYAFUProtectFlashReq, sizeof(AMIYAFUProtectFlashReq_T),
                                             (uint8 *)pAMIYAFUProtectFlashRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUEraseCopyFlash(IPMI20_SESSION_T *pSession,
	               AMIYAFUEraseCopyFlashReq_T* pAMIYAFUEraseCopyFlashReq,
	               AMIYAFUEraseCopyFlashRes_T* pAMIYAFUEraseCopyFlashRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUEraseCopyFlashRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_ERASE_COPY_FLASH,
                                             (uint8 *)pAMIYAFUEraseCopyFlashReq, sizeof(AMIYAFUEraseCopyFlashReq_T),
                                             (uint8 *)pAMIYAFUEraseCopyFlashRes, &dwResLen, timeout );
    return( wRet );
}
uint16 IPMICMD_AMIYAFUGetECFStatus(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetECFStatusReq_T* pAMIYAFUGetECFStatusReq,
	               AMIYAFUGetECFStatusRes_T* pAMIYAFUGetECFStatusRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUGetECFStatusRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_ECF_STATUS,
                                             (uint8 *)pAMIYAFUGetECFStatusReq, sizeof(AMIYAFUGetECFStatusReq_T),
                                             (uint8 *)pAMIYAFUGetECFStatusRes, &dwResLen, timeout );
    return( wRet );
}


uint16 IPMICMD_AMIYAFUVerifyFlash(IPMI20_SESSION_T *pSession,
	               AMIYAFUVerifyFlashReq_T* pAMIYAFUVerifyFlashReq,
	               AMIYAFUVerifyFlashRes_T* pAMIYAFUVerfyFlashRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUVerifyFlashRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_VERIFY_FLASH,
                                             (uint8 *)pAMIYAFUVerifyFlashReq, sizeof(AMIYAFUVerifyFlashReq_T),
                                             (uint8 *)pAMIYAFUVerfyFlashRes, &dwResLen, timeout );
    return( wRet );
}
uint16 IPMICMD_AMIYAFUGetVerifyStatus(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetVerifyStatusReq_T* pAMIYAFUGetVerifyStatusReq,
	               AMIYAFUGetVerifyStatusRes_T* pAMIYAFUGetVerifyStatusRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUGetVerifyStatusRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_VERIFY_STATUS,
                                             (uint8 *)pAMIYAFUGetVerifyStatusReq, sizeof(AMIYAFUGetVerifyStatusReq_T),
                                             (uint8 *)pAMIYAFUGetVerifyStatusRes, &dwResLen, timeout );
    return( wRet );

}

uint16 IPMICMD_AMIYAFUReadMemory(IPMI20_SESSION_T *pSession,
	                AMIYAFUReadMemoryReq_T* pAMIYAFUReadMemoryReq,
	                AMIYAFUReadMemoryRes_T* pAMIYAFUReadMemoryRes,
	                int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUReadMemoryRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_READ_MEMORY,
                                             (uint8 *)pAMIYAFUReadMemoryReq, sizeof(AMIYAFUReadMemoryReq_T),
                                             (uint8 *)pAMIYAFUReadMemoryRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUWriteMemory(IPMI20_SESSION_T *pSession,
	               AMIYAFUWriteMemoryReq_T* pAMIYAFUWriteMemoryReq,
	               AMIYAFUWriteMemoryRes_T* pAMIYAFUWriteMemoryRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUWriteMemoryRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_WRITE_MEMORY,
                                             (uint8 *)pAMIYAFUWriteMemoryReq,sizeof(AMIYAFUWriteMemoryReq_T)+pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen,
                                             (uint8 *)pAMIYAFUWriteMemoryRes, &dwResLen, timeout );
    return( wRet ); 
}

uint16 IPMICMD_AMIYAFUCopyMemory(IPMI20_SESSION_T *pSession,
	               AMIYAFUCopyMemoryReq_T* pAMIYAFUCopyMemoryReq,
	               AMIYAFUCopyMemoryRes_T* pAMIYAFUCopyMemoryRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUCopyMemoryRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_COPY_MEMORY,
                                             (uint8 *)pAMIYAFUCopyMemoryReq, sizeof(AMIYAFUCopyMemoryReq_T),
                                             (uint8 *)pAMIYAFUCopyMemoryRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUCompareMemory(IPMI20_SESSION_T *pSession,
	                AMIYAFUCompareMemoryReq_T* pAMIYAFUCompareMemoryReq,
	                AMIYAFUCompareMemoryRes_T* pAMIYAFUCompareMemoryRes,
	                int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUCompareMemoryRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_COMPARE_MEMORY,
                                             (uint8 *)pAMIYAFUCompareMemoryReq, sizeof(AMIYAFUCompareMemoryReq_T),
                                             (uint8 *)pAMIYAFUCompareMemoryRes, &dwResLen, timeout );
    return( wRet );
} 

uint16 IPMICMD_AMIYAFUClearMemory(IPMI20_SESSION_T *pSession,
	               AMIYAFUClearMemoryReq_T* pAMIYAFUClearMemoryReq,
	               AMIYAFUClearMemoryRes_T* pAMIYAFUClearMemoryRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUClearMemoryRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_CLEAR_MEMORY,
                                             (uint8 *)pAMIYAFUClearMemoryReq, sizeof(AMIYAFUClearMemoryReq_T),
                                             (uint8 *)pAMIYAFUClearMemoryRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUGetBootConfig(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetBootConfigReq_T* pAMIYAFUGetBootConfigReq,
	               AMIYAFUGetBootConfigRes_T* pAMIYAFUGetBootConfigRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUGetBootConfigRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_BOOT_CONFIG,
                                             (uint8 *)pAMIYAFUGetBootConfigReq, sizeof(AMIYAFUGetBootConfigReq_T),
                                             (uint8 *)pAMIYAFUGetBootConfigRes, &dwResLen, timeout );
    return( wRet );
}


uint16 IPMICMD_AMIYAFUSetBootConfig(IPMI20_SESSION_T *pSession,
	               AMIYAFUSetBootConfigReq_T* pAMIYAFUSetBootConfigReq,
	               AMIYAFUSetBootConfigRes_T* pAMIYAFUSetBootConfigRes,
	               int timeout,uint32 ReqLen)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(AMIYAFUSetBootConfigRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_SET_BOOT_CONFIG,
                                             (uint8 *)pAMIYAFUSetBootConfigReq,ReqLen,
                                             (uint8 *)pAMIYAFUSetBootConfigRes, &dwResLen, timeout); 
    return( wRet );
}

uint16 IPMICMD_AMIYAFUGetAllBootVars(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetBootVarsReq_T* pAMIYAFUGetBootVarsReq,
	               AMIYAFUGetBootVarsRes_T* pAMIYAFUGetBootVarsRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUGetBootVarsRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_GET_BOOT_VARS,
                                             (uint8 *)pAMIYAFUGetBootVarsReq, sizeof(AMIYAFUGetBootVarsReq_T),
                                             (uint8 *)pAMIYAFUGetBootVarsRes, &dwResLen, timeout );
    return( wRet );
}



uint16 IPMICMD_AMIYAFUDeactivateFlash(IPMI20_SESSION_T *pSession,
	               AMIYAFUDeactivateFlashReq_T* pAMIYAFUDeactivateFlashReq,
	               AMIYAFUDeactivateFlashRes_T* pAMIYAFUDeactivateFlashRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUDeactivateFlashRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_DEACTIVATE_FLASH_MODE,
                                             (uint8 *)pAMIYAFUDeactivateFlashReq, sizeof(AMIYAFUDeactivateFlashReq_T),
                                             (uint8 *)pAMIYAFUDeactivateFlashRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUResetDevice(IPMI20_SESSION_T *pSession,
	               AMIYAFUResetDeviceReq_T* pAMIYAFUResetDeviceReq,
	               AMIYAFUResetDeviceRes_T* pAMIYAFUResetDeviceRes,
	               int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUResetDeviceRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_RESET_DEVICE,
                                             (uint8 *)pAMIYAFUResetDeviceReq,sizeof(AMIYAFUResetDeviceReq_T),
                                             (uint8 *)pAMIYAFUResetDeviceRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUDualImageSupport(IPMI20_SESSION_T *pSession,
                    AMIYAFUDualImgSupReq_T* pAMIYAFUDualImgSupReq,
                    AMIYAFUDualImgSupRes_T* pAMIYAFUDualImgSupRes,
                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUDualImgSupRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                ( ( NetFn_OEM << 2 ) + 0x00 ),CMD_AMI_YAFU_DUAL_IMAGE_SUP, 
                                (uint8 *)pAMIYAFUDualImgSupReq, sizeof(AMIYAFUDualImgSupReq_T),
                                (uint8*)pAMIYAFUDualImgSupRes,&dwResLen,timeout);

    return wRet;
}

uint16 IPMICMD_AMIYAFUFirmwareSelectFlash(IPMI20_SESSION_T *pSession,
	                    AMIYAFUFWSelectFlashModeReq_T* pAMIYAFUFirmwareSelectFlashReq,
	                    AMIYAFUFWSelectFlashModeRes_T* pAMIYAFUFirmwareSelectFlashRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUFWSelectFlashModeRes_T );

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_FIRMWARE_SELECT_FLASH,
                                             (uint8 *)pAMIYAFUFirmwareSelectFlashReq, sizeof(AMIYAFUFWSelectFlashModeReq_T),
                                             (uint8 *)pAMIYAFUFirmwareSelectFlashRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIMiscellaneousInfo(IPMI20_SESSION_T *pSession,
	                    AMIYAFUMiscellaneousReq_T* pAMIYAFUMiscellaneousReq,
	                    AMIYAFUMiscellaneousRes_T* pAMIYAFUMiscellaneousRes,
	                    int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUMiscellaneousRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_MISCELLANEOUS_INFO,
                                             (uint8 *)pAMIYAFUMiscellaneousReq, sizeof(AMIYAFUMiscellaneousReq_T),
                                             (uint8 *)pAMIYAFUMiscellaneousRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMIYAFUCompareMeVersion(IPMI20_SESSION_T *pSession,
                        AMIYAFUCompareMeVersionReq_T* pAMIYAFUCompareMeVersionReq,
                        AMIYAFUCompareMeVersionRes_T* pAMIYAFUCompareMeVersionRes,
                        int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIYAFUCompareMeVersionRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NetFn_OEM << 2 ) + 0x00 ), CMD_AMI_YAFU_COMPARE_ME_VERSION,
                                            (uint8 *)pAMIYAFUCompareMeVersionReq, sizeof(AMIYAFUCompareMeVersionReq_T),
                                            (uint8 *)pAMIYAFUCompareMeVersionRes, &dwResLen, timeout );
    return( wRet );
}

