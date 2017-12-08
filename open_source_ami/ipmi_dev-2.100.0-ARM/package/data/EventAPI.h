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
 * EventAPI.h
 * Event Access API.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef EVENTAPI_H
#define EVENTAPI_H
#include "Types.h"
#include "IPMI_Events.h"

/*-----------------------------------------------------------------
 * @fn API_PlatformEvtMsg
 * @brief Adds a Platform event message to BMC.
 * @param pEventMsg	- Pointer to the buffer holding the event message.
 * @return 	-0  if Success
 *			-1	if failed.
 *-----------------------------------------------------------------*/
extern int API_PlatformEvtMsg (_FAR_ INT8U* pEventMsg, int BMCInst);



#endif	/* SELAPI_H */

