/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * Filename : WS_EventingManager.h
 * 
 * File description : The eventing manager for WSMAN server
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_EVENTING_MANAGER_H_
#define _WS_EVENTING_MANAGER_H_
#include "WS_Structures.h"
#include "WS_EventingDaemon.h"

WS_INT16 EventingManager(struct SOAPStructure *soapStruct, Eventing_Resp *pEventingResp);
extern int g_wsFifoDesc;
#endif //_WS_EVENTING_MANAGER_H_
