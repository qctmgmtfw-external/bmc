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
#include "coreusb.h"
#include "descriptors.h"
#include "scsi.h"
#include "bot_if.h"
#include "helper.h"
#include "dbgout.h"


/* Descriptor Table used for internal usage during Creation of Descriptors */
typedef struct 
{
	uint8 *DescBuff;
	uint16 DescSize;
	uint16 UsedSize;
	uint8  NextStrId;
	uint8  NextCfgId;
	uint8  NextIfNum;
	uint8  NextEpNum;
	uint8  *StringTable[255];
	uint8  StringLen[255];
	USB_DEVICE_QUALIFIER_DESC	DevQualifier;
} DESCRIPTOR_TABLE; 

/* String 0 - Gives LANGID for English */
static uint8 LangIdEnglish[4] = {0x04,STRING,0x09, 0x04};

/* Descriptor Related variables for each device */
static DESCRIPTOR_TABLE DescTable[MAX_USB_HW] = 
{
	{NULL,0,0,1,1,0,1,{&LangIdEnglish[0]},{4}},
	{NULL,0,0,1,1,0,1,{&LangIdEnglish[0]},{4}},
	{NULL,0,0,1,1,0,1,{&LangIdEnglish[0]},{4}},
	{NULL,0,0,1,1,0,1,{&LangIdEnglish[0]},{4}},
	{NULL,0,0,1,1,0,1,{&LangIdEnglish[0]},{4}},
};

#define LO_BYTE(x)	(uint8)((x) & 0xFF) 
#define HI_BYTE(x)	(uint8)(((x)>>8) & 0xFF) 

/* Interface Information */
static INTERFACE_INFO InterInfo[MAX_USB_HW][MAX_INTERFACES] = 
{ 
	{
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},		
	},	
	{
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},		
	},	
	{
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},		
	},	
	{
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},		
	},	
	{
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},		
	},	
	{
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},		
	},	

};

static int32 GetFirstEndpointDescIndex(uint8 DevNo, uint8 ConfigIndex, uint8 IfaceIndex);

/******************************************************************************/
/*************************** Interface related functions **********************/
/******************************************************************************/
uint8
GetNumConfigurations (uint8 DevNo)
{
	return DescTable [DevNo].NextCfgId - 1;
}
	
void * 
GetInterfaceData(uint8 DevNo, uint8 IfNum)
{
	if (!(InterInfo[DevNo][IfNum].InterfaceData))
		TCRIT("ERROR: GetInterfaceData for Device 0x%x and Inteface 0x%x returned NULL\n",DevNo,IfNum);
	return InterInfo[DevNo][IfNum].InterfaceData;
}

void
SetInterfaceFlag(uint8 DevNo, uint8 IfNum)
{
	InterInfo[DevNo][IfNum].WakeupFlag=1;	
	return;
}

void
ClearInterfaceFlag(uint8 DevNo, uint8 IfNum)
{
	InterInfo[DevNo][IfNum].WakeupFlag=0;	
	return;
}

uint8
GetInterfaceFlag(uint8 DevNo, uint8 IfNum)
{
	return InterInfo[DevNo][IfNum].WakeupFlag;
}


REQ_HANDLER 
GetInterfaceReqHandler(uint8 DevNo, uint8 IfNum)
{
	uint8 CfgIndex;
	uint8 NumInterfaces;

	if (DevInfo[DevNo].UsbDevice.IsThisHUBDevice)
	{
		return InterInfo[DevNo][0].ClassReqHandler;
	}

	CfgIndex = DevInfo[DevNo].CfgIndex;
	if (CfgIndex == 0)
		return NULL;
	NumInterfaces = GetNumInterfaces (DevNo, CfgIndex);
	if (NumInterfaces == 0)
		return NULL;
	if (IfNum >= NumInterfaces)
		return NULL;
	
	return InterInfo[DevNo][IfNum].ClassReqHandler;
}

EP_TX_HANDLER 
GetInterfaceTxHandler(uint8 DevNo, uint8 IfNum)
{
	return InterInfo[DevNo][IfNum].TxHandler;
}

EP_RX_HANDLER 
GetInterfaceRxHandler(uint8 DevNo, uint8 IfNum)
{
	return InterInfo[DevNo][IfNum].RxHandler;
}

PROC_HANDLER 
GetInterfaceProcHandler(uint8 DevNo, uint8 IfNum)
{
	return InterInfo[DevNo][IfNum].ProcHandler;
}

HALT_HANDLER 
GetInterfaceHaltHandler(uint8 DevNo, uint8 IfNum)
{
	return InterInfo[DevNo][IfNum].HaltHandler;
}

