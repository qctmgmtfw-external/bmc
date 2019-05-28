/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef USB_IOCTL_H
#define USB_IOCTL_H

/*Ioctls */
#define USB_GET_IUSB_DEVICES	_IOC(_IOC_READ, 'U',0x00,0x3FFF)
#define USB_GET_CONNECT_STATE	_IOC(_IOC_READ, 'U',0x01,0x3FFF)
#define USB_KEYBD_DATA		_IOC(_IOC_WRITE,'U',0x11,0x3FFF)
#define USB_KEYBD_LED		_IOC(_IOC_READ ,'U',0x12,0x3FFF)
#define USB_KEYBD_EXIT		_IOC(_IOC_WRITE,'U',0x13,0x3FFF)
#define USB_KEYBD_LED_NO_WAIT	_IOC(_IOC_READ,'U',0x14,0x3FFF)
#define USB_KEYBD_LED_RESET	_IOC(_IOC_WRITE,'U',0x15,0x3FFF)

#define USB_MOUSE_DATA		_IOC(_IOC_WRITE,'U',0x21,0x3FFF)
#define MOUSE_ABS_TO_REL        _IOC(_IOC_WRITE,'U',0x22,0x3FFF)
#define MOUSE_REL_TO_ABS        _IOC(_IOC_WRITE,'U',0x23,0x3FFF)
#define MOUSE_GET_CURRENT_MODE  _IOC(_IOC_WRITE,'U',0x24,0x3FFF)
#define USB_GET_ADDRESS  	_IOC(_IOC_WRITE,'U',0x25,0x3FFF) 

#define USB_CDROM_REQ		_IOC(_IOC_READ, 'U',0x31,0x3FFF)
#define USB_CDROM_RES		_IOC(_IOC_WRITE,'U',0x32,0x3FFF)
#define USB_CDROM_EXIT		_IOC(_IOC_WRITE,'U',0x33,0x3FFF)
#define USB_CDROM_ACTIVATE	_IOC(_IOC_WRITE,'U',0x34,0x3FFF)


#define USB_HDISK_REQ		_IOC(_IOC_READ, 'U',0x41,0x3FFF)
#define USB_HDISK_RES		_IOC(_IOC_WRITE,'U',0x42,0x3FFF)
#define USB_HDISK_EXIT		_IOC(_IOC_WRITE,'U',0x43,0x3FFF)
#define USB_HDISK_ACTIVATE	_IOC(_IOC_WRITE,'U',0x44,0x3FFF)
#define USB_HDISK_SET_TYPE	_IOC(_IOC_WRITE,'U',0x45,0x3FFF)
#define USB_HDISK_GET_TYPE	_IOC(_IOC_READ, 'U',0x46,0x3FFF)

#define USB_FLOPPY_REQ		_IOC(_IOC_READ, 'U',0x51,0x3FFF)
#define USB_FLOPPY_RES		_IOC(_IOC_WRITE,'U',0x52,0x3FFF)
#define USB_FLOPPY_EXIT		_IOC(_IOC_WRITE,'U',0x53,0x3FFF)
#define USB_FLOPPY_ACTIVATE	_IOC(_IOC_WRITE,'U',0x54,0x3FFF)

#define USB_VENDOR_REQ		_IOC(_IOC_READ, 'U',0x61,0x3FFF)
#define USB_VENDOR_RES		_IOC(_IOC_WRITE,'U',0x62,0x3FFF)
#define USB_VENDOR_EXIT		_IOC(_IOC_WRITE,'U',0x63,0x3FFF)

#define USB_LUNCOMBO_ADD_CD	_IOC(_IOC_WRITE,'U',0x91,0x3FFF) 
#define USB_LUNCOMBO_ADD_FD	_IOC(_IOC_WRITE,'U',0x92,0x3FFF) 
#define USB_LUNCOMBO_ADD_HD	_IOC(_IOC_WRITE,'U',0x93,0x3FFF) 
#define USB_LUNCOMBO_ADD_VF	_IOC(_IOC_WRITE,'U',0x94,0x3FFF) 
#define USB_LUNCOMBO_RST_IF	_IOC(_IOC_WRITE,'U',0x95,0x3FFF) 

#define USB_CDROM_ADD_CD	_IOC(_IOC_WRITE,'U',0xA1,0x3FFF) 
#define USB_CDROM_RST_IF	_IOC(_IOC_WRITE,'U',0xA2,0x3FFF) 

#define USB_FLOPPY_ADD_FD	_IOC(_IOC_WRITE,'U',0xA6,0x3FFF) 
#define USB_FLOPPY_RST_IF	_IOC(_IOC_WRITE,'U',0xA7,0x3FFF) 

#define USB_HDISK_ADD_HD	_IOC(_IOC_WRITE,'U',0xAB,0x3FFF) 
#define USB_HDISK_RST_IF	_IOC(_IOC_WRITE,'U',0xAC,0x3FFF) 

#define USB_DEVICE_DISCONNECT	_IOC(_IOC_WRITE,'U',0xE3,0x3FFF)
#define USB_DEVICE_RECONNECT	_IOC(_IOC_WRITE,'U',0xE4,0x3FFF)

#define USB_GET_INTERFACES	_IOC(_IOC_READ,'U',0xF1,0x3FFF)
#define USB_REQ_INTERFACE	_IOC(_IOC_READ,'U',0xF2,0x3FFF)
#define USB_REL_INTERFACE	_IOC(_IOC_WRITE,'U',0xF3,0x3FFF)
#define USB_DISABLE_ALL_DEVICE _IOC(_IOC_WRITE,'U',0xF4,0x3FFF)
#define USB_ENABLE_ALL_DEVICE _IOC(_IOC_WRITE,'U',0xF5,0x3FFF)
#define USB_GET_ALL_DEVICE_STATUS _IOC(_IOC_WRITE,'U',0xF6,0x3FFF)
//USB 2.0 enabling/disabling support
#define USB_DISABLE_MEDIA_DEVICE _IOC(_IOC_WRITE,'U',0xF7,0x3FFF)
#define USB_ENABLE_MEDIA_DEVICE _IOC(_IOC_WRITE,'U',0xF8,0x3FFF)

#endif  /* USB_IOCTL_H */

/****************************************************************************/
