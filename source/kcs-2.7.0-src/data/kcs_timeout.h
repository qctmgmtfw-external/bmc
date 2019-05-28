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

#ifndef KCS_TIMEOUT_H
#define KCS_TIMEOUT_H

typedef struct 
{
	wait_queue_head_t queue;
	struct timer_list tim;
	volatile u8  Timeout;
} __attribute__((packed)) Kcs_OsSleepStruct;

#endif
