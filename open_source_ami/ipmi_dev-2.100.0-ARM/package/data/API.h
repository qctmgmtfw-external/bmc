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
 * PMApi.h
 * Platform manager APIs.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef PMAPI_H
#define PMAPI_H
#include "Types.h"
#include "IPMIDefs.h"
#include "Message.h"
#include "MsgAPI.h"
#include "ChassisAPI.h"
#include "EventAPI.h"
#include "SELAPI.h"
#include "SDRAPI.h"
#include "FRUAPI.h"
#include "AlertAPI.h"
#include "MiscAPI.h"

#include "IPMI_App.h"
#include "IPMI_AppDevice.h"
#include "IPMI_Sensor.h"

/*------------------------------------------------------------------
 * @fn API_ExecuteCmd
 * @brief Execute a IPMI command by posting the request to
 * Message handler and receives the response.
 *------------------------------------------------------------------*/
extern int    API_ExecuteCmd (MsgPkt_T *pMsgPkt, int BMCInst);


#endif	/* PMAPI_H */

