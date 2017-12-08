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

/******************************************************************
* 
* Filename: semaph.h
*
******************************************************************/
#ifndef __SEMAPH_H__
#define __SEMAPH_H__

#include <semaphore.h>

int sem_create_sysv(key_t key, int initval);
int sem_open_sysv(key_t key);
void sem_rm_sysv(int id);
void sem_close_sysv(int id);
int sem_wait_sysv(int id,long timeoutval);
int sem_signal_sysv(int id);
int sem_op_sysv(int id, int value,long timeoutval);

int LINUX_GetFileLock(char * file, char nLockType);
int LINUX_ReleaseFileLock(char * file, char nLockType);

/* Binary Semaphores Declaration*/
sem_t* CreateSemaphore(char *semaphoreKey);
void WaitSemaphore(sem_t* semaphore);
void ReleaseSemaphore(sem_t* semaphore,char *semaphoreKey);

#endif	
