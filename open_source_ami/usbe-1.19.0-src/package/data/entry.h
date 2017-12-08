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

#ifndef __ENTRY_H__
#define __ENTRY_H__

#define MAX_IUSB_DEVICES		20

/* Entry points to USB Module */
int  Entry_UsbInit(void);
irqreturn_t Entry_UsbIsr(uint8 DevNo,uint8 epnum);
void Entry_UsbExit(void);

/*Descriptor access*/
int IsDevCdrom(uint8 DevNo);
int IsDevFloppy(uint8 DevNo);
int IsDevHid(uint8 DevNo);
int IsDevBotCombo(uint8 DevNo);
int IsDevSuperCombo(uint8 DevNo);

int GetiUsbDeviceList(IUSB_DEVICE_LIST *DevList);
int GetFreeInterfaces (uint8 DeviceType, uint8 LockType, IUSB_DEVICE_LIST *DevList);
int RequestInterface (IUSB_REQ_REL_DEVICE_INFO* iUSBInfo);
int ReleaseInterface (IUSB_REQ_REL_DEVICE_INFO* iUSBInfo);


#endif /* __ENTRY_H__*/



