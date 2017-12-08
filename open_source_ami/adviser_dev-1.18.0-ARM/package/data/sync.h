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

/**
 * sync.h
 * Synchronisation mechanism related definitions
**/

#ifndef SYNCHRONISE_H
#define SYNCHRONISE_H

#include <pthread.h>

#define SEM_INIT_VAL				0

int acquire_mutex(pthread_mutex_t *mutex_var);
int release_mutex (pthread_mutex_t *mutex_var);
int init_mutexes ();
int init_semaphores ();

#endif /* SYNCHRONISE_H */

