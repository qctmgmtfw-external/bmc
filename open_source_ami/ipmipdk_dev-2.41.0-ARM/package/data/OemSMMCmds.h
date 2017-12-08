/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2008, American Megatrends Inc.             **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *
 * OemSMMCmds.h
 * OemSMMCmds.h used by SMM  channel
 *
 * Author: Anurag Bhatia <anuragb@ami.com>
 *         Vinesh Chirstopher <vineshc@ami.com>
 *
 *****************************************************************/

#include "MsgHndlr.h"

#define CMD_EXAMPLE_1   0x1
#define CMD_EXAMPLE_2   0x2
#define CMD_EXAMPLE_3   0x3


 typedef struct
{
    INT8U			NetFnLUN;
    INT8U			Cmd;
    pCmdHndlr_T		pHandler;
    INT8U			Len;

} OemSMMCmdHndlr_T;



extern int  SMMExample_1(_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

extern int  SMMExample_2(_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

extern int SMMExample_3(_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

