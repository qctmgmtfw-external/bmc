/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef ARCH_HEADER_H
#define ARCH_HEADER_H


#include "linux.h"


/* 
   Architecture/OS dependent data and functions .These data structures 
   and functions should be defined in the Arch/OS specific code 
*/

/*
   Usb_OsSleepStruct and Usb_OsWaitStruct are defined in Arch/OS 
   specific header files 
*/

typedef int (*DPC)(void *arg);	

int  Usb_OsRegisterIsr(char *Name, int Vector,uint8 DevNo,uint8 epnum, uint32 Flags);
void Usb_OsUnregisterIsr(char *Name, int Vector,uint8 DevNo,uint8 epnum, uint32 Flags);

void Usb_OsInitSleepStruct(Usb_OsSleepStruct *sleep);
long Usb_OsSleepOnTimeout (Usb_OsSleepStruct *sleep, uint8* Var, long timeout); 
void Usb_OsWakeupOnTimeout(Usb_OsSleepStruct *sleep);

void Usb_OsInitDPC  (DPC UsbDeferProcess);
void Usb_OsWakeupDPC(void);
void Usb_OsKillDPC(void);

void Usb_OsDelay(long msecs);

long Usb_OsSleepOnTimeout_Interruptible (Usb_OsSleepStruct *sleep, uint8* Var, long timeout); 

/* Networking related functions */
int Net_OSRegisterDriver(NET_DEVICE *Dev);
void Net_OSUnregisterDriver(NET_DEVICE *Dev);
int Net_OSPushPacket(NET_DEVICE *Dev, uint8 *Data, int Len);
void Net_TxComplete(NET_PACKET *Pkt);


#endif /* ARCH_HEADER_H */




