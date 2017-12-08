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
 * Cmworker.h
 * Client worker threads and associated data structures
**/

#ifndef CMWORKER_H
#define CMWORKER_H

#include "session_data.h"

#define WAIT_INFINITE		5000
#define TS_SENDING		10

/**
 * Worker thread main entry point function prototype.
**/
typedef int (*p_cmworker_t) (session_info_t* p_info);

// prototype
int cm_worker_thread (session_info_t* p_session_info);

#endif	/* CMWORKER_H */

