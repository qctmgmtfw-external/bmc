/****************************************************************
****************************************************************
**                                                            **
**    (C)Copyright 2006-2007, American Megatrends Inc.        **
**                                                            **
**            All Rights Reserved.                            **
**                                                            **
**        6145-F, Northbelt Parkway, Norcross,                **
**                                                            **
**        Georgia - 30071, USA. Phone-(770)-246-8600.         **
**                                                            **
****************************************************************
*****************************************************************
*
* PendTask.h
*      Any IPMI command operation which requires more
*      response time can be posted to this task.
*
* Author: Vinothkumar S <vinothkumars@ami.com>
*
*****************************************************************/
#ifndef PEND_TASK_H
#define PEND_TASK_H

#include "Types.h"
#include "OSPort.h"
#include "Message.h"
#include "IPMIDefs.h"

#define PEND_STATUS_COMPLETED 0x00
#define PEND_STATUS_PENDING 0x0F
#define IPMI				0x01
#define WEBGO				0x02
#define REBOOT				0x03

/*----------------------------------------------------
 * Pending Task Operation
 *----------------------------------------------------*/
typedef enum
{
    PEND_OP_SET_IP = 0,             /* Set IP Address */
    PEND_OP_SET_SUBNET,             /* Set SubNet Mask */ 
    PEND_OP_SET_GATEWAY,         /* Set Gateway */ 		
    PEND_OP_SET_SOURCE,             /*SET Address source*/
    PEND_OP_SET_DNS,                    /*SET DNS Information */

    PEND_OP_DELAYED_COLD_RESET,  /* Delayed Cold Reset */
    PEND_OP_DELAYED_CR_CONFIG,
    PEND_OP_SEND_EMAIL_ALERT,
    PEND_OP_PEND_VM_CONFIG,
    PEND_OP_PEND_OOB_CONFIG,
    PEND_OP_IFUPDOWN,
    PEND_OP_NIC_ENABLE_DISABLE,
    PEND_OP_SET_VLAN_ID,
    PEND_OP_DECONFIG_VLAN_IFC,
    PEND_OP_SET_IPV4_HEADERS,
    PEND_RMCP_PORT_CHANGE,
    PEND_OP_SET_ALL_DNS_CFG,
    PEND_OP_SET_IPV6_CFG,
    PEND_OP_SET_DNSV6_CFG,
    PEND_OP_SET_ETH_IFACE_STATE,
    PEND_OP_SET_MAC_ADDRESS,
    PEND_OP_SET_IPV6_ENABLE=102,
    PEND_OP_SET_IPV6_IP_ADDR_SOURCE,
    PEND_OP_SET_IPV6_IP_ADDR,
    PEND_OP_SET_IPV6_PREFIX_LENGTH,
    PEND_OP_SET_IPV6_GATEWAY,
    PEND_OP_SET_IPV6_DNS_CFG,
    PEND_OP_SET_BOND_IFACE_STATE,
    PEND_OP_SET_ACTIVE_SLAVE,
    PEND_OP_RESTART_SERVICES
    
 } PendTaskOperation_E;


/* Function pointer for Pending command handler */
typedef int (*pPendCmdHndlr_T) (INT8U* pData, INT32U ReqLen,INT8U Channel,int BMCInst);

/* Pending command handler type definition */
typedef struct
{
    PendTaskOperation_E				Operation;	/* Pending Operation */
    pPendCmdHndlr_T					PendHndlr;	/* Pending handler */

} PendCmdHndlrTbl_T;

/* Pending command status type definition */
typedef struct
{
    PendTaskOperation_E    Action;  /* Pending Operation */
    int PendStatus; /* Pending Status */
} PendCmdStatus_T;

/*----------------------------------------------------
 * Pend Task Handler Queue
 *----------------------------------------------------*/
extern void* PendCmdTask (void *pArg);
extern int PostPendTask (PendTaskOperation_E Operation, INT8U *pData, INT32U DataLen,INT8U Channel,int BMCInst);
extern int SetPendStatus (PendTaskOperation_E Action, int Status);
extern int GetPendStatus (PendTaskOperation_E Action);

#endif	/* PEND_TASK_H */
