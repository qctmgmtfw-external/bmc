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

#ifndef __KCS_IOCTL_H__
#define __KCS_IOCTL_H__

/* IOCTL Commands and structure */
typedef enum
{
        SET_SMS_BIT,
        CLEAR_SMS_BIT,
        ENABLE_KCS_INTERRUPT,
        DISABLE_KCS_INTERRUPT,
        START_HW_UNIT_TEST,
        READ_KCS_DATA,
        WRITE_KCS_DATA,
        SET_OBF_BIT,
        KCS_ENABLE,
        KCS_DISABLE,
        SET_STATUS_DATA,
        END_OF_FUNCLIST,
} eKCSFunc;


struct kcs_data_t {
	unsigned char channel_num;
	unsigned int num_bytes;
	unsigned char * buffer;
};

struct kcs_status_data_t {
	unsigned char channel_num;
	unsigned char status;
};

#endif
