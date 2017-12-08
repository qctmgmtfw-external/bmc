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

#ifndef __RESET_IOCTL_H__
#define __RESET_IOCTL_H__

typedef enum
{
        GET_LPCRST_COUNT,
        SET_LPCRST_COUNT,
	WAIT_ON_RESET,
} eLPCRSTFunc;

#endif
