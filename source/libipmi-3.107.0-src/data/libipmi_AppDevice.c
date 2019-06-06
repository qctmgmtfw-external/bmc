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
* Filename: libipmi_AppDevice.c
*
* Descriptions: Contains implementation of application network 
*   function IPMI commands
*
* Author: Rajasekhar
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_AppDevice.h"
#include "IPMI_AMIConf.h"
#include "libipmi_AMIOEM.h"
#if ( _WIN64 || _WIN32 ) 
#define snprintf sprintf_s
#endif

#include "string.h"
#include "dbgout.h"
/**
\breif	Executes GetUser IPMI Command.
 @param	pSession		[in]Session handle
 @param	pReqGetUser		[in]Get User Request Data
 @param	pResGetUser		[out]Get User Response Data
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/


uint16	IPMICMD_GetUser(IPMI20_SESSION_T *pSession, GetUserNameReq_T	*pReqGetUser,
							GetUserNameRes_T	*pResGetUser, int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetUserNameRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_USER_NAME,
											(uint8*)pReqGetUser, sizeof(GetUserNameReq_T),
											(uint8 *)pResGetUser, &dwResLen,
											timeout);

	return wRet;
}

/**
\breif	Executes SetUser IPMI Command.
 @param	pSession		[in]Session handle
 @param	pReqSetUser		[in]Set User Request Data
 @param	pResSetUser		[out]Set User Response Data
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16	IPMICMD_SetUser(IPMI20_SESSION_T *pSession, SetUserNameReq_T	*pReqSetUser,
							SetUserNameRes_T	*pResSetUser, int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(SetUserNameRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_SET_USER_NAME,
											(uint8*)pReqSetUser, sizeof(SetUserNameReq_T),
											(uint8 *)pResSetUser, &dwResLen,
											timeout);
	return wRet;
}


uint16	IPMICMD_SetUserPassword(IPMI20_SESSION_T *pSession, SetUserPswdReq_T	*pReqSetUserPswd,
							SetUserPswdRes_T	*pResSetUserPswd, int timeout)
{
	uint16	wRet;
	uint32	dwResLen, dwReqLen;

	dwResLen = sizeof(SetUserPswdRes_T);
	if ((0 == (pReqSetUserPswd->UserID & TWENTY_BYTE_PWD)) && 
	    (IPMI15_MAX_PASSWORD_LEN >= strlen((char *)pReqSetUserPswd->Password)))
	{
	    dwReqLen = sizeof(SetUserPswdReq_T) - MAX_PASSWORD_LEN + IPMI15_MAX_PASSWORD_LEN;
	}
	else
	{
	    dwReqLen = sizeof(SetUserPswdReq_T);
	}
		
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_SET_USER_PASSWORD,
											(uint8*)pReqSetUserPswd, dwReqLen,
											(uint8 *)pResSetUserPswd, &dwResLen,
											timeout);
	return wRet;
}



uint16 IPMICMD_SetUserAccess(IPMI20_SESSION_T* pSession,
							 SetUserAccessReq_T* pReqSetUserAccess,
							 SetUserAccessRes_T* pResSetUserAccess,
							 int timeout)
{
	uint16 wRet;
	uint32 dwResLen;

	dwResLen = sizeof(SetUserAccessRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_SET_USER_ACCESS,
											(uint8*)pReqSetUserAccess, sizeof(SetUserAccessReq_T),
											(uint8*)pResSetUserAccess, &dwResLen,
											timeout);
	return wRet;
}


uint16 IPMICMD_GetUserAccess(IPMI20_SESSION_T* pSession,
							 GetUserAccessReq_T* pReqGetUserAccess,
							 GetUserAccessRes_T* pResGetUserAccess,
							 int timeout)
{
	uint16 wRet;
	uint32 dwResLen;


	dwResLen = sizeof(GetUserAccessRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_USER_ACCESS,
											(uint8*)pReqGetUserAccess, sizeof(GetUserAccessReq_T),
											(uint8 *)pResGetUserAccess, &dwResLen,
											timeout);

	return wRet;

}

uint16 IPMICMD_GetUserAccessPayload(IPMI20_SESSION_T* pSession,
							 GetUsrPayldAccReq_T* pReqGetUserAccessPayload,
							 GetUsrPayldAccRes_T* pResGetUserAccessPayload,
							 int timeout)
{
	uint16 wRet;
	uint32 dwResLen;


	dwResLen = sizeof(GetUsrPayldAccRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_USR_PAYLOAD_ACCESS,
                                           (uint8*)pReqGetUserAccessPayload, sizeof(GetUsrPayldAccReq_T),
											(uint8 *)pResGetUserAccessPayload, &dwResLen,
											timeout);

	return wRet;

}

uint16 IPMICMD_SetUserAccessPayload(IPMI20_SESSION_T* pSession,
							 SetUsrPayldAccReq_T* pReqSetUserAccessPayload,
							 SetUsrPayldAccRes_T* pResSetUserAccessPayload,
							 int timeout)
{
	uint16 wRet;
	uint32 dwResLen;


	dwResLen = sizeof(SetUsrPayldAccRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_SET_USR_PAYLOAD_ACCESS,
                                           (uint8*)pReqSetUserAccessPayload, sizeof(SetUsrPayldAccReq_T),
											(uint8 *)pResSetUserAccessPayload, &dwResLen,
											timeout);

	return wRet;

}



uint16 IPMICMD_GetChannelInfo(IPMI20_SESSION_T* pSession,
							 GetChInfoReq_T* pReqGetChInfo,
							 GetChInfoRes_T* pResGetChInfo,
							 int timeout)
{

	uint16 wRet;
	uint32 dwResLen;

	dwResLen = sizeof(GetChInfoRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_CH_INFO,
											(uint8*)pReqGetChInfo, sizeof(GetChInfoReq_T),
											(uint8 *)pResGetChInfo, &dwResLen,
											timeout);
	return wRet;
}

uint16 IPMICMD_GetChannelAccess(IPMI20_SESSION_T* pSession,
							 GetChAccessReq_T* pReqGetChAccess,
							 GetChAccessRes_T* pResGetChAccess,
							 int timeout)
{

	uint16 wRet;
	uint32 dwResLen;

	dwResLen = sizeof(GetChAccessRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_CH_ACCESS,
											(uint8*)pReqGetChAccess, sizeof(GetChAccessReq_T),
											(uint8 *)pResGetChAccess, &dwResLen,
											timeout);
	return wRet;
}

uint16 IPMICMD_SetChannelAccess(IPMI20_SESSION_T* pSession,
							 SetChAccessReq_T* pReqSetChAccess,
							 SetChAccessRes_T* pResSetChAccess,
							 int timeout)
{

	uint16 wRet;
	uint32 dwResLen;

	dwResLen = sizeof(SetChAccessRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_SET_CH_ACCESS,
											(uint8*)pReqSetChAccess, sizeof(SetChAccessReq_T),
											(uint8 *)pResSetChAccess, &dwResLen,
											timeout);
	return wRet;
}

uint16 IPMICMD_GetSessionInfo(IPMI20_SESSION_T* pSession,
							 GetSesInfoReq_T* pReqGetSesInfo,
							 GetSesInfoRes_T* pResGetSesInfo,
							 int timeout)
{

	uint16 wRet;
	uint32 dwResLen;
    uint32 dwReqLen = 0;

	dwResLen = sizeof(GetSesInfoRes_T);

    switch (pReqGetSesInfo->SessionIndex)
    {
    case 0:
        dwReqLen = 1;
        break;
    case 0xFF:
        dwReqLen = 5;
        break;

    case 0xFE:
        dwReqLen = 2;
        break;

    default:
        dwReqLen = 1;
        TWARN("GetSessionInfo called for probably Nth active session\n");
        break;
    }

    

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_SESSION_INFO,
											(uint8*)pReqGetSesInfo, dwReqLen,
											(uint8 *)pResGetSesInfo, &dwResLen,
											timeout);
	return wRet;
}

uint16 IPMICMD_GetSysInfoParam(IPMI20_SESSION_T* pSession,
						     GetSystemInfoParamReq_T* pReqGetSysInfo,
						     GetSystemInfoParamRes_T* pResGetSysInfo,
							 int timeout)
{
	uint16 wRet;
	uint32 dwResLen = 2;
    uint32 dwReqLen;
    
    dwReqLen = sizeof(GetSystemInfoParamReq_T);
    
    switch(pReqGetSysInfo->ParamSelector)
    {
    case SET_IN_PROGRESS_PARAM:
    	dwResLen += sizeof(pResGetSysInfo->SysInfo.SetInProgress);
    	break;
    case SYS_FW_VERSION_PARAM:
    	dwResLen += sizeof(pResGetSysInfo->SysInfo.SysVerInfo);
    	break;
    case SYS_NAME_PARAM:
    	dwResLen += sizeof(pResGetSysInfo->SysInfo.SysNameInfo);
    	break;
    case PRIM_OS_NAME_PARAM:
    	dwResLen += sizeof(pResGetSysInfo->SysInfo.PrimOSInfo);
    	break;
    case OS_NAME_PARAM:
    	dwResLen += sizeof(pResGetSysInfo->SysInfo.OSInfo);
    	break;
    }
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
    											DEFAULT_NET_FN_LUN, CMD_GET_SYSTEM_INFO_PARAM,
    											(uint8*)pReqGetSysInfo, dwReqLen,
    											(uint8 *)pResGetSysInfo, &dwResLen,
    											timeout);
    	return wRet;

}


uint16 IPMICMD_GetWDT(IPMI20_SESSION_T* pSession,
							 GetWDTRes_T* pResGetWDT,
							 int timeout)
{
	uint8	Req[20];
	uint16 wRet;
	uint32 dwResLen;

	dwResLen = sizeof(GetWDTRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_WDT,
											Req, 0,
											(uint8 *)pResGetWDT, &dwResLen,
											timeout);
	return wRet;
}

/**
\breif	Executes GetSystemGUID IPMI command.
 @param	pSession		[in]Session handle
 @param	pGetDeviceID	[out]System GUID structure according to IPMI Spec
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16	IPMICMD_GetSystemGUID( IPMI20_SESSION_T *pSession, GetSysGUIDRes_T *pGetSystemGUID, int timeout )
{
	uint8		Req [20];
	uint32		dwResLen;
	uint16		wRet;

	dwResLen = sizeof(GetSysGUIDRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_SYSTEM_GUID,
											Req, 0,
											(uint8*)pGetSystemGUID, &dwResLen,
											timeout);
	return wRet;
}


uint16 IPMICMD_SendMessage(IPMI20_SESSION_T *pSession,uint8 *pSendMsgReq,
                                                          uint8 *pSendMsgRes,uint32 SendMsgReqLen,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(SendMsgRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_SEND_MSG,
											pSendMsgReq, SendMsgReqLen,
											pSendMsgRes, &dwResLen,
											timeout);    
    return wRet;
}

/**************************************************************************************
*************************************Higher level commands*****************************
**************************************************************************************/