UNHALT_HANDLER 
GetInterfaceUnHaltHandler(uint8 DevNo, uint8 IfNum)
{
	return InterInfo[DevNo][IfNum].UnHaltHandler;
}



REM_HANDLER 
GetInterfaceDisconnectHandler(uint8 DevNo, uint8 IfNum)
{
	return InterInfo[DevNo][IfNum].DisconnectHandler;
}


uint8
GetDataInEp(uint8 DevNo,uint8 ifnum)
{
	return InterInfo[DevNo][ifnum].DataInEp;
}

uint8
GetDataOutEp(uint8 DevNo,uint8 ifnum)
{
	return InterInfo[DevNo][ifnum].DataOutEp;
}

uint8
GetNonDataEp(uint8 DevNo,uint8 ifnum)
{
	return InterInfo[DevNo][ifnum].NonDataEp;
}

uint8 *
GetDescriptorofDevice(uint8 DevNo,uint16 *Len)
{
	*Len = DescTable[DevNo].DescSize;
	return DescTable[DevNo].DescBuff;
} 


uint8 *
GetStringDescriptor(uint8 DevNo,uint8 StrIndex, uint16 *Len) 
{
	*Len = DescTable[DevNo].StringLen[StrIndex];
	return DescTable[DevNo].StringTable[StrIndex];
}


USB_DEVICE_DESC *
GetDeviceDesc(uint8 DevNo)
{
	uint8 *Buffer  = DescTable[DevNo].DescBuff;
	uint16 BuffLen = DescTable[DevNo].DescSize;
	
	if (BuffLen < sizeof(USB_DEVICE_DESC))
		return NULL;
		
	if (Buffer[1] != DEVICE)		
		return NULL;

	return (USB_DEVICE_DESC *)Buffer;
}


USB_DEVICE_QUALIFIER_DESC *
GetDeviceQualifierDesc(uint8 DevNo)
{
	DescTable [DevNo].DevQualifier.bLength = sizeof(USB_DEVICE_QUALIFIER_DESC);
	DescTable [DevNo].DevQualifier.bDescriptorType = DEVICE_QUALIFIER;
	DescTable [DevNo].DevQualifier.bcdUSBL 			= 0x10;
	DescTable [DevNo].DevQualifier.bcdUSBH 			= 0x01;		/* USB 1.10 */
	DescTable [DevNo].DevQualifier.bDeviceClass		= 0;
	DescTable [DevNo].DevQualifier.bDeviceSubClass 	= 0;
	DescTable [DevNo].DevQualifier.bDeviceProtocol 	= 0;
	DescTable [DevNo].DevQualifier.bMaxPacketSize0 	= DevInfo[DevNo].UsbDevice.EP0Size;
	DescTable [DevNo].DevQualifier.bNumConfigurations  = 0;
	DescTable [DevNo].DevQualifier.bReserved			= 0;
	return (USB_DEVICE_QUALIFIER_DESC *) &DescTable [DevNo].DevQualifier;
}

USB_CONFIG_DESC *
GetConfigDesc(uint8 DevNo, uint8 ConfigIndex)
{
	int32 Index;
	USB_CONFIG_DESC *CfgDesc;
	uint8 *Buffer  = DescTable[DevNo].DescBuff;
	uint16 BuffLen = DescTable[DevNo].DescSize;

	if (DescTable[DevNo].DescBuff == NULL)
		return NULL;

	/* Set Index to Point to device desc */
	Index=0;

	/* Skip Device Desc */
	Index += Buffer[Index];			
	
	/* Search for the desired Config Index */	
	while (Index < BuffLen) 
	{
		if (Buffer[Index+1]	== CONFIGURATION)
		{
	 		CfgDesc = (USB_CONFIG_DESC *)(&Buffer[Index]);	
			if (CfgDesc->bConfigurationValue == ConfigIndex)
				return CfgDesc;
		}
		Index += Buffer[Index];
	}

	return NULL;
}


