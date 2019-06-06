/*******************************************************************
********************************************************************
****                                                              **
****    (C)Copyright 2008-2009, American Megatrends Inc.          **
****                                                              **
****    All Rights Reserved.                                      **
****                                                              **
****    5555 , Oakbrook Pkwy, Norcross,                           **
****                                                              **
****    Georgia - 30093, USA. Phone-(770)-246-8600.               **
****                                                              **
********************************************************************
********************************************************************
********************************************************************
**
** UDSIfc.c
** Unix Domain Socket Interface Handler
**
** Author: Suresh V (sureshv@amiindia.co.in)
*******************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include "OSPort.h"
#include "IPMIConf.h"
#include "IPMI_Main.h"
#include "nwcfg.h"
#include "IPMI_App.h"
#include "UDSIfc.h"
#include "Session.h"
#include "IPMIDefs.h"
#include "libipmi_struct.h"
#include "featuredef.h"
#include "blowfish.h"
#include "Encode.h"
#include "ThrdPrio.h"
#include <openssl/rand.h>

/*Function Prototypes*/
void CloseUDSSocket(int Socket,int BMCInst);
static int ProcessUDSReq(MsgPkt_T *pReq,int BMCInst);
int InitUnixDomainSocket(int BMCInst);
void *RecvUDSPkt(void *pArg);
int AddUDSSocket (int Socket,INT8U IsLoopBackSocket, INT8U IsFixedSocket, int BMCInst);
int RemoveUDSSocket (int Socket, int BMCInst);
int ReadUDSData(MsgPkt_T *MsgPkt,int Socket,int BMCInst);
int SendUDSPkt (MsgPkt_T *pRes,int BMCInst);
int SetUDSFd(fd_set *newfd,int *maximum,int BMCInst);
int FillUDSResponsePacket(MsgPkt_T *pReq,MsgPkt_T *pRes,uint8 UDSCompletionCode,int BMCInst);
static int UpdateUDSTimeout (int BMCInst);
static void*  UDSTimer (void *pArg);
int AddUDSInfo(MsgPkt_T *pUDSReq,MsgPkt_T *pUDSRes,int BMCInst);
INT8U RemoveUDSSession(SOCKET Socket,int BMCInst);
int CheckReservedCmd(INT8U Cmd);
ChannelInfo_T* GetLANChannelInfoForUDS(INT8U ReqUDSChannel,INT8U *LANChannel,INT8U *ResChannelNum,int BMCInst);
static int ProcessUDSBridgeReq(MsgPkt_T * pReq, int BMCInst);

IPMILibMetaInfo_T g_UDSMetaInfo;

static INT8U ReservedCmd[]={
                      CMD_GET_DEV_ID,
                      CMD_GET_USER_NAME,
                      CMD_SET_USER_PASSWORD};

IfcType_T IfcType = UDS_IFC;

/**
 * @fn UDSIfcTask
 * @brief This function is used to start the UDS Interface Task
 * @param Addr
**/
void *UDSIfcTask(void *Addr)
{
    MsgPkt_T Req;
    int BMCInst=0;
    char keyInstance[MAX_STR_LENGTH];
    BMCArg *IPMIArgs=(BMCArg *)Addr;
    BMCInst=IPMIArgs->BMCInst;
    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
    prctl(PR_SET_NAME,__FUNCTION__,0,0,0);
    int curThreadIndex = 0;

    if(g_corefeatures.ipmi_thread_monitor_support == ENABLED)
    {
        pthread_t thread_id = pthread_self();
        for(; curThreadIndex <= gthreadIndex; curThreadIndex++)
        {
            if(pthread_equal(gthreadIDs[curThreadIndex], thread_id))
            {
                IPMI_DBG_PRINT_2("UDSIfc.c: UDSIfcTask thread[ID: %d] found with index=%d.\n", thread_id, curThreadIndex);
                gthread_monitor_health[curThreadIndex].validThreadID = THREAD_ID_FOUND;
                gthread_monitor_health[curThreadIndex].BMCInst = BMCInst;
                if(snprintf(gthread_monitor_health[curThreadIndex].threadName,sizeof(gthread_monitor_health[curThreadIndex].threadName),"%s",__FUNCTION__)>=(signed)sizeof(gthread_monitor_health[curThreadIndex].threadName))
                TCRIT("UDSIfcTask-source buffer truncated\n");
                break;
            }
        }
    }

    /*Set the Thread Scheduling Priority*/
    if(g_corefeatures.interface_thread_priority == ENABLED)
    {
        pthread_t thread_id = pthread_self();
        SetUDSIfcPriority(thread_id);
    }

    memset(keyInstance,0,MAX_STR_LENGTH);

    pBMCInfo->pUDSocketTbl = (SocketTbl_T*) malloc(sizeof(SocketTbl_T)*(pBMCInfo->IpmiConfig.MaxSession+1));
    if(pBMCInfo->pUDSocketTbl == NULL)
    {
        IPMI_ERROR("UDSIfc.c:Cannot allocate Memory for Socket Table");
        return (void *)UDS_FAILURE;
    }

    memset(pBMCInfo->pUDSocketTbl, 0,sizeof(SocketTbl_T)*(pBMCInfo->IpmiConfig.MaxSession+1));

    OS_CREATE_Q (UDS_IFC_Q, Q_SIZE,BMCInst);
    OS_GET_Q(UDS_IFC_Q,O_RDWR,BMCInst);

    OS_CREATE_Q (UDS_RES_Q, Q_SIZE,BMCInst);
    OS_GET_Q(UDS_RES_Q,O_RDWR,BMCInst);

    /* Open Unix Domain Socket */
    if(UDS_SUCCESS > InitUnixDomainSocket(BMCInst))
    {
      OS_THREAD_MUTEX_RELEASE(&pBMCInfo->ThreadSyncMutex);
      return (void *)UDS_FAILURE;
    }

    /*Create a thread to recv UDS Pkt */
    gthreadIndex++;
    IPMI_DBG_PRINT_1("UDSIfc.c: Creating RecvUDSPkt thread with index %d\n", gthreadIndex);
    OS_CREATE_TASK_THREAD(RecvUDSPkt, (void*)&BMCInst, err, gthreadIDs[gthreadIndex]);

    /* Create a thread to handle socket timeout */
    gthreadIndex++;
    IPMI_DBG_PRINT_1("UDSIfc.c: Creating UDSTimer thread with index %d\n", gthreadIndex);
    OS_CREATE_TASK_THREAD(UDSTimer, (void*)&BMCInst, err, gthreadIDs[gthreadIndex]);

    OS_THREAD_MUTEX_RELEASE(&pBMCInfo->ThreadSyncMutex);

    while(TRUE)
    {
        if(g_corefeatures.ipmi_thread_monitor_support == ENABLED && gthread_monitor_health[curThreadIndex].validThreadID == THREAD_ID_FOUND)
        {
            OS_THREAD_MUTEX_ACQUIRE(&ThreadMonitorMutex, WAIT_INFINITE);
            gthread_monitor_health[curThreadIndex].monitor = THREAD_STATE_MONITOR_NO;
            gthread_monitor_health[curThreadIndex].time = 0;
            OS_THREAD_MUTEX_RELEASE(&ThreadMonitorMutex);
        }
        if(UDS_SUCCESS != GetMsg(&Req,UDS_IFC_Q,WAIT_INFINITE,BMCInst))
        {
            IPMI_WARNING("UDSIfc.c: Error Fetching Data from UDSIfcQ\n");
            continue;
        }
        if(g_corefeatures.ipmi_thread_monitor_support == ENABLED && gthread_monitor_health[curThreadIndex].validThreadID == THREAD_ID_FOUND)
        {
            OS_THREAD_MUTEX_ACQUIRE(&ThreadMonitorMutex, WAIT_INFINITE);
            gthread_monitor_health[curThreadIndex].monitor = THREAD_STATE_MONITOR_YES;
            OS_THREAD_MUTEX_RELEASE(&ThreadMonitorMutex);
        }

        switch(Req.Param)
        {
            case UDS_SMB_PARAM:
                ProcessUDSReq(&Req,BMCInst);
                break;

           case BRIDGING_REQUEST:
                ProcessUDSBridgeReq(&Req,BMCInst);
                break;

            default:
                IPMI_WARNING("UDSIfc.c:Invalid Request");
                break;
        }

    }

    return UDS_SUCCESS;
}