uint16 LIBIPMI_HL_GetMaxNumUsers(IPMI20_SESSION_T* pSession,
									  INT8U *nNumUsers, int timeout)
{

	uint16 wRet;
	GetUserAccessReq_T ReqGetUserAccess;
	GetUserAccessRes_T ResGetUserAccess;

	//use this sessions channel number
	/*using this sessions channel no - the assumption is valid only for this product
	and not for all IPMI BMCs
	this is becuase there max no of user IDs may be different for each channel..though
	I wonder why anyone would implement it that way*/
	//if(pSession->hNetworkSession != NULL)
		//ReqGetUserAccess.ChannelNum = pSession->hNetworkSession->byChannelNum;
	if(pSession->hUDSSession != NULL)
    {
        ReqGetUserAccess.ChannelNum = pSession->hUDSSession->byChannelNum;
    }
    else if(pSession->hNetworkSession != NULL)
    {
        ReqGetUserAccess.ChannelNum = pSession->hNetworkSession->byChannelNum;
    }

	ReqGetUserAccess.UserID = 1; //we just do 1 here in order to get the max no of users


	//first do GetUserAccess to determine the no of users
	wRet = IPMICMD_GetUserAccess(pSession,
								&ReqGetUserAccess,
								&ResGetUserAccess,
								timeout);

	if(wRet == 0)
	{
		TDBG("Successful getuseraccess to find no of users\n");
		*nNumUsers = ResGetUserAccess.MaxNoUserID;
	}
	else
	{
		TDBG("Failure in getuseraccess to find no of users\n");
		*nNumUsers = 0;
	}

	return wRet;

}

