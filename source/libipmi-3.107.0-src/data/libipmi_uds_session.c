/*******************************************************************
 * ********************************************************************
 * ****                                                              **
 * ****    (C)Copyright 2008-2009, American Megatrends Inc.          **
 * ****                                                              **
 * ****    All Rights Reserved.                                      **
 * ****                                                              **
 * ****    5555 , Oakbrook Pkwy, Norcross,                           **
 * ****                                                              **
 * ****    Georgia - 30093, USA. Phone-(770)-246-8600.               **
 * ****                                                              **
 * ********************************************************************
 * ********************************************************************
 * ********************************************************************
 * **
 * ** libipmi_uds_session.c
 * ** Unix Domain Socket library file
 * **
 * ** Author: Suresh V (sureshv@amiindia.co.in)
 * *******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "IPMIDefs.h"
#include "IPMI_AppDevice.h"
#include "IPMI_IPM.h"
#include "UDSIfc.h"
#include "libipmi_helper.h"
#include <sys/syscall.h>
#include "Support.h"
#include "dbgout.h"
#include "libipmi_network.h"
#define gettid()  syscall(SYS_gettid)

#define MAX_USERS 16
#define MAX_IPMI_MESSAGE_SIZE 1024 * 60
#define BMC_SLAVE_ADDRESS 0x20

void UDS_Close(UDS_CHANNEL_T *UDSInfo);

uint8 UDS_ReceiveData( UDS_CHANNEL_T *UDSInfo, char *szBuffer, uint32* pdwSize, int timeout);

int UDS_SendData(UDS_CHANNEL_T *UDSInfo,char *szBuffer,uint32 dwsize,int timeout);

uint16 Close_IPMI20_UDS_Session( IPMI20_SESSION_T *pSession );

uint16 Send_RAW_IPMI2_0_UDS_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pszReqData, uint32 dwReqDataLen,
										uint8 *pszResData, uint32 *pdwResDataLen,
										INT8U AuthEnable,int timeout);

uint16 Create_IPMI20_UDS_Session(IPMI20_SESSION_T *pSession, char *pszSocketPath,
                           char *pszUserName, char *pszPassword, uint8 *byPrivLevel,
                           char *pUserName, uint8 *ipaddr, int timeout,int AuthFlag,INT8U ChannelNum);

uint16 Create_IPMI20_SMBMC_UDS_Session(IPMI20_SESSION_T *pSession, char *pszSocketPath,
                           char *pszUserName, char *pszPassword, uint8 *byPrivLevel,
                           char *pUserName, uint8 *ipaddr, int timeout,int AuthFlag,INT8U ChannelNum);

int UDS_Connect(UDS_CHANNEL_T *UDSInfo,char *SocketPath);

int GetUserNameForUserID(IPMI20_SESSION_T *pSession,INT8U UserID,char *UserName,INT8U AuthEnable,int timeout);

int CheckUserPassword(IPMI20_SESSION_T *pSession, INT8U UserID, char *Password, INT8U AuthEnable, int timeout);

uint32 FillUDSMessagePacket(IPMI20_SESSION_T *pSession, uint8 *Buffer,
                                                        uint8 byNetFnLUN, uint8 byCommand,
                                                        uint8 *pbyReqData, uint32 dwReqDataLen,INT8U AuthEnable);


int UDSGetDeviceID(IPMI20_SESSION_T *pSession,INT8U AuthEnable,int timeout);


/*This API is used to create the IPMI Session
for the web,CIM and Smash*/
uint16 Create_IPMI20_UDS_Session(IPMI20_SESSION_T *pSession, char *pszSocketPath,
                             char *pszUserName, char *pszPassword, uint8 *byPrivLevel,
                             char *pUserName, uint8 *ipaddr, int timeout, int AuthFlag,INT8U ChannelNum)
{
    uint8 retval = 0;
    INT8U UserID = 0;
    int ret=0;

    pSession->hUDSSession = (IPMI20_UDS_SESSION_T*)malloc( sizeof(IPMI20_UDS_SESSION_T) );
    if(pSession->hUDSSession == NULL)
    {
        DEBUG_PRINTF("Could not allocate Memory\n");
        return -1;
    }
    memset(pSession->hUDSSession, 0, sizeof(IPMI20_UDS_SESSION_T));

    pSession->hSMBMCUDSSession= (IPMI20_UDS_SESSION_T *)malloc( sizeof(IPMI20_UDS_SESSION_T) );
    if(pSession->hSMBMCUDSSession == NULL)
    {
        DEBUG_PRINTF("Could not allocate Memory\n");
        return -1;
    }
    memset(pSession->hSMBMCUDSSession, 0, sizeof(IPMI20_UDS_SESSION_T));
    pSession->SMBMCActiveInstance = SYSTEM_NODE;

    if(strlen(pszUserName)!=0)
    {
        ret=snprintf(pSession->hUDSSession->szUserName,sizeof(pSession->hUDSSession->szUserName),"%s",pszUserName);
        if((ret > (signed)sizeof(pSession->hUDSSession->szUserName))||(ret<0))
        {
            TCRIT("Buffer overflow in %s\n",__FUNCTION__);
            free(pSession->hUDSSession);
            free(pSession->hSMBMCUDSSession);
            return -1;
        }
    }
    if(strlen(pszPassword)!=0)
    {
        ret=snprintf(pSession->hUDSSession->szPwd,sizeof(pSession->hUDSSession->szPwd),"%s",pszPassword);
        if((ret > (signed)sizeof(pSession->hUDSSession->szPwd))||(ret<0))
        {
            TCRIT("Buffer overflow in %s\n",__FUNCTION__);
            free(pSession->hUDSSession);
            free(pSession->hSMBMCUDSSession);
            return -1;
        }
    }

    if( (NULL != pUserName) && (0 != strlen(pUserName)) && (strlen(pUserName) < MAX_USER_NAME_LEN) )
    {
        ret=snprintf(pSession->hUDSSession->szUName,sizeof(pSession->hUDSSession->szUName),"%s",pUserName);
        if(ret >= (signed)sizeof(pSession->hUDSSession->szUName))
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
		free(pSession->hUDSSession);
		free(pSession->hSMBMCUDSSession);
        	return -1;
        }
    }
    else
    {
        memset(pSession->hUDSSession->szUName,0,sizeof(pSession->hUDSSession->szUName));
    }
    if(NULL != ipaddr)
    {
        memcpy(pSession->hUDSSession->abyIPAddr, ipaddr, IP6_ADDR_LEN);
    }
    else
    {
        memset(pSession->hUDSSession->abyIPAddr, 0, IP6_ADDR_LEN);
    }


    pSession->hUDSSession->byPreSession = TRUE;
    pSession->hUDSSession->dwSessionID = 0;
    pSession->hUDSSession->byRole = *byPrivLevel;
    pSession->hUDSSession->byChannelNum = ChannelNum;

    pSession->byMediumType = UDS_MEDIUM;
    pSession->g_NetFnOEM = 0xFF;

    retval = UDS_Connect(&pSession->hUDSSession->hUDSInfo,pszSocketPath);
    if(LIBIPMI_STATUS_SUCCESS != retval)
    {
        DEBUG_PRINTF("Unable to Connect\n");
        return STATUS_CODE(MEDIUM_ERROR_FLAG, retval);
    }
    if(AUTH_FLAG == AuthFlag)
    {
        retval = LIBIPMI_GetUserIDForUserName(pSession, pszUserName, pszPassword, &UserID, AuthFlag, timeout);
        if(LIBIPMI_STATUS_SUCCESS == retval)
        {
            if (UserID != 0)
            {
                *byPrivLevel = pSession->hUDSSession->byRole;
                pSession->hUDSSession->byPreSession = 0;
                pSession->bySessionStarted = SESSION_STARTED;
                pSession->hUDSSession->byDefTimeout = timeout;
                return LIBIPMI_E_SUCCESS;
            }
        }
        else
            return STATUS_CODE(0, LIBIPMI_E_INVALID_USER_NAME);

    }
    else if(AUTH_BYPASS_FLAG == AuthFlag)
    {
        retval = UDSGetDeviceID(pSession,AuthFlag,timeout);
        if(retval == LIBIPMI_E_SUCCESS)
        {
            *byPrivLevel = pSession->hUDSSession->byRole;
            pSession->hUDSSession->byPreSession = 0;
            pSession->bySessionStarted = SESSION_STARTED;
            pSession->hUDSSession->byDefTimeout = timeout;
            return LIBIPMI_E_SUCCESS;
        }
        return retval;
    }
    else
        return LIBIPMI_E_AUTHTYPE_NOT_SUPPORTED;

    return 0;
}

