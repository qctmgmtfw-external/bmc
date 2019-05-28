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
#include "helper.h"
#include "dbgout.h"
#include "usb_module.h"

extern uint8 ExtProp[76];

static
void
ProcessVendorRequest(uint8 DevNo,DEVICE_REQUEST *pRequest)
{
	uint8 *Buffer;
	uint16 Length;
	
	pRequest->Command ="VENDOR REQUEST";
	//printk("ProcessVendorRequest(): DevNo: 0x%x ReqType: 0x%x Req: 0x%x::pRequest->wIndex::%d:pRequest->Command):%s\n",
	//	    				DevNo,pRequest->bmRequestType,pRequest->bRequest,pRequest->wIndex,pRequest->Command);
	
	if ( (pRequest->wIndex == 0x05) && (pRequest->bRequest == 0xA5))
	{
		 Length = sizeof(USB_EXTENDED_PROPERTY_DESC);
		 
		 if (pRequest->wLength < Length)
			Length = pRequest->wLength;
		 
		 Buffer =  ExtProp;
		 if (Length < pRequest->wLength)
		 {
		 	pRequest->wLength = Length;
		    memcpy(pRequest->Data, Buffer, Length);
		 }
		 else
		 {
		 	pRequest->wLength = Length;
		    memcpy(pRequest->Data, Buffer, Length);
		 }
		 pRequest->Status = 0;
	}
	else
	{
		pRequest->Status = 1;
	}
	
	return;
}


static
void
ProcessClassRequest(uint8 DevNo,DEVICE_REQUEST *pRequest)
{

	REQ_HANDLER	ClassReqHandler;
	int RetVal;

	TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessClassRequest(): ");
	if (pRequest->bmRequestType & 0x80)		
	{
		TDBG_FLAGGED(usbe, DEBUG_SETUP,"Device to Host Transfer of Length %d for ",pRequest->wLength);
	}
	else
	{
		TDBG_FLAGGED(usbe, DEBUG_SETUP,"Host to Device Transfer of Length %d for ",pRequest->wLength);
	}
	switch (REQ_RECIP_MASK(pRequest->bmRequestType))
	{
		case REQ_INTERFACE:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"Interface Recepient");		
			break;
		case REQ_DEVICE:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"Device Recepient");		
			break;
		case REQ_ENDPOINT:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"EndPoint Recepient");		
			break;
		default:			
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"Unknown Recepient");		
			break;
	}		
	TDBG_FLAGGED(usbe, DEBUG_SETUP," For Request 0x%x with wValue 0x%x and wIndex 0x%x\n ",
				pRequest->bRequest,pRequest->wValue,pRequest->wIndex);

	
	/* Call Interface Specific Handler */
	RetVal = 1;
	ClassReqHandler = GetInterfaceReqHandler(DevNo,pRequest->wIndex);
	if (ClassReqHandler)
		RetVal =(*ClassReqHandler)(DevNo,pRequest->wIndex,pRequest);

	if (RetVal == 1)
	{
		pRequest->Status = 1;
		pRequest->Command = "UNSUPPORTED CLASS REQUEST";
	}
	else
	{
		pRequest->Status = 0;
		pRequest->Command = "CLASS REQUEST";
	}
}


static
void
ProcessGetStatus(uint8 DevNo,DEVICE_REQUEST *pRequest)
{

	uint8 *Status = pRequest->Data;
	uint8 Stall,Enable;
	

	TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessGetStatus(): Received for ReqType 0x%x and wIndex 0x%x\n",
							pRequest->bmRequestType,pRequest->wIndex);

	pRequest->Command = "GET_STATUS";

	/* Do Input Validation */	
	if ((pRequest->wValue != 0) || (pRequest->wLength != 2)||
 	   ((REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_DEVICE) && (pRequest->wIndex !=	0)))
	{ 	
		pRequest->Status = 1;
		return;
	}			
	
	/* Interface Status is always zero */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_INTERFACE)
		Status[0] = 0x00;

	/* Device Status : We are always self powered */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_DEVICE)
	{
		if (UsbGetRemoteWakeup(DevNo))
			Status[0] = 0x03;	/* Self Powered and Remote Wakeup */
		else
			Status[0] = 0x01;	/* Self Powered */			
	}		
		
	/* EndPoint Status: Set if halted or not */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_ENDPOINT)
	{
	
		/* Get the status of the endpoint */
		if (UsbGetEpStatus(DevNo,((uint8)pRequest->wIndex & 0x7F),
					((uint8)pRequest->wIndex & 0x80)?DIR_IN:DIR_OUT,&Enable,&Stall) == 0)
			Status[0] = (Stall)?0x01:0x00;
	}
	
	pRequest->Status = 0;
	return;
}

