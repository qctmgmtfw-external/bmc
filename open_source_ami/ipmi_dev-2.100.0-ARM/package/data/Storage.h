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
 * Storage.h
 * Storage Command Handler
 *
 * Author: Rama Bisa <ramab@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef STORAGE_H
#define STORAGE_H

#include "Types.h"
#include "MsgHndlr.h"
#include "IPMI_Storage.h"

/**
 * @var g_Storage_CmdHndlr
 * @brief Storage Command Handler Table
**/
extern const CmdHndlrMap_T  g_Storage_CmdHndlr [];


/**
 * @defgroup storage Storage Module
 * IPMI storage command interface. This module implements 
 * SDR, FRU and SEL storage and access/configuration commands.
 * @{
**/
/** @} */

#endif  /* STORAGE_H */