/**
 * @fn InitUnixDomainSocket
 * @brief This function is used to create the Socket for each BMC
 * @param BMCInst
**/
int InitUnixDomainSocket(int BMCInst)
{
    char SocketName[MAX_STR_LENGTH];
    struct sockaddr_un local;
    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
    struct stat buf;//used for stat command

    memset(SocketName,0,sizeof(SocketName));
    memset(&local,0,sizeof(local));

    sprintf(SocketName,"%s%d",SOCKET_PATH,BMCInst);

    local.sun_family = AF_UNIX;
    strncpy(local.sun_path,SocketName,sizeof(local.sun_path));
    if(stat(local.sun_path,&buf) != -1){
        if(unlink(local.sun_path) ==-1 )
        {
            IPMI_ERROR("UDSIfc.c : unlink error : %s\n",local.sun_path);
            return UDS_FAILURE;
        }	
    }

    pBMCInfo->UDSConfig.UDSSocket=socket(AF_UNIX,SOCK_STREAM,0);
    if(UDS_FAILURE == pBMCInfo->UDSConfig.UDSSocket)
    {
        IPMI_ERROR("UDSIfc.c : Unable to create the UNIX Domain Socket\n");
        return UDS_FAILURE;
    }

   /* Bind pBMCInfo->UDSConfig.UDSSocket to the particular socket file in the path*/
    if (UDS_FAILURE == bind(pBMCInfo->UDSConfig.UDSSocket,(struct sockaddr *)&local,sizeof(local)))
    {
        IPMI_ERROR("UDSIfc.c : Error binding  UNIX Domain Socket, %d, %s\n", errno, strerror(errno));
        return UDS_FAILURE;
    }
    if (UDS_FAILURE == listen(pBMCInfo->UDSConfig.UDSSocket,pBMCInfo->IpmiConfig.MaxSession))
    {
        IPMI_ERROR ("UDSIfc.c : Error listen\n");
        return UDS_FAILURE;
    }
    
    /* Changing the Permission of other user to
     * avoid permission denied problems while
     * client tries to connect to server */
    if(UDS_SUCCESS > chmod(local.sun_path,S_IRWXU|S_IRGRP|S_IXGRP|S_IRWXO))
    {
        IPMI_WARNING("UDSIfc.c : Cannot Change the permission of Unix Domain Socket\n");
    }

    return UDS_SUCCESS;
}

