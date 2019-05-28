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
#include <coreTypes.h>
#include <linux/wait.h>
#include "descriptors.h"
#include "hid.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"
#include "pipe.h"

#define MAX_ENDPOINT_SIZE 64

#define DATA_QUEUE_SIZE 16

typedef struct
{
	uint8 		Entry     [DATA_QUEUE_SIZE][MAX_PIPE_DATA_SIZE];
	uint32 		EntrySize [DATA_QUEUE_SIZE];
	int 		WrIndex;
	int 		RdIndex;
	int 		Count;
	spinlock_t	Lock;
	wait_queue_head_t Wait;
	
} PIPE_QUEUE_T;

static PIPE_QUEUE_T PipeQueue;
static int PipeInit=0;
static int PipeDevNo=-1;
static int PipeIfNum=-1;


/************************ Pipe report Descriptor **************************/


static uint8 PipeReport[] =
{
	/* Vendor Specific 0xFF00 - Non standard HID device */
	USAGE_PAGE2(0xFF00) >> 16,
	(uint8)(USAGE_PAGE2(0xFF00) >> 8), 
	USAGE_PAGE2(0xFF00) & 0xFF,
	/* Vendor Specific 0x100 - Non standard HID device */
	USAGE2(0x0100) >> 16,
	(uint8)(USAGE2(0x0100) >> 8),
	USAGE2(0x0100) & 0xFF,
	COLLECTION1(APPLICATION) >> 8,
	COLLECTION1(APPLICATION) & 0xFF,

	/* Data are 8 bits wide and value range from 0 to 255 */
 	REPORT_SIZE1(8) >> 8,
 	REPORT_SIZE1(8) & 0xFF,
 	LOGICAL_MINIMUM1(0) >> 8,
 	LOGICAL_MINIMUM1(0) & 0xFF,
 	LOGICAL_MAXIMUM1(255) >> 8,
 	LOGICAL_MAXIMUM1(255) & 0xFF,

 	REPORT_COUNT1(64) >> 8,
	REPORT_COUNT1(64) & 0xFF,
	/* Vendor Specific 0x100 - Non standard HID device */
	USAGE2(0x0100) >> 16,
	(uint8)(USAGE2(0x0100) >> 8),
	USAGE2(0x0100) & 0xFF,
	INPUT1(VARIABLE) >> 8,
	INPUT1(VARIABLE) & 0xFF,

 	REPORT_COUNT1(64) >> 8,
 	REPORT_COUNT1(64) & 0xFF,
	/* Vendor Specific 0x100 - Non standard HID device */
	USAGE2(0x0100) >> 16,
	(uint8)(USAGE2(0x0100) >> 8),
	USAGE2(0x0100) & 0xFF,
 	OUTPUT1(VARIABLE) >> 8,
 	OUTPUT1(VARIABLE) & 0xFF,
	END_COLLECTION
};


uint16
GetPipeReportSize(void)
{
	return sizeof(PipeReport);
}


static USB_HID_DESC *	   
GetHidDesc(uint8 DevNo,uint8 CfgIndex, uint8 IntIndex)
{
	USB_INTERFACE_DESC *IfaceDesc;
	uint8 *Buffer;

	IfaceDesc = UsbCore.CoreUsbGetInterfaceDesc (DevNo, CfgIndex, IntIndex);
	if (IfaceDesc == NULL)
		return NULL;
	
	Buffer = (uint8*) IfaceDesc;
	Buffer += sizeof (USB_INTERFACE_DESC);
	if (Buffer [1] == HID_DESC)
			return ((USB_HID_DESC *)Buffer);
	else
			return NULL;

}