uint16 LIBIPMI_HL_FindChannel(IPMI20_SESSION_T* pSession,
									  INT8U ChannelMediumToFind,
									  INT8* nChannelNum, int timeout)
{
	uint16 wRet;
	GetChInfoReq_T ReqGetChInfo = {0};
	GetChInfoRes_T ResGetChInfo;
	INT8U i;

	for(i=*nChannelNum;i<MAX_CHANNEL_NUM;i++)
	{
		ReqGetChInfo.ChannelNum = i;
		wRet = IPMICMD_GetChannelInfo(pSession,
									  &ReqGetChInfo,
									  &ResGetChInfo,
									  timeout);
		if(wRet != 0)
		{
			TDBG("Error getting channel info in FindNetworkChannel for number %d\n",i);
			//*nChannelNum = -1;
			continue;
			//return wRet;
		}

		if((ResGetChInfo.ChannelMedium == ChannelMediumToFind) && (i != 0x0e))
		{
			*nChannelNum = i;
			TDBG("Found channel medium %x at %d\n",ChannelMediumToFind,*nChannelNum);
			return 0;
		}
	}

	//we couldnt find it
	//*nChannelNum = -1;
	return -1;
}


uint16 LIBIPMI_HL_GetChannelAccess(IPMI20_SESSION_T* pSession,
                                   INT8U ChannelMedium,
                                      GetChAccessRes_T* pChAccessRes, int timeout)
{
	uint16 wRet;
	GetChAccessReq_T ReqGetChAccess = {0,0};

	INT8 nChannelNo;

    wRet = LIBIPMI_HL_FindChannel(pSession,ChannelMedium,&nChannelNo,timeout);
    if(wRet != 0)
    {
        TCRIT("Error finding channel no for channel medium %d in  GetChannelAcces",ChannelMedium);
        return wRet;
    }

    //now do a get Channel access

    ReqGetChAccess.ChannelNum = nChannelNo;
    ReqGetChAccess.AccessFlag |= 0x40;
    wRet = IPMICMD_GetChannelAccess(pSession,&ReqGetChAccess,pChAccessRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error getting channel access in GetChannelAcces and error is %x",wRet);
        return wRet;
    }



    return wRet;
}


uint16 LIBIPMI_HL_SetChannelAccessPrivilege(IPMI20_SESSION_T* pSession,
                                   INT8U ChannelMedium,INT8U Privilege,
                                      SetChAccessRes_T* pChAccessRes, int timeout)
{
	uint16 wRet;
	SetChAccessReq_T ReqSetChAccess = {0,0,0};

	INT8 nChannelNo;

    wRet = LIBIPMI_HL_FindChannel(pSession,ChannelMedium,&nChannelNo,timeout);
    if(wRet != 0)
    {
        TCRIT("Error finding channel no for channel medium %d in  SetChannelAcces",ChannelMedium);
        return wRet;
    }

    //now do a get Channel access
    ReqSetChAccess.ChannelNum = nChannelNo;
    ReqSetChAccess.ChannelAccess = 0x0;
    ReqSetChAccess.Privilege = 0x40 | Privilege; //non volatile
    wRet = IPMICMD_SetChannelAccess(pSession,&ReqSetChAccess,pChAccessRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error setting channel access in SetChannelAcces ");
        return wRet;
    }

    

    return wRet;
}




uint16 LIBIPMI_HL_GetAllUserInfo(IPMI20_SESSION_T *pSession,
							INT8U* nNumUsers,
							GetUserNameRes_T* pResGetUserName,
							GetUserAccessRes_T* pResGetUserAccess_Network,
							GetUserAccessRes_T* pResGetUserAccess_Serial,
                            GetUsrPayldAccRes_T* pResGetUserAccessPayload_SOL,
							int timeout)
{

	uint16 wRet;
	GetUserNameReq_T	ReqGetUser;
	GetUserAccessReq_T	ReqGetUserAccess;
    GetUsrPayldAccReq_T ReqGetUserAccessPayload;
	INT8 nNetworkChNo = 0;
	INT8 nSerialChNo = 0;
	int i;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	wRet = LIBIPMI_HL_GetMaxNumUsers(pSession,nNumUsers,timeout);

	if(wRet != 0)
	{
		TCRIT("Error getting max num users\n");
		return wRet;
	}

	TDBG("Number of users is %d\n",*nNumUsers);

	//now accumulate all users
	for(i=1;i<=*nNumUsers;i++)
	{
			ReqGetUser.UserID = i;
			//go get user name
			wRet = IPMICMD_GetUser(pSession,&ReqGetUser,&pResGetUserName[i],timeout);
			if(wRet == 0)
			{
				TDBG("Successfull get of user name");
				if(pResGetUserName[i].UserName[0] == 0)
				{
					if(i == 1)
					{
						tempret=snprintf((char*)pResGetUserName[i].UserName,sizeof(pResGetUserName[i].UserName),"%s",(char*)"anonymous");
						if((tempret<0)||(tempret>=(signed)sizeof(pResGetUserName[i].UserName)))
                                                {
                                                    TCRIT("Buffer Overflow");
                                                    return -1;
                                                }
					}
				}
			}
			else
			{
				if( IS_IPMI_ERROR(wRet) && (GET_ERROR_CODE(wRet) == CC_INV_DATA_FIELD) ) //invlaid data field
				{
					//if we get here the user is not configured
					//this is because the stack returns this if the user is not configured
					//TCRIT("Found a not yet configured user \n");
					//so we set pResGetUserName[i] to NULL
					pResGetUserName[i].UserName[0] = 0;
				}
				else
				{
					TCRIT("error getting user name\n");
				}
			}

            //Get channel information about network channel
        	wRet = LIBIPMI_HL_FindChannel(pSession, 0x04, &nNetworkChNo, timeout);
            //TCRIT_ASSERT((wRet == 0),"Could not find Network Channel number\n");

            if (wRet == 0)
            {
    			//now get network ch access perms for this user
    			ReqGetUserAccess.ChannelNum = nNetworkChNo;
    			ReqGetUserAccess.UserID = i;
    			wRet = IPMICMD_GetUserAccess(pSession,&ReqGetUserAccess,
    										&pResGetUserAccess_Network[i],
    										timeout);
    			if(wRet != 0)
    			{
    				TCRIT("Could not get permissions on network channel\n");
    				return wRet;
    			}
    			else
    			{
    				TDBG("perm got successfully for network channel\n");
    				TDBG("user access perms are %x\n",pResGetUserAccess_Network[i].ChannelAccess);
    			}
            }

            //Get channel information about network channel
        	wRet = LIBIPMI_HL_FindChannel(pSession, 0x05, &nSerialChNo, timeout);
            //TCRIT_ASSERT((wRet == 0),"Could not find Network Channel number\n");

            if (wRet == 0)
            {
    			ReqGetUserAccess.ChannelNum = nSerialChNo;
    			ReqGetUserAccess.UserID = i;
    			wRet = IPMICMD_GetUserAccess(pSession,&ReqGetUserAccess,
    										&pResGetUserAccess_Serial[i],
    										timeout);
    			if(wRet != 0)
    			{
    				TCRIT("Could not get permissions on serial channel\n");
    				return wRet;
    			}
    			else
    			{
    				TDBG("perm got successfully for serial channel\n");
                    
    			}
            }
            else
            {
                 /*IPMI returns CC - INV Data Field when the Serial is diabled
                 So fill the Serial Priv as NO ACCESS  for this User*/
                 pResGetUserAccess_Serial[i].ChannelAccess=0x0f;
            }

            //get payload access
            memset(&pResGetUserAccessPayload_SOL[i],0,sizeof(GetUsrPayldAccRes_T));
            ReqGetUserAccessPayload.ChannelNum = nNetworkChNo;
            ReqGetUserAccessPayload.UserId = i;
            wRet = IPMICMD_GetUserAccessPayload(pSession,&ReqGetUserAccessPayload,
                                        &pResGetUserAccessPayload_SOL[i],
                                        timeout);
            if(wRet != 0)
            {
                if(IS_IPMI_ERROR(wRet) && GET_ERROR_CODE(wRet) == CC_INV_DATA_FIELD)
                {
                    //probably disabled user
                    if((pResGetUserAccess_Network[i].CurrentUserID & 0xC0)>>6 == 0x02)
                    {
                        TDBG("user is anyway disbaled. Can safely ignore this error\n");
                    }
                    else
                    {
                        TCRIT("Got invalid data field for SOl permissions eventhough userr is not disabled..adjust\n");
			//TBD WE MAY HAVE A CASE WHEN PLAYING WITH ICTS WHERE
			//USERNAME MAY BE ADDED AND HE MAY BE NABLED BUT PAYLOAD ACCESS
			//MAY RETURN INV DATA FIELD. SO WE IGNORE FOR NOW
                        //return wRet;
                    }
                }
                else
                {
                    TCRIT("Could not get permissions for SOL retunr value is %d\n",wRet);
                    return wRet;
                }
                
            }
            else
            {
                TDBG("perm got successfully for sol\n");

            }

	}
	return 0;
}