USB_INTERFACE_DESC *
GetInterfaceDesc(uint8 DevNo, uint8 ConfigIndex, uint8 IfaceIndex)
{
	int32 Index;
	USB_CONFIG_DESC *CfgDesc;
	USB_INTERFACE_DESC *IfaceDesc;
	uint8 *Buffer  = DescTable[DevNo].DescBuff;
	uint16 BuffLen = DescTable[DevNo].DescSize;
	
	/* Get the Desired Configuration Desc */
	CfgDesc = GetConfigDesc(DevNo,ConfigIndex);
	if (CfgDesc == NULL)
		return NULL;
		
	/* Adjust Index to point to Config Desc */		
	Index  = ((uint8 *)CfgDesc) - Buffer;
	
	/* Skip Configuration Desc */
	Index += Buffer[Index]; 		

	/* Search for the desired Interface Index */	
	while (Index < BuffLen) 
	{
		if (Buffer[Index+1]	== INTERFACE)
		{
	 		IfaceDesc = (USB_INTERFACE_DESC *)(&Buffer[Index]);	
			if (IfaceDesc->bInterfaceNumber == IfaceIndex)
				return IfaceDesc;
		}
		
		/* We have reached the next configuration. So stop now */
		if (Buffer[Index+1]	== CONFIGURATION)
				break;

		Index += Buffer[Index];
	}	

	return NULL;	
}


USB_ENDPOINT_DESC *
GetEndpointDesc(uint8 DevNo, uint8 ConfigIndex, uint8 IfaceIndex, uint8 EndPointNum, uint8 EpDir)
{
	int32 Index;
	USB_ENDPOINT_DESC *EpDesc;
	uint8 *Buffer  = DescTable[DevNo].DescBuff;
	uint16 BuffLen = DescTable[DevNo].DescSize;
	
	/* Get Starting Endpoint Index */
	Index = GetFirstEndpointDescIndex(DevNo,ConfigIndex,IfaceIndex);
	if (Index == 0)
		return 0;
	
	/* Search for the desired Endpoint Index */	
	while (Index < BuffLen) 
	{
		if (Buffer[Index+1]	== ENDPOINT)
		{
	 		EpDesc = (USB_ENDPOINT_DESC *)(&Buffer[Index]);	
			if (((EpDesc->bEndpointAddress & 0x0F) == EndPointNum) &&
				((EpDesc->bEndpointAddress & 0x80)  == (EpDir << 7)))
				return EpDesc;
		}
		
		/* We have reached the next non endpoint (Interface/Configuration) descriptor. So stop now */
  		if (Buffer[Index+1]	!= ENDPOINT)
			break;					
		Index += Buffer[Index];
	}	

	return NULL;	
}


uint16
GetConfigDescSize(uint8 DevNo, uint8 ConfigIndex)
{

	int32 Index, StartIndex; 
	USB_CONFIG_DESC *CfgDesc;
	uint8 *Buffer  = DescTable[DevNo].DescBuff;
	uint16 BuffLen = DescTable[DevNo].DescSize;
	
		
	/* Get the Desired Configuration Desc */
	CfgDesc = GetConfigDesc(DevNo,ConfigIndex);
	if (CfgDesc == NULL)
		return 0;
		
	/* Adjust Index to point to Config Desc */		
	Index  = ((uint8 *)CfgDesc) - Buffer;
	
	/* Save the starting value */
	StartIndex = Index;
	
	/* Skip Configuration Desc */
	Index += Buffer[Index]; 		

	/* Scan till end of Buffer or the next config is found */
	while (Index < BuffLen) 
	{
		if (Buffer[Index+1]	== CONFIGURATION)
			break;

		Index += Buffer[Index];
	}	

	/* Size is ending - starting value */
	return (uint16)(Index-StartIndex);	

}

uint8
GetStartingEpNum(uint8 DevNo, uint8 CfgIndex, uint8 IntIndex)
{
	int32 Index;
	USB_ENDPOINT_DESC *EpDesc;
	uint8 *Buffer  = DescTable[DevNo].DescBuff;
		
	/* Get Starting Endpoint Index */
	Index = GetFirstEndpointDescIndex(DevNo,CfgIndex,IntIndex);
	if (Index == 0)
		return 0;
	
	/* Get Associated EndPoint and return its EpNum */
	EpDesc = (USB_ENDPOINT_DESC *)(&Buffer[Index]);	
	
	return (EpDesc->bEndpointAddress & 0x0F);

}

uint8  
GetNumInterfaces(uint8 DevNo, uint8 CfgIndex)
{
	USB_CONFIG_DESC *CfgDesc;
	
	CfgDesc = GetConfigDesc(DevNo,CfgIndex);
	if (CfgDesc == NULL)
		return 0;
	
	return CfgDesc->bNumInterfaces;
}


uint8  
GetNumEndpoints(uint8 DevNo, uint8 CfgIndex, uint8 IntIndex)
{
	USB_INTERFACE_DESC *InterDesc;
	
	InterDesc = GetInterfaceDesc(DevNo,CfgIndex,IntIndex);
	if (InterDesc == NULL)
		return 0;
	
	return InterDesc->bNumEndpoints;
}




