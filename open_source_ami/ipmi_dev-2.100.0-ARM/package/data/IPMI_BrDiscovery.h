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
 ****************************************************************
 ****************************************************************
 *
 * IPMI_BrDiscovery.h
 * IPMI Bridge Discovery structures
 *
 * Author: Balasaravanan.P <balasaravananp@amiindia.co.in>
 *
 *****************************************************************/
#ifndef IPMI_BRIDGE_DISCOVERY_H
#define IPMI_BRIDGE_DISCOVERY_H

#include "Types.h"

#pragma pack( 1 )

/* GetAddressesRes_T */
typedef struct 
{
    INT8U   CompletionCode;     
    INT16U  ICMBAddresses [8];

} PACKED  GetAddressesRes_T;


/* GetChassisDevIDRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   ChassisDevID;

} PACKED  GetChassisDevIDRes_T;

#pragma pack( )

#endif  /* IPMI_BRIDGE_DISCOVERY_H */