uint16 LIBIPMI_HL_GetOneUserInfo(IPMI20_SESSION_T *pSession,
                            int UserID,
                            GetUserNameRes_T* pResGetUserName,
							GetUserAccessRes_T* pResGetUserAccess_Network,
							GetUserAccessRes_T* pResGetUserAccess_Serial,
                            GetUsrPayldAccRes_T* pResGetUserAccessPayload_SOL,
							int timeout)
{

	uint16 wRet;
	GetUserNameReq_T	ReqGetUser;
	GetUserAccessReq_T	ReqGetUserAccess;
    GetUsrPayldAccReq_T ReqGetUserAccessPayload;
	INT8 nNetworkChNo = 0;
	INT8 nSerialChNo = 0;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

        ReqGetUser.UserID = UserID;
        //go get user name
        wRet = IPMICMD_GetUser(pSession,&ReqGetUser,pResGetUserName,timeout);
        if(wRet == 0)
        {
            TDBG("Successfull get of user name");
            if(pResGetUserName->UserName[0] == 0)
            {
                if(UserID == 1)
                {
                    tempret=snprintf((char*)pResGetUserName->UserName,sizeof(pResGetUserName->UserName),"%s",(char*)"anonymous");
			  if((tempret<0)||(tempret>=(signed)sizeof(pResGetUserName->UserName)))
                          {
                              TCRIT("Buffer Overflow");
                              return -1;
                          }
                }
            }
        }
        else
        {

            if( IS_IPMI_ERROR(wRet) && (GET_ERROR_CODE(wRet) == CC_INV_DATA_FIELD) ) //invlaid data field
            {
                //if we get here the user is not configured
                //this is because the stack returns this if the user is not configured
                //TCRIT("Found a not yet configured user \n");
                //so we set pResGetUserName[i] to NULL
                pResGetUserName->UserName[0] = 0;

            }
            else
            {
                TCRIT("error getting user name\n");
            }
        }

    //Get channel information about network channel
	wRet = LIBIPMI_HL_FindChannel(pSession, 0x04, &nNetworkChNo, timeout);
    //TCRIT_ASSERT((wRet == 0),"Could not find Network Channel number\n");

    if (wRet == 0)
    {
        //now get network ch access perms for this user
        ReqGetUserAccess.ChannelNum = pSession->hUDSSession->byChannelNum;
        ReqGetUserAccess.UserID = UserID;
        wRet = IPMICMD_GetUserAccess(pSession,&ReqGetUserAccess,
                                    pResGetUserAccess_Network,
                                    timeout);
        if(wRet != 0)
        {
            TDBG("Could not get permissions on network channel\n");
            return wRet;
        }
        else
        {
            TDBG("perm got successfully for network channel\n");
            TDBG("user access perms are %x\n",pResGetUserAccess_Network->ChannelAccess);
        
        }
    }

    //Get channel information about serial channel
    wRet = LIBIPMI_HL_FindChannel(pSession, 0x05, &nSerialChNo, timeout);
	//TCRIT_ASSERT((wRet == 0),"Could not find serial channel number\n");

    if (wRet == 0)
    {
        ReqGetUserAccess.ChannelNum = nSerialChNo;
        ReqGetUserAccess.UserID = UserID;
        wRet = IPMICMD_GetUserAccess(pSession,&ReqGetUserAccess,
                                    pResGetUserAccess_Serial,
                                    timeout);
        if(wRet != 0)
        {
            TCRIT("Could not get permissions on serial channel\n");
            return wRet;
        }
        else
        {
            TDBG("perm got successfully for serial channel\n");
            
        }
    }

        //get payload access
        memset(pResGetUserAccessPayload_SOL,0,sizeof(GetUsrPayldAccRes_T));
        ReqGetUserAccessPayload.ChannelNum = pSession->hUDSSession->byChannelNum;
        ReqGetUserAccessPayload.UserId = UserID;
        wRet = IPMICMD_GetUserAccessPayload(pSession,&ReqGetUserAccessPayload,
                                    pResGetUserAccessPayload_SOL,
                                    timeout);
        if(wRet != 0)
        {
            if(IS_IPMI_ERROR(wRet) && GET_ERROR_CODE(wRet) == CC_INV_DATA_FIELD)
            {
                //probably disabled user
                if((pResGetUserAccess_Network->CurrentUserID & 0xC0)>>6 == 0x02)
                {
                    TDBG("user is anyway disbaled. Can safely ignore this error\n");
                }
                else
                {
                    TCRIT("Got invalid data field for SOl permissions eventhough userr is not disabled\n");
                    return wRet;
                }
            }
            else
            {
                TCRIT("Could not get permissions for SOL retunr value is %d\n",wRet);
                return wRet;
            }
            
        }
        else
        {
            TDBG("perm got successfully for sol\n");

        }

	return 0;
}