uint16  
GetMaxPacketSize(uint8 DevNo, uint8 CfgIndex, uint8 EpNum, uint8 EpDir)
{

	uint8 bNumInterfaces,IfNum;
	USB_ENDPOINT_DESC *EndDesc;	
	uint16 PacketSize;
	
	bNumInterfaces = GetNumInterfaces(DevNo,CfgIndex);
	
	for (IfNum=0;IfNum<bNumInterfaces;IfNum++)
	{
		EndDesc = GetEndpointDesc(DevNo,CfgIndex,IfNum,EpNum, EpDir);
		if (EndDesc != NULL)
		{
			PacketSize = (uint16)(EndDesc->wMaxPacketSizeL) + ((uint16)(EndDesc->wMaxPacketSizeH) *0x100);
			return PacketSize;
		}		
	}
	return 0;

}

uint8  
GetInterface(uint8 DevNo, uint8 CfgIndex, uint8 EpNum, uint8 EpDir)
{
	uint8 bNumInterfaces,IfNum;
	
	bNumInterfaces = GetNumInterfaces(DevNo,CfgIndex);
	
	for (IfNum=0;IfNum<bNumInterfaces;IfNum++)
	{
		if (GetEndpointDesc(DevNo,CfgIndex,IfNum,EpNum,EpDir) != NULL)
			return IfNum;
	}
	return 0xFF;
}

uint8  
GetEndPointType(uint8 DevNo, uint8 CfgIndex, uint8 EpNum, uint8 EpDir)
{
	uint8 bNumInterfaces,IfNum;
	USB_ENDPOINT_DESC *EndDesc;	
	
	bNumInterfaces = GetNumInterfaces(DevNo,CfgIndex);
	
	for (IfNum=0;IfNum<bNumInterfaces;IfNum++)
	{
		EndDesc = GetEndpointDesc(DevNo,CfgIndex,IfNum,EpNum, EpDir);
		if (EndDesc != NULL)
			return EndDesc->bmAttributes;
	}
	return CONTROL;	
}

static
uint8 *
ConvertAsciizToUniCode(char *Str, uint16 *UniCodeLen)
{
	int StrLen,i;
	uint8 *UniCodeStr;
	
	StrLen = strlen(Str);
	*UniCodeLen = (StrLen+1)*2 + 2;
	
	UniCodeStr = (uint8 *)kmalloc(*UniCodeLen, GFP_ATOMIC);
	if (UniCodeStr == NULL)
		return NULL;
		
		
	UniCodeStr[0] = *UniCodeLen;
	UniCodeStr[1] = STRING;		
	
	i=2;		
	while (*Str)
	{
		UniCodeStr[i] 	= *Str;
		UniCodeStr[i+1] = 0;
		i+=2;
		Str++;
	}
	UniCodeStr[i] 	= 0;
	UniCodeStr[i+1] = 0;


	return UniCodeStr;
}


int
AddDescriptor(uint8 DevNo, uint8* Desc , uint16 DescSize)
{
	uint8 *Buffer;
	/* Add the Descriptor to the Buffer */	
	if (DescSize > (DescTable[DevNo].DescSize - DescTable[DevNo].UsedSize))
		return 1;

	Buffer = DescTable[DevNo].DescBuff + DescTable[DevNo].UsedSize;
	memcpy(Buffer,Desc,DescSize);
	DescTable[DevNo].UsedSize += DescSize;
	return 0;
}



uint8*
CreateDescriptor(uint8 DevNo,uint16 DescSize) 
{
	if (DevNo >= MAX_USB_HW)
		return NULL;
		
	DescTable[DevNo].DescBuff =(uint8 *) kmalloc(DescSize, GFP_ATOMIC);
	if (DescTable[DevNo].DescBuff == NULL)
		return NULL;
		
	DescTable[DevNo].DescSize = DescSize;
	DescTable[DevNo].UsedSize = 0;	
	
	return DescTable[DevNo].DescBuff;
}


