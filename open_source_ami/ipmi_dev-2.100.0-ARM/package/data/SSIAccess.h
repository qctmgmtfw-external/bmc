/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2011, American Megatrends Inc.             **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * SSIAccess.h
 * SSI Handle Access methods
 *
 * Author: Kevin Cheng <kevincheng@ami.com.tw>
 *
 ****************************************************************/

#ifndef _H_SSIACCESS_
#define _H_SSIACCESS_

#include "PDKAccess.h"

#define SSICB_LIB "/usr/local/lib/libssicb.so"

typedef enum
{
    SSICB_EVENTFWD = 0,
    SSICB_OPSTATEMACH,
    SSICB_QUEUECOND,
    SSICB_GETFRUOBJ,
    SSICB_GETCURRSTAT,
    MAX_SSI_HANDLE
} SSI_Handles;

typedef struct
{
    INT16U SSIHandleNum;
    INT8U  SSIHandleName[255];
} SSI_Init;

typedef struct
{
    void *func;
} SSIHandle_func;

extern pfunc g_SSIHandle[MAX_SSI_HANDLE];


/*---------------------------------------------------------------------------
 * @fn InitSSIHandles
 *
 * @brief Initializes handles for SSI specified functions.
 *
 * @return  0	- if success.
 *          -1  - if error.
 *---------------------------------------------------------------------------*/
extern int InitSSIHandles();

#endif //_H_SSIACCESS_