uint16 LIBIPMI_HL_AddUser(IPMI20_SESSION_T *pSession,
INT8U UserID,char* UserName,char* ClearPswd,INT8U NetworkPriv,INT8U SerialPriv,
int timeout)
{

	//the stack should validate the user name for us
	uint16 wRet;
	INT8 nNetworkChNo = 0;
	INT8 nSerialChNo = 0,i=0;
	SetUserNameReq_T ReqSetUserName = {0,"0"};
	SetUserNameRes_T ResSetUserName = {0};
	SetUserPswdReq_T ReqSetUserPswd = {0,0,"0"};
	SetUserPswdRes_T ResSetUserPswd = {0};
	SetUserAccessReq_T ReqSetUserAccess = {0,0,0,0};
	SetUserAccessRes_T ResSetUserAccess = {0};
	SetUsrPayldAccReq_T  ReqSetUserPayloadAccess ={0,0,"0"};
	SetUsrPayldAccRes_T  ResSetUserPayloadAccess ={0};

	
	TDBG("Data in add user is %d %s %s %x %x\n",UserID,UserName,ClearPswd,NetworkPriv,SerialPriv);

	ReqSetUserName.UserID = UserID;
	strncpy((char*)ReqSetUserName.UserName,(char*)UserName,MAX_USERNAME_LEN);
	wRet = IPMICMD_SetUser(pSession,&ReqSetUserName,&ResSetUserName,timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting user name\n");
		return wRet;
	}

	ReqSetUserPswd.UserID = UserID;
	ReqSetUserPswd.Operation = 0x2; //enable and set password
	strncpy((char*)ReqSetUserPswd.Password,(char*)ClearPswd,MAX_PASSWORD_LEN);
	wRet = IPMICMD_SetUserPassword(pSession,&ReqSetUserPswd,&ResSetUserPswd,timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting user password..Set password operation\n");
		return wRet;
	}

	//now do a enable
	ReqSetUserPswd.Operation = 0x1;
	wRet = IPMICMD_SetUserPassword(pSession,&ReqSetUserPswd,&ResSetUserPswd,timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting user password..enable operation\n");
		return wRet;
	}

    for(i=0;i<MAX_CHANNEL_NUM;i++) 
    {
        //Get channel information about network channel
        wRet = LIBIPMI_HL_FindChannel(pSession, 0x04, &nNetworkChNo, timeout);
        //TCRIT_ASSERT((wRet == 0),"Could not find Network Channel number\n");

        if (wRet == 0)
        {
            ReqSetUserAccess.UserID = UserID;
            ReqSetUserAccess.ChannelNoUserAccess = BIT7 | BIT5 | BIT4 | nNetworkChNo;
            ReqSetUserAccess.AccessLimit = NetworkPriv;
            wRet = IPMICMD_SetUserAccess(pSession,&ReqSetUserAccess,&ResSetUserAccess,timeout);
            if(wRet != 0)
            {
                TCRIT("Error setting user access for network\n");
                return wRet;
            }
            /* Setting the User Payload Acces */
            ReqSetUserPayloadAccess.UserId = UserID;
            ReqSetUserPayloadAccess.ChannelNum=nNetworkChNo;
            ReqSetUserPayloadAccess.PayloadEnables [0]=0x2;

            wRet = IPMICMD_SetUserAccessPayload(pSession,&ReqSetUserPayloadAccess,&ResSetUserPayloadAccess,timeout);
            if(wRet != 0)
            {
                TCRIT("Error setting user Setting Payload Access for User \n");
                return wRet;
            }
            nNetworkChNo++;
        }
        
    }
    //Get channel information about serial channel
    wRet = LIBIPMI_HL_FindChannel(pSession, 0x05, &nSerialChNo, timeout);
	//TCRIT_ASSERT((wRet == 0),"Could not find serial channel number\n");

    if (wRet == 0)
    {
    	ReqSetUserAccess.UserID = UserID;
    	ReqSetUserAccess.ChannelNoUserAccess = BIT7 | BIT5 | BIT4 | nSerialChNo;
    	ReqSetUserAccess.AccessLimit = SerialPriv;
    	wRet = IPMICMD_SetUserAccess(pSession,&ReqSetUserAccess,&ResSetUserAccess,timeout);
    	if(wRet != 0)
    	{
    		TCRIT("Error setting user access for serial\n");
    		return wRet;
    	}
    }

    // User added successfully
    return 0;
}

