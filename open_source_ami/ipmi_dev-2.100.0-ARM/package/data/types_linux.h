/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * types_win32.h
 * Windows specific types.
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 * 
 *****************************************************************/
#ifndef TYPES_LINUX_H
#define TYPES_LINUX_H


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1

#define GetLastError()	1


#endif  // TYPES_LINUX_H
