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
 ******************************************************************
 * 
 * AlertAPI.h
 * Alerting API.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *  		
 ******************************************************************/
#ifndef ALERTAPI_H
#define ALERTAPI_H
#include "Types.h"
#include "SELRecord.h"

/*--------------------------------------------------------------------------
 * @fn API_SendLANAlert
 * @brief This function sends PET Alerts through the LAN channel.
 * @param EvtRecord		- Received Event record.
 * @param DestSel		- Destination Selector
 * @param AlertImmFlag	- 1 if alert immediate, 0 otherwise.
 *--------------------------------------------------------------------------*/
extern int API_SendLANAlert (SELEventRecord_T* EvtRecord,  INT8U DestSel,
                                                            INT8U   AlertImmFlag,
                                                            INT8U             EventSeverity, int BMCInst);

#endif	/* ALERTAPI_H */