uint16 LIBIPMI_HL_ModUser(IPMI20_SESSION_T *pSession,
			INT8U UserID,INT8U IsPwdChange,
			char* UserName,char* ClearPswd,
			INT8U NetworkPriv,INT8U SerialPriv,INT8U EnableUser,
			INT8U ModifyUserName, INT8U EnableSOL,
			int timeout)
{
	//the stack should validate the user name for us
	uint16 wRet,i;
	INT8 nNetworkChNo = 0;
	INT8 nSerialChNo = 0;
	SetUserPswdReq_T ReqSetUserPswd = {0,0,"0"};
	SetUserPswdRes_T ResSetUserPswd = {0};
	SetUserAccessReq_T ReqSetUserAccess = {0,0,0,0};
	SetUserAccessRes_T ResSetUserAccess = {0};
	SetUserNameReq_T ReqSetUserName = {0,"0"};
	SetUserNameRes_T ResSetUserName = {0};
	SetUsrPayldAccReq_T ReqSetUserAccessPayload_SOL;
	SetUsrPayldAccRes_T ResSetUserAccessPayload_SOL;
	int CalledDisabled = 0;

	//we dont do changeusername in moduser..for that you delete
	//and readd a new user...that is the best thing to do with linux files
	if(ModifyUserName == 1)
	{
		ReqSetUserName.UserID = UserID & 0x7F;
		strncpy((char*)ReqSetUserName.UserName,(char*)UserName,MAX_USERNAME_LEN);
		wRet = IPMICMD_SetUser(pSession,&ReqSetUserName,&ResSetUserName,timeout);
		if(wRet != 0)
		{
			TCRIT("Error setting user name\n");
			return wRet;
		}
		else
		{
			TDBG("setting user name success\n");
		}
	}

	if(IsPwdChange)
	{
		ReqSetUserPswd.UserID = UserID;
		ReqSetUserPswd.Operation = 0x02; //enable and set password
		strncpy((char*)ReqSetUserPswd.Password,(char*)ClearPswd,MAX_PASSWORD_LEN);
		wRet = IPMICMD_SetUserPassword(pSession,&ReqSetUserPswd,&ResSetUserPswd,timeout);
		if(wRet != 0)
		{
			//Quanta---
			if(ResSetUserPswd.CompletionCode == CC_REQ_INV_LEN)
				return CC_REQ_INV_LEN;
			//---Quanta
			TCRIT("Error setting user password..Set password operation\n");
			return wRet;
		}
	}
	else
	{
		TDBG("Skipping pswd change..no password change\n");
	}

	//now do a enable if asked
	if(EnableUser)
	{
		TDBG("Doing a enable user\n");
		ReqSetUserPswd.UserID = UserID;
		ReqSetUserPswd.Operation = 0x1;
		wRet = IPMICMD_SetUserPassword(pSession,&ReqSetUserPswd,&ResSetUserPswd,timeout);
		if(wRet != 0)
		{
			TCRIT("Error setting user password..enable operation\n");
			return wRet;
		}
	}
	else
	{
		TDBG("Doing a disable user\n");
		CalledDisabled = 1;

		ReqSetUserPswd.UserID = UserID;
		ReqSetUserPswd.Operation = 0x0;
		wRet = IPMICMD_SetUserPassword(pSession,&ReqSetUserPswd,&ResSetUserPswd,timeout);
		if(wRet != 0)
		{
			TCRIT("Error setting user password..disable operation\n");
			return wRet;
		}
	}

	UserID = UserID & 0x7F;
//	if(UserID != 2) // We don't want to change any permissions for 'root'
//	{
		for(i=0;i<MAX_CHANNEL_NUM;i++) 
		{
			wRet = LIBIPMI_HL_FindChannel(pSession, 0x04, &nNetworkChNo, timeout);
			if (wRet == 0)
			{
				TDBG("Setting user privilege acces for channelno %x \n",nNetworkChNo);
				ReqSetUserAccess.UserID = UserID;
				ReqSetUserAccess.ChannelNoUserAccess = (EnableUser == 1) ? (BIT7 | BIT5 | BIT4 | (nNetworkChNo & 0x0F)) : (BIT7 | BIT5 | (nNetworkChNo & 0x0F));

				TDBG ("For Network privilege, ChannelNoUserAccess::%x\n",ReqSetUserAccess.ChannelNoUserAccess);
				ReqSetUserAccess.AccessLimit = NetworkPriv;
				wRet = IPMICMD_SetUserAccess(pSession,&ReqSetUserAccess,&ResSetUserAccess,timeout);
				if(wRet != 0)
				{
					//temporarily ok to have cc error since we stack does not permit setting permissions
					//for a disabled user
					if(IS_IPMI_ERROR(wRet) && (GET_ERROR_CODE(wRet) == CC_INV_DATA_FIELD) && CalledDisabled == 1) //invlaid data field
					{
						TCRIT("Error in set user access for reference::%x\n",wRet);
						TDBG("Invalid data fields\n");
					}
					else
					{
						TCRIT("Error setting permission in network, error other than invalid data field\n");
						return wRet;
					}
				}
				else
				{
					TDBG ("Success setting User Access for Network privilege\n");
				}

				memset(&ReqSetUserAccessPayload_SOL,0,sizeof(SetUsrPayldAccReq_T));
				ReqSetUserAccessPayload_SOL.ChannelNum = nNetworkChNo;
				ReqSetUserAccessPayload_SOL.UserId = UserID;
				if(EnableSOL)
				{
					TDBG("enabling SOL\n");
					ReqSetUserAccessPayload_SOL.UserId |= 0x00;
				}
				else
				{
					TDBG("disabling SOL\n");
					ReqSetUserAccessPayload_SOL.UserId |= 0x40;
				}
				ReqSetUserAccessPayload_SOL.PayloadEnables[0] = 0x02; //SOL payload
				wRet = IPMICMD_SetUserAccessPayload(pSession,&ReqSetUserAccessPayload_SOL,
								&ResSetUserAccessPayload_SOL, timeout);
				if(wRet != 0)
				{
					TCRIT("Error setting user payload\n");
				}
				else
				{
					TDBG("set user payload successful\n");
				}
			}
			nNetworkChNo++;
		}

		//Get channel information about serial channel
		wRet = LIBIPMI_HL_FindChannel(pSession, 0x05, &nSerialChNo, timeout);
		if (wRet == 0)
		{
			ReqSetUserAccess.UserID = UserID;
			ReqSetUserAccess.ChannelNoUserAccess = (EnableUser == 1) ? (BIT7 | BIT5 | BIT4 | (nSerialChNo & 0x0F)) : (BIT7 | BIT5 | (nSerialChNo & 0x0F));

			TDBG ("For Serial privilege, ChannelNoUserAccess::%x\n",ReqSetUserAccess.ChannelNoUserAccess);
			ReqSetUserAccess.AccessLimit = SerialPriv;
			wRet = IPMICMD_SetUserAccess(pSession,&ReqSetUserAccess,&ResSetUserAccess,timeout);
			if(wRet != 0)
			{
				//temporarily ok to have cc error since we stack does not permit setting permissions
				//for a disabled user
				if( IS_IPMI_ERROR(wRet) && (GET_ERROR_CODE(wRet) == CC_INV_DATA_FIELD) && CalledDisabled == 1 ) //invlaid data field
				{
					TDBG("Invalid data fields\n");
				}
				else
				{
					TCRIT("Error setting permission in serial, error other than invalid data field\n");
					return wRet;
				}
			}
			else
			{
				TDBG ("Success setting User Access for Serial privilege\n");
			}
		}
	//}
	
	return 0;
}

