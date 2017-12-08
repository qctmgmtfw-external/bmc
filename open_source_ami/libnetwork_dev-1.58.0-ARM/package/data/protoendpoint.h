
/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2007, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * Filename : protoendpoint.h
 * Author   : Aruna. V (arunav@amiindia.co.in)
 *
 ****************************************************************/
 
#ifndef _PROTOENDPOINT_H
#define _PROTOENDPOINT_H

#include "netports.h"

/*
 * @define      NETSTAT_OPTIONS
 * @brief       options to be used for netstat established command
 */
#define NETSTAT_OPTIONS "-tn"

/*
 * Structure Name : netstat_data
 * Description    : This structure has members for holding the output
                    values of Netstat Established command.
*/

typedef struct netstat_data {
        char localAddress[16];
	char remoteAddress[16];
	unsigned long ulLocalPort;
	unsigned long ulRemotePort;
	struct netstat_data * next;
} Netstat_data;

/** Forms the LocalIP:port and RemoteIP:Port list
    using netstat command **/
int PEP_GetPortFromNetstat(Netstat_data ** ptrNetData, 
		       unsigned long * pulPort);

/** Gets a single particular instance with the given 
    Name property value for all the ProtocolEndpoints**/
int PEP_IsConnectionEstablished(char * pstrName);

/** Frees the Netstat list structure **/
void free_Netdata_list(Netstat_data * ptrNetData);

#endif /* _PROTOENDPOINT_H */
