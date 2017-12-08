/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#ifndef __BT_IOCTL_H__
#define __BT_IOCTL_H__

/* IOCTL Commands */
typedef enum
{
        SET_SMS_BIT,
        CLEAR_SMS_BIT,
        READ_BT_DATA,
	    WRITE_BT_DATA,
        BT_ENABLE,
        BT_DISABLE,
        END_OF_FUNCLIST,
} eBTFunc;


#endif /* __BT_IOCTL_H__ */