static
void
ProcessSetAddress(uint8 DevNo,DEVICE_REQUEST *pRequest)
{

	TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessSetAddress(): Received with Addr 0x%x\n",pRequest->wValue); 

	pRequest->Command = "SET ADDRESS";

	/* Do Input Validation */	
	if ((pRequest->wValue > 127)|| (pRequest->wIndex) || (pRequest->wLength))
	{
		pRequest->Status = 1;
		return;
	}		

	/* Set the Address */	
	UsbSetAddr(DevNo,pRequest->wValue & 0x7F,1); 		

	pRequest->Status = 0;
	return;
}

static
void
ProcessSetConfiguration(uint8 DevNo,DEVICE_REQUEST *pRequest)
{

	TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessSetConfiguration(): Received with wValue 0x%x\n",pRequest->wValue); 

	pRequest->Command = "SET_CONFIGURATION";

	/* Do Input Validation */	
	if ((pRequest->wIndex) || (pRequest->wLength) || (pRequest->wValue & 0xFF00))
	{
		pRequest->Status = 1;
		return;
	}		
	
	if (pRequest->wValue == 0)	 
		UsbUnCfgDevice(DevNo);
	else
		UsbCfgDevice(DevNo,pRequest->wValue);
		

	/* No Data Stage. So Complete Request Immediatly */
	pRequest->Status = 0;
	return;
}



static
void
ProcessGetDescriptor(uint8 DevNo,DEVICE_REQUEST *pRequest)
{

	uint8 *Buffer;
	uint16 Length;
	uint8 Index;
	
	Length = pRequest->wLength;
	Index  = pRequest->wValue & 0xFF;	

	pRequest->Command = "GET DESCRIPTOR";

	/*Special case of Get Descriptor: Call Interface specific Descriptors */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_INTERFACE)	
	{
		ProcessClassRequest(DevNo,pRequest);
		return;
	}

	pRequest->Status = 0;
	switch (pRequest->wValue >> 8)
	{
		case DESC_DEVICE:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessGetDescriptor(DEVICE): Requested for Length %d\n",pRequest->wLength);
			Buffer = (uint8*) GetDeviceDesc(DevNo);
			Length = sizeof(USB_DEVICE_DESC);
			if (pRequest->wLength < Length)
				Length = pRequest->wLength;
			break;
					
		case DESC_DEVICE_QUALIFIER:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessGetDescriptor(DEVICE_QUQLIFIER): Requested for Length %d\n",pRequest->wLength);
			Buffer = (uint8*) GetDeviceQualifierDesc(DevNo);
			Length = sizeof(USB_DEVICE_QUALIFIER_DESC);
			if (pRequest->wLength < Length)
				Length = pRequest->wLength;
			break;
			
		case DESC_CONFIGURATION:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessGetDescriptor(CONFIG): Requested for Length %d Index %d\n",pRequest->wLength, Index);
			/* Why is the host sending CfgIndex as 0? Doesn't CfgIndex starts from 1 ?*/ /*????*/
			Index = Index+1;
			Length = GetConfigDescSize(DevNo,Index);		
			Buffer = (uint8 *)GetConfigDesc(DevNo,Index);
			if (pRequest->wLength < Length)
				Length = pRequest->wLength;
			break;
		
		case DESC_STRING:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessGetDescriptor(STRING): For Length %d LangId 0x%x and Index 0x%x\n",
					pRequest->wLength,pRequest->wIndex,Index);

			Buffer=GetStringDescriptor(DevNo,Index,&Length); 
			if (pRequest->wLength < Length)
				Length = pRequest->wLength;
			
			break;
			
		default:
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessGetDescriptor((0x%x) : Unsupported\n",pRequest->wValue >> 8);			
			pRequest->Status = 1;					
			return;
			
	}
	memcpy(pRequest->Data, Buffer, Length);
	pRequest->wLength = Length;
	return;
}


