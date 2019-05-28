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

#ifndef __SNOOP_H__
#define __SNOOP_H__

typedef struct 
{
	void (*enable_snoop_interrupt) (int ch_num);
	void (*disable_snoop_interrupt) (int ch_num);
	void (*reset_snoop) (int ch_num);
} snoop_hal_operations_t;


typedef struct
{
	void (*get_snoop_core_data) (struct kfifo ** pcurrent_codes, struct kfifo ** pprevious_codes, int dev_id, int ch_num);
} snoop_core_funcs_t;


#endif

