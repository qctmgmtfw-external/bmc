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

#include "header.h"
#include "descriptors.h"
#include "arch.h"
#include "helper.h"
#include "dbgout.h"
#include "usb_module.h"


#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
int
ReadUsbeStatus(struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int
ReadUsbeStatus(char *buf, char **start, off_t offset, int count, int *eof, void *data)
#endif
{

	int len=0;
	int DevNo;
	int NumDevices = 0;
	int NumInterfaces;
	uint8 NumEndpoints, TotalEndPoints;
	uint8 EpNum;
	uint8 InterfaceNum;
	USB_ENDPOINT_DESC *EndDesc;
	uint8 EpType;
	uint8 EpDir;
	uint16 EpSize;
	uint8 CfgIndex;
	uint8 Direction;
	
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	/* This is a requirement. So don't change it */
	*start = buf;
#endif

/* We don't support seeked reads. read should start from 0 */	
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
	if (*offset != 0)
		return 0;
#else
	*eof=1;
	if (offset != 0)
		return 0;
#endif
	

	/* Fill the buffer with the data */
	len += sprintf(buf+len,"\n----------------------------------------------------------\n");
	len += sprintf(buf+len,"USB Driver Status\n");
	len += sprintf(buf+len,"Copyright (c) 2009 American Megatrends Inc.\n\n");
	len += sprintf(buf+len,"\n----------------------------------------------------------\n");

	for (DevNo = 0; DevNo < MAX_USB_HW; DevNo++)
	{
		if (DevInfo[DevNo].Valid) NumDevices++;
    }
	len += sprintf(buf+len,"Number of USB Devices = %d\n",NumDevices);

	for (DevNo = 0; DevNo < NumDevices; DevNo++)
	{
        len += sprintf(buf+len,"\tUSB Device %d Details...\n",DevNo+1);
		if (DevInfo[DevNo].UsbDevType == CREATE_HID_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = HID\n");
		else if (DevInfo[DevNo].UsbDevType == CREATE_LUN_COMBO_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = LUN Combo\n");
		else if (DevInfo[DevNo].UsbDevType == CREATE_CDROM_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = CDROM\n");
		else if (DevInfo[DevNo].UsbDevType == CREATE_HARDDISK_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = Hard disk\n");
		else if (DevInfo[DevNo].UsbDevType == CREATE_FLOPPY_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = Floppy\n");
		else if (DevInfo[DevNo].UsbDevType == CREATE_COMBO_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = Combo\n");
		else if (DevInfo[DevNo].UsbDevType == CREATE_SUPERCOMBO_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = Super Combo\n");
		else if (DevInfo[DevNo].UsbDevType == CREATE_ETH_DESCRIPTOR)
			len += sprintf(buf+len,"\t\tDevice Type = Ethernet\n");

		TotalEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints;
		CfgIndex = DevInfo[DevNo].CfgIndex;
		if (CfgIndex == 0) continue;
		NumInterfaces = GetNumInterfaces(DevNo, CfgIndex);
		len += sprintf(buf+len,"\t\tNumber of Interfaces = %d\n", NumInterfaces);
		for (InterfaceNum = 0; InterfaceNum < NumInterfaces; InterfaceNum++)
		{
            NumEndpoints = GetNumEndpoints(DevNo, CfgIndex, InterfaceNum);
			len += sprintf(buf+len,"\t\tInterface %d has %d endpoint(s)\n", InterfaceNum+1, NumEndpoints);
			for (EpNum = 1; ((EpNum <= TotalEndPoints) && NumEndpoints); EpNum++) 		
			{
				for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
				{
					EndDesc = GetEndpointDesc(DevNo, CfgIndex,InterfaceNum, EpNum, Direction);
					if (EndDesc == NULL) continue;
					NumEndpoints -= 1;
					len += sprintf(buf+len,"\t\t\tEndpoint number = %d\n", EpNum);	
					{
						EpType = (EndDesc->bmAttributes) & 0x03;
						if (EpType == CONTROL)
							len += sprintf(buf+len,"\t\t\tEndpoint Type = Control\n");		
						else if (EpType == ISOCHRONOUS)
							len += sprintf(buf+len,"\t\t\tEndpoint Type = IsoChronous\n");		
						else if (EpType == BULK)
							len += sprintf(buf+len,"\t\t\tEndpoint Type = Bulk\n");		
						else if (EpType == INTERRUPT)
							len += sprintf(buf+len,"\t\t\tEndpoint Type = Interrupt\n");		
						EpDir  = Direction;
						if (EpDir)
							len += sprintf(buf+len,"\t\t\tEndpoint Direction = IN\n");	
						else
							len += sprintf(buf+len,"\t\t\tEndpoint Direction = OUT\n");	
						EpSize = GetEndPointBufSize (DevNo, EpNum, EpDir << 7);
						len += sprintf(buf+len,"\t\t\tEndpoint Size = %d\n", EpSize);	
					}
				}
			}
		}
	}

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	/* Set End of File if no more data */
	*eof = 1;
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
	*offset+=len;
#endif
	/* Return length of data */
	return len;
}