static
void
ProcessGetConfiguration(uint8 DevNo,DEVICE_REQUEST *pRequest)
{

	TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessGetConfiguration(): Called\n");

	pRequest->Command = "GET_CONFIGURATION";

	/* Do Input Validation */	
	if ((pRequest->wIndex) || (pRequest->wLength!=1) || (pRequest->wValue))
	{
		pRequest->Status = 1;
		return;
	}		

	pRequest->Status = 0;
	pRequest->Data[0] = UsbGetCfgIndex(DevNo);

	return;
}


void
ProcessClearFeature(uint8 DevNo,DEVICE_REQUEST *pRequest)
{
	
	uint8 Stall,Enable;

	TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessClearFeature(): Called\n");
	pRequest->Command = "CLEAR_FEATURE";
	
	/* Do Input Validation */	
	if (pRequest->wLength)
	{
		pRequest->Status = 1;
		return;
	}		
	
	/* Clear Feature for Interface */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_INTERFACE)
	{
		if (pRequest->wValue)
		{
			pRequest->Command = "CLEAR_FEATURE for INTERFACE";
			pRequest->Status = 1;
			return;
		}	
	}

	/* Clear Feature for Device */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_DEVICE)
	{
		if (pRequest->wValue == DEVICE_REMOTE_WAKEUP)
			UsbSetRemoteWakeup(DevNo,0);				
		else
		{
			pRequest->Command = "CLEAR_FEATURE for DEVICE";
			pRequest->Status = 1;
			return;
		}			
	}			

	/* Clear Feature for EndPoint */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_ENDPOINT)
	{	
		if (pRequest->wValue == ENDPOINT_HALT)
		{
			
			if (UsbGetEpStatus(DevNo,((uint8)pRequest->wIndex & 0x7F),
					((uint8)pRequest->wIndex & 0x80)?DIR_IN:DIR_OUT,&Enable,&Stall) == 0)
			{	
				//printk("Status :Ep %d En %d St %d \n", ((uint8)pRequest->wIndex & 0x7F) , Enable , Stall); 
				if(Stall == 0x00) 
				{	
					/* Some OS (RHEL4.4) directly issues UnStall with no Stall before */
					UsbStallEp(DevNo,(uint8)pRequest->wIndex & 0x7F,
					((uint8)pRequest->wIndex & 0x80)?DIR_IN:DIR_OUT);
					UsbUnStallEp(DevNo,(uint8)pRequest->wIndex & 0x7F,
						((uint8)pRequest->wIndex & 0x80)?DIR_IN:DIR_OUT);		
				}
				else if(Stall == 0x01)
					UsbUnStallEp(DevNo,(uint8)pRequest->wIndex & 0x7F,
							((uint8)pRequest->wIndex & 0x80)?DIR_IN:DIR_OUT);		

			}
			
		}	
		else
		{
			pRequest->Command = "CLEAR_FEATURE for ENDPOINT";
			pRequest->Status = 1;
			return;
		}			
	}
	
	/* No Data Stage. So Complete Request Immediatly */
	pRequest->Status = 0;
	return;

}


