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
 * App.h
 * Application Command Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef APP_H
#define APP_H

#include "Types.h"
#include "MsgHndlr.h"
#include "IPMI_App.h"

/*** Extern Definitions ***/
extern const CmdHndlrMap_T  g_App_CmdHndlr []; /**< IPM Device and
                        Application Device command handler table. */

/**
 * @defgroup apcf Application Device Module
 * IPMI Application device Command Handlers. Invoked by the message handler
 * @{
 **/
/** @} */

#endif  /* APP_H */

