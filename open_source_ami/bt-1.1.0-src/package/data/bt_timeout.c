/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#include <linux/module.h>
#include <linux/kernel.h>
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/kernel.h>
#include <linux/ide.h>
#include "bt_timeout.h"	

void 
Bt_OsInitSleepStruct(Bt_OsSleepStruct *Sleep)
{
	init_waitqueue_head(&(Sleep->queue));
	return;
}

void 
Bt_OsWakeupOnTimeout(Bt_OsSleepStruct *Sleep)
{
	/* Wakeup Process */
    wake_up(&(Sleep->queue));
}

long 
Bt_OsSleepOnTimeout(Bt_OsSleepStruct *Sleep,u8 *Var,long msecs)
{
	long timeout;	/* In jiffies */
	u8 *volatile Condition = Var;

	/* Convert milliseconds into jiffies*/
	timeout = (HZ*msecs)/1000;

	/* Sleep on the Condition for a wakeup */
	if (msecs)
	{
		return wait_event_interruptible_timeout(Sleep->queue,(*Condition), timeout);
	}
	else
	{
		wait_event_interruptible(Sleep->queue, (*Condition));
		return 1;
	
	}
}


