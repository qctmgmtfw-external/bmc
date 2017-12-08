/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
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
 * IPMBIFc.H
 * 
 *
 * Author: Rama Bisa <ramab@ami.com> 
 *
 * 
 *****************************************************************/
#ifndef IPMBIFC_H
#define IPMBIFC_H

#include "Types.h"

#define IPMB_SECONDARY_IFC_Q          MSG_PIPES_PATH "IPMBSecondaryIfcQ" 
#define IPMB_SECONDARY_RES_Q          MSG_PIPES_PATH "IPMBSecondaryResQ" 

/*** External Definitions ***/
#define BMC_SLAVE_ADDRESS   0x20
#define BMC_LUN             0x00
#define MAX_IPMB_PKT_SIZE   128

#define IPMB_REQUEST			1
#define IPMB_EVT_MSG_REQUEST	2
#define MIN_IPMB_MSG_LENGTH		7
#define PLATFORM_EVENT_MSG_CMD	0x02
#define MAX_BUS_NAME_LEN 64

typedef struct
{
    INT8U  IPMBBusNum;
    INT8S  IPMBBusName[MAX_BUS_NAME_LEN];

}RecvPktArgs_T;

#endif /* IPMBIFC_H	*/
