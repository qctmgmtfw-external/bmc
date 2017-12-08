/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2007, American Megatrends Inc.        **
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
 * SOLVars.h
 * SOL Macros
 *
 *
 *****************************************************************/
#ifndef _SOL_VARS_H_
#define _SOL_VARS_H_

#define SOL_SEND_IMMEDIATE_WATERMARK(BMCInst)           g_BMCInfo[BMCInst].SOLConfig.send_char_threshold
#define SOL_SEND_TIMEOUT(BMCInst)                       g_BMCInfo[BMCInst].SOLConfig.send_char_timeout
#define SOL_ACK_TIMEOUT(BMCInst)                                g_BMCInfo[BMCInst].SOLConfig.sol_ack_to_ticks

#endif /* _SOL_VARS_H_ */
