/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
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

  Module	: NCSI Interface Structures and Functions

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include "ncsi.h"
#include "interfaces.h"

NCSI_IF_INFO *ncsi_if_head = NULL;
DECLARE_MUTEX(ncsi_if_lock);

NCSI_IF_INFO *
AddNetInterface(struct net_device *dev)
{
	NCSI_IF_INFO *info;

	/* Check if valid decice */
	if (dev == NULL)
	{
		printk("NCSI: WARNING: Cannot add NULL interface\n");
		return NULL;
	}

	down(&ncsi_if_lock);

	/* Allocate Memory for Interface struct */
	info = kmalloc(sizeof(NCSI_IF_INFO),GFP_KERNEL);
	if (info == NULL)
	{
		printk("NCSI:ERROR: Unable to allocate memory for %s\n",dev->name);
		up(&ncsi_if_lock);
		return NULL;
	}
	memset(info,0,sizeof(NCSI_IF_INFO));
	info->vlanID=0;
	info->dev = dev;
	info->AutoSelect = 1;
	info->LinkStatus = 0; 
	info->LastCommand = 0x0F;	/* Invalid Command (0x0F Not defined in specs)*/
	INIT_WORK(&info->detect_work,NCSI_Detect);
	INIT_WORK(&info->enable_work,NCSI_Enable);
	INIT_WORK(&info->disable_work,NCSI_Disable);
	
	/* Because the ethernet header is 14 bytes,the NCSI data are not aligned at 32 bit
           boundaries, givin alignment errors. So pad 2 dummy bytes before the data to 
	   make it aligned */
	info->SendData = &info->SendBuffer[2]; 
	info->RecvData = &info->RecvBuffer[2]; 	
	
	if (ncsi_if_head == NULL)
		info->next = NULL;	
	else
		info->next = ncsi_if_head;
	ncsi_if_head = info;
	up(&ncsi_if_lock);
	return info;
}

int
RemoveNetInterface(struct net_device *dev)
{
	NCSI_IF_INFO *info,*prev;

	/* Check if valid decice */
	if (dev == NULL)
	{
		printk("NCSI: WARNING: Cannot remove NULL interface\n");
		return 1;
	}

	down(&ncsi_if_lock);
	if (ncsi_if_head == NULL)
	{
	    printk("NCSI: ERROR: (List is Empty) Cannot delete Interface %s\n",dev->name);
		up(&ncsi_if_lock);
		return 1;
	}

	info = ncsi_if_head;
	prev = NULL;
	while (info)
	{
		if (info->dev == dev)
			break;
		prev = info;
		info=info->next;
	}
	if (prev == NULL)
		ncsi_if_head = info->next;
	else
		prev->next = info->next;

	up(&ncsi_if_lock);

	if (info)
	{
		NCSI_Net_Driver_DeRegister(info);
		kfree(info);
	}
	else
	    printk("NCSI: ERROR: Unable to delete Interface %s\n",dev->name);
	return 0;
}

void
RemoveAllNetInterfaces(void)
{
	NCSI_IF_INFO *info,*cur;

	down(&ncsi_if_lock);
	info = ncsi_if_head;
	cur = NULL;
	while (info)
	{
		NCSI_Net_Driver_DeRegister(info);
		cur= info;
		info=info->next;
		kfree(cur);
	}
	up(&ncsi_if_lock);
	return;
}

NCSI_IF_INFO *
GetInterfaceInfo(struct net_device *dev)
{
	NCSI_IF_INFO *info;

	down(&ncsi_if_lock);
	info = ncsi_if_head;
	while (info)
	{
		if (info->dev == dev)
			break;
		info=info->next;
	}
	up(&ncsi_if_lock);
	return info;
}

NCSI_IF_INFO *
GetInterfaceInfoByName(char *interface)
{
	NCSI_IF_INFO *info;

	down(&ncsi_if_lock);
	info = ncsi_if_head;
	while (info)
	{
		if (strcmp(info->dev->name,interface) == 0)
			break;
		info=info->next;
	}
	up(&ncsi_if_lock);
	return info;
}

