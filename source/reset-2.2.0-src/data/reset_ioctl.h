/*****************************************************************
 **                                                             **
 **     (C) Copyright 2009-2015, American Megatrends Inc.       **
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

#define GET_LPCRST_COUNT	_IOC(_IOC_READ, 'R',0x00,0x3FFF)
#define SET_LPCRST_COUNT	_IOC(_IOC_WRITE, 'R',0x01,0x3FFF)
#define WAIT_ON_RESET          _IOC(_IOC_READ,'R',0x03,0x3FFF)

#endif
