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
 * AMI.h
 * AMI Command Handler
 *
 * Author: Basavaraj Astekar <basavaraja@ami.com>
 *
 *****************************************************************/
#ifndef AMI_H
#define AMI_H

#include "MsgHndlr.h"


/*** Extern Definitions ***/
extern const ExCmdHndlrMap_T  g_AMI_CmdHndlr []; /**< AMI specific command handler table. */

/**
 * @defgroup acf AMI Specific Module
  * IPMI AMI Specific Command Handlers. Invoked by the message handler
 * @{
 **/
/** @} */

#endif  /* APP_H */
