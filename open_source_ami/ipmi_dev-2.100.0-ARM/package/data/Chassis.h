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
 * chassis.h
 * Chassis.c extern declarations
 *
 *  Author: Rama Bisa <ramab@ami.com>
 *
 ******************************************************************/

#ifndef CHASSIS_H
#define CHASSIS_H

#include "Types.h"
#include "MsgHndlr.h"
#include "IPMI_Chassis.h"


/*** Extern Definitions ***/
extern const CmdHndlrMap_T g_Chassis_CmdHndlr []; /**< Chassis command handler table. */

/**
 * @defgroup ccf Chassis Device Module
 * IPMI Chassis Command Handlers. Invoked by the message handler
 * @{
 **/
/** @} */

#endif /* CHASSIS_H	*/