/*note that there is no del user operation in IPMI.
THis accomplishes by deluser by setting the user back to NULL*/
uint16 LIBIPMI_HL_DelUser(IPMI20_SESSION_T *pSession,INT8U UserID,int timeout)
{
	uint16 wRet;
	SetUserNameReq_T ReqSetUserName;
	SetUserNameRes_T ResSetUserName;

	ReqSetUserName.UserID = UserID;
	memset(ReqSetUserName.UserName,0xFF,MAX_USERNAME_LEN);
	wRet = IPMICMD_SetUser(pSession,&ReqSetUserName,&ResSetUserName,timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting user name to NULL in deluser\n");
	}

	return wRet;
}



/**
\breif	Higher level function for setting user info.
		Calls IPMICMD_SetUser internally.
 @param	pSession		[in]Session handle
 @param	pszUsername		[in]User Name
 @param	byID			[in]UserId
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 LIBIPMI_HL_SetUser( IPMI20_SESSION_T *pSession, char *pszUsername, uint8 byID, int timeout )
{
	int							nUserNameLen;
	//uint32						dwResLen;
	uint16						wRet;
	SetUserNameReq_T		ReqSetUserInfo;
	SetUserNameRes_T		ResSetUserInfo;

	//complications here
	//IPMI stack takes max no of users based on
	//let the IPMI stack respond with invalid user id..dont check it here
	if( (nUserNameLen=strlen(pszUsername)) == 0 )
	{
		TDBG("Invalid UserName\n");
		return STATUS_CODE(0, LIBIPMI_E_INVALID_USER_NAME);
	}

	memset(&ReqSetUserInfo, 0, sizeof(SetUserNameReq_T) );

	/* Truncate the username to 16 chars */
	(nUserNameLen > MAX_USERNAME_LEN)?nUserNameLen = MAX_USERNAME_LEN:nUserNameLen;

	/* Fill Request User Structure */
	ReqSetUserInfo.UserID = byID & ~(0xC0);
	memcpy(ReqSetUserInfo.UserName, pszUsername, nUserNameLen);

	//dwResLen = sizeof(SetUserNameRes_T);
	wRet = IPMICMD_SetUser(pSession, &ReqSetUserInfo, &ResSetUserInfo, timeout);

	if(wRet == LIBIPMI_E_SUCCESS)
		return STATUS_CODE(LIBIPMI_STATUS_SUCCESS, ResSetUserInfo.CompletionCode);

	return wRet;
}

/**
\breif	Higher level function for getting user info.
		Calls IPMICMD_GetUser internally.
 @param	pSession		[in]Session handle
 @param	pszUsername		[out]User Name
 @param	byID			[in]UserId
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 LIBIPMI_HL_GetUser( IPMI20_SESSION_T *pSession, char *pszUsername, uint8 byID, int timeout )
{
	uint32						dwResLen;
	uint16						wRet;
	GetUserNameReq_T		ReqGetUserInfo;
	GetUserNameRes_T		ResGetUserInfo;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/



	dwResLen = sizeof(GetUserNameRes_T);
	ReqGetUserInfo.UserID = byID & ~(0xC0);

	/* Call RAW IPMI Command */
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_USER_NAME,
											(uint8 *)&ReqGetUserInfo, sizeof(GetUserNameReq_T),
											(uint8 *)&ResGetUserInfo, &dwResLen,
											timeout);

	if(wRet == LIBIPMI_E_SUCCESS)
	{
		tempret=snprintf((char*)pszUsername,MAX_USERNAME_LEN,"%s",(char*)ResGetUserInfo.UserName);
            if((tempret<0)||(tempret>=MAX_USERNAME_LEN))
            {
                TCRIT("Buffer Overflow %s %s %d",(char*)ResGetUserInfo.UserName,(char*)pszUsername,tempret);
                return -1;
            }
		return STATUS_CODE(LIBIPMI_STATUS_SUCCESS, ResGetUserInfo.CompletionCode);
	}
	else
	{
		pszUsername[0] = (char)0;
	}

	return wRet;
}


//gets the session information for the current session
uint16 LIBIPMI_HL_GetSessionInfoCurrent( IPMI20_SESSION_T *pSession, GetSesInfoRes_T* pSesInfoRes, int timeout )
{
    GetSesInfoReq_T SesInfoReq;
    uint16 wRet = 0;

    SesInfoReq.SessionIndex = 0x00; //for this session

    wRet = IPMICMD_GetSessionInfo(pSession,&SesInfoReq,pSesInfoRes,timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error getting session info for this session\n");
    
    }
    return wRet;
}

//gets the system information for the current session

uint16 LIBIPMI_HL_GetSysInfo_SetInProgress( IPMI20_SESSION_T *pSession, GetSystemInfoParamRes_T* pSysInfoRes, int timeout )
{
	GetSystemInfoParamReq_T SysInfoReq;
	uint16 wRet = 0;
	
	SysInfoReq.ParamSelector = 0x00;
	SysInfoReq.ParamRev = 0x00;
	SysInfoReq.BlockSelector = 0x00;
	SysInfoReq.SetSelector = 0x00;
	
	wRet = IPMICMD_GetSysInfoParam(pSession,&SysInfoReq,pSysInfoRes,timeout);
	
	if(wRet != LIBIPMI_E_SUCCESS)
	{
	    TCRIT("Error getting System info parameters...\n");
	    
	}
	return wRet;
}

uint16 LIBIPMI_HL_GetSysInfo_FW_Version( IPMI20_SESSION_T *pSession, GetSystemInfoParamRes_T* pSysInfoRes, int timeout )
{
	GetSystemInfoParamReq_T SysInfoReq;
	uint16 wRet = 0;
	
	SysInfoReq.ParamSelector = 0x01;
	SysInfoReq.ParamRev = 0x00;
	SysInfoReq.BlockSelector = 0x00;
	SysInfoReq.SetSelector = 0x00;
	
	wRet = IPMICMD_GetSysInfoParam(pSession,&SysInfoReq,pSysInfoRes,timeout);
	
	if(wRet != LIBIPMI_E_SUCCESS)
	{
	    TCRIT("Error getting System Firmware Version Parameter...\n");
	    
	}
	return wRet;
}