int
AddDeviceDesc(uint8 DevNo, uint16 VendorId, uint16 ProductId, uint16 DeviceRev,
								char *ManufStr, char*ProdStr, int IsThisLunDev)
{
	USB_DEVICE_DESC 		DevDesc;
	uint8 					*Str;
	uint16					StrLen;

	DevDesc.bLength 			= sizeof(USB_DEVICE_DESC);
	DevDesc.bDescriptorType 	= DEVICE;
	DevDesc.bcdUSBL 			= 0x10;
	DevDesc.bcdUSBH 			= 0x01;		/* USB 1.10 */
	DevDesc.bDeviceClass		= 0;
	DevDesc.bDeviceSubClass 	= 0;
	DevDesc.bDeviceProtocol 	= 0;
	DevDesc.bMaxPacketSize0 	= DevInfo[DevNo].UsbDevice.EP0Size;
	DevDesc.idVendorL 			= LO_BYTE(VendorId);
	DevDesc.idVendorH 			= HI_BYTE(VendorId);
	DevDesc.idProductL 			= LO_BYTE(ProductId);
	DevDesc.idProductH 			= HI_BYTE(ProductId);
	DevDesc.bcdDeviceL 			= LO_BYTE(DeviceRev);
	DevDesc.bcdDeviceH 			= HI_BYTE(DeviceRev);
	DevDesc.iManufacturer		= 0;
	DevDesc.iProduct 			= 0;	
	DevDesc.iSerialNumber 		= 0;
	DevDesc.bNumConfigurations  = 0;
	
	if (ManufStr)
	{
		Str = ConvertAsciizToUniCode(ManufStr,&StrLen);
		if (Str)
		{
			DevDesc.iManufacturer 	= DescTable[DevNo].NextStrId;
			DescTable[DevNo].StringTable[DescTable[DevNo].NextStrId] = Str;
			DescTable[DevNo].StringLen[DescTable[DevNo].NextStrId] = StrLen;			
	    	DescTable[DevNo].NextStrId++;		
		}		    	
	}		
	if (ProdStr)		
	{
		Str = ConvertAsciizToUniCode(ProdStr,&StrLen);		
		if (Str)
		{
			DevDesc.iProduct = DescTable[DevNo].NextStrId;
			DescTable[DevNo].StringTable[DescTable[DevNo].NextStrId] = Str;
			DescTable[DevNo].StringLen[DescTable[DevNo].NextStrId] = StrLen;						
			DescTable[DevNo].NextStrId++;
		}			
	}		
	if (IsThisLunDev)
	{
		/*LUN Implementation require Serial String */
		Str = ConvertAsciizToUniCode("serial",&StrLen);		
		DevDesc.iSerialNumber = DescTable[DevNo].NextStrId;
		DescTable[DevNo].StringTable[DescTable[DevNo].NextStrId] = Str;
		DescTable[DevNo].StringLen[DescTable[DevNo].NextStrId] = StrLen;						
		DescTable[DevNo].NextStrId++;
	}

	/* Add the Descriptor to the Buffer */		
	return AddDescriptor(DevNo,(uint8 *)&DevDesc,sizeof(USB_DEVICE_DESC));
	
}	


int
AddCfgDesc(uint8 DevNo, uint8 NumInterfaces, uint8 hid)
{
	USB_CONFIG_DESC CfgDesc;
	uint8 CfgTotalSize;
	USB_DEVICE_DESC *Device;
	int i;
	
	CfgTotalSize= DescTable[DevNo].DescSize - sizeof(USB_DEVICE_DESC);
	
	CfgDesc.bLength	 			=	sizeof(USB_CONFIG_DESC);
	CfgDesc.bDescriptorType 	= 	CONFIGURATION;
	CfgDesc.wTotalLengthL 		=  	LO_BYTE(CfgTotalSize);
	CfgDesc.wTotalLengthH 		=   HI_BYTE(CfgTotalSize);
	CfgDesc.bNumInterfaces 		=   NumInterfaces;
	CfgDesc.bConfigurationValue =	DescTable[DevNo].NextCfgId++;

	CfgDesc.iConfiguration 		= 	0,
	CfgDesc.maxPower 			= 	0;	/* Self Powered , So no power from bus*/
	if (hid)
		CfgDesc.bmAttributes 		=	SELF_POWERED|REMOTE_WAKEUP; 
	else
		CfgDesc.bmAttributes 		=	SELF_POWERED;


	DescTable[DevNo].NextIfNum = 0;

	/* AddDeviceDesc does not allow us to add more than 1 config
	   so add it manually */
	Device = GetDeviceDesc(DevNo);
	Device->bNumConfigurations++;

	memset (DevEpInfo[DevNo].epnumbuf, 0, sizeof (DevEpInfo[DevNo].epnumbuf));
	for (i = 0; i < DevEpInfo[DevNo].num_eps; i++)
	{
		DevEpInfo[DevNo].ep_config[i].is_ep_used = 0;
	}
	
	/* Add the Descriptor to the Buffer */		
	return AddDescriptor(DevNo,(uint8*)&CfgDesc,sizeof(USB_CONFIG_DESC));
	
}



