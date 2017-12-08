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
 * Errno.h
 * Error numbers
**/

#ifndef ERRNO_H
#define ERRNO_H

#define ERROR					-1
#define SUCCESS					0
#define CM_SUCCESS				0
#define CM_ERROR				-1

#define ERR_TIMEOUT				1
#define ERR_CRITICAL			2
#define ERR_CONNECTION_LOST		3
#define ERR_ABORTED				4
#define ERR_AUTH				5
#define ERR_NOSESSION			6
#define ERR_INVALID_SESSION		7
#define ERR_MAX_SESSION			8
#define ERR_PERM				9
#define ERR_INVALID_CMD			10
#define ERR_SEND				11
#define ERR_NULL_PKT			12
#define ERR_INVALID_BUFFER		13
#define ERR_RECV				14
#define ERR_INVALID_SOCKET		15

#endif /* ERRNO_H */