/**
 * @fn RecvUDSPkt
 * @brief This functon is used to Recv the UDS Pkt
 * @param BMCInst
**/
void *RecvUDSPkt(void *pArg)
{
    MsgPkt_T MsgPkt;
    struct timeval Timeout;
    struct sockaddr_un local;
    int  *Inst = (int *)pArg;
    int BMCInst,RetVal,max,Index = 0;
    unsigned int locallen;
    int UDSSocket= UDS_FAILURE;
    prctl(PR_SET_NAME,__FUNCTION__,0,0,0);
    int curThreadIndex = 0;
    BMCInst = *Inst;
    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
    SocketTbl_T *SocketTable = pBMCInfo->pUDSocketTbl;

    if(g_corefeatures.ipmi_thread_monitor_support == ENABLED)
    {
        pthread_t thread_id = pthread_self();
        for(; curThreadIndex <= gthreadIndex; curThreadIndex++)
        {
            if(pthread_equal(gthreadIDs[curThreadIndex], thread_id))
            {
                IPMI_DBG_PRINT_2("UDSIfc.c: RecvUDSPkt thread[ID: %d] found with index=%d.\n", thread_id, curThreadIndex);
                gthread_monitor_health[curThreadIndex].validThreadID = THREAD_ID_FOUND;
                gthread_monitor_health[curThreadIndex].BMCInst = BMCInst;
                if(snprintf(gthread_monitor_health[curThreadIndex].threadName,sizeof(gthread_monitor_health[curThreadIndex].threadName),"%s",__FUNCTION__)>=(signed)sizeof(gthread_monitor_health[curThreadIndex].threadName))
                TCRIT("RecvUDSPkt-source buffer truncated\n");
                break;
            }
        }
    }

    while(TRUE)
    {

    memset(&local,0,sizeof(local));

    FD_ZERO(&pBMCInfo->UDSConfig.cfdread);
    FD_SET(pBMCInfo->UDSConfig.UDSSocket,&pBMCInfo->UDSConfig.cfdread);
    max=pBMCInfo->UDSConfig.UDSSocket;

    locallen=sizeof(local);

        Timeout.tv_sec  =   pBMCInfo->IpmiConfig.SessionTimeOut; 
	Timeout.tv_usec =   0;

        /*Setting the File Descriptors in the fdread set again*/
        SetUDSFd(&pBMCInfo->UDSConfig.cfdread,&max,BMCInst);

        if(g_corefeatures.ipmi_thread_monitor_support == ENABLED && gthread_monitor_health[curThreadIndex].validThreadID == THREAD_ID_FOUND)
        {
            OS_THREAD_MUTEX_ACQUIRE(&ThreadMonitorMutex, WAIT_INFINITE);
            gthread_monitor_health[curThreadIndex].monitor = THREAD_STATE_MONITOR_NO;
            gthread_monitor_health[curThreadIndex].time = 0;
            OS_THREAD_MUTEX_RELEASE(&ThreadMonitorMutex);
        }
        /*Waits for an event to occur on socket*/
        RetVal = select ((max+1), &pBMCInfo->UDSConfig.cfdread, NULL, NULL, &Timeout);
        if (UDS_FAILURE == RetVal)
        {
            continue;
        }
        if (UDS_SUCCESS == RetVal)
        {
            /* Its due to timeout - continue */
            continue;
        }
        if(g_corefeatures.ipmi_thread_monitor_support == ENABLED && gthread_monitor_health[curThreadIndex].validThreadID == THREAD_ID_FOUND)
        {
            OS_THREAD_MUTEX_ACQUIRE(&ThreadMonitorMutex, WAIT_INFINITE);
            gthread_monitor_health[curThreadIndex].monitor = THREAD_STATE_MONITOR_YES;
            OS_THREAD_MUTEX_RELEASE(&ThreadMonitorMutex);
        }

        /*Accepting Connection*/
        if(FD_ISSET(pBMCInfo->UDSConfig.UDSSocket,&pBMCInfo->UDSConfig.cfdread))
        {
            UDSSocket=accept(pBMCInfo->UDSConfig.UDSSocket,(struct sockaddr *)&local,&locallen);
            if(UDS_FAILURE == UDSSocket)
            {
                IPMI_WARNING("UDSIfc.c:Accept Failed in UDSInterface");
                continue;
            }
            else
            {
            	OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].UDSSocketTblMutex, WAIT_INFINITE);
                if(AddUDSSocket(UDSSocket,FALSE,TRUE,BMCInst) != UDS_SUCCESS)
                {
                    close(UDSSocket);
                    OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].UDSSocketTblMutex);
                    continue;
                }
                OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].UDSSocketTblMutex);
                max=(UDSSocket)>=max?UDSSocket:max;
            }
        }

        /*Reading Data from Client*/
        for(Index = 0;Index < (pBMCInfo->IpmiConfig.MaxSession+1); Index++ )
        {
        	OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].UDSSocketTblMutex, WAIT_INFINITE);
            if((FD_ISSET(SocketTable[Index].Socket,&pBMCInfo->UDSConfig.cfdread))&&(SocketTable[Index].Valid))
            {
                if(UDS_SUCCESS ==  ReadUDSData( &MsgPkt,SocketTable[Index].Socket,BMCInst))
                {
                    if (UDS_SUCCESS != PostMsg (&MsgPkt, UDS_IFC_Q,BMCInst))
                    {
                        IPMI_WARNING("UDSIfc.c:Cannot Post Data to UDSIfcQ");
                    }

                    /* Fortify issue :: False Positive Buffer Overflow */
                    SocketTable[Index].Time = 0;
                }
                
                else
                {
                    if(SocketTable[Index].Time > UDS_TIMER_MILS_INTERVAL) { //Quanta, avoid clear session after session created and do nothing. 
                        FD_CLR(SocketTable[Index].Socket,&pBMCInfo->UDSConfig.cfdread);
                        close(SocketTable[Index].Socket);
                        RemoveUDSSession(SocketTable[Index].Socket,BMCInst);
                        RemoveUDSSocket(SocketTable[Index].Socket,BMCInst);
                    }
                }
            }
            OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].UDSSocketTblMutex);
        }
   }

}

/**
 * @fn AddUDSSocket
 * @brief This function is used to Add Socket to the socket table
 * @param Socket,BMCInst
 * @note Please acquire the UDS socket mutex lock before calling this function.
**/
int
AddUDSSocket (int Socket,INT8U IsLoopBackSocket, INT8U IsFixedSocket, int BMCInst)
{
    int Index = 0;
    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
    SocketTbl_T *SocketTable = pBMCInfo->pUDSocketTbl;

    for (Index = 0; Index < (pBMCInfo->IpmiConfig.MaxSession + 1); Index++ )
    {
        if (UDS_SUCCESS == SocketTable[Index].Valid)
        {
            /* Fortify issue :: False Positive Buffer Overflow */
            SocketTable[Index].Socket = Socket;
            SocketTable[Index].Valid  = 1;
            SocketTable[Index].Time   = 0;
            SocketTable[Index].IsLoopBackSocket= IsLoopBackSocket;
            SocketTable[Index].IsFixedSocket=IsFixedSocket;
            return UDS_SUCCESS;
        }
    }

    IPMI_WARNING ("UDSIfc.c : Error adding new socket to the table\n");
    return UDS_FAILURE;
}