int
AddInterfaceDesc(uint8 DevNo, uint8 bNumEndPoints, uint8 bClass, 
						uint8 bSubClass, uint8 bProtocol, char *IntStr,INTERFACE_INFO *InterfaceInfo)
{
	USB_INTERFACE_DESC IntDesc;
	uint8 *Str;
	uint16 StrLen;
	
	IntDesc.bLength  				=	sizeof(USB_INTERFACE_DESC);
	IntDesc.bDescriptorType			=	INTERFACE;
	IntDesc.bInterfaceNumber 		=   DescTable[DevNo].NextIfNum++;
	IntDesc.bAlternateSetting 		=   0;
	IntDesc.bNumEndpoints 			=   bNumEndPoints;
	IntDesc.bInterfaceClass 		= 	bClass;
	IntDesc.bInterfaceSubClass 		=	bSubClass;
	IntDesc.bInterfaceProtocol 		= 	bProtocol;
	IntDesc.iInterface 				=	0;

	if (IntStr)
	{
		Str = ConvertAsciizToUniCode(IntStr,&StrLen);
		if (Str)
		{
			IntDesc.iInterface = DescTable[DevNo].NextStrId;
			DescTable[DevNo].StringTable[DescTable[DevNo].NextStrId] = Str;
			DescTable[DevNo].StringLen[DescTable[DevNo].NextStrId] = StrLen;						
	    	DescTable[DevNo].NextStrId++;		
		}		    	
	}		
	
	/* Save Interface Information */
	memcpy(&InterInfo[DevNo][IntDesc.bInterfaceNumber],InterfaceInfo,sizeof(INTERFACE_INFO));
			
	/* Add the Descriptor to the Buffer */		
	return AddDescriptor(DevNo,(uint8 *)&IntDesc,sizeof(USB_INTERFACE_DESC));
	
}

#if 0
int
display_ep_info (uint8 DevNo)
{
		int i;
		uint8 *Buffer = (uint8*) &DevEpInfo[DevNo].ep_config[0];
		printk ("Device Number = %d\n", DevNo);
		printk ("number of eps = %d\n", DevEpInfo[DevNo].num_eps);
		printk ("ep data is as follows\n");
		for (i = 0; i < sizeof (ep_config_t) * MAX_IN_AND_OUT_ENDPOINTS; i++)
		{
				if ((i != 0) && ((i % 6) == 0)) printk ("\n");
				printk ("0x%02x ", Buffer[i]);
				
		}
		printk ("\n");
		return 0;
}
#endif

static uint8
select_suitable_endpoint (uint8 DevNo, uint8 Dir, uint8 Type, uint16 MinPktSize, 
							   uint16 MaxPktSize, uint8 Interval, uint8 DataEp, int* pEpBufSize)
{
	int i;
	uint8 ep = 0x0;
	uint16 ep_size = 0;
	int index = 0;

	for (i = 0; i < DevEpInfo[DevNo].num_eps; i++)
	{
		if (DevEpInfo[DevNo].ep_config[i].is_ep_used) continue;
		if ((Dir != DevEpInfo[DevNo].ep_config[i].ep_dir) &&
			(CONFIGURABLE != DevEpInfo[DevNo].ep_config[i].ep_dir))	 continue;
        if ((Type != DevEpInfo[DevNo].ep_config[i].ep_type) && 
		    (CONFIGURABLE != DevEpInfo[DevNo].ep_config[i].ep_type)) continue;
		if (DevEpInfo[DevNo].ep_config[i].ep_size < MinPktSize) continue;
		if (DevEpInfo[DevNo].ep_config[i].ep_size == MaxPktSize)
		{
#if 0			
			if (DevEpInfo[DevNo].epnumbuf[DevEpInfo[DevNo].ep_config[i].ep_num])
			{
//				 printk ("in USB select_suitable_endpoint (): Skipping Endpoint %d\n", 
//						 DevEpInfo[DevNo].ep_config[i].ep_num);
				 continue;
			}
#endif			
			index = i;
			ep = DevEpInfo[DevNo].ep_config[i].ep_num;
			ep_size = DevEpInfo[DevNo].ep_config[i].ep_size;
			break;
		}
		if (ep_size >= DevEpInfo[DevNo].ep_config[i].ep_size) continue;
#if 0		
		if (DevEpInfo[DevNo].epnumbuf[DevEpInfo[DevNo].ep_config[i].ep_num])
		{
//			 printk ("in USB select_suitable_endpoint (): Skipping Endpoint %d\n", 
//					 DevEpInfo[DevNo].ep_config[i].ep_num);
			 continue;
		}
		else
#endif			
		{
			index = i;
			ep = DevEpInfo[DevNo].ep_config[i].ep_num;
			ep_size = DevEpInfo[DevNo].ep_config[i].ep_size;
		}
	}
	DevEpInfo[DevNo].ep_config[index].is_ep_used = 1;
	if (DevEpInfo[DevNo].ep_config[index].ep_dir == CONFIGURABLE)
			DevEpInfo[DevNo].ep_config[index].ep_dir = Dir;
	DevEpInfo[DevNo].epnumbuf[ep] = 1;
	*pEpBufSize = DevEpInfo[DevNo].ep_config[index].ep_size;
//	printk ("in USB select_suitable_endpoint (): devno= %d and ep = %d\n", DevNo, ep);
	if (ep == 0) ep = 0xFF;
	return ep;
}