uint16 Create_IPMI20_SMBMC_UDS_Session(IPMI20_SESSION_T *pSession, char *pszSocketPath,
                             char *pszUserName, char *pszPassword, uint8 *byPrivLevel,
                             char *pUserName, uint8 *ipaddr, int timeout, int AuthFlag, INT8U ChannelNum)
{
    uint8 retval = 0;
    int tempret=0;/*To check return value of snprintf, it is not used any where else*/

    if(strlen(pszUserName)!=0)
    {
        tempret=snprintf(pSession->hSMBMCUDSSession->szUserName,sizeof(pSession->hSMBMCUDSSession->szUserName),"%s",pszUserName);
        if((tempret >= (signed)sizeof(pSession->hSMBMCUDSSession->szUserName))||(tempret<0))
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
		return -1;
        }
    }
    if(strlen(pszPassword)!=0)
    {
        tempret=snprintf(pSession->hSMBMCUDSSession->szPwd,sizeof(pSession->hSMBMCUDSSession->szPwd),"%s",pszPassword);
        if((tempret >= (signed)sizeof(pSession->hSMBMCUDSSession->szPwd))||(tempret<0))
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
		return -1;
        }
    }

    if( (NULL != pUserName) && (0 != strlen(pUserName)) && (strlen(pUserName) < MAX_USER_NAME_LEN) )
    {
        tempret=snprintf(pSession->hSMBMCUDSSession->szUName,sizeof(pSession->hSMBMCUDSSession->szUName),"%s",pUserName);
        if((tempret >= (signed)sizeof(pSession->hSMBMCUDSSession->szUName))||(tempret<0))
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
		return -1;
        }
    }
    else
    {
        memset(pSession->hSMBMCUDSSession->szUName, 0,sizeof(pSession->hSMBMCUDSSession->szUName));
    }
    if(NULL != ipaddr)
    {
        memcpy(pSession->hSMBMCUDSSession->abyIPAddr, ipaddr, IP6_ADDR_LEN);
    }
    else
    {
        memset(pSession->hSMBMCUDSSession->abyIPAddr, 0, IP6_ADDR_LEN);
    }
    memset(pSession->hSMBMCUDSSession->abyIPAddr, 0, IP6_ADDR_LEN);

    pSession->hSMBMCUDSSession->byPreSession = TRUE;
    pSession->hSMBMCUDSSession->dwSessionID = 0;
    pSession->hSMBMCUDSSession->byRole = *byPrivLevel;
    pSession->hSMBMCUDSSession->byChannelNum = ChannelNum;

    retval = UDS_Connect(&pSession->hSMBMCUDSSession->hUDSInfo,pszSocketPath);
    if(LIBIPMI_STATUS_SUCCESS != retval)
    {
        DEBUG_PRINTF("Unable to Connect\n");
        return STATUS_CODE(MEDIUM_ERROR_FLAG, retval);
    }

    if(AUTH_BYPASS_FLAG == AuthFlag)
    {
        retval = UDSGetDeviceID(pSession,AuthFlag,timeout);
        if(retval == LIBIPMI_E_SUCCESS)
        {
            *byPrivLevel = pSession->hSMBMCUDSSession->byRole;
            pSession->hSMBMCUDSSession->byPreSession = 0;
            pSession->bySessionStarted = SESSION_STARTED;
            pSession->hSMBMCUDSSession->byDefTimeout = timeout;
            return LIBIPMI_E_SUCCESS;
        }
        return retval;
    }
    else
        return LIBIPMI_E_AUTHTYPE_NOT_SUPPORTED;

    return 0;
}

