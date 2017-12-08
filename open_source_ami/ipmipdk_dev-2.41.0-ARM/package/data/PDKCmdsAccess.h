/******************************************************************
 ******************************************************************
 ***                                                                                                           **
 ***    (C)Copyright 2011, American Megatrends Inc.                            **
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
 * PDKCmdsAccess.h
 * PDK Cmd Hooks Access methods
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 
 ******************************************************************/
#ifndef _PDK_CMDS_ACCESS_
#define _PDK_CMDS_ACCESS_

#include "PDKAccess.h"

#ifdef CONFIG_SPX_FEATURE_GLOBAL_PDKCMDS_LIB
#define PDKCMDS_LIB CONFIG_SPX_FEATURE_GLOBAL_PDKCMDS_LIB
#else
#define PDKCMDS_LIB "/usr/local/lib/libipmipdkcmds.so"
#endif



typedef enum
{
    PDKCMDS_GETOEMMSGHNDLRMAP =0,
    PDKCMDS_PDKPREPROCESSCMD,
    PDKCMDS_PDKPOSTPROCESSCMD,
    PDKCMDS_GETCMDSUPCFGMASK,
    PDKCMDS_PDKAMIGETCHNUM,
    PDKCMDS_PDKGETOEMSMMCMDHNDLR,
    PDKCMDS_PDKISCOMMANDENABLED,
    MAX_PDKCMDS_HANDLE
}PDKCMDS_HOOK;

extern pfunc g_PDKCmdsHandle[MAX_PDKCMDS_HANDLE];
extern int InitPDKCmdHooks();

#endif /* _PDK_CMDS_ACCESS_ */
