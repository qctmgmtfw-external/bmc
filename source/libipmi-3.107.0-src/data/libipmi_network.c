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
* Filename: libipmi_network.c
*
* Descriptions: Contains implementation of network(UDP) layer API
*   required by RMCP layer for connecting to BMC thru network medium
*
* Author: Rajasekhar
*
******************************************************************/
#include "libipmi_errorcodes.h"
#include "platform.h"


#define RMCP_CLASS_MSG_OFFSET           3
#define IPMI_MSG_AUTH_TYPE_OFFSET       4
#define RMCP_ASF_PING_MESSAGE_LENGTH    12
#define IPMI_MSG_LEN_OFFSET             13
#define IPMI20_MSG_LEN_OFFSET           14
#define RMCP_CLASS_MSG_ASF              0x06
#define RMCP_CLASS_MSG_IPMI             0x07

#if LIBIPMI_IS_OS_LINUX()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#define snprintf sprintf_s
#endif
#include "dbgout.h"
#include "libipmi_struct.h"
#include "IPMI_RMCP.h"
#include "libipmi_rmcp.h"
#include "libipmi_network.h"
/*----------------------------------------------------------
UDP_Connect

Create a socket and store it in the supplied handle for
future use.
-----------------------------------------------------------*/
uint8 UDP_Connect(LAN_CHANNEL_T *LanInfo, char *szIPAddress, uint16 wPort)
{

    SOCKET              client_sock = -1;
    struct addrinfo     *pRes;
    struct addrinfo     *pTmp;
    char Port[6];
    int len=0,error=0;
    uint8 nRetVal='\0';

#if LIBIPMI_IS_OS_WINDOWS()
	int			nRet;
	/* Initialize WSA Sockets for windows */
	WORD wVersionRequested = MAKEWORD(2,2);
	WSADATA wsaData;

	/* Initialize WinSock and check version */
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		DEBUG_PRINTF("LIBIPMI_NETWORK.C: Wrong WinSock Version Selected \n");
		return LIBIPMI_MEDIUM_E_WSA_INIT_FAILURE;
	}
#endif

    sprintf(Port,"%d",wPort);
    error = getaddrinfo(szIPAddress,Port,NULL,&pRes);
    if(error != 0)
    {
        DEBUG_PRINTF("LIBIPMI_NETWORK.C: Failed to resolve the IP address\n");
        return LIBIPMI_MEDIUM_E_RESOLVED_ADDR_FAILURE;
    }

    for(pTmp=pRes;pTmp != NULL;pTmp = pTmp->ai_next)
    {

        /* Open the socket */
        client_sock = socket(pTmp->ai_family, SOCK_DGRAM, 0);

        if(client_sock == -1)
        {
            DEBUG_PRINTF("LIBIPMI_NETWORK.C: Unable to create socket \n");
            nRetVal= LIBIPMI_MEDIUM_E_INVALID_SOCKET;
            continue;
        }
        /*Connect*/
        if ( connect (client_sock,pTmp->ai_addr,pTmp->ai_addrlen) < 0)
        {
            DEBUG_PRINTF("LIBIPMI_NETWORK.C: Unable to connect to Server\n");
#if LIBIPMI_IS_OS_WINDOWS()
            closesocket(client_sock);
#elif LIBIPMI_IS_OS_LINUX()
            close(client_sock);
#endif
            nRetVal = LIBIPMI_MEDIUM_E_CONNECT_FAILURE;
            continue;
        }
        else
        {
            if(pTmp->ai_family == AF_INET6)
            {
               LanInfo->bFamily = AF_INET6;
                len = sizeof(struct in6_addr);
                memcpy(LanInfo->szIPAddress,&((struct sockaddr_in6 *)(pTmp->ai_addr))->sin6_addr.s6_addr,len);
            }
            else
            {
                LanInfo->bFamily = AF_INET;
                len = sizeof(struct in_addr);
                memcpy(LanInfo->szIPAddress,&((struct sockaddr_in *)(pTmp->ai_addr))->sin_addr.s_addr,len);
            }
            LanInfo->byIsConnected = 1;
            break;
        }

    }

    freeaddrinfo(pRes);

    if(LanInfo->byIsConnected != 1)
    {
#if LIBIPMI_IS_OS_WINDOWS()
            closesocket(client_sock);
#elif LIBIPMI_IS_OS_LINUX()
            close(client_sock);
#endif
        return nRetVal;
    }

	/* Store the information */
	LanInfo->wPort = wPort;
	LanInfo->hSocket = client_sock;
	LanInfo->byIsConnected = 1;

	return LIBIPMI_E_SUCCESS;
}

