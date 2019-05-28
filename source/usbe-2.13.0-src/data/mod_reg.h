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

#ifndef _MOD_REG_H_
#define _MOD_REG_H_

#include "usb_core.h"
#include "usb_hw.h"

extern UsbHwInit DevInit[];
extern UsbHwExit DevExit[];
extern CREATE_DESC CreateDesc[];

/* External Function Prototypes */
extern int get_usb_core_funcs (USB_CORE *UsbCoreFunc);
extern int register_usb_device (usb_device_driver*, USB_DEV *UsbDev);
extern int unregister_usb_device (usb_device_driver* );
extern int register_usb_chip_driver_module (usb_ctrl_driver *);
extern int unregister_usb_chip_driver_module (usb_ctrl_driver *);


#endif /* _MOD_REG_H_ */


