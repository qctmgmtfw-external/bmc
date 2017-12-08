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
 * DeviceConfig.h
 * Device Configuration Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include "Types.h"
#include "MsgHndlr.h"
#include "IPMI_DeviceConfig.h"

/**
 * @var g_Config_CmdHndlr
 * @brief Transport Configuration Commands Map.
**/
extern const CmdHndlrMap_T  g_Config_CmdHndlr [];


/**
 * @defgroup devcfg Device Configuration Module
 * IPMI Device Configuration command interface. This module
 * implements configuration and access commands for transport modules
 * such as LAN, Serial/Modem, Serial Over LAN.
 * @{
**/
/** @} */

#endif  /* DEVICE_CONFIG_H */