void
ProcessSetFeature(uint8 DevNo,DEVICE_REQUEST *pRequest)
{
	

	TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessSetFeature(): Called\n");

	pRequest->Command = "SET_FEATURE";

	/* Do Input Validation */	
	if (pRequest->wLength)
	{
		pRequest->Status = 1;
		return;
	}		
	
	/* Set Feature for Interface */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_INTERFACE)
	{
		if (pRequest->wValue)
		{
			pRequest->Command = "SET_FEATURE for INTERFACE";
			pRequest->Status = 1;
			return;
		}	
	}

	/* Set Feature for Device */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_DEVICE)
	{
		if (pRequest->wValue == DEVICE_REMOTE_WAKEUP)
			UsbSetRemoteWakeup(DevNo,1);		
		else
		{
			
			pRequest->Command = "SET_FEATURE for DEVICE";
			pRequest->Status = 1;
			return;
		}			
	}			

	/* Set Feature for EndPoint */
	if (REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_ENDPOINT)
	{	
		if (pRequest->wValue == ENDPOINT_HALT)
		{
			UsbStallEp(DevNo,(uint8)pRequest->wIndex & 0x7F,((uint8)pRequest->wIndex & 0x80)?DIR_IN:DIR_OUT);		
		}	
		else
		{
			pRequest->Command = "SET_FEATURE for ENDPOINT";
			pRequest->Status = 1;
			return;
		}			
	}
	pRequest->Status = 0;
	return;

}


	

static
void
ProcessStandardRequest(uint8 DevNo,DEVICE_REQUEST *pRequest)
{

	
	switch (pRequest->bRequest)
	{
		case GET_STATUS:
			ProcessGetStatus(DevNo,pRequest);
			break;
	
		case SET_ADDRESS:
			ProcessSetAddress(DevNo,pRequest);
			break;
			
		case GET_DESCRIPTOR:
			ProcessGetDescriptor(DevNo,pRequest);
			break;

		case SET_CONFIGURATION:
			ProcessSetConfiguration(DevNo,pRequest);
			break;
			
		case GET_CONFIGURATION:
			ProcessGetConfiguration(DevNo,pRequest);
			break;
			
		case CLEAR_FEATURE:
			ProcessClearFeature(DevNo,pRequest);
			break;
			
		case SET_FEATURE:
			ProcessSetFeature(DevNo,pRequest);			
			break;

/*
		Currently Iam not supporting the following four standard requests.
		All these requests fall thru the default case, which stall the endpoint
		by calling Request Error.
	
		case SYNCH_FRAME:
			ProcessSyncFrame(DevNo,pRequest);
			break;
			
		case SET_DESCRIPTOR:
			ProcessSetDescriptor(DevNo,pRequest);
			break;

		case GET_INTERFACE:
			ProcessGetConfiguration(DevNo,pRequest);
			break;

		case SET_INTERFACE:
			ProcessSetConfiguration(DevNo,pRequest);
			break;
*/			
			
		default:
			pRequest->Command = "STANDARD_REQUEST(UNSUPPORTED)";
			pRequest->Status = 1;
			printk("ProcessStandardRequest(): WARNING : Unsupported Request. EP Stalled\n"); 
			printk("ProcessStandardRequest(): WARNING : bmRequestType: 0x%x bmRequest: 0x%x\n",
									pRequest->bmRequestType,pRequest->bRequest);

			break;
	}

	return;
}


void
ProcessSetup(uint8 DevNo,DEVICE_REQUEST *pRequest)
{
	
	switch (REQ_TYPE_MASK(pRequest->bmRequestType)
	{
		case REQ_STANDARD:	
			ProcessStandardRequest(DevNo,pRequest);
			break;
		case REQ_CLASS:
			ProcessClassRequest(DevNo,pRequest);
			break;	
		case REQ_VENDOR:
			ProcessVendorRequest(DevNo,pRequest);
			break;	
		default:
			pRequest->Command = "DEVICE REQUEST(UNKNOWN TYPE)";
			pRequest->Status = 1;
			TDBG_FLAGGED(usbe, DEBUG_SETUP,"ProcessSetup(): ERROR: Unknown Request for Device No %d,bmRequestType: 0x%x bmRequest: 0x%x\n",
									DevNo, pRequest->bmRequestType,pRequest->bRequest);

			break;									
	}
	
	return;
}

