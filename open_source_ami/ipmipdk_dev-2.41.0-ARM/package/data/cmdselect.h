/******************************************************************
 ******************************************************************
 ***                                                                                                           **
 ***    (C)Copyright 2011, American Megatrends Inc.                                     **
 ***                                                                                                           **
 ***    All Rights Reserved.                                                                          **
 ***                                                                                                           **
 ***    5555 , Oakbrook Pkwy, Norcross,                                                       **
 ***                                                                                                           **
 ***    Georgia - 30093, USA. Phone-(770)-246-8600.                                  **
 ***                                                                                                           **
 ******************************************************************
 ******************************************************************
 ******************************************************************
 *
 * cmdselect.h
 * Command selection routines
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 ******************************************************************/
#ifndef _CMDSELECT_
#define _CMDSELECT_

#define ENABLED         0x01
#define DISABLED        0xFF

#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#define NETFN_SSI       0x2c
#endif
#define NETFN_DCMI      0x2c
#define NETFN_PNM       0x30
#define NETFN_APML      0x36

#define GRPEXT_NA       0x00
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#define GRPEXT_SSI      0x02
#endif
#define GRPEXT_DCMI     0xDC

typedef struct
{
    INT8U CmdNum;
    INT8U Status;
}NetFnCmds_T;

typedef struct
{
    INT8U NetFn;
    INT8U GroupExtCode;
    const NetFnCmds_T *NetFunction;
}NETFNTable_T;

#endif //_CMDSELECT_