/**
 * @fn RemoveUDSSocket
 * @brief This function is used to remove the socket from socket table
 * @param Socket,BMCInst
 * @note Please acquire the UDS socket mutex lock before calling this function.
**/
int
RemoveUDSSocket (int Socket, int BMCInst)
{
    int Index = 0;
    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
    SocketTbl_T *SocketTable = pBMCInfo->pUDSocketTbl;

    for (Index = 0; Index < (pBMCInfo->IpmiConfig.MaxSession + 1); Index++)
    {
        if (Socket == SocketTable[Index].Socket)
        {
            /* Fortify issue :: False Positive Buffer Overflow */
            SocketTable[Index].Socket = 0;
            SocketTable[Index].Valid  = 0;
            return UDS_SUCCESS;
        }
    }

    return UDS_FAILURE;
}

/**
 * @fn ReadUDSData
 * @brief This function is used to read the data from socket
 * @param MsgPkt,Socket,BMCInst
**/
int ReadUDSData(MsgPkt_T *MsgPkt,int Socket,int BMCInst)
{
    INT8U* pData =(INT8U *) & MsgPkt->Data[0];
    INT16U RecvdLen = 0,RemLen = 0,Len = 0;
    unsigned int SourceLen = 0;
    struct sockaddr_un Source;
    IPMIUDSMsg_T *pIPMIUDSMsg = (IPMIUDSMsg_T *)&pData[0];
    SourceLen = sizeof(Source);
    TDBG("\n unused parameters %d\n",BMCInst);
    
    while (RecvdLen < sizeof(IPMIUDSMsg_T))
    {
        Len = recvfrom (Socket, &pData[RecvdLen],sizeof(IPMIUDSMsg_T) - RecvdLen,0,(struct sockaddr *)&Source,&SourceLen);
        if (Len <= 0)
        {
            return UDS_FAILURE;
        }
           RecvdLen += Len;
    }

    if (pIPMIUDSMsg->IPMIMsgLen < RecvdLen)
    {
    	IPMI_ERROR("UDSIfc.c: UDS message packet length is invalid.\n");
    	return UDS_FAILURE;
    }
    if (pIPMIUDSMsg->IPMIMsgLen > MSG_PAYLOAD_SIZE)
    {
    	IPMI_ERROR("UDSIfc.c: UDS message packet too big.\n");
    	return UDS_FAILURE;
    }
    RemLen = pIPMIUDSMsg->IPMIMsgLen-RecvdLen;

    while(RemLen>0)
    {
        Len = recvfrom (Socket,&pData[RecvdLen],pIPMIUDSMsg->IPMIMsgLen-RecvdLen,0,(struct sockaddr *)&Source,&SourceLen);
        if(Len <= 0)
        {
            return UDS_FAILURE;
        }
        RecvdLen += Len;
        RemLen -= Len;
    }

    MsgPkt->SessionID = pIPMIUDSMsg->SessionID;
    MsgPkt->Cmd = pIPMIUDSMsg->Cmd;
    MsgPkt->NetFnLUN = pIPMIUDSMsg->NetFnLUN;
    MsgPkt->Privilege = pIPMIUDSMsg->Privilege;
    MsgPkt->Socket = Socket;
    MsgPkt->Size = RecvdLen;
    MsgPkt->Param = UDS_SMB_PARAM;
    MsgPkt->Channel = pIPMIUDSMsg->ChannelNum;
    MsgPkt->SessionType = UDS_SESSION_TYPE;
    if(snprintf((char *)MsgPkt->SrcQ,sizeof(MsgPkt->SrcQ),"%s",UDS_RES_Q)>=(signed)sizeof(MsgPkt->SrcQ))
    TCRIT("ReadUDSDatasource buffer truncated\n");

    return UDS_SUCCESS;
}

/**
 * @fn ProcessUDSBridging Request
 * @brief This function is used to Process the Bridging Request
 * @param pReq,BMCInst
**/
static int ProcessUDSBridgeReq(MsgPkt_T * pReq, int BMCInst)
{
    /* Process UDS Bridge Request from the IPMB Devices and send it to the 
        internal applications ASAP */
      UDSSessionTbl_T   *pUDSSessionInfo = GetUDSSessionInfo (UDS_SESSION_ID_INFO, pReq->Data,BMCInst);
       if(NULL == pUDSSessionInfo)
      {
          IPMI_DBG_PRINT("UDS Session already closed... so dropping the Bridging Response\n");
          return UDS_FAILURE;
      }

       pReq->Socket = pUDSSessionInfo->UDSSocket;

       if(UDS_SUCCESS == SendUDSPkt(pReq,BMCInst))
       {
           IPMI_DBG_PRINT("Bridging Response Sent Successfully\n");
           return UDS_SUCCESS;
       }

    return UDS_FAILURE;
}