/*This Function is used to connect UDS Client Socket*/
int UDS_Connect(UDS_CHANNEL_T *UDSInfo,char *SocketPath)
{
    SOCKET client_sock;
    struct sockaddr_un serv_addr;
    int tempret=0;/*To check return value of snprintf, it is not used any where else*/


    /* Open the socket */
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if(client_sock == -1)
    {
        DEBUG_PRINTF("LIBIPMI_UDS_SESSION.C: Unable to create socket \n");
        return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
    }

    /*Connect the client to server*/

    memset ((char *) &serv_addr, 0, sizeof (serv_addr));
    serv_addr.sun_family = AF_UNIX;
    tempret=snprintf(serv_addr.sun_path,sizeof(serv_addr.sun_path),"%s",SocketPath);
    if((tempret<0)||(tempret>=(signed)sizeof(serv_addr.sun_path)))
    {
        DEBUG_PRINTF("BUFFER OVERFLOW");
	close(client_sock);
        return LIBIPMI_MEDIUM_E_INVALID_DATA;
    }

    if ( connect (client_sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        DEBUG_PRINTF("LIBIPMI_UDS_SESSION.C: Unable to connect to Server:\n");
	close(client_sock);
        return LIBIPMI_MEDIUM_E_CONNECT_FAILURE;
    }

    UDSInfo->hSocket = client_sock;
    UDSInfo->byIsConnected = 1;

    return LIBIPMI_E_SUCCESS;
}


/**
This Function is used to get the Username for the given UserID.
UserID is IN argument, UserName is OUT argument.
**/
int GetUserNameForUserID(IPMI20_SESSION_T *pSession,INT8U UserID,char *UserName,INT8U AuthEnable,int timeout)
{
    uint8 pbyIPMIReq [MAX_REQUEST_SIZE];
    uint8 pbyIPMIRes [MAX_RESPONSE_SIZE];
    GetUserNameReq_T* pUserNameReq;
    GetUserNameRes_T* pUserNameRes;
    uint32 reslen = 0;
    int retval = 0;

    pUserNameReq = (GetUserNameReq_T *)&pbyIPMIReq[0];
    memset(pUserNameReq,0,sizeof(GetUserNameReq_T));

    pUserNameReq->UserID = UserID;
    reslen = sizeof(GetUserNameRes_T);
    retval = Send_RAW_IPMI2_0_UDS_Command ( pSession,
                                                                                DEFAULT_NET_FN_LUN, CMD_GET_USER_NAME,
                                                                                pbyIPMIReq, sizeof (GetUserNameReq_T),
                                                                                pbyIPMIRes, &reslen,
                                                                                AuthEnable,timeout);

    if(retval == LIBIPMI_E_SUCCESS)
    {
        pUserNameRes = (GetUserNameRes_T *)&pbyIPMIRes[0];
        if(pUserNameRes->CompletionCode == CC_NORMAL)
        {
			memmove(UserName,pUserNameRes->UserName,MAX_USERNAME_LEN);
            return CC_NORMAL;
        }
        else
            return (pUserNameRes->CompletionCode);
    }
    else
        return retval;

    return LIBIPMI_E_SUCCESS;
}

/**
This function is used to check the password matches with the corresponding User ID.
First it will get 20byte password, if it matches then it returns SUCCCESS.
otherwise it will try for 16byte password, if it matches then it returns SUCCESS.
UserID and Password are IN arguments.
**/
int CheckUserPassword(IPMI20_SESSION_T *pSession, INT8U UserID, char *Password, INT8U AuthEnable, int timeout)
{
    SetUserPswdReq_T UDSReqSetUserPsswd;
    SetUserPswdRes_T UDSResSetUserPsswd;
    uint32 reslen = 0;
    int retval = 0;

    /*First check whether it is a 20 byte password*/
    memset(&UDSReqSetUserPsswd,0,sizeof(SetUserPswdReq_T));
    memset(&UDSResSetUserPsswd,0,sizeof(SetUserPswdRes_T));
    UDSReqSetUserPsswd.UserID = (INT8U)(BIT7|UserID);
    UDSReqSetUserPsswd.Operation = 0x03;
    strncpy((char*)UDSReqSetUserPsswd.Password, (char *)Password, MAX_PASSWORD_LEN);

    reslen = sizeof(SetUserPswdRes_T);
    retval =  Send_RAW_IPMI2_0_UDS_Command(pSession,
                                                                              DEFAULT_NET_FN_LUN,CMD_SET_USER_PASSWORD,
                                                                              (uint8 *)&UDSReqSetUserPsswd,sizeof(SetUserPswdReq_T),
                                                                              (uint8 *)&UDSResSetUserPsswd,&reslen,
                                                                              AuthEnable,timeout);

    if((retval == STATUS_CODE(IPMI_ERROR_FLAG,CC_PASSWORD_TEST_FAILED_WRONG_SIZE)) &&
       (IPMI15_MAX_PASSWORD_LEN >= strlen (Password)))
    {
        /*Password may be 16 Byte Password so trying it out*/
        memset(&UDSReqSetUserPsswd,0,sizeof(SetUserPswdReq_T));
        memset(&UDSResSetUserPsswd,0,sizeof(SetUserPswdRes_T));

        UDSReqSetUserPsswd.UserID = (INT8U)(UserID);
        UDSReqSetUserPsswd.Operation = 0x03;
        strncpy((char *)UDSReqSetUserPsswd.Password, (char *)Password, MAX_PASSWORD_LEN);
        reslen = sizeof(SetUserPswdRes_T);

        retval = Send_RAW_IPMI2_0_UDS_Command(pSession,
                                                                                DEFAULT_NET_FN_LUN,CMD_SET_USER_PASSWORD,
                                                                                (uint8 *)&UDSReqSetUserPsswd,sizeof(SetUserPswdReq_T)-4,
                                                                                (uint8 *)&UDSResSetUserPsswd,&reslen,
                                                                                AuthEnable,timeout);

        return retval;
    }
    else
        return retval;

    return 0;
}

/*This Function is used to 
get the corresponding Session ID
from UDS Inteface for CIM,Smash*/
int UDSGetDeviceID(IPMI20_SESSION_T *pSession,INT8U AuthEnable,int timeout)
{
    int RetVal = 0;
    uint32 dwIPMIResLen;
    uint8 pbyIPMIReq[MAX_IPMI_MESSAGE_SIZE];
    uint8 pbyIPMIRes[MAX_RESPONSE_SIZE];
    uint32 ReqLen;


    memset(pbyIPMIReq,0,sizeof(pbyIPMIReq));
    memset(pbyIPMIRes,0,sizeof(pbyIPMIRes));

    /* The Request Data is only 
    Command Number and NetFn
    so ReqLen is Zero here*/
    ReqLen = 0;
    dwIPMIResLen = sizeof(GetDevIDRes_T);
    RetVal = Send_RAW_IPMI2_0_UDS_Command (pSession,DEFAULT_NET_FN_LUN,
                                                    CMD_GET_DEV_ID,pbyIPMIReq,ReqLen,
                                                    pbyIPMIRes, &dwIPMIResLen,AuthEnable,timeout);
    if(RetVal == LIBIPMI_E_SUCCESS)
    {
         GetDevIDRes_T *pUDSGetDevIDRes = (GetDevIDRes_T *)&pbyIPMIRes[0];
         if(pUDSGetDevIDRes->CompletionCode == CC_NORMAL)
         {
            return CC_NORMAL;
         }
         else
            return(pUDSGetDevIDRes->CompletionCode);
    }
    else
        return RetVal;

    return LIBIPMI_E_SUCCESS;
}

/*This Function is used to send data
* to UDS Interface in IPMI Layer*/
uint16 Send_RAW_IPMI2_0_UDS_Command(IPMI20_SESSION_T *pSession,
                                                                        uint8 byNetFnLUN, uint8 byCommand,
                                                                        uint8 *pszReqData, uint32 dwReqDataLen,
                                                                        uint8 *pszResData, uint32 *pdwResDataLen,
                                                                        INT8U AuthEnable,int timeout)
{
   uint8 pbyIPMIUDSMsgReq[MAX_IPMI_MESSAGE_SIZE];
   uint8 pbyIPMIUDSResData[MAX_RESPONSE_SIZE];
   uint32 retlen=0,byretval=0,UDSSendMsgLen = 0;

   memset(pbyIPMIUDSMsgReq,0,MAX_IPMI_MESSAGE_SIZE);
   memset(pbyIPMIUDSResData,0,MAX_RESPONSE_SIZE);

   retlen = FillUDSMessagePacket(pSession,pbyIPMIUDSMsgReq,byNetFnLUN,byCommand,pszReqData,dwReqDataLen,AuthEnable);

    if(retlen == 0 )
    {
       DEBUG_PRINTF("Data Length Invalid\n");
       return -1;
    }

    if ( SYSTEM_NODE != pSession->SMBMCActiveInstance ) {
        byretval = UDS_SendData(&pSession->hSMBMCUDSSession->hUDSInfo,(char *)pbyIPMIUDSMsgReq,retlen,timeout);
    } else {
        byretval = UDS_SendData(&pSession->hUDSSession->hUDSInfo,(char *)pbyIPMIUDSMsgReq,retlen,timeout);
    }
	
    if(byretval != LIBIPMI_STATUS_SUCCESS)
        return byretval;

    if ( SYSTEM_NODE != pSession->SMBMCActiveInstance ) {
        byretval = UDS_ReceiveData(&pSession->hSMBMCUDSSession->hUDSInfo, (char *)pbyIPMIUDSResData, pdwResDataLen,timeout);
    } else {
        byretval = UDS_ReceiveData(&pSession->hUDSSession->hUDSInfo, (char *)pbyIPMIUDSResData, pdwResDataLen,timeout);
    }

    if(byretval == LIBIPMI_STATUS_SUCCESS)
    {
        IPMIUDSMsg_T *pIPMIUDSMsg = (IPMIUDSMsg_T *)&pbyIPMIUDSResData[0];
        INT8U *bycompletioncode = (INT8U *)&pbyIPMIUDSResData[sizeof(IPMIUDSMsg_T)];

        uint32 maxlen = *pdwResDataLen;
        uint32 newlen = pIPMIUDSMsg->IPMIMsgLen-sizeof(IPMIUDSMsg_T) - 1;

        if (maxlen && (newlen > maxlen))
        {
        	TCRIT("%s: Response too big: %u (expected max: %u). NetfnLUN: %02x, command: %02x.\n", __func__, newlen, maxlen, byNetFnLUN, byCommand);
        	return -1;
        }
        if ((NET_FN(pIPMIUDSMsg->NetFnLUN) != NET_FN(byNetFnLUN)) && (pIPMIUDSMsg->Cmd != byCommand))
        {
        	TCRIT("%s: NetFN and command mismatched\n", __func__);
        	return -1;
        }
        
        *pdwResDataLen = pIPMIUDSMsg->IPMIMsgLen-sizeof(IPMIUDSMsg_T) - 1;
        /* Fortify issue :: False Positive Buffer Overflow */
        memcpy(pszResData,&pbyIPMIUDSResData[sizeof(IPMIUDSMsg_T)],*pdwResDataLen);
        
        if(*bycompletioncode != CC_NORMAL)
            return STATUS_CODE(IPMI_ERROR_FLAG, *bycompletioncode);
        else
        {
            if((0 == pSession->hUDSSession->dwSessionID) && (pIPMIUDSMsg->Cmd == CMD_SET_USER_PASSWORD)&&(pIPMIUDSMsg->AuthFlag == AUTH_FLAG))
            {
                pSession->hUDSSession->dwSessionID = pIPMIUDSMsg->SessionID;
                pSession->hUDSSession->byRole = pIPMIUDSMsg->Privilege;
                
                if(UDS_CHANNEL == pSession->hUDSSession->byChannelNum)
                {
                    pSession->hUDSSession->byChannelNum = pIPMIUDSMsg->ChannelNum;
                }
            }
            else if( SYSTEM_NODE != pSession->SMBMCActiveInstance )
            {
                if((0 == pSession->hSMBMCUDSSession->dwSessionID) && (pIPMIUDSMsg->Cmd == CMD_GET_DEV_ID) && (pIPMIUDSMsg->AuthFlag == AUTH_BYPASS_FLAG))
                {
                    pSession->hSMBMCUDSSession->dwSessionID = pIPMIUDSMsg->SessionID;
                    pSession->hSMBMCUDSSession->byRole = pIPMIUDSMsg->Privilege;
            
                    if(UDS_SMBMC_CHANNEL == pSession->hSMBMCUDSSession->byChannelNum)
                    {
                        pSession->hSMBMCUDSSession->byChannelNum = pIPMIUDSMsg->ChannelNum;
                    }
                }
            }
            else if( SYSTEM_NODE == pSession->SMBMCActiveInstance )
            {
                if((0 == pSession->hUDSSession->dwSessionID) && (pIPMIUDSMsg->Cmd == CMD_GET_DEV_ID) && (pIPMIUDSMsg->AuthFlag == AUTH_BYPASS_FLAG))
                {
                    pSession->hUDSSession->dwSessionID = pIPMIUDSMsg->SessionID;
                    pSession->hUDSSession->byRole = pIPMIUDSMsg->Privilege;
            
                    if(UDS_CHANNEL == pSession->hUDSSession->byChannelNum)
                    {
                        pSession->hUDSSession->byChannelNum = pIPMIUDSMsg->ChannelNum;
                    }
                }
            }
            else if (CMD_SEND_MSG == pIPMIUDSMsg->Cmd && (NETFN_APP==(NET_FN(pIPMIUDSMsg->NetFnLUN) & 0xFE)))
            {
                 memset(pbyIPMIUDSResData,0,MAX_RESPONSE_SIZE);
                 byretval = UDS_ReceiveData(&pSession->hUDSSession->hUDSInfo, (char *)pbyIPMIUDSResData, pdwResDataLen,timeout);
                if( LIBIPMI_STATUS_SUCCESS != byretval)
                {
                    return byretval;
                }

                 UDSSendMsgLen =  sizeof(IPMIUDSMsg_T) +sizeof(IPMIMsgHdr_T) +3;
                 pIPMIUDSMsg = (IPMIUDSMsg_T *)&pbyIPMIUDSResData[0];
                *pdwResDataLen = pIPMIUDSMsg->IPMIMsgLen- UDSSendMsgLen;
                if(CC_NORMAL != pbyIPMIUDSResData[ sizeof(IPMIUDSMsg_T)])
                {
                    return STATUS_CODE(IPMI_ERROR_FLAG, *bycompletioncode);
                }

                 memcpy(pszResData,&pbyIPMIUDSResData[UDSSendMsgLen-1],*pdwResDataLen);
            }
        }
    }
            else
            return STATUS_CODE(IPMI_ERROR_FLAG, byretval);

    return LIBIPMI_STATUS_SUCCESS;
}

/*This Functions Fills the Packet
 * with the valid fields for UDS*/
uint32 FillUDSMessagePacket(IPMI20_SESSION_T *pSession, uint8 *Buffer,
                                                        uint8 byNetFnLUN, uint8 byCommand,
                                                        uint8 *pbyReqData, uint32 dwReqDataLen,INT8U AuthEnable)
{
    uint8 *pbyIPMIUDSData = NULL,*cs2 = NULL;
    IPMIUDSMsg_T *pUDSMsg = NULL;
    IPMIMsgHdr_T *pbyBridgeIPMIHdr = NULL;

    pUDSMsg = (IPMIUDSMsg_T *)&Buffer[0];
    pbyIPMIUDSData = &Buffer[sizeof(IPMIUDSMsg_T)];

    if ( SYSTEM_NODE != pSession->SMBMCActiveInstance ) {
        pUDSMsg->SessionID = pSession->hSMBMCUDSSession->dwSessionID;
        pUDSMsg->ChannelNum = pSession->hSMBMCUDSSession->byChannelNum;
        pUDSMsg->Privilege = pSession->hSMBMCUDSSession->byRole;
    } else {
        pUDSMsg->SessionID = pSession->hUDSSession->dwSessionID;
        pUDSMsg->ChannelNum = pSession->hUDSSession->byChannelNum;
        pUDSMsg->Privilege = pSession->hUDSSession->byRole;
    }

    pUDSMsg->AuthFlag = AuthEnable;
    pUDSMsg->Cmd = byCommand;
    pUDSMsg->NetFnLUN = byNetFnLUN;

    if ( SYSTEM_NODE != pSession->SMBMCActiveInstance ) {
        memcpy(pUDSMsg->UserName, pSession->hSMBMCUDSSession->szUName, MAX_USER_NAME_LEN);
        memcpy(pUDSMsg->IPAddr, pSession->hSMBMCUDSSession->abyIPAddr, IP6_ADDR_LEN);
    } else {
        memcpy(pUDSMsg->UserName, pSession->hUDSSession->szUName, MAX_USER_NAME_LEN);
        memcpy(pUDSMsg->IPAddr, pSession->hUDSSession->abyIPAddr, IP6_ADDR_LEN);
    }

    pUDSMsg->ProcessID = getpid();
    pUDSMsg->ThreadID = gettid();

     // Fill the UDS Send Message Format for ease
    if((CMD_SEND_MSG == byCommand) && (NETFN_APP==(NET_FN(pUDSMsg->NetFnLUN) & 0xFE)))
    { 
        pbyIPMIUDSData[0] = pbyReqData[0];
        pbyBridgeIPMIHdr = (IPMIMsgHdr_T *)(pbyIPMIUDSData + 1);
        pbyBridgeIPMIHdr->ResAddr 	= pbyReqData[1];
        pbyBridgeIPMIHdr->NetFnLUN 	= pbyReqData[2] << 2;
        pbyBridgeIPMIHdr->ChkSum	= LIBIPMI_CalculateCheckSum( (uint8 *)pbyBridgeIPMIHdr, (uint8 *)&(pbyBridgeIPMIHdr->ChkSum) ); /* cheksum_1 */
        pbyBridgeIPMIHdr->ReqAddr 	= BMC_SLAVE_ADDRESS;
        if ( SYSTEM_NODE != pSession->SMBMCActiveInstance ) {
            pbyBridgeIPMIHdr->RqSeqLUN	= pSession->hSMBMCUDSSession->byIPMBSeqNum << 2;
        } else {
            pbyBridgeIPMIHdr->RqSeqLUN	= pSession->hUDSSession->byIPMBSeqNum << 2;
        }

        pbyBridgeIPMIHdr->Cmd		= pbyReqData[3];

        /* Taking the first 4 bytes from the Req Packet and tranferring them to 1 + IPMIMsgHdr size */
        /* Copy the rest of the Request bytes at the end */
        if((1+sizeof(IPMIMsgHdr_T)+(dwReqDataLen -4)) <= (IPMI_BUFFER_SIZE - sizeof(IPMIUDSMsg_T) -1))
        {
            memcpy (pbyIPMIUDSData+1+sizeof(IPMIMsgHdr_T), pbyReqData+4, dwReqDataLen -4);
        }
        else
        {
            TCRIT("Buffer overflow in %s\n",__FUNCTION__);
            return 0;
        }

        /* Add the two checksums */
        cs2 = (uint8*)(pbyIPMIUDSData + 1 + sizeof(IPMIMsgHdr_T)+dwReqDataLen -4);

        *cs2 = LIBIPMI_CalculateCheckSum( (&(pbyBridgeIPMIHdr->ChkSum))+1, cs2);

        /* Increment the sequence number */
        if ( SYSTEM_NODE != pSession->SMBMCActiveInstance ) {
            pSession->hSMBMCUDSSession->byIPMBSeqNum = (pSession->hSMBMCUDSSession->byIPMBSeqNum+1) % 0x40;
        } else {
            pSession->hUDSSession->byIPMBSeqNum = (pSession->hUDSSession->byIPMBSeqNum+1) % 0x40;
        }

        pUDSMsg->IPMIMsgLen = sizeof(IPMIUDSMsg_T)+sizeof(IPMIMsgHdr_T) + (dwReqDataLen-2) +1;
     }
    else
    {	 
        if(dwReqDataLen <= (IPMI_BUFFER_SIZE - sizeof(IPMIUDSMsg_T)))
        {
            memcpy(pbyIPMIUDSData, pbyReqData, dwReqDataLen);
        }
        else
        {
           TCRIT("Buffer overflow in %s\n",__FUNCTION__);
           return 0;
    	}
         pUDSMsg->IPMIMsgLen = sizeof(IPMIUDSMsg_T) + dwReqDataLen +1;
    }

     return pUDSMsg->IPMIMsgLen;
}

/*This Function is used to send the
 * data to UDS Interface*/
int UDS_SendData(UDS_CHANNEL_T *UDSInfo,char *szBuffer,uint32 dwsize,int timeout)
{
    int nTotalBytesSent = 0, nBytesSent = 0;
    struct timeval wait_time, *pwait_time;
    fd_set wdfs;
    int ret;
    int nDes;

TCPSendUDSDataSelect:
    /* Wait for the socket to get ready */
    if(timeout >= 0)
    {
        wait_time.tv_sec = timeout;
        wait_time.tv_usec = 0;
        pwait_time = &wait_time;
    }
    else
        pwait_time = NULL;

    FD_ZERO(&wdfs);
    FD_SET(UDSInfo->hSocket, &wdfs);
    nDes = UDSInfo->hSocket + 1;

    /* wait for specified amount of time */
        ret = select (nDes, NULL, &wdfs, NULL, pwait_time);

        /* socket error */
        if(ret == SOCKET_ERROR)
        {

            if(errno == EINTR)
            {
                DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
                goto TCPSendUDSDataSelect;
            }
            else
            {
                DEBUG_PRINTF("Select failed while waiting for write pipe \n");
                close(UDSInfo->hSocket);
                return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
            }
        }
        /* timed out */
        else if(ret == 0)
        {
            DEBUG_PRINTF("Connection timedout: Write Pipe is full \n");
            close(UDSInfo->hSocket);
            return LIBIPMI_MEDIUM_E_TIMED_OUT_ON_SEND;
        }

    if(FD_ISSET(UDSInfo->hSocket,&wdfs))
    {
            /* socket is ready to send data */
            nTotalBytesSent = 0;

        /* Till the last byte is sent */
            while( nTotalBytesSent < (int)dwsize )
            {
                /* send data */
                nBytesSent = send (UDSInfo->hSocket,szBuffer+nTotalBytesSent,dwsize-nTotalBytesSent, MSG_NOSIGNAL);
                /* send failure */
                if ((nBytesSent == SOCKET_ERROR) && ((EBADF == errno) || (EPIPE == errno)))
                {
                    DEBUG_PRINTF("Error while sending data \n");
                    close(UDSInfo->hSocket);
                    return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
                }
                else
                {
                    nTotalBytesSent += nBytesSent;
                }
            }
    return LIBIPMI_STATUS_SUCCESS;

    }

    DEBUG_PRINTF("select came out while trying to write for no good reason!!\n");
    close(UDSInfo->hSocket);
    return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
 }


/*This Function is used to recieve
  data from UDS Interface as response*/
uint8 UDS_ReceiveData( UDS_CHANNEL_T *UDSInfo, char *szBuffer, uint32* pdwSize, int timeout )
{
    struct timeval wait_time, *pwait_time;
    fd_set rdfs;
    int	nDes = 0,totalrecvd = 0,ret = 0;
    IPMIUDSMsg_T *pData = (IPMIUDSMsg_T *)szBuffer;
    uint32 nBytesRecvd = 0;

    while(1)
    {
    /* Wait for the socket to get ready */
    if(timeout >= 0)
    {
        wait_time.tv_sec = timeout;
        wait_time.tv_usec = 0;
        pwait_time = &wait_time;
    }
    else
        pwait_time = NULL;

    FD_ZERO(&rdfs);
    FD_SET(UDSInfo->hSocket, &rdfs);
    nDes = UDSInfo->hSocket + 1;

        /* wait for specified amount of time */
        ret = select (nDes, &rdfs, NULL, NULL, pwait_time);

        /* socket error */
        if(ret == SOCKET_ERROR)
        {
            if(errno == EINTR)
            {
                DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
                continue;
            }
            else
            {
                DEBUG_PRINTF("select failed while waiting for read pipe \n");
                continue;
            }
        }
        else if(ret == 0)
        {
              DEBUG_PRINTF("Connection timed out\n");
              return LIBIPMI_MEDIUM_E_TIMED_OUT;
         }

        /* socket is ready to receive data */
        if(FD_ISSET(UDSInfo->hSocket,&rdfs))
        {
            nBytesRecvd = recv(UDSInfo->hSocket,szBuffer+totalrecvd,MAX_RESPONSE_SIZE - totalrecvd,0);
            if(nBytesRecvd == 0)
            {
                DEBUG_PRINTF("End of File Encountered ... may be socket closure\n");
                close(UDSInfo->hSocket);
                return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
            }
            if((signed)nBytesRecvd == SOCKET_ERROR)
            {
                DEBUG_PRINTF("Socket Error... may be socket closure\n");
                close(UDSInfo->hSocket);
                return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
            }

            totalrecvd += nBytesRecvd;

            if(totalrecvd < (signed)sizeof(IPMIUDSMsg_T))
            {
              continue;
            }

            if(totalrecvd < pData->IPMIMsgLen)
            {
                if (pData->IPMIMsgLen > MAX_RESPONSE_SIZE)
                {
                    DEBUG_PRINTF("Invalid IPMIMsgLen in %s\n", __func__);
                    close(UDSInfo->hSocket);
                    return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
                }
              continue;
            }
        *pdwSize = totalrecvd;
        return LIBIPMI_STATUS_SUCCESS;
       }
       close(UDSInfo->hSocket);
       return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
     }
}



void UDS_Close(UDS_CHANNEL_T *UDSInfo )
{
    close(UDSInfo->hSocket);
    /* reset the connected flag */
    UDSInfo->byIsConnected = 0;

    return;
}

uint16 Close_IPMI20_UDS_Session( IPMI20_SESSION_T *pSession )
{
    uint16	wRet;
    uint32	Req;
    uint8	byRes[MAX_RESPONSE_SIZE];
    uint32	dwResLen = sizeof(byRes);

   if(pSession->hUDSSession == NULL)
   {
       DEBUG_PRINTF("Session Information is Null\n");
       return LIBIPMI_MEDIUM_E_INVALID_DATA;
   }

    Req = pSession->hUDSSession->dwSessionID;
    wRet =  Send_RAW_IPMI2_0_UDS_Command (pSession,DEFAULT_NET_FN_LUN,
                                                        CMD_CLOSE_SESSION,(uint8 *)&Req,
                                                        sizeof (uint32),byRes, &dwResLen,
                                                        0,pSession->hUDSSession->byDefTimeout);

    return wRet;
}
