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
/****************************************************************
  Author	: Samvinesh Christopher

  Module	: NCSI Communication Layer 

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/netdevice.h>
#include <asm/byteorder.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/wait.h>
#include "ncsi.h"
#include "interfaces.h"
#include "datadisp.h"

extern int verbose;

DECLARE_WAIT_QUEUE_HEAD(NCSI_Response);

void
ProcessNCSI(NCSI_IF_INFO *info, struct sk_buff *skb)
{
	NCSI_HDR *hdr;

	/* If it is a AEN return */
	if (CheckIfAEN(info, skb))
		return;
	
	info->RecvLen = skb->len;
	memcpy(info->RecvData, skb_network_header(skb), info->RecvLen);
	
	if (verbose & DUMP_BUFFER)
		DumpContents(0,(char *)info->RecvData,info->RecvLen,FORMAT_BYTE,NULL);


	/* Process NCSI Packet */
	hdr = (NCSI_HDR *)info->RecvData;

	if ((0x80|info->LastCommand) != hdr->Command)
	{
		if (info->LastCommand != hdr->Command)	// Sometimes Controller misbehaves and send the packet with same packet as response. 
			printk(KERN_DEBUG "NCSI(%s): Command: Expected 0x%x Got 0x%x\n",info->dev->name,
					info->LastCommand|0x80,hdr->Command);
		return;
	}

	if (info->LastInstanceID  != hdr->I_ID)
	{
		printk(KERN_DEBUG "NCSI(%s): Instance ID : Expected 0x%x Got 0x%x\n",info->dev->name,
							info->LastInstanceID,hdr->I_ID);
		return;
	}

	if (info->LastChannelID  != hdr->CH_ID)
	{
		printk(KERN_DEBUG "NCSI(%s): Channel ID : Expected 0x%x Got 0x%x\n",info->dev->name,
							info->LastChannelID,hdr->CH_ID);
		return;
	}
	if (info->LastManagementID  != hdr->MC_ID )
	{
		printk(KERN_DEBUG "NCSI(%s): MC ID : Expected 0x%x Got 0x%x\n",info->dev->name,
							info->LastManagementID,hdr->MC_ID);
		return;
	}

	/* Wakeup Sender */
	info->Timeout = 0;
	wake_up_interruptible(&NCSI_Response);

	return;
}

int 
SendNCSICommand (NCSI_IF_INFO *info)
{
	int i;
	struct sk_buff *skb;

	for (i = 0; i < NCSI_RETRIES; i++)
	{
		if (verbose & SHOW_SEND_COMMANDS)
			printk("NCSI(%s): Sending Request (0x%x) to 0x%x...\n",info->dev->name,
					info->LastCommand,info->LastChannelID);

		if (verbose & DUMP_BUFFER)
			DumpContents(0,info->SendData+sizeof(ETH_HDR),info->SendLen-sizeof(ETH_HDR),FORMAT_BYTE,NULL);


		skb = alloc_skb(info->SendLen + LL_RESERVED_SPACE(info->dev),GFP_ATOMIC);
		if (skb == NULL)
		{
			printk("NCSI(%s): ERROR Unable to allocate skb\n",info->dev->name);
			return NCSI_ERR_NOMEM;
		}
		skb_reserve(skb,LL_RESERVED_SPACE(info->dev));
		skb_put(skb,info->SendLen);
		memcpy(skb->data,info->SendData,info->SendLen);
		skb->dev = info->dev;
		skb->protocol = htons(ETH_P_NCSI);
		info->Timeout = 1;
		if (dev_queue_xmit(skb) < 0)
		{
			printk("NCSI(%s): ERROR Unable to send skb\n",info->dev->name);
			return NCSI_ERR_NETWORK;
		}

		/* Sleep for response or timeout */
		wait_event_interruptible_timeout(NCSI_Response, (info->Timeout == 0),NCSI_RESP_TIMEOUT);

		/* If time out */
		if (info->Timeout)
		{
			/* Some controller needs delay before retry a command. */
			msleep(NCSI_RETRY_TIMEOUT);
			if (verbose & SHOW_RETRY_MESSAGES)
				printk ("NCSI:Receive Timeout out\n");
			if (verbose & SHOW_RETRY_MESSAGES)
				printk ("NCSI: Resending Command (Retry = %d)...\n",i+1);
		}
		else
		{
			if (verbose & SHOW_SUCCESS_COMMANDS)
			{
				printk ("NCSI(%s): Request (0x%x) to 0x%x Success\n",info->dev->name,
					info->LastCommand,info->LastChannelID);
			}
			return NCSI_ERR_SUCCESS;
		}
	}

	return NCSI_ERR_TIMEOUT;
}

	
