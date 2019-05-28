/*****************************************************************
 **                                                             **
 **     (C) Copyright 2009-2015, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

#ifndef __RESET_H__
#define __RESET_H__

typedef struct
{
} reset_hal_operations_t;

typedef struct
{
        int (*process_reset_intr) (int dev_id);
} reset_core_funcs_t;

int install_reset_handler(int (*pt2Func)(void));
int uninstall_reset_handler(int (*pt2Func)(void));

#endif