int
PipeReqHandler(uint8 DevNo, uint8 ifnum,DEVICE_REQUEST *pRequest)
{
	HID_IF_DATA *PipeData;
	USB_HID_DESC *PipeHidDesc;	
	uint8 CfgIndex;
	int RetVal = 0;	
	
	TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): Called! for ifnum =%d\n", ifnum);

	pRequest->Command = "PIPE REQUEST";
	if (REQ_RECIP_MASK(pRequest->bmRequestType) != REQ_INTERFACE)
		return 1;
		
	PipeData =(HID_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(DevNo,ifnum);
	if (PipeData == NULL)
		return 1;

	switch(pRequest->bRequest) 
	{
		case GET_REPORT:
			/* Save the DevNum and Interface Number */
			if (PipeDevNo < 0)
			{
				PipeDevNo=DevNo;
				PipeIfNum=ifnum;
			}
			/* First Time. Init Pipe Data Structures */
			if (!PipeInit)
			{
				PipeQueue.WrIndex = 0;
				PipeQueue.RdIndex = 0;
				PipeQueue.WrIndex = 0;
				PipeQueue.Count = 0;
				spin_lock_init (&PipeQueue.Lock);
				init_waitqueue_head(&PipeQueue.Wait);
				PipeInit = 1;
			}

			if(pRequest->wLength > sizeof(PipeData->ReportReq))
				pRequest->wLength = sizeof(PipeData->ReportReq);
			memcpy(pRequest->Data, &(PipeData->ReportReq), pRequest->wLength);
			break;
			
		case GET_IDLE: 
			TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): GET_IDLE request\n");
			if(pRequest->wLength > sizeof(PipeData->Idle))
					pRequest->wLength = sizeof(PipeData->Idle);
				memcpy(pRequest->Data, &(PipeData->Idle), pRequest->wLength);
			break;
			
		case GET_PROTOCOL:
			TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): GET_PROTOCOL request\n");
			if(pRequest->wLength > sizeof(PipeData->Protocol))
				pRequest->wLength = sizeof(PipeData->Protocol);
			memcpy(pRequest->Data, &(PipeData->Protocol), pRequest->wLength);
			break;
		case SET_REPORT:
			TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): SET_REPORT request \n");
			printk("WARNING:PipeReqHandler(): SET_REPORT request . What data is it ?\n");
			break;
			
		case SET_IDLE:
			TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): SET_IDLE request with Value 0x%x\n",(pRequest->wValue >> 8));			
			PipeData->Idle = (uint8)(pRequest->wValue >> 8);
			break;
			
		case SET_PROTOCOL:
			TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): SET_PROTOCOL request\n");
			PipeData->Protocol = (uint8) pRequest->wValue;
			break;
			
		case GET_DESCRIPTOR:
			switch (pRequest->wValue >> 8)
			{
				case HID_DESC:
					TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): Received GET_DESCRIPTOR(HID_DESC)\n");
					CfgIndex = UsbCore.CoreUsbDevGetCfgIndex (DevNo);
					PipeHidDesc = GetHidDesc(DevNo,CfgIndex,ifnum);					
					if(pRequest->wLength > sizeof(USB_HID_DESC))
						pRequest->wLength = sizeof(USB_HID_DESC);
					memcpy(pRequest->Data, (uint8 *)PipeHidDesc, pRequest->wLength);
					break;
					
				case REPORT_DESC:
					TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeReqHandler(): Received GET_DESCRIPTOR(REPORT_DESC)\n");
					if (pRequest->wLength > sizeof(PipeReport))
						pRequest->wLength = sizeof(PipeReport);
					memcpy(pRequest->Data, &PipeReport[0], pRequest->wLength);
					break;
					
				case PHYSICAL_DESC:
					TWARN("PipeReqHandler(): Received GET_DESCRIPTOR(PHYSICAL_DESC). Not Supported!!!\n");
					/* not supported */
					RetVal =1;
					break;
					
				default:
					TWARN("PipeReqHandler(): Received GET_DESCRIPTOR(unknown). Not Supported!!!\n");
					/* not supported */
					RetVal =1;
					break;
			}
			break;
		default:
			TWARN("PipeReqHandler(): Unknown CLASS request. Not Supported!!!\n");
			/* not supported */
			RetVal =1;
			break;
	}
	
	return RetVal;
}

static void
ClearPipeDataQueue (void)
{
	unsigned long Flags;

	spin_lock_irqsave (&PipeQueue.Lock, Flags);
	PipeInit=0;
	PipeQueue.RdIndex = 0;
	PipeQueue.WrIndex = 0;
	PipeQueue.Count = 0;
	spin_unlock_irqrestore (&PipeQueue.Lock, Flags);
}


void
PipeRemHandler(uint8 DevNo,uint8 ifnum)
{
	HID_IF_DATA *IntData;
	
	TDBG_FLAGGED(hid, DEBUG_PIPE,"PipeRemHandler(): Disconnect Handler Called\n");

	IntData =(HID_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(DevNo,ifnum);
	if (IntData == NULL)
		return;	

	IntData->Idle 	  = 0;
	IntData->Protocol = 1;
	IntData->ReportReq = 0;

	ClearPipeDataQueue ();
	PipeDevNo=-1;
	PipeIfNum=-1;
	return;
}


void
PipeTxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum)
{
	TDBG_FLAGGED(hid,DEBUG_PIPE,"PipeTxHandler Completion Called on %d:%d:%d\n",DevNo,IfNum,EpNum);
}