uint16 LIBIPMI_HL_GetSysInfo_Name( IPMI20_SESSION_T *pSession, char* pSysInfoName, int timeout )
{
	GetSystemInfoParamReq_T SysInfoReq;
	GetSystemInfoParamRes_T SysInfoRes;
	uint8 i;
	char Temp[20]={0};
	uint16 wRet = 0;
	
	SysInfoReq.ParamSelector = 0x02;
	SysInfoReq.ParamRev = 0x00;
	SysInfoReq.BlockSelector = 0x00;
	SysInfoReq.SetSelector = 0x00;
	
	pSysInfoName[0] = (char)0;
	
	wRet = IPMICMD_GetSysInfoParam(pSession,&SysInfoReq,&SysInfoRes,timeout);
	
	if(wRet == LIBIPMI_E_SUCCESS)
	{
		uint8 *pbyRes;
		pbyRes = (uint8 *)&SysInfoRes.SysInfo.SysNameInfo;
		for(i=0;i<sizeof(SysInfoRes.SysInfo.SysNameInfo);i++)
		{
			if( sizeof(Temp) <= (unsigned) snprintf(Temp, sizeof(Temp), "%x ",pbyRes[i]))
				TCRIT("LIBIPMI_HL_GetSysInfo_Name - Source buffer is truncated");
			strcat(pSysInfoName,Temp);
			
		}
	    //TCRIT("Error getting System Name Parameter...\n");
	    
	}
	return wRet;
}

uint16 LIBIPMI_HL_GetSysInfo_Primary_OS_Name( IPMI20_SESSION_T *pSession, GetSystemInfoParamRes_T* pSysInfoRes, int timeout )
{
	GetSystemInfoParamReq_T SysInfoReq;
	uint16 wRet = 0;
	
	SysInfoReq.ParamSelector = 0x03;
	SysInfoReq.ParamRev = 0x00;
	SysInfoReq.BlockSelector = 0x00;
	SysInfoReq.SetSelector = 0x00;
	
	wRet = IPMICMD_GetSysInfoParam(pSession,&SysInfoReq,pSysInfoRes,timeout);
	
	if(wRet != LIBIPMI_E_SUCCESS)
	{
	    TCRIT("Error getting System Primary OS Name Parameter...\n");
	    
	}
	return wRet;
}

uint16 LIBIPMI_HL_GetSysInfo_OS_Name( IPMI20_SESSION_T *pSession, GetSystemInfoParamRes_T* pSysInfoRes, int timeout )
{
	GetSystemInfoParamReq_T SysInfoReq;
	uint16 wRet = 0;
	
	SysInfoReq.ParamSelector = 0x04;
	SysInfoReq.ParamRev = 0x00;
	SysInfoReq.BlockSelector = 0x00;
	SysInfoReq.SetSelector = 0x00;
	
	wRet = IPMICMD_GetSysInfoParam(pSession,&SysInfoReq,pSysInfoRes,timeout);
	
	if(wRet != LIBIPMI_E_SUCCESS)
	{
	    TCRIT("Error getting System OS Name Parameter...\n");
	    
	}
	return wRet;
}

/**
\breif	Higher level function for retrieving System GUID.
		Calls IPMICMD_GetSystenmGUID internally.
 @param	pSession		[in]Session handle
 @param	pszDeviceID		[out]System GUID in the form of chars
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 LIBIPMI_HL_GetSystemGUID( IPMI20_SESSION_T *pSession, char *pszSystemGUID, int timeout )
{
	uint8	i;
	uint16	wRet;
	char	szTemp[10];
	GetSysGUIDRes_T SystemGUID;

	pszSystemGUID[0] = (char)0;
	
	wRet = IPMICMD_GetSystemGUID( pSession, &SystemGUID, timeout);

	if (wRet == LIBIPMI_E_SUCCESS)
	{
		uint8	*pbyRes;
		pbyRes = (uint8 *) &SystemGUID;
		for (i = 0; i < (uint8)sizeof(GetSysGUIDRes_T); i++ )
		{
			if( sizeof(szTemp) <= (unsigned) snprintf (szTemp, sizeof(szTemp), "%x ", pbyRes[i]))
				TCRIT("LIBIPMI_HL_GetSystemGUID - Source buffer is truncated.");
			strcat(pszSystemGUID, szTemp);
		}
	}
	
	return wRet;
}

uint16 LIBIPMI_HL_GetUserPrivilege(IPMI20_SESSION_T *pSession, INT8U UserID, INT32U *Privilege, int timeout)
{
    uint16 wRet;
    GetChInfoReq_T ReqGetChInfo = {0};
    GetChInfoRes_T ResGetChInfo;
    GetUserAccessReq_T ReqGetUserAccess;
    GetUserAccessRes_T ResGetUserAccess;
    INT8U i, j = 0;

    if(NULL == Privilege)
    {
        return -1;
    }
    
    for(i=0; i<MAX_CHANNEL_NUM; i++)
    {
        ReqGetChInfo.ChannelNum = i;
        wRet = IPMICMD_GetChannelInfo(pSession, &ReqGetChInfo, &ResGetChInfo, timeout);
        if(wRet != 0)
        {
            TDBG("Error getting channel info in FindNetworkChannel for number %d\n",i);
            continue;
        }

        if(((LAN_CHANNEL_MEDIUM_TYPE == ResGetChInfo.ChannelMedium) || 
            (SERIAL_CHANNEL_MEDIUM_TYPE == ResGetChInfo.ChannelMedium)) && (i != 0x0e))
        {
            ReqGetUserAccess.UserID = UserID;
            ReqGetUserAccess.ChannelNum = i;
            wRet = IPMICMD_GetUserAccess(pSession, &ReqGetUserAccess, &ResGetUserAccess, timeout);
            if(wRet!=0)
            {
                TDBG("Big error getting user info\n");
                continue;
            }
            if((ResGetUserAccess.ChannelAccess & BIT4) ==0)
            {
                TDBG("User access is disabled \n");
                continue;
            }
            if((ResGetUserAccess.CurrentUserID & (BIT7 | BIT6)) == BIT7)
            {
                TDBG("User is disbaled\n");
                return -1;
            }
            
            *Privilege |= ((ResGetUserAccess.ChannelAccess & 0x0F) << (4 * j++));
        }
    }

    return 0;
}

