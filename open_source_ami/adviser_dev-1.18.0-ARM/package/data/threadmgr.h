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
 * threadmgr.h
 * Thread Manager
**/
#ifndef THREAD_MGR_H
#define THREAD_MGR_H
		
/**
 * @brief Waits for all the threads to exit
**/
int wait_for_threads_to_exit (void);

/**
 * @brief Creates all the threads used in this program
**/
int create_threads (void);


#endif	/* THREAD_MGR_H */

