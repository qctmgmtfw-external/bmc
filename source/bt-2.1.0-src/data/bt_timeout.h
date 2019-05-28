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

#ifndef BT_TIMEOUT_H
#define BT_TIMEOUT_H

typedef struct 
{
	wait_queue_head_t queue;

} __attribute__((packed)) Bt_OsSleepStruct;

void Bt_OsInitSleepStruct(Bt_OsSleepStruct *Sleep);
void Bt_OsWakeupOnTimeout(Bt_OsSleepStruct *Sleep);
long Bt_OsSleepOnTimeout(Bt_OsSleepStruct *Sleep,u8 *Var,long msecs);


#endif /* BT_TIMEOUT_H */
