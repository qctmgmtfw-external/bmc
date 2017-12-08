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
 ****************************************************************/
/*****************************************************************
 *
 * Sensor.h
 * Sensor Command Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef SENSOREVENT_H
#define SENSOREVENT_H

#include "Types.h"
#include "MsgHndlr.h"
#include "IPMI_SensorEvent.h"


/**
 * @var g_SensorEvent_CmdHndlr
 * @brief Sensor event command handler map.
**/
extern const CmdHndlrMap_T  g_SensorEvent_CmdHndlr [];

/**
 * @defgroup senevt Sensor Event Module
 * IPMI Sensor Event interface commands. These module handles sensor
 * configuration and monitoring and event configuration commands.
 * @{
**/
/** @} */

#endif  /* SENSOREVENT_H */
