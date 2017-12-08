/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
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
	void (*enable_snoop_interrupt) (void);
	void (*disable_snoop_interrupt) (void);
	void (*reset_snoop) (void);
} snoop_hal_operations_t;


typedef struct
{
	void (*get_snoop_core_data) ( struct kfifo ** pcurrent_codes, struct kfifo ** pprevious_codes, int dev_id );
} snoop_core_funcs_t;


#endif