/**
 * @fn ProcessUDSReq
 * @brief This function is used to Process the Request
 * @param pReq,BMCInst
**/
static int ProcessUDSReq(MsgPkt_T *pReq,int BMCInst)
{
    MsgPkt_T pRes,pTmpRes;
    int RetVal = 0;
    UDSSessionTbl_T *pUDSSessionInfo = NULL;

    /*Check Whether the request is from valid session id*/
    if(pReq->SessionID != 0)
    {
          pUDSSessionInfo = GetUDSSessionInfo (UDS_SESSION_ID_INFO,&pReq->SessionID, BMCInst);
          if(pUDSSessionInfo == NULL)
          {
                IPMI_WARNING("Session Info is NULL for the requested Session ID\n");
                FillUDSResponsePacket(pReq,&pRes,CC_INV_DATA_FIELD,BMCInst);
                SendUDSPkt(&pRes,BMCInst);
                return UDS_FAILURE;
          }
          /*Resetting the TimeoutValue for the requested session*/
          pUDSSessionInfo->SessionTimeoutValue = g_BMCInfo[BMCInst].IpmiConfig.SessionTimeOut;
    }
    else if((pReq->SessionID == 0) && CheckReservedCmd(pReq->Cmd))
    {
        FillUDSResponsePacket(pReq,&pRes,CC_INV_DATA_FIELD,BMCInst);
        SendUDSPkt(&pRes,BMCInst);
        return UDS_FAILURE;
    }

   if((CMD_SEND_MSG == pReq->Cmd) && (NETFN_APP == NET_FN(pReq->NetFnLUN)))
   {
       memcpy(&pTmpRes,pReq,sizeof(MsgPkt_T)-MSG_PAYLOAD_SIZE);
   }

    UpdateGetMsgTime ( pReq,IfcType, BMCInst);

    /*Posting the request to Msg Handler for processing the command */
    if (UDS_SUCCESS != PostMsg (pReq, MSG_HNDLR_Q, BMCInst))
    {
        IPMI_ASSERT (FALSE);
        return UDS_FAILURE;
    }

    do {
        RetVal = GetMsg (&pRes, UDS_RES_Q, pReq->ResTimeOut, BMCInst);
    }while( (g_corefeatures.ipmi_res_timeout == ENABLED) &&
            (!RetVal) && (pTmpRes.SessionID != pReq->SessionID )  && !IsResponseMatch(pReq,&pRes, UDS_IFC));

    if (-2 == RetVal)
    {
        /* Set the UDS Response Failure in REQUEST packet */
        FillUDSResponsePacket(pReq,&pRes,g_coremacros.res_timeout_compcode,BMCInst);
        SendUDSPkt(&pRes,BMCInst);
        return UDS_FAILURE;
    }

    if(!CheckReservedCmd(pRes.Cmd) && (pRes.SessionID == 0) && (pRes.Cmd != ReservedCmd[1]))
    {
        RetVal = AddUDSInfo(pReq,&pRes,BMCInst);
    }

     if((CMD_SEND_MSG == pReq->Cmd) && (NETFN_APP ==  NET_FN(pReq->NetFnLUN)))
    {
        pRes.Socket = pTmpRes.Socket;
    }

    /* Sending the Packet to the corresponding client*/
    if((UDS_SUCCESS == RetVal) || (UDS_FAILURE == RetVal))
    {
        SendUDSPkt(&pRes,BMCInst);
    }

    return UDS_SUCCESS;
}

/**
 * @fn SendUDSPkt
 * @brief This function sends the IPMI UDS Response to the requestor
 * @param pRes - Response message.
**/
int SendUDSPkt (MsgPkt_T *pRes,int BMCInst )
{
	TDBG("\n unused parameters %d\n",BMCInst);
    /* Send the UDS response packet */
     if(UDS_FAILURE == send(pRes->Socket,pRes->Data,pRes->Size,MSG_NOSIGNAL))
     {
       if((EBADF == errno) || (EPIPE == errno))
       {
               return UDS_SUCCESS;
       }
       else
       {
           IPMI_WARNING("UDSIfc.c: Send UDS Pkt Failed\n");
           return UDS_FAILURE;
       }
     }
    return UDS_SUCCESS;
}

/**
 * @fn SetFd
 * @brief This function sets the fd in fdread
 * @param newfd,BMCInst
**/
int SetUDSFd(fd_set *newfd,int *maximum,int BMCInst)
{
    int Index = 0;
    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
    SocketTbl_T *UDSSocketTbl = pBMCInfo->pUDSocketTbl;

    FD_ZERO(newfd);

    FD_SET(pBMCInfo->UDSConfig.UDSSocket,newfd);

    *maximum  = pBMCInfo->UDSConfig.UDSSocket;

    for(Index = 0;Index < (pBMCInfo->IpmiConfig.MaxSession+1); Index++)
    {
    	OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].UDSSocketTblMutex,WAIT_INFINITE);
        if((1 == UDSSocketTbl[Index].Valid)&&( 0 != UDSSocketTbl[Index].Socket))
        {
            FD_SET(UDSSocketTbl[Index].Socket,newfd);

            if(*maximum < UDSSocketTbl[Index].Socket)
            {
                *maximum = UDSSocketTbl[Index].Socket;
            }
        }
        OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].UDSSocketTblMutex);
    }

  return UDS_SUCCESS;
}

/**
 * @fn FillUDSResponsePacket
 * @brief This function fills the pRes when Error Occured
 * @param pReq,pRes,CompletionCode,BMCInst
**/
int FillUDSResponsePacket(MsgPkt_T *pReq,MsgPkt_T *pRes,uint8 UDSCompletionCode,int BMCInst)
{
    IPMIUDSMsg_T *pIPMIUDSMsgReq = (IPMIUDSMsg_T *)&pReq->Data[0];
    IPMIUDSMsg_T *pIPMIUDSMsgRes = (IPMIUDSMsg_T *)&pRes->Data[0];
    uint8 *byCompletionCode = (uint8 *)&pRes->Data[sizeof(IPMIUDSMsg_T)];

    TDBG("\n unused parameters %d\n",BMCInst);
    pRes->Socket = pReq->Socket;
    pReq->Cmd = pIPMIUDSMsgReq->Cmd;

    pIPMIUDSMsgRes->SessionID = pIPMIUDSMsgReq->SessionID;
    pIPMIUDSMsgRes->Cmd = pIPMIUDSMsgReq->Cmd;
    pIPMIUDSMsgRes->NetFnLUN = pIPMIUDSMsgReq->NetFnLUN;
    pIPMIUDSMsgRes->Privilege = pIPMIUDSMsgReq->Privilege;
    pIPMIUDSMsgRes->IPMIMsgLen = sizeof(IPMIUDSMsg_T)+sizeof(INT8U)+1;
    pIPMIUDSMsgRes->ChannelNum = pIPMIUDSMsgReq->ChannelNum;
    pIPMIUDSMsgRes->AuthFlag = pIPMIUDSMsgReq->AuthFlag;
    
    if(snprintf( (char *)pIPMIUDSMsgRes->UserName,MAX_USER_NAME_LEN ,"%s" ,(char *)pIPMIUDSMsgReq->UserName) >= MAX_USER_NAME_LEN)
    {
        TCRIT("Buffer Overflow in FillUDSResponsePacket\n");
        *byCompletionCode = CC_OUT_OF_SPACE;
        return UDS_FAILURE;
    }
    _fmemcpy(pIPMIUDSMsgRes->IPAddr, pIPMIUDSMsgReq->IPAddr, sizeof (struct in6_addr));

    pRes->Size = pIPMIUDSMsgRes->IPMIMsgLen;

    *byCompletionCode = UDSCompletionCode; 

    return UDS_SUCCESS;
}

