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

#ifndef __BT_IOCTL_H__
#define __BT_IOCTL_H__

#define SET_SMS_BIT      _IOC(_IOC_WRITE,'B',0x00,0x3FFF)
#define CLEAR_SMS_BIT  _IOC(_IOC_WRITE,'B',0x01,0x3FFF)
#define READ_BT_DATA   _IOC(_IOC_READ,'B',0x03,0x3FFF)
#define WRITE_BT_DATA _IOC(_IOC_WRITE,'B',0x04,0x3FFF)
#define BT_ENABLE          _IOC(_IOC_WRITE,'B',0x05,0x3FFF)
#define BT_DISABLE        _IOC(_IOC_WRITE,'B',0x06,0x3FFF)

#endif /* __BT_IOCTL_H__ */