int
AddEndPointDesc(uint8 DevNo, uint8 Dir, uint8 Type, uint16 MinPktSize, 
								uint16 MaxPktSize, uint8 Interval, uint8 DataEp)
{
	USB_ENDPOINT_DESC EndDesc;
	uint8 ifnum;
	int   retval;
	int   EpBufSize = 0;

	DescTable[DevNo].NextEpNum = select_suitable_endpoint (DevNo, Dir, Type, MinPktSize, MaxPktSize, Interval, DataEp, &EpBufSize);
	if (DescTable[DevNo].NextEpNum == 0xFF)
	{
			TCRIT ("Error: Unable to find suitable end point for DevNo %d Type %d epDir %d\n", DevNo, Type, Dir);
			return 1;
	}

	EndDesc.bLength				=	sizeof(USB_ENDPOINT_DESC),
	EndDesc.bDescriptorType 	= 	ENDPOINT,
	EndDesc.bEndpointAddress	= 	DescTable[DevNo].NextEpNum | Dir ;	
	EndDesc.bmAttributes 		= 	Type,
	EndDesc.wMaxPacketSizeL 	=   LO_BYTE(EpBufSize/*MaxPktSize*/);
	EndDesc.wMaxPacketSizeH		=   HI_BYTE(EpBufSize/*MaxPktSize*/);
	EndDesc.bInterval			= 	Interval;


	/* Add the Descriptor to the Buffer */		
	retval = AddDescriptor(DevNo,(uint8 *)&EndDesc,sizeof(USB_ENDPOINT_DESC));
	if (retval == 1)
		return 1;

	/* If this is used as Data In or Data Out, Save it in interface info */
	ifnum = GetInterface(DevNo, DescTable[DevNo].NextCfgId - 1, DescTable[DevNo].NextEpNum, Dir >> 7);



	if (DataEp == DATA_EP)
	{
		if (Dir == IN)
			InterInfo[DevNo][ifnum].DataInEp = DescTable[DevNo].NextEpNum;	
		else
			InterInfo[DevNo][ifnum].DataOutEp = DescTable[DevNo].NextEpNum;	
 		
	}		
	else {
			InterInfo[DevNo][ifnum].NonDataEp = DescTable[DevNo].NextEpNum;
	}

//	DescTable[DevNo].NextEpNum++;

	return retval;
	
}


int
AddHidDesc(uint8 DevNo, uint16 ReportSize)
{
	USB_HID_DESC HidDesc;

	
	HidDesc.bLength				= 	sizeof(USB_HID_DESC);
	HidDesc.bHidDescriptorType 	= 	HID_DESC;
	HidDesc.bcdHIDL 			=	0x10;
	HidDesc.bcdHIDH 			=	0x01;	/* HID spec 1.1 */
	HidDesc.bCountryCode 		=	0;		/* not supported */
	HidDesc.bNumDescriptors 	=	1;
	HidDesc.bClassDescriptorType= 	REPORT_DESC;
	HidDesc.wDescriptorLengthL 	=	LO_BYTE(ReportSize);
	HidDesc.wDescriptorLengthH 	=	HI_BYTE(ReportSize);

		
	/* Add the Descriptor to the Buffer */		
	return AddDescriptor(DevNo,(uint8 *)&HidDesc,sizeof(USB_HID_DESC));
	
}