/**
 * @fn UpdateUDSTimeout
 * @brief This function Updates the timeout value for each socket opened
 * @param BMCInst
**/
static int
UpdateUDSTimeout (int BMCInst)
{
    int Index = 0;
    int TimeOut=0;
    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
    SocketTbl_T *SocketTable = pBMCInfo->pUDSocketTbl;

	TimeOut = (pBMCInfo->IpmiConfig.SessionTimeOut)*1000000;//Quanta
    for (Index = 0; Index < (pBMCInfo->IpmiConfig.MaxSession + 1); Index++ )
    {
    	OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->UDSSocketTblMutex,WAIT_INFINITE);
        if(SocketTable[Index].Valid)
        {
            //TimeOut = pBMCInfo->IpmiConfig.SessionTimeOut;//Quanta--
            if (SocketTable[Index].Time >= TimeOut)
            {
                IPMI_DBG_PRINT("Local Socket Timed Out\n");
                close (SocketTable[Index].Socket);
                RemoveUDSSession(SocketTable[Index].Socket,BMCInst);
                //RemoveUDSSocket(SocketTable[Index].Socket,BMCInst);
                /* Fortify issue :: False Positive Buffer Overflow */
                SocketTable[Index].Socket = 0;
                SocketTable[Index].Valid  = 0;

            }
            else
            {   /* update the time */
                /* Fortify issue :: False Positive Buffer Overflow */
                SocketTable[Index].Time += UDS_TIMER_MILS_INTERVAL;//UDS_TIMER_INTERVAL;//Quanta
            }
        }
        OS_THREAD_MUTEX_RELEASE(&pBMCInfo->UDSSocketTblMutex);
    }

    return UDS_SUCCESS;
}

/**
 * @fn  UDSTimer
 * @brief This function handles the time out for uds connections.
 * @param None
 **/
static
void*  UDSTimer (void *pArg)
{
    int *inst = (int*)pArg;
    int BMCInst= *inst;
    prctl(PR_SET_NAME,__FUNCTION__,0,0,0);

    while (TRUE)
    {
        UpdateUDSTimeout (BMCInst);
        //sleep (UDS_TIMER_INTERVAL);//Quanta--
        usleep(UDS_TIMER_MILS_INTERVAL);//Quanta
    }

    return UDS_SUCCESS;
}


/**
 * @fn  AddUDSInfo
 * @brief This function adds the session information in Session Table
 * @param UDS Request Packet,UDS Response Packet,BMCInst
 **/
