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


/***************************************************************
* @file  	Synchronise.c
* @author 	govind kothandapani		<govindk@ami.com>
*		Baskar Parthiban		<bparthiban@ami.com>
*		Varadachari Sudan Ayanam 	<varadacharia@ami.com>
* @brief  	Mutex and Semaphore related functions are defined
****************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "adviserdbg.h"
#include "cmworker.h"
#include "sync.h"

/* Mutex Variables */
pthread_mutex_t		pending_clients_guard;
pthread_mutex_t		m_cm_guard;
pthread_mutex_t		abort_clients_guard;
pthread_mutex_t		*session_guard = NULL;
sem_t			*worker_wait_event = NULL;
sem_t			m_ev_transfer_complete;

/**
 * @brief acquire_mutex - Locks the mutex variable
* @return 0 on success, -1 or error
**/

int
acquire_mutex(pthread_mutex_t *mutex_var)
{
	if(pthread_mutex_lock (mutex_var) != 0)
    {
        TCRIT("Acquire pthread_mutex_lock () failed.\n");
        return -1; // failure
    }
    
	return 0; //Success
}
/**
 * Releases the mutex variable
**/
int
release_mutex (pthread_mutex_t *mutex_var)
{
    if(pthread_mutex_unlock (mutex_var) != 0 )
    {
        TCRIT("pthread_mutex_unlock ()  failed.\n");
        return -1; // failure
    }
    return 0; // success
}