/*----------------------------------------------------------
UDP_Close

Closes the socket
-----------------------------------------------------------*/
void UDP_Close(LAN_CHANNEL_T *LanInfo )
{
#if LIBIPMI_IS_OS_WINDOWS()
	closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
	close(LanInfo->hSocket);
#endif

	/* reset the connected flag */
	LanInfo->byIsConnected = 0;

	return;
}

/*----------------------------------------------------------
UDP_SendData

Send the data to BMC. If write pipe is not empty, waits for
specified time and returns
-----------------------------------------------------------*/
uint8 UDP_SendData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32 dwSize, int timeout )
{
    int                             nTotalBytesSent, nBytesSent;
    struct timeval          wait_time, *pwait_time;
    fd_set                       wdfs;
    int                              ret;
    int                              nDes;
    struct sockaddr_in          server_addr;
    struct sockaddr_in6         server6_addr;

    /* Server parameters */

    if (LanInfo->bFamily == AF_INET)
    {
        memset (&server_addr, 0, sizeof (server_addr));
        server_addr.sin_family              = AF_INET;
        server_addr.sin_port                = htons ( LanInfo->wPort);
        memcpy(&server_addr.sin_addr.s_addr,LanInfo->szIPAddress,sizeof(struct in_addr));
    }
    else
    {
        memset (&server6_addr, 0, sizeof (server6_addr));
        server6_addr.sin6_family              = AF_INET6;
        server6_addr.sin6_port                = htons ( LanInfo->wPort);
        memcpy(&server6_addr.sin6_addr.s6_addr,LanInfo->szIPAddress,sizeof(struct in6_addr));
    }
	/* Wait for the socket to get ready */
	if(timeout >= 0)
	{
		wait_time.tv_sec = timeout;
		wait_time.tv_usec = 0;
		pwait_time = &wait_time;
	}
	else
		pwait_time = NULL;

#if LIBIPMI_IS_OS_WINDOWS()
	wdfs.fd_count = 1;
	wdfs.fd_array [0] = LanInfo->hSocket;
	nDes = 0;
#elif LIBIPMI_IS_OS_LINUX()
	FD_ZERO(&wdfs);
	FD_SET(LanInfo->hSocket, &wdfs);
	nDes = LanInfo->hSocket + 1;
#endif

UDPSendDataSelect:
	/* wait for specified amount of time */
	ret = select (nDes, NULL, &wdfs, NULL, pwait_time);

	/* socket error */
	if(ret == SOCKET_ERROR)
	{
#if LIBIPMI_IS_OS_WINDOWS()
		/* Windows */
		if(WSAGetLastError() == WSAEINTR)
		{
			DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
			goto UDPSendDataSelect;
		}
#elif LIBIPMI_IS_OS_LINUX()
		if(errno == EINTR)
		{
			DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
			goto UDPSendDataSelect;
		}
#endif
		else
		{
			DEBUG_PRINTF("select failed while waiting for write pipe \n");
			return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
		}
	}
	/* timed out */
	else if(ret == 0)
	{
		DEBUG_PRINTF("Connection timedout: Write Pipe is full \n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}

	/* socket is ready to send data */
	nTotalBytesSent = 0;

	/* Till the last byte is sent */
	while( nTotalBytesSent < (int)dwSize )

	{
            if (LanInfo->bFamily == AF_INET)
            {
                /* send data */
                nBytesSent = sendto (LanInfo->hSocket, szBuffer, dwSize, 0,
                                                                   (struct sockaddr*) &server_addr, sizeof (server_addr) );
           }
        else
        {
                /* send data */
                nBytesSent = sendto (LanInfo->hSocket, szBuffer, dwSize, 0,
                                                                   (struct sockaddr*) &server6_addr, sizeof (server6_addr) );
        }

		/* send failure */
		if (nBytesSent == SOCKET_ERROR)
		{
			DEBUG_PRINTF("Error while sending data \n");
			return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
		}
		else
			nTotalBytesSent += nBytesSent;

	}

	return LIBIPMI_STATUS_SUCCESS;
}

/*----------------------------------------------------------
UDP_ReceiveData

Receieves the data to BMC. If read pipe is empty, waits for 
specified time and returns
-----------------------------------------------------------*/
uint8 UDP_ReceiveData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32* pdwSize, int timeout )
{
	int						nBytesRecvd;
	struct timeval				wait_time, *pwait_time;
	fd_set					rdfs;
	int						ret;
	unsigned int			nServerAddrLen;
	int					nDes;
    struct sockaddr_in          server_addr;
    struct sockaddr_in6         server6_addr;

    /* Server parameters */

    if (LanInfo->bFamily == AF_INET)
    {
        memset (&server_addr, 0, sizeof (server_addr));
        server_addr.sin_family              = AF_INET;
        server_addr.sin_port                = htons ( LanInfo->wPort);
        memcpy(&server_addr.sin_addr.s_addr,LanInfo->szIPAddress,sizeof(struct in_addr));
    }
    else
    {
        memset (&server6_addr, 0, sizeof (server6_addr));
        server6_addr.sin6_family              = AF_INET6;
        server6_addr.sin6_port                = htons ( LanInfo->wPort);
        memcpy(&server6_addr.sin6_addr.s6_addr,LanInfo->szIPAddress,sizeof(struct in6_addr));
    }

	/* Wait for the socket to get ready */
	if(timeout >= 0)
	{
		wait_time.tv_sec = timeout;
		wait_time.tv_usec = 0;
		pwait_time = &wait_time;
	}
	else
		pwait_time = NULL;

#if LIBIPMI_IS_OS_WINDOWS()
	rdfs.fd_count = 1;
	rdfs.fd_array [0] = LanInfo->hSocket;
	nDes = 0;
#elif LIBIPMI_IS_OS_LINUX()
	FD_ZERO(&rdfs);
	FD_SET(LanInfo->hSocket, &rdfs);
	nDes = LanInfo->hSocket + 1;
#endif

UDPRecvDataSelect:
	/* wait for specified amount of time */
	ret = select (nDes, &rdfs, NULL, NULL, pwait_time);

	/* socket error */
	if(ret == SOCKET_ERROR)
	{
#if LIBIPMI_IS_OS_WINDOWS()
		/* Windows */
		if(WSAGetLastError() == WSAEINTR)
		{
			DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
			goto UDPRecvDataSelect;
		}
#elif LIBIPMI_IS_OS_LINUX()
		if(errno == EINTR)
		{
			DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
			goto UDPRecvDataSelect;
		}
#endif
		else
		{
			DEBUG_PRINTF("select failed while waiting for read pipe \n");
			return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
		}
	}
	/* timed out */
	else if(ret == 0)
	{
		DEBUG_PRINTF("Connection timedout: no data receivable \n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}

    if (LanInfo->bFamily == AF_INET)
    {
        nServerAddrLen = sizeof (server_addr);
        /* socket is ready to receive data */
        nBytesRecvd = recvfrom (LanInfo->hSocket, szBuffer, MAX_RESPONSE_SIZE, 0,
                                                    (struct sockaddr*) &server_addr, &nServerAddrLen );
    }
    else
    {
        nServerAddrLen = sizeof (server6_addr);
        /* socket is ready to receive data */
        nBytesRecvd = recvfrom (LanInfo->hSocket, szBuffer, MAX_RESPONSE_SIZE, 0,
                                                    (struct sockaddr*) &server6_addr, &nServerAddrLen );
    }

	/* Error while receiving data */
	if(nBytesRecvd == SOCKET_ERROR)
	{
		DEBUG_PRINTF("Error while receiving data\n");
		return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
	}

	*pdwSize = nBytesRecvd;


	return LIBIPMI_STATUS_SUCCESS;
}


/**************************************************************************************/


/*----------------------------------------------------------
TCP_Connect

Create a socket and store it in the supplied handle for
future use.
-----------------------------------------------------------*/
uint8 TCP_Connect(LAN_CHANNEL_T *LanInfo, char *szIPAddress, uint16 wPort)
{

    SOCKET      client_sock = -1;
    struct addrinfo     *pRes;
    struct addrinfo     *pTmp;
    char Port[6];
    int error = 0,ret=0;
    uint8 nRetVal='\0';

#if LIBIPMI_IS_OS_WINDOWS()
    int         nRet;
    /* Initialize WSA Sockets for windows */
    WORD wVersionRequested = MAKEWORD(2,2);
    WSADATA wsaData;

    /* Initialize WinSock and check version */
    nRet = WSAStartup(wVersionRequested, &wsaData);
    if (wsaData.wVersion != wVersionRequested)
    {
    DEBUG_PRINTF("LIBIPMI_NETWORK.C: Wrong WinSock Version Selected \n");
    return LIBIPMI_MEDIUM_E_WSA_INIT_FAILURE;
    }
#endif

    sprintf(Port,"%d",wPort);
    error = getaddrinfo(szIPAddress,Port,NULL,&pRes);
    if(error != 0)
    {
        DEBUG_PRINTF("LIBIPMI_NETWORK.C: Failed to resolve the IP address\n");
        return LIBIPMI_MEDIUM_E_RESOLVED_ADDR_FAILURE;
    }

    for(pTmp=pRes;pTmp != NULL;pTmp = pTmp->ai_next)
    {

        /* Open the socket */
        client_sock = socket(pTmp->ai_family, SOCK_STREAM, 0); /* Fortify Issues :: False Positive */

        if(client_sock == -1)
        {
            DEBUG_PRINTF("LIBIPMI_NETWORK.C: Unable to create socket \n");
            nRetVal= LIBIPMI_MEDIUM_E_INVALID_SOCKET;
            continue;
        }
        /*Connect*/
        if ( connect (client_sock,pTmp->ai_addr,pTmp->ai_addrlen) < 0)
        {
            DEBUG_PRINTF("LIBIPMI_NETWORK.C: Unable to connect to Server\n");
#if LIBIPMI_IS_OS_WINDOWS()
            closesocket(client_sock);
#elif LIBIPMI_IS_OS_LINUX()
            close(client_sock);
#endif
            nRetVal = LIBIPMI_MEDIUM_E_CONNECT_FAILURE;
            continue;
        }
        else
        {
            LanInfo->byIsConnected = 1;
            break;
        }

    }

    freeaddrinfo(pRes);

    if(LanInfo->byIsConnected != 1)
    {
        return nRetVal;
    }

    /* Store the information */
    ret= snprintf(LanInfo->szIPAddress,sizeof(LanInfo->szIPAddress),"%s",szIPAddress);
    if(ret >= (signed)sizeof(LanInfo->szIPAddress))
    {
    	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
    	return 0;
    }
    LanInfo->wPort = wPort;
    LanInfo->hSocket = client_sock;


    return LIBIPMI_E_SUCCESS;
}

/*----------------------------------------------------------
TCP_Close

Closes the socket
-----------------------------------------------------------*/
void TCP_Close(LAN_CHANNEL_T *LanInfo )
{
#if LIBIPMI_IS_OS_WINDOWS()
	closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
	close(LanInfo->hSocket);
#endif

	/* reset the connected flag */
	LanInfo->byIsConnected = 0;

	return;
}

/*----------------------------------------------------------
TCP_SendData

Send the data to BMC. If write pipe is not empty, waits for
specified time and returns
-----------------------------------------------------------*/
uint8 TCP_SendData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32 dwSize, int timeout )
{
	int						nTotalBytesSent, nBytesSent;
	struct timeval				wait_time, *pwait_time;
	fd_set					wdfs;
	int						ret;
	int					nDes;

	/* Wait for the socket to get ready */
	if(timeout >= 0)
	{
		wait_time.tv_sec = timeout;
		wait_time.tv_usec = 0;
		pwait_time = &wait_time;
	}
	else
		pwait_time = NULL;

#if LIBIPMI_IS_OS_WINDOWS()
	wdfs.fd_count = 1;
	wdfs.fd_array [0] = LanInfo->hSocket;
	nDes = 0;
#elif LIBIPMI_IS_OS_LINUX()
	FD_ZERO(&wdfs);
	FD_SET(LanInfo->hSocket, &wdfs);
	nDes = LanInfo->hSocket + 1;
#endif

TCPSendDataSelect:
	/* wait for specified amount of time */
	ret = select (nDes, NULL, &wdfs, NULL, pwait_time);

	/* socket error */
	if(ret == SOCKET_ERROR)
	{
#if LIBIPMI_IS_OS_WINDOWS()
		/* Windows */
		if(WSAGetLastError() == WSAEINTR)
		{
			DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
			goto TCPSendDataSelect;
		}
#elif LIBIPMI_IS_OS_LINUX()
		if(errno == EINTR)
		{
			DEBUG_PRINTF("select: %s. Continue...\n",strerror(errno));
			goto TCPSendDataSelect;
		}
#endif
		else
		{
			DEBUG_PRINTF("Select failed while waiting for write pipe \n");		
#if LIBIPMI_IS_OS_WINDOWS()
			closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
			close(LanInfo->hSocket);
#endif
			return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
		}
	}
	/* timed out */
	else if(ret == 0)
	{
		DEBUG_PRINTF("Connection timedout: Write Pipe is full \n");
		//let us clsoe things here
		//saw a strange thing that was causing various goahead crashes
		//if recv indicates end of file and we try to do another send on that
		//we get a crash deep inside the send call somewhere..so we got to be extra careful
		//so we close the socket
#if LIBIPMI_IS_OS_WINDOWS()
			closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
			close(LanInfo->hSocket);
#endif
		return LIBIPMI_MEDIUM_E_TIMED_OUT_ON_SEND;
	}

	if(FD_ISSET(LanInfo->hSocket,&wdfs))
	{

		/* socket is ready to send data */
		nTotalBytesSent = 0;

		/* Till the last byte is sent */
		while( nTotalBytesSent < (int)dwSize )
	
		{
			/* send data */
			nBytesSent = send (LanInfo->hSocket, szBuffer, dwSize, 0);
			
	
			/* send failure */
			if (nBytesSent == SOCKET_ERROR)
			{
				DEBUG_PRINTF("Error while sending data \n");
#if LIBIPMI_IS_OS_WINDOWS()
			closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
			close(LanInfo->hSocket);
#endif
				return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
			}
			else
				nTotalBytesSent += nBytesSent;
	
		}
		
		return LIBIPMI_STATUS_SUCCESS;
		
	}

	DEBUG_PRINTF("select came out while trying to write for no good reason!!\n");
#if LIBIPMI_IS_OS_WINDOWS()
			closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
			close(LanInfo->hSocket);
#endif
	return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
}

/*----------------------------------------------------------
TCP_ReceiveData

Receieves the data to BMC. If read pipe is empty, waits for
specified time and returns
-----------------------------------------------------------*/
#define RETURN_WITH_SIZE(yy) 				\
(*pdwSize) = (yy);					\
return LIBIPMI_STATUS_SUCCESS;				\

uint8 TCP_ReceiveData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32* pdwSize, int timeout )
{
	int						nBytesRecvd;
	struct timeval				wait_time, *pwait_time;
	fd_set					rdfs;
	int						ret;
	int					nDes;
	
	int totalrecd = 0;
	char *pData = szBuffer;
	int              IPMIMsgLen;
    	int              IPMIMsgOffset;

	/* Wait for the socket to get ready */
	if(timeout >= 0)
	{
		wait_time.tv_sec = timeout;
		wait_time.tv_usec = 0;
		pwait_time = &wait_time;
	}
	else
		pwait_time = NULL;

#if LIBIPMI_IS_OS_WINDOWS()
	rdfs.fd_count = 1;
	rdfs.fd_array [0] = LanInfo->hSocket;
	nDes = 0;
#elif LIBIPMI_IS_OS_LINUX()
	FD_ZERO(&rdfs);
	FD_SET(LanInfo->hSocket, &rdfs);
	nDes = LanInfo->hSocket + 1;
#endif
while(1)
{
	/* wait for specified amount of time */
	ret = select (nDes, &rdfs, NULL, NULL, pwait_time);

	/* socket error */
	if(ret == SOCKET_ERROR)
	{
#if LIBIPMI_IS_OS_WINDOWS()
		/* Windows */
		if(WSAGetLastError() == WSAEINTR)
		{
			DEBUG_PRINTF("Select: %s. Continue...\n",strerror(errno));
			continue;
		}
#elif LIBIPMI_IS_OS_LINUX()
		if(errno == EINTR)
		{
			DEBUG_PRINTF("Select: %s. Continue...\n",strerror(errno));
			continue;
		}
#endif
		else
		{
			DEBUG_PRINTF("Select failed while waiting for read pipe \n");
		#if LIBIPMI_IS_OS_WINDOWS()
					closesocket(LanInfo->hSocket);
		#elif LIBIPMI_IS_OS_LINUX()
					close(LanInfo->hSocket);
		#endif
			return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
		}
	}
	/* timed out */
	else if(ret == 0)
	{
		//DEBUG_PRINTF("Connection timedout: no data receivable \n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}
	
	if(FD_ISSET(LanInfo->hSocket,&rdfs))
	{

		/* socket is ready to receive data */
		nBytesRecvd = recv (LanInfo->hSocket, szBuffer+totalrecd, MAX_RESPONSE_SIZE, 0);
		if(nBytesRecvd == 0)
		{
			DEBUG_PRINTF("End of file encountered...maybe socket closure\n");
#if LIBIPMI_IS_OS_WINDOWS()
			closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
			close(LanInfo->hSocket);
#endif
			return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
		}
		/* Error while receiving data */
		if(nBytesRecvd == SOCKET_ERROR)
		{
			DEBUG_PRINTF("Error while receiving data\n");
#if LIBIPMI_IS_OS_WINDOWS()
			closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
			close(LanInfo->hSocket);
#endif
			return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
		} 										\

		
		totalrecd += nBytesRecvd;
				
		//first get RMCP_CLASS_OFFSET 
		if(totalrecd < RMCP_CLASS_MSG_OFFSET)
		{
			continue;
		}
		//we have upto RMCP class Msg offset here
		//if it is a ping
		if (RMCP_CLASS_MSG_ASF == pData[RMCP_CLASS_MSG_OFFSET])
		{
			/* Read remaining RMCP ASF ping message */
			if(totalrecd < RMCP_ASF_PING_MESSAGE_LENGTH)
			{
				continue;
			}
			else if(totalrecd == RMCP_ASF_PING_MESSAGE_LENGTH)
			{
				RETURN_WITH_SIZE(totalrecd);
			}
		}
		/*else if IPMI RMCP request */
		else if (RMCP_CLASS_MSG_IPMI == pData[RMCP_CLASS_MSG_OFFSET])
		{
			if(totalrecd < IPMI_MSG_AUTH_TYPE_OFFSET)
			{
				continue;
			}
			/* Get the IPMI message length offset based on format/authentication type */
			if (pData [IPMI_MSG_AUTH_TYPE_OFFSET] == RMCP_PLUS_FORMAT)
			{
			IPMIMsgOffset = IPMI20_MSG_LEN_OFFSET + 1;
			}
			else if (pData [IPMI_MSG_AUTH_TYPE_OFFSET] == 0)//AUTH_TYPE_NONE
			{
			IPMIMsgOffset = IPMI_MSG_LEN_OFFSET;
			}
			else
			{
			IPMIMsgOffset = IPMI_MSG_LEN_OFFSET + AUTH_CODE_LEN;
			}
			
			if(totalrecd < IPMIMsgOffset)
			{
				continue;
			}
			
			/* Get the IPMI message length based on RMCP format type */
			if (pData [IPMI_MSG_AUTH_TYPE_OFFSET] == RMCP_PLUS_FORMAT)
			{
				IPMIMsgLen = TO_LITTLE_SHORT (*((INT16U*)&pData [IPMI20_MSG_LEN_OFFSET]));
			}
			else
			{
				IPMIMsgLen = pData [IPMIMsgOffset];
			}
			
			 /* Read the remaining IPMI message packets */
        		if (totalrecd < IPMIMsgLen)
			{
				continue;
			}

			//we have the packet here
			RETURN_WITH_SIZE(totalrecd);
			
		}
		
		


		return LIBIPMI_STATUS_SUCCESS;
	}
	
	DEBUG_PRINTF("strange..select came out for no good reason!!\n");
#if LIBIPMI_IS_OS_WINDOWS()
			closesocket(LanInfo->hSocket);
#elif LIBIPMI_IS_OS_LINUX()
			close(LanInfo->hSocket);
#endif
	return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
}
	
}



#if 0
static
int  ReadDataAccumulate (MsgPkt_T *pMsgPkt, int Socket)
{
    int                 SourceLen;
    struct sockaddr_in  Source;
    INT8U*              pData = pMsgPkt->Data;
    INT16S              Len;
    INT16U              RecvdLen = 0;
    INT16U              IPMIMsgLen;
    INT16U              IPMIMsgOffset;

    SourceLen = sizeof (Source);

    /* Read minimum bytes to find class of message */
    while (RecvdLen < RMCP_CLASS_MSG_OFFSET)
    {
        Len = recvfrom (Socket, &pData[RecvdLen], MAX_LAN_BUFFER_SIZE,0,(struct sockaddr *)&Source,&SourceLen);
        if (Len <= 0)
        {
            return -1;
        }
        RecvdLen += Len;
    }

    /*  if RMCP Presence Ping Requested	*/
    if (RMCP_CLASS_MSG_ASF == pData[RMCP_CLASS_MSG_OFFSET])
    {
        /* Read remaining RMCP ASF ping message */
        while (RecvdLen < RMCP_ASF_PING_MESSAGE_LENGTH)
        {
            Len = recvfrom (Socket, &pData[RecvdLen], MAX_LAN_BUFFER_SIZE,0,(struct sockaddr *)&Source,&SourceLen);
            if (Len <= 0)
            {
                return -1;
            }
            RecvdLen += Len;
        }
    }
    /*else if IPMI RMCP request */
    else if (RMCP_CLASS_MSG_IPMI == pData[RMCP_CLASS_MSG_OFFSET])
    {
        /* Read minimum no of bytes for IPMI Auth type offset*/
        while (RecvdLen < IPMI_MSG_AUTH_TYPE_OFFSET)
        {
            Len = recvfrom (Socket, &pData[RecvdLen], MAX_LAN_BUFFER_SIZE,0,(struct sockaddr *)&Source,&SourceLen);
            if (Len <= 0)
            {
                return -1;
            }
            RecvdLen += Len;
        }

        /* Get the IPMI message length offset based on format/authentication type */
        if (pData [IPMI_MSG_AUTH_TYPE_OFFSET] == RMCP_PLUS_FORMAT)
        {
            IPMIMsgOffset = IPMI20_MSG_LEN_OFFSET + 1;
        }
        else if (pData [IPMI_MSG_AUTH_TYPE_OFFSET] == AUTH_TYPE_NONE)
        {
            IPMIMsgOffset = IPMI_MSG_LEN_OFFSET;
        }
        else
        {
            IPMIMsgOffset = IPMI_MSG_LEN_OFFSET + AUTH_CODE_LEN;
        }

        /* Read minimum no of bytes for IPMI message length offset*/
        while (RecvdLen < IPMIMsgOffset)
        {
            Len = recvfrom (Socket, &pData[RecvdLen], MAX_LAN_BUFFER_SIZE,0,(struct sockaddr *)&Source,&SourceLen);
            if (Len <= 0)
            {
                return -1;
            }
            RecvdLen += Len;
        }

        /* Get the IPMI message length based on RMCP format type */
        if (pData [IPMI_MSG_AUTH_TYPE_OFFSET] == RMCP_PLUS_FORMAT)
        {
            IPMIMsgLen = ipmitoh_u16 (*((INT16U*)&pData [IPMI20_MSG_LEN_OFFSET]));
        }
        else
        {
            IPMIMsgLen = pData [IPMIMsgOffset];
        }

        /* Read the remaining IPMI message packets */
        while (RecvdLen < IPMIMsgLen)
        {
            Len = recvfrom (Socket, &pData[RecvdLen], MAX_LAN_BUFFER_SIZE,0,(struct sockaddr *)&Source,&SourceLen);
            if (Len <= 0)
            {
                return -1;
            }
            RecvdLen += Len;
        }

    }/* else other RMCP class are not supported. */
    else
    {
        IPMI_WARNING ("Unknown RMCP class\n");
    }

    IPMI_DBG_PRINT ("LANIfc.c : Received LAN request packet\n");
    IPMI_DBG_PRINT_BUF (pData, RecvdLen);

    pMsgPkt->Param    = LAN_SMB_REQUEST;
    pMsgPkt->Size     = RecvdLen;
    pMsgPkt->UDPPort  = Source.sin_port;
    pMsgPkt->Socket   = Socket;
    memcpy (pMsgPkt->IPAddr, &Source.sin_addr.s_addr, sizeof (struct in_addr));

    return 0;
}
#endif