int AddUDSInfo(MsgPkt_T *pUDSReq,MsgPkt_T *pUDSRes,int BMCInst)
{
    uint8 *byCompletionCode = (uint8 *)&pUDSRes->Data[sizeof(IPMIUDSMsg_T)];
    IPMIUDSMsg_T *pRes = (IPMIUDSMsg_T *)&pUDSRes->Data[0];
    _FAR_ ChannelInfo_T *pChanneludsInfo = NULL;
    _FAR_ ChannelUserInfo_T *pChUserInfo = NULL;
    _FAR_ UserInfo_T *pUserInfo = NULL;
    UDSSessionTbl_T UDSSessionInfo;
    INT32U SessionID;
    INT8U UserID,Index,SessionHandle=0;
    _FAR_ BMCInfo_t *pBMCInfo = &g_BMCInfo[BMCInst];
    char   UserPswd[MAX_PASSWORD_LEN];
    INT8U ChannelNum = CH_NOT_USED;
    INT8U PwdEncKey[MAX_SIZE_KEY + 1] = {0};
    char  EncryptedPswd[MAX_ENCRYPTED_PSWD_LEN + 1] = {0};
    _fmemset(&UDSSessionInfo,0,sizeof(UDSSessionTbl_T));

    if(*byCompletionCode == CC_NORMAL)
    {
        if(ReservedCmd[0] == pRes->Cmd)
        {
            UserID = 0;
        }
        else
        {
            UserID = (uint8)pUDSReq->Data[sizeof(IPMIUDSMsg_T)];
        }

        if(pBMCInfo ->UDSSessionTblInfo.SessionCount >= pBMCInfo->IpmiConfig.MaxSession)
        {
            IPMI_WARNING("UDSIfc.c:Maximum Session Limit Reached\n");
            FillUDSResponsePacket(pUDSReq,pUDSRes,CC_ACTIVATE_SESS_NO_SESSION_SLOT_AVAILABLE,BMCInst);
            SendUDSPkt(pUDSRes,BMCInst);
            CloseUDSSocket(pUDSRes->Socket,BMCInst);
            return UDS_PARAM_FAILURE;
        }

        do
        {
            RAND_bytes((INT8U*)&SessionID,sizeof(INT32U));
        } while ((NULL != GetUDSSessionInfo (UDS_SESSION_ID_INFO,&SessionID, BMCInst)) || (0 == SessionID));

        SessionHandle = BMC_GET_SHARED_MEM(BMCInst)->UDSSessionHandle;
        
        do
        {
            SessionHandle += 1;
        } while (NULL != GetUDSSessionInfo (UDS_SESSION_HANDLE_INFO,&SessionHandle, BMCInst));

       LOCK_BMC_SHARED_MEM(BMCInst);
       BMC_GET_SHARED_MEM(BMCInst)->UDSSessionHandle = SessionHandle;
       UNLOCK_BMC_SHARED_MEM(BMCInst);

        if( UDS_SMBMC_CHANNEL != pUDSReq->Channel)
        {
            pChanneludsInfo = GetLANChannelInfoForUDS(pUDSReq->Channel,&ChannelNum,&pRes->ChannelNum,BMCInst);
            if(pChanneludsInfo == NULL)
            {
                IPMI_WARNING("UDSIfc.c:Invalid Channel\n");
                FillUDSResponsePacket(pUDSReq,pUDSRes,CC_INV_DATA_FIELD,BMCInst);
                SendUDSPkt(pUDSRes,BMCInst);
                CloseUDSSocket(pUDSRes->Socket,BMCInst);
                return UDS_PARAM_FAILURE;
            }
        }

        if( (0 != UserID) && ( UDS_SMBMC_CHANNEL != pUDSReq->Channel ))
        {
            pUserInfo = getUserIdInfo (UserID & 0x3F, BMCInst);
            if (NULL == pUserInfo)
            {
                FillUDSResponsePacket(pUDSReq,pUDSRes,CC_INV_DATA_FIELD,BMCInst);
                SendUDSPkt(pUDSRes,BMCInst);
                CloseUDSSocket(pUDSRes->Socket,BMCInst);
                return UDS_PARAM_FAILURE;
            }
            if (g_corefeatures.userpswd_encryption == ENABLED)
            {
                /* Get Encryption Key from the MBMCInfo_t structure */
                LOCK_BMC_SHARED_MEM(BMCInst);
                    memcpy(PwdEncKey, &(g_MBMCInfo.PwdEncKey), MAX_SIZE_KEY);
                UNLOCK_BMC_SHARED_MEM(BMCInst);
                if(g_corefeatures.encrypted_pwd_in_hexa_format == ENABLED)
                {
                    //Extract Hex Array content from string
                    ConvertStrtoHex((char *)&pBMCInfo->EncryptedUserInfo[(UserID - 1) & 0x3F].EncryptedHexPswd[0], (char *)EncryptedPswd, MAX_ENCRYPTED_PSWD_LEN);
                }
                else
                {
                    Decode64((char *)EncryptedPswd, (char *)&pBMCInfo->EncryptedUserInfo[(UserID - 1) & 0x3F].EncryptedHexPswd[0], MAX_ENCRYPTED_PSWD_LEN);
                }
                if(DecryptPassword((INT8 *)EncryptedPswd, MAX_PASSWORD_LEN, (INT8 *)UserPswd, MAX_PASSWORD_LEN, PwdEncKey))
                {
                    TCRIT("Error in decrypting the user password for user ID:%d. .\n", UserID);
                    FillUDSResponsePacket(pUDSReq, pUDSRes, CC_UNSPECIFIED_ERR, BMCInst);
                    SendUDSPkt(pUDSRes,BMCInst);
                    CloseUDSSocket(pUDSRes->Socket, BMCInst);
                    return UDS_PARAM_FAILURE;
                }
            }
            else
            {
                _fmemcpy (UserPswd, pUserInfo->UserPassword, MAX_PASSWORD_LEN);
            }

#if 0 //Not needed as already done the same before use little below
            if((strlen((char *)pUserInfo->UserName) != 0) && (strlen((char *)UserPswd)!= 0) && (pUserInfo->UserStatus == FALSE))
            {
                FillUDSResponsePacket(pUDSReq,pUDSRes,CC_INV_CMD,BMCInst);
                SendUDSPkt(pUDSRes,BMCInst);
                CloseUDSSocket(pUDSRes->Socket,BMCInst);
                return UDS_PARAM_FAILURE;
            }
#endif

            OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->ChUserMutex,WAIT_INFINITE);
            if(pChanneludsInfo == NULL || pChanneludsInfo->ChannelUserInfo == NULL)
            	return UDS_PARAM_FAILURE;
            pChUserInfo = getChUserIdInfo (UserID & 0x3F, &Index, pChanneludsInfo->ChannelUserInfo, BMCInst);
            if (NULL == pChUserInfo)
            {
                OS_THREAD_MUTEX_RELEASE(&pBMCInfo->ChUserMutex);
                FillUDSResponsePacket(pUDSReq,pUDSRes,CC_INV_DATA_FIELD,BMCInst);
                SendUDSPkt(pUDSRes,BMCInst);
                CloseUDSSocket(pUDSRes->Socket,BMCInst);
                return UDS_PARAM_FAILURE;
            }
            else if(pChUserInfo->AccessLimit == PRIV_LEVEL_NO_ACCESS)
            {
                OS_THREAD_MUTEX_RELEASE(&pBMCInfo->ChUserMutex);
                FillUDSResponsePacket(pUDSReq,pUDSRes,CC_INSUFFIENT_PRIVILEGE,BMCInst);
                SendUDSPkt(pUDSRes,BMCInst);
                CloseUDSSocket(pUDSRes->Socket,BMCInst);
                return UDS_PARAM_FAILURE;
            }
            else if((strlen((char *)pUserInfo->UserName) != 0) && (strlen((char *)pUserInfo->UserPassword)!= 0) && (pChUserInfo->IPMIMessaging == FALSE))
            {
                OS_THREAD_MUTEX_RELEASE(&pBMCInfo->ChUserMutex);
                FillUDSResponsePacket(pUDSReq,pUDSRes,CC_INV_CMD,BMCInst);
                SendUDSPkt(pUDSRes,BMCInst);
                CloseUDSSocket(pUDSRes->Socket,BMCInst);
                return UDS_PARAM_FAILURE;
            }
            OS_THREAD_MUTEX_RELEASE(&pBMCInfo->ChUserMutex);
        }

        if(0 != UserID &&(pChUserInfo))
        {
            UDSSessionInfo.LoggedInPrivilege = pChUserInfo->AccessLimit;
        }
        else
        {
            UDSSessionInfo.LoggedInPrivilege = pUDSReq->Privilege;
        }

        UDSSessionInfo.Activated = TRUE;
        UDSSessionInfo.UDSSocket = pUDSRes->Socket;
        UDSSessionInfo.LoggedInUserID= UserID;
        UDSSessionInfo.LoggedInPrivilege = pUDSReq->Privilege;
        UDSSessionInfo.LoggedInChannel = ChannelNum;
        UDSSessionInfo.SessionID = SessionID;
        UDSSessionInfo.SessionTimeoutValue = pBMCInfo->IpmiConfig.SessionTimeOut;
        UDSSessionInfo.AuthenticationMechanism = pRes->AuthFlag;
        UDSSessionInfo.LoggedInSessionHandle = SessionHandle;
        UDSSessionInfo.UDSChannelNum = UDS_CHANNEL;

        _fmemcpy( (char *)UDSSessionInfo.LoggedInUsername, (char *)((IPMIUDSMsg_T *)&pUDSReq->Data[0])->UserName,MAX_USERNAME_LEN);
        _fmemcpy( (char *)UDSSessionInfo.LoggedInPassword, UserPswd,MAX_PASSWORD_LEN);
        _fmemcpy( (char *)UDSSessionInfo.IPAddr, (char *)((IPMIUDSMsg_T *)&pUDSReq->Data[0])->IPAddr,IP6_ADDR_LEN);

        UDSSessionInfo.ProcessID = ((IPMIUDSMsg_T *)&pUDSReq->Data[0])->ProcessID;
        UDSSessionInfo.ThreadID = ((IPMIUDSMsg_T *)&pUDSReq->Data[0])->ThreadID;
        if (AddUDSSession(&UDSSessionInfo,BMCInst) == 0)
        {
        	/*Assigning the Privilege Level and Session ID*/
        	pRes->SessionID = SessionID;
        	pRes->Privilege = UDSSessionInfo.LoggedInPrivilege;

        	return UDS_SUCCESS;
        }
    }
  return UDS_FAILURE;
}


