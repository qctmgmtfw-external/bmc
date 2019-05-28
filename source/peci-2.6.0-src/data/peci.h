/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
 
#ifndef __PECI_H__
#define __PECI_H__

#include "peciifc.h"

typedef struct 
{
	int (*send_peci_cmd)(peci_cmd_t *cmd_pipe);
} peci_hal_operations_t;

typedef struct
{
	int (*process_peci_intr) (int dev_id, unsigned char ch_num);
} peci_core_funcs_t;

#define FEEDBACK_LENGTH		(3)
#define MAX_WRITE_LENGTH	(20 - FEEDBACK_LENGTH)
#define MAX_READ_LENGTH 	(20)

#endif
