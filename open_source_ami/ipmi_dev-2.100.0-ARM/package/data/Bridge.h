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
 *
 * Bridge.h
 * Bridge Command Handler
 *
 * Author: Gowtham.M.S <gowthamms@amiindia.co.in>
 *
 *****************************************************************/
#ifndef BRIDGE_H
#define BRIDGE_H

#include "Types.h"
#include "MsgHndlr.h"
#include "IPMI_Bridge.h"

/**
 * @var g_Bridge_CmdHndlr
 * @brief Bridge Command Hanlder Map.
**/
extern const CmdHndlrMap_T  g_Bridge_CmdHndlr [];


/**
 * @defgroup bridge Bridge Module
 * IPMI Intelligent Chassis Management Bus interface commands.
 * This module implements chassis discovery, bridge management
 * and event generation commands.
 * @{
**/
/** @} */

#endif  /* BRIDGE_H*/