/**
 * @fn  RemoveUDSSession
 * @brief This function deletes the session information in Session Table
 * @param Socket,BMCInst
 **/
INT8U RemoveUDSSession(SOCKET Socket,int BMCInst)
{
    _FAR_ UDSSessionTbl_T *pUDSSessionInfo = NULL;

    pUDSSessionInfo = GetUDSSessionInfo(UDS_SOCKET_ID_INFO,&Socket,BMCInst);
    if(NULL != pUDSSessionInfo)
    {
        DeleteUDSSession(pUDSSessionInfo,BMCInst);
    }

    return UDS_SUCCESS;
}

/**
 * @fn  CloseUDSSocket
 * @brief This function removes the socket and closes it
 * @param Socket,BMCInst
 **/
void CloseUDSSocket(int Socket,int BMCInst)
{
	OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].UDSSocketTblMutex, WAIT_INFINITE);
    close(Socket);
    RemoveUDSSocket(Socket,BMCInst);
    OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].UDSSocketTblMutex);
}

/**
 * @fn CheckReservedCmd
 * @brief This function is used to check whether the triggered command
 *            belongs to Reserved List
 * @param Cmd - Command Number
 **/
int CheckReservedCmd(INT8U Cmd)
{
    unsigned int index = 0;

    for(index = 0;index < sizeof(ReservedCmd); index++)
    {
        if(ReservedCmd[index] == Cmd)
        {
            return UDS_SUCCESS;
        }
    }

  return UDS_CMD_FAILURE;
}

/**
 * @fn GetLANChannelInfoForUDS
 * @brief This function is used to get the LAN Channel Info
 * @param - Requested UDS Channel number,BMC Instance and out pointer which points
                   to LAN Channel Number.
**/
ChannelInfo_T* GetLANChannelInfoForUDS(INT8U ReqUDSChannel,INT8U *LANChannel,INT8U *ResChannelNum,int BMCInst)
{
    BMCInfo_t *pBMCInfo = (BMCInfo_t *)&g_BMCInfo[BMCInst];
    INT8U Ch = 0;
    ChannelInfo_T *UDSChannelInfo = NULL;

    for(Ch = 1;Ch <= MAX_LAN_CHANNELS; Ch++)
    {
        if(ReqUDSChannel == UDS_CHANNEL)
        {
             if((pBMCInfo->RMCPLAN1Ch != CH_NOT_USED) && (*LANChannel == CH_NOT_USED))
             {
                 *LANChannel = pBMCInfo->RMCPLAN1Ch;
             }
             else if((pBMCInfo->RMCPLAN2Ch != CH_NOT_USED) && (*LANChannel != pBMCInfo->RMCPLAN2Ch))
             {
                 *LANChannel = pBMCInfo->RMCPLAN2Ch;
             }
             else if((pBMCInfo->RMCPLAN3Ch != CH_NOT_USED) && (*LANChannel != pBMCInfo->RMCPLAN3Ch))
             {
                 *LANChannel = pBMCInfo->RMCPLAN3Ch;
             }
        }
        else
        {
             *LANChannel = ReqUDSChannel;
        }

        UDSChannelInfo = getChannelInfo(*LANChannel,BMCInst);
        if(UDSChannelInfo == NULL)
        {
            if((ReqUDSChannel != UDS_CHANNEL) || (Ch == MAX_LAN_CHANNELS)) 
            {
                *LANChannel = CH_NOT_USED;
                return NULL;
            }
        }
        else if(UDSChannelInfo != NULL)
        {
            *ResChannelNum = *LANChannel;
            return UDSChannelInfo;
        }
     }

    *LANChannel = CH_NOT_USED;
    return NULL;

}