static
int32
GetFirstEndpointDescIndex(uint8 DevNo, uint8 ConfigIndex, uint8 IfaceIndex)
{
	int32 Index;
	USB_INTERFACE_DESC *IfaceDesc;
	uint8 *Buffer  = DescTable[DevNo].DescBuff;
	uint16 BuffLen = DescTable[DevNo].DescSize;
	
	/* Get the Desired Interface Desc */
	IfaceDesc = GetInterfaceDesc(DevNo,ConfigIndex,IfaceIndex);
	if (IfaceDesc == NULL)
		return 0;
	
	/* If no endpoints return */		
	if (IfaceDesc->bNumEndpoints == 0)
		return 0;		
		
	/* Adjust Index to point to Interface Desc */		
	Index  = ((uint8 *)IfaceDesc) - Buffer;
	
	/* Skip Interface Desc */
	Index += Buffer[Index]; 		

	/* Skip any non Endpoint Desc like HID,HUB e.t.c*/
	while (Index < BuffLen) 
	{
		if (Buffer[Index+1]	== ENDPOINT)
			return Index;
		Index += Buffer[Index];
	}	

	return 0;			
	
}

void
DestroyDescriptor(uint8 DevNo)
{
	uint8 i;

	/* Clear Descriptor Table */
	if (DescTable[DevNo].DescBuff)
	{
		DescTable[DevNo].DescSize =0;
		DescTable[DevNo].UsedSize =0;
		kfree(DescTable[DevNo].DescBuff);
		DescTable[DevNo].DescBuff=NULL;
	}
	
	/* Clear String Table */
	for (i=1;i<DescTable[DevNo].NextStrId;i++)
	{
		if (DescTable[DevNo].StringTable[i])
		{
			DescTable[DevNo].StringLen[i]	= 0;
			kfree(DescTable[DevNo].StringTable[i]);
			DescTable[DevNo].StringTable[i] = NULL;
		}
	}
	
	/* ReInitialize DescTable to Orginal Values */
	DescTable[DevNo].NextIfNum		= 0;
	DescTable[DevNo].NextEpNum		= 1;
	DescTable[DevNo].NextCfgId      = 1;
	DescTable[DevNo].StringTable[0]	= &LangIdEnglish[0];
	DescTable[DevNo].StringLen[0]	= 4;
	DescTable[DevNo].NextStrId		= 1;

	/* Clear Interface Information */
	for (i=0;i<MAX_INTERFACES;i++)
	{
		void *IfData = InterInfo[DevNo][i].InterfaceData;
		if (IfData) {
			vfree(InterInfo[DevNo][i].InterfaceData);
		}
		InterInfo[DevNo][i].InterfaceData = NULL;
		memset(&(InterInfo[DevNo][i]),0,sizeof(INTERFACE_INFO));
	}

	return;
}

int
AddHubDesc(uint8 DevNo, uint16 ReportSize, uint8 NumPorts)
{
        USB_HUB_DESC HubDesc;


        HubDesc.bDescLength             = sizeof(USB_HUB_DESC);
        HubDesc.bDescriptorType         = HUB_DESC;
        HubDesc.bNbrPorts               = NumPorts; // The number of devices of HUB
        HubDesc.wHubCharacteristicsL    = 0x09; //0xE0;   
        HubDesc.wHubCharacteristicsH    = 0x00;
        HubDesc.bPwrOn2PwrGood  		= 0x32;
        HubDesc.bHubContrCurrent		= 0x64;
        HubDesc.DeiveRemovable 			= 0x00;
        HubDesc.PortPwrCtrlMask 		= 0xFF; //0x00

        /* Add the Descriptor to the Buffer */
        return AddDescriptor(DevNo,(uint8 *)&HubDesc,sizeof(USB_HUB_DESC));
}

USB_HUB_DESC*
GetHubDesc(uint8 DevNo,uint8 CfgIndex, uint8 IntIndex)
{
        int32 Index;
        USB_INTERFACE_DESC *IfaceDesc;
        uint8 *Buffer  = DescTable[DevNo].DescBuff;

	   	return (USB_HUB_DESC *)(&Buffer[36]);

       /* Get the Desired Interface Desc */
		IfaceDesc = GetInterfaceDesc(DevNo,CfgIndex,IntIndex);
        if (IfaceDesc == NULL)
                return NULL;

        /* Adjust Index to point to Config Desc */
        Index  = ((uint8 *)IfaceDesc) - Buffer;

        /* Skip Interface Desc */
        Index += Buffer[Index];

        /* This should be HUB_DESC if it is present */
        if (Buffer[Index+1] == HUB_DESC)
                return (USB_HUB_DESC *)(&Buffer[Index]);

        return NULL;

}

int
AddStringDescriptor(uint8 DevNo, char *Str)
{
	uint16 Length;
	
	DescTable[DevNo].StringTable[DescTable[DevNo].NextStrId] = 
		ConvertAsciizToUniCode(Str, &Length);
	
	DescTable[DevNo].StringLen[DescTable[DevNo].NextStrId] = Length;
	
	return DescTable[DevNo].NextStrId++;
}


