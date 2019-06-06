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
* Filename: libipmi_usermgmt.c
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_api.h"
#include "libipmi_usermgmt.h"

#include "string.h"


uint16 LIBIPMI_SetUser( IPMI20_SESSION_T *pSession, char *pszUsername, uint8 byID, int timeout )
{
	SetUser_T	UserInfo;
	int			nUserNameLen;
	char		byRes[MAX_RESPONSE_SIZE];
	uint32		dwResLen;
	uint16		wRet;
	
	
	if(byID	>= MAX_USERS)
	{
		DEBUG_PRINTF("Invalid UserID\n");
		return STATUS_CODE(0, LIBIPMI_E_INVALID_USER_ID);
	}
	else if( (nUserNameLen=strlen(pszUsername)) == 0 )
	{
		DEBUG_PRINTF("Invalid UserName\n");
		return STATUS_CODE(0, LIBIPMI_E_INVALID_USER_NAME);
	}

	memset(&UserInfo, 0, sizeof(SetUser_T) );
	
	/* Truncate the username to 16 chars */
	(nUserNameLen > MAX_USER_NAME_LEN)?nUserNameLen = MAX_USER_NAME_LEN:nUserNameLen;
	
	/* IPMI Spec Table 22-33 */
	UserInfo.byUserID = byID & ~(0xC0); 
	memcpy(UserInfo.szUserName, pszUsername, nUserNameLen);

	dwResLen = 1;
	/* Call RAW IPMI Command */
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_SET_USER_NAME,
											(uint8*)&UserInfo, sizeof(SetUser_T),
											byRes, &dwResLen,
											timeout);
	if(wRet == LIBIPMI_E_SUCCESS)
		return STATUS_CODE(LIBIPMI_STATUS_SUCCESS, byRes[0]);
	
	return wRet;
}

uint16 LIBIPMI_GetUser( IPMI20_SESSION_T *pSession, char *pszUsername, uint8 byID, int timeout )
{
	GetUser_T	UserInfo;
	uint32		dwRetLen;
	uint16		wRet;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	
	if(byID	>= MAX_USERS)
	{
		DEBUG_PRINTF("Invalid UserID\n");
		return STATUS_CODE(0, LIBIPMI_E_INVALID_USER_ID);
	}

	memset(&UserInfo, 0, sizeof(GetUser_T) );

	dwRetLen = sizeof(GetUser_T);
	byID &= ~(0xC0);
	/* Call RAW IPMI Command */
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, CMD_GET_USER_NAME,
											&byID, sizeof(uint8),
											(uint8 *)&UserInfo, &dwRetLen,
											timeout);

	if(wRet == LIBIPMI_E_SUCCESS)
	{
		tempret=snprintf(pszUsername,MAX_CHAR_LEN,"%s", UserInfo.szUserName);
		if((tempret<0)||(tempret>=sizeof(adcfg.ADRACDomainStr)))
                {
                    TCRIT("Buffer Overflow");
                    return -1;
                }
		return STATUS_CODE(LIBIPMI_STATUS_SUCCESS, UserInfo.byCompletionCode);
	}
	else
	{
		pszUsername[0] = (char)0;
	}
	
	return wRet;
}

