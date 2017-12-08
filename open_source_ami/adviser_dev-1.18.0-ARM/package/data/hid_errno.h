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

#ifndef HID_ERRNO_H
#define HID_ERRNO_H



#define _ERR_HID_EXTEND    (0x500)
#define HID_ERROR(x)    ((x) | (_ERR_HID_EXTEND))


#define HID_ESUCCESS                (0x0000)
#define HID_EMAX_CONN_LIMIT_REACHED HID_ERROR(1)
#define HID_ESERVER_PORTS_BUSY      HID_ERROR(2)
#define HID_EDEV_ALLOW_ACCESS       HID_ERROR(3)
#define HID_EDEV_DENY_ACCESS        HID_ERROR(4)
#define HID_ENODEV_FOUND            HID_ERROR(5)
#define HID_EINVALID_CMD            HID_ERROR(6)
#define HID_EKBC_OBUF_FULL          HID_ERROR(7)
#define HID_EKBC_IBUF_FULL          HID_ERROR(8)
#define HID_EPOR_INIT               HID_ERROR(9)        
#define HID_EPOR_KBD_NORESP         HID_ERROR(10) 
#define HID_EPOR_KBD_ABS            HID_ERROR(11)
#define HID_EKBD_LOW                HID_ERROR(12)
#define HID_EMOUSE_LOW              HID_ERROR(13)
#define HID_EKBD_HIGH               HID_ERROR(14)
#define HID_EMOUSE_HIGH             HID_ERROR(15)
#define HID_ELED_CHANGED            HID_ERROR(16)
#define HID_EMOUSE_DATA_ERROR       HID_ERROR(17)
#define HID_EKBD_DATA_ERROR         HID_ERROR(18)
#define HID_EOUTBUF_SIZE_LESS       HID_ERROR(19)
#define HID_EUART_READ              HID_ERROR(20)
#define HID_EAUTH					HID_ERROR(21)
#define HID_ENOSESSION				HID_ERROR(22)
#define HID_EINVALIDSESSION			HID_ERROR(23)
#define HID_EAGAIN                 HID_ERROR(24)
#define HID_EDEVPROTOCOL           HID_ERROR(25)


#endif // HID_ERRNO_H