void
PipeRxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum)
{
	uint32 Len;	
	uint8 *RxData;
	unsigned long Flags;
	int WriteIndex;
	uint32 BufIndex;
	uint32 Remain;

	RxData  = UsbGetRxData(DevNo,EpNum);
        Len     = UsbGetRxDataLen(DevNo,EpNum);
//	TDBG_FLAGGED(hid,DEBUG_PIPE,"PipeRXHandler Called on %d:%d:%d for Data Len %d\n",DevNo,IfNum,EpNum,Len);


	/* Add to Queue if space available */
	spin_lock_irqsave (&PipeQueue.Lock, Flags);
	if (PipeQueue.Count == DATA_QUEUE_SIZE)
	{
		printk("WARNING: PipeRxhandler(). No Space to save data\n");
		spin_unlock_irqrestore (&PipeQueue.Lock, Flags);
		return;
	}
	
	WriteIndex = PipeQueue.WrIndex;

	/* Check if the Buffer has enough space */
	BufIndex= PipeQueue.EntrySize[WriteIndex];
	Remain = MAX_PIPE_DATA_SIZE-BufIndex;
	if (Len > Remain)
	{
		printk("WARNING: PipeRxhandler(). More data than the buffer capacity. Data will be lost\n");
		Len = Remain;
	}

	/* Write the data and length */
	PipeQueue.EntrySize[WriteIndex] += Len;
	if (Len != 0)
		memcpy ((void*)&(PipeQueue.Entry[WriteIndex][BufIndex]), RxData,Len);

	/* If end of transfer */
	if ((Len == 0) || (Len  < MAX_ENDPOINT_SIZE))
	{
		TDBG_FLAGGED(hid,DEBUG_PIPE,"PipeRXHandler Received on %d:%d:%d for Data with Len %d\n",DevNo,IfNum,EpNum,PipeQueue.EntrySize[WriteIndex]);
		PipeQueue.WrIndex = (WriteIndex + 1) % DATA_QUEUE_SIZE;
		PipeQueue.Count++;
		wake_up_interruptible(&PipeQueue.Wait);
	}
	spin_unlock_irqrestore (&PipeQueue.Lock, Flags);
	return;

}


static
int
RecvPipeDataFromHost(uint8 *Buff)
{
	int len=-1;
	unsigned long Flags;
	int ReadIndex;
	TDBG_FLAGGED(hid,DEBUG_PIPE,"RecvPipeData() Called \n");

	if (!PipeInit)
	{	
		printk("ERROR: Calling RecvPipeData() without Pipe Initialized\n");
		return -1;
	}
retryRead:
	spin_lock_irqsave (&PipeQueue.Lock, Flags);
	if (PipeQueue.Count)
	{

		ReadIndex = PipeQueue.RdIndex;
		len= PipeQueue.EntrySize[ReadIndex];
		if (len >0)
			memcpy(Buff,(void*)PipeQueue.Entry[ReadIndex],len);
		else
		   len=-1;
		
		PipeQueue.EntrySize[ReadIndex] = 0;	/* Reset Lenght which is used as Buffer Index in write */
		PipeQueue.RdIndex = (ReadIndex + 1) % DATA_QUEUE_SIZE;
		PipeQueue.Count--;
		spin_unlock_irqrestore (&PipeQueue.Lock, Flags);
	}
	else
	{
		spin_unlock_irqrestore (&PipeQueue.Lock, Flags);
		if (wait_event_interruptible(PipeQueue.Wait ,PipeQueue.Count) < 0)
			return -1;
		goto retryRead;
	}
	if (len <= 0 )
		goto retryRead;
	TDBG_FLAGGED(hid,DEBUG_PIPE,"RecvPipeData(): Returning %d Bytes of Pipe data from host\n",len);
	return len;
}



static
int
SendPipeDataToHost(uint8 DevNo,uint8 IfNum,uint8 *Buff,uint32 Len)
{
   	uint8 epnum;
	int   Ret;
	if (PipeDevNo < 0)
	{
		printk("ERROR: Trying to send data to Host via unconfigured Pipe !!");
		return -1;
	}
	DevNo=PipeDevNo;
	IfNum=PipeIfNum;

	/* Get In Endpoint of Pipe */
	epnum = UsbCore.CoreUsbGetDataInEp(DevNo,IfNum);

	/* Send Pipe Data */
	TDBG_FLAGGED(hid, DEBUG_PIPE,"Sending %d Bytes of Pipe data to Host via Device %d:%d:%d\n",Len,DevNo,IfNum,epnum);

	Ret = UsbCore.CoreUsbWriteData(DevNo,epnum,Buff,Len,ZERO_LEN_PKT);
	if (Ret != 0)
		printk("ERROR: SendPipeData(): Failed to Transmit Data (timed out or interrupted)\n");

	return Ret;
}


static char LocalBuffer[MAX_PIPE_DATA_SIZE];
ssize_t  
PipeRead(struct file * file , char * buf, size_t count, loff_t *ppos)
{
	int  len;
	size_t remain = count;

	while (remain > 0)
	{
		len = RecvPipeDataFromHost(LocalBuffer);
		if (len < 0)
			break;
		len = (len > remain)?remain:len;
		if (copy_to_user(buf,LocalBuffer,len))
			return -EFAULT;
		buf+=len;
		remain-=len;
	}
	return count-remain;
}

ssize_t 
PipeWrite(struct file * file , const char __user *buf, size_t count, loff_t *ppos)
{

	char *LocalBuffer;

	LocalBuffer= (char *)kmalloc(count,GFP_ATOMIC);
	if (LocalBuffer == NULL)
		return -ENOMEM;

	if (copy_from_user(LocalBuffer,buf, count)) 
	{
		kfree(LocalBuffer);
		return -EFAULT;
	}
	if (SendPipeDataToHost (0,0,LocalBuffer,count) < 0)
	{
		kfree(LocalBuffer);
		return -EFAULT;
	}
	
	return count;
}


/* Note : Limit read and write sizes to max of 4K. Otherwise, host app and usb xmit timeout 
	  happens. If bigger size needed, you need to tweak these sizes */







