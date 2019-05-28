/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
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

#define SET_SMS_BIT                     _IOC(_IOC_WRITE,'K',0x00,0x3FFF)
#define CLEAR_SMS_BIT                 _IOC(_IOC_WRITE,'K',0x01,0x3FFF)
#define ENABLE_KCS_INTERRUPT   _IOC(_IOC_WRITE,'K',0x03,0x3FFF)
#define DISABLE_KCS_INTERRUPT  _IOC(_IOC_WRITE,'K',0x04,0x3FFF)
#define START_HW_UNIT_TEST      _IOC(_IOC_WRITE,'K',0x05,0x3FFF)
#define READ_KCS_DATA                _IOC(_IOC_READ,'K',0x06,0x3FFF)
#define WRITE_KCS_DATA              _IOC(_IOC_WRITE,'K',0x07,0x3FFF)
#define SET_OBF_BIT                     _IOC(_IOC_WRITE,'K',0x08,0x3FFF)
#define KCS_ENABLE                       _IOC(_IOC_WRITE,'K',0x09,0x3FFF)
#define KCS_DISABLE                      _IOC(_IOC_WRITE,'K',0x0a,0x3FFF)
#define SET_STATUS_DATA            _IOC(_IOC_WRITE,'K',0x0b,0x3FFF)

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
