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

#ifndef __VMEDIA_IFC_H__
#define __VMEDIA_IFC_H__

#include "iusb.h"

int RequestiUSBDevice (IUSB_REQ_REL_DEVICE_INFO* iUSBDevInfo);
int ReleaseiUSBDevice (IUSB_REQ_REL_DEVICE_INFO* iUSBDevInfo);
int SetAuthInfo (IUSB_REQ_REL_DEVICE_INFO iUSBDevInfo, u8* Data, u8 IsiUSBHeader);


#endif /* __VMEDIA_IFC_H__ */


