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

  Module	: NCSI Detect and Enable Modules
			
  Revision	: 1.0  

  Changelog : 1.0 - Initial Version [SC]

*****************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <asm/byteorder.h>
#include <linux/delay.h>
#include "ncsi.h"
#include "interfaces.h"

static
int
EnableChannel(NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
	UINT8   VlanMode;

	/* Issue Select Package with Hw Arbit Enable */
	if (NCSI_Issue_SelectPackage(info,PackageID,0) != 0)
	{
		printk("NCSI(%s):%d.%d Select Package (Hw Arbit Enable)Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
	{
		printk("NCSI(%s):%d.%d Clear Init State Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	/* Setup VLANID */
	if(info->vlanID) 	//This call is to bring up the VLAN H/W filetring
	{
		if (NCSI_Issue_SetVLANFilter(info,PackageID,ChannelID,info->vlanID,1) != 0)
		{
			printk("NCSI(%s):%d.%d Setting VLAN TAG %d Failed\n",info->dev->name,PackageID, ChannelID,info->vlanID);
		}
		VlanMode=VLAN_MODE_VLAN_ONLY;	//In VLAN mode - only VLAN packets allowed. VLAN filtering done here.
	}
	else
	{
		VlanMode=VLAN_MODE_ANY_VLAN_NON_VLAN;		//In VLAN mode - VLAN and NON_VLAN (LAN) packets allowed.
	}

	/* Enable VLANID filter or Cleanup VLAN filter */
	if (NCSI_Issue_EnableVLAN(info,PackageID,ChannelID,VlanMode) != 0)
	{
		printk("NCSI(%s):%d.%d VLAN filter enable failed\n",info->dev->name,PackageID, ChannelID);
	}

	/* Issue an Enable TX for the channel */
	if (NCSI_Issue_ChannelCommands(info,CMD_ENABLE_CHANNEL_TX,PackageID,ChannelID) != 0)
	{
		printk("NCSI(%s):%d.%d Enable Channel Tx Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}
		
	/* Enable the channel */
	if (NCSI_Issue_ChannelCommands(info,CMD_ENABLE_CHANNEL,PackageID,ChannelID) != 0)
	{
		printk("NCSI(%s):%d.%d Enable Channel Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	printk("NCSI(%s): Channel %d.%d Enabled\n",info->dev->name,PackageID, ChannelID);	
	return 0;
}

static
int
DisableChannel(NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
	/* Issue Select Package with Hw Arbit Enable */
	if (NCSI_Issue_SelectPackage(info,PackageID,0) != 0)
	{
		printk("NCSI(%s):%d.%d Select Package (Hw Arbit Enable)Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
	{
		printk("NCSI(%s):%d.%d Clear Init State Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	/* Issue an DISABLE TX for the channel */
	if (NCSI_Issue_ChannelCommands(info,CMD_DISABLE_CHANNEL_TX,PackageID,ChannelID) != 0)
	{
		printk("NCSI(%s):%d.%d Disable Channel Tx Failed\n",
					info->dev->name,PackageID, ChannelID);
		return 1;
	}
		
	/* Disable the channel */
	if (NCSI_Issue_DisableChannel(info,PackageID,ChannelID,0) != 0)
	{
		printk("NCSI(%s):%d.%d Disable Channel Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	printk("NCSI(%s): Channel %d.%d Disabled\n",info->dev->name,PackageID, ChannelID);	
	return 0;
}

void
NCSI_Detect(struct work_struct *data)
{
	NCSI_IF_INFO *info;

	//info = (NCSI_IF_INFO *)data;
	info = container_of(data, NCSI_IF_INFO, detect_work);
	NCSI_Detect_Info(info);
	return;
}

int
NCSI_Change_MacAddr(struct net_device *dev)
{
	UINT8 MACAddr[6];
	NCSI_IF_INFO *info;
	int index;
	int flags;

	/* Get the curent MAC Address of the device */
	/*NCSI Need in the reverse order */
	MACAddr[0] = dev->dev_addr[5];
	MACAddr[1] = dev->dev_addr[4];
	MACAddr[2] = dev->dev_addr[3];
	MACAddr[3] = dev->dev_addr[2];
	MACAddr[4] = dev->dev_addr[1];
	MACAddr[5] = dev->dev_addr[0];
	if ((MACAddr[0] == 0) && (MACAddr[1] == 0) && (MACAddr[2] == 0) &&
	    (MACAddr[3] == 0) && (MACAddr[4] == 0) && (MACAddr[5] == 0))
	{
		printk("NCSI(%s): Error! Mac Address is 0. Cannot enable NCSI\n",dev->name);
		return -1;
	}

	info = GetInterfaceInfo(dev);
	if (info == NULL)
		return -1;

	/* if the interface is not NCSI, return */
	if (info->TotalChannels == 0)
		return -1;

	printk("NCSI(%s): Changing MAC Addr to  %02X:%02X:%02X:%02X:%02X:%02X\n",dev->name,
		MACAddr[5],MACAddr[4],MACAddr[3],MACAddr[2],MACAddr[1],MACAddr[0]);

	if(rtnl_trylock() == 0)
	{	
		flags = dev->flags;
		if (!(flags & IFF_UP))
			dev_open(dev);
	}
	else
	{
		flags = dev->flags;
		if (!(flags & IFF_UP))	
			dev_open(dev);
	}

	
	for (index = 0; index < MAX_CHANNELS ; index ++)
	{
		if (info->ChannelInfo[index].Enabled == 0)
			continue;

		if (NCSI_Issue_SetMacAddress(info,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID,
					MACAddr,1,0) != NCSI_ERR_SUCCESS)
		{
			printk("NCSI(%s): ERROR: Change Mac Address(%d.%d) Failed \n",dev->name,info->ChannelInfo[index].PackageID,
								info->ChannelInfo[index].ChannelID);
		}
		else
		{
			printk("NCSI(%s): Change  Mac Address(%d.%d) Passed \n",dev->name,info->ChannelInfo[index].PackageID,
								info->ChannelInfo[index].ChannelID);
		}
	
	}

	if (!(flags & IFF_UP))	
		dev_close(dev);

	return 0;
}

void
NCSI_Detect_Info(NCSI_IF_INFO *info)
{
	struct net_device *dev;

	UINT8 PrevPackageID;
	UINT8 PackageID;
	UINT8 ChannelID;
	UINT8 MACAddr[6];
	int i;
	int ValidPackages =0;
	int TotalChannels =0;
	UINT32 Ver1,Ver2;
	UINT8 Major;
	int retval;

	dev  = info->dev;

	/* Get MAC Address to use */
	/*NCSI Need in the reverse order */
	//memcpy(&MACAddr[0],dev->dev_addr,6);
	MACAddr[0] = dev->dev_addr[5];
	MACAddr[1] = dev->dev_addr[4];
	MACAddr[2] = dev->dev_addr[3];
	MACAddr[3] = dev->dev_addr[2];
	MACAddr[4] = dev->dev_addr[1];
	MACAddr[5] = dev->dev_addr[0];
	printk("NCSI(%s): MAC Addr = %02X:%02X:%02X:%02X:%02X:%02X\n",dev->name,
		MACAddr[5],MACAddr[4],MACAddr[3],MACAddr[2],MACAddr[1],MACAddr[0]);
					
	if ((MACAddr[0] == 0) && (MACAddr[1] == 0) && (MACAddr[2] == 0) &&
	    (MACAddr[3] == 0) && (MACAddr[4] == 0) && (MACAddr[5] == 0))
	{
		printk("NCSI(%s): Error! Mac Address is 0. Cannot enable NCSI\n",dev->name);
		return;
	}

	/* Blindly deselect all  packages */
	for (PackageID = 0; PackageID < MAX_PACKAGE_ID; PackageID++)
		NCSI_Issue_DeSelectPackage(info,PackageID);

	/* Discover Packages and Channels */
	for (PackageID = 0; PackageID < MAX_PACKAGE_ID; PackageID++)
	{
		/* Issue Select Package with Hw Arbit Disable*/
		if (NCSI_Issue_SelectPackage(info,PackageID,1) != 0)
			continue;


		/* Find the number of channels support by this packages */
		for (ChannelID = 0; ChannelID < MAX_PACKAGE_ID; ChannelID++)
		{
			/* Issue Cleear Init State  for each channel */
			if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
				break;

			/* Get Version ID and verify it is > 1.0  */
			if (NCSI_Issue_GetVersionID(info,PackageID,ChannelID,&Ver1,&Ver2) != 0)
			{
				printk("NCSI(%s):%d.%d Get Version IDFailed\n",dev->name,PackageID, ChannelID);	
				continue;
			}
			Major = (Ver1 >> 24) & 0xFF;
			if ((Major & 0xF0)== 0xF0)
				Major = Major & 0x0F;
			if (Major < 1)
			{
				printk("NCSI(%s):%d.%d Version(0x%08lx) is < 1.0  Not supported\n",
						dev->name,PackageID, ChannelID,Ver1);
				continue;
			}

			/* Get Capabilities and set ArbitSupport */
			if (NCSI_Issue_GetCapabilities(info,PackageID,ChannelID, 
						&info->ChannelInfo[TotalChannels].Caps) != 0)
			{
				printk("NCSI(%s):%d.%d Get Capabilities Failed\n",dev->name,PackageID, ChannelID);	
				continue;
			}
			if (info->ChannelInfo[TotalChannels].Caps & HW_ARBITRATION_SUPPORT)
				info->ChannelInfo[TotalChannels].ArbitSupport = 1;
			else
				info->ChannelInfo[TotalChannels].ArbitSupport = 0;

			info->ChannelInfo[TotalChannels].PackageID = PackageID;
			info->ChannelInfo[TotalChannels].ChannelID = ChannelID;
			printk("NCSI(%s):Found NC-SI at Package:Channel (%d:%d)\n", dev->name,PackageID,ChannelID);
			TotalChannels++;
	
		}
		/* Deselect previusly selected package */
		NCSI_Issue_DeSelectPackage(info,PackageID);
	}

	if (TotalChannels == 0)
		printk("NCSI(%s): No NCSI Interfaces detected\n", dev->name);


	PrevPackageID = -1;
	/* Configure the detected channels */
	for(i=0;i<TotalChannels;i++)
	{
		PackageID = info->ChannelInfo[i].PackageID;
		ChannelID = info->ChannelInfo[i].ChannelID;

		/* Issue Select Package with Hw Arbit Disable*/
		if (NCSI_Issue_SelectPackage(info,PackageID,1) != 0)
		{
			printk("NCSI(%s):%d.%d Select Package Failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}

		/* Issue Cleear Init State to enter into init state  */
		if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
		{
			printk("NCSI(%s):%d.%d Clear Init State Failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}

		/* Issue a Reset Channel to clear all previous config */
		if (NCSI_Issue_ResetChannel(info,PackageID,ChannelID) != 0)
		{
			printk("NCSI(%s):%d.%d Reset Channel Failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}

		/* Issue Cleear Init State to enter into init state  */
		if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
		{
			printk("NCSI(%s):%d.%d Clear Init State Failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}

		/* Setup MAC Address */
		if (NCSI_Issue_SetMacAddress(info,PackageID,ChannelID,MACAddr,1,0) != 0)
		{
			printk("NCSI(%s):%d.%d Set Mac Address Failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}

#if 0
		/* Setup VLANID */
		printk("NCSI(%s):%d.%d Setting VLAN in NCSI with VLAN TAG %d ...\n",dev->name,PackageID,ChannelID,CFG_PROJ_VLAN_ID);
		if (NCSI_Issue_SetVLANFilter(info,PackageID,ChannelID,CFG_PROJ_VLAN_ID,1) != 0)
		{
			printk("NCSI(%s):%d.%d Setting VLAN TAG %d Failed\n",dev->name,PackageID, ChannelID,CFG_PROJ_VLAN_ID);
			continue;
		}
		
		/* Enable VLANID filter */
		if (NCSI_Issue_EnableVLAN(info,PackageID,ChannelID,VLAN_MODE_ANY_VLAN_NON_VLAN) != 0)
		{
			printk("NCSI(%s):%d.%d VLAN filter enable failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}
		printk("NCSI(%s):%d.%d Setting VLAN in NCSI is completed with VLAN TAG %d\n",dev->name,PackageID,ChannelID,CFG_PROJ_VLAN_ID);
#endif

		/* Enable Broaccast filter */
		if (NCSI_Issue_EnableBcastFilter(info,PackageID,ChannelID,1,1,1,1) != 0)
		{
			printk("NCSI(%s):%d.%d Enable Bcast Filter Failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}
		/* Setup AEN Messages */
		if (NCSI_Issue_EnableAEN(info,(UINT8)PackageID,(UINT8)ChannelID,1,1,1) != 0)
		{
			printk("NCSI(%s):%d.%d Enable AEN Failed\n",dev->name,PackageID, ChannelID);	
			continue;
		}

		/* Enable Auto Negotiation */
		retval = NCSI_Issue_SetLink(info,(UINT8)PackageID,(UINT8)ChannelID,1,0,0);
		if (retval == 0) 
		{	
			/* Wait for one second for Set Link to complete */
			mdelay(1000);
		}
		else
		{
			if (retval == NCSI_ERR_CONFLICT)
				printk("NCSI(%s): %d.%d Set Link Conflict\n",dev->name,PackageID, ChannelID);
			else
			{
				printk("NCSI(%s): %d.%d Set Link Failed\n",dev->name,PackageID, ChannelID);
				continue;
			}
		}

		
/* Set this channel info is setup */ info->ChannelInfo[i].Valid = 1;

		/* Count the number of valid packages*/
		if (PrevPackageID != PackageID)
		{
			ValidPackages++;
			PrevPackageID = PackageID;
		}

		/* Deselect previusly selected package */
		NCSI_Issue_DeSelectPackage(info,PackageID);
	}

	if (ValidPackages > 1)
	{
		printk("NCSI(%s):Multiple Packages Found\n",dev->name);
		for(i=0;i<TotalChannels;i++)
		{
			if (info->ChannelInfo[i].Valid == 0)
				continue;
			if (info->ChannelInfo[i].ArbitSupport == 0)
			{
				PackageID = info->ChannelInfo[i].PackageID;
				ChannelID = info->ChannelInfo[i].ChannelID;
				printk("NCSI(%s):WARNING: Channel (%d:%d) does not support Arbitration\n",
							dev->name,PackageID,ChannelID);
			}	
		}
	}
	info->TotalChannels = TotalChannels;

	if ((TotalChannels) && (ValidPackages))
		 NCSI_Net_Driver_Register(info);

	return;
}

static
int
NCSI_Enable_Ex(struct work_struct *data)
{
	NCSI_IF_INFO *info;
	//info = (NCSI_IF_INFO *)data;
	info = container_of(data, NCSI_IF_INFO, enable_work);

	return NCSI_Enable_Info(info);
}

int
NCSI_Enable_Info(NCSI_IF_INFO *info)
{
	int i;
	UINT8 PackageID;
	UINT8 ChannelID;


	/* Check for User Specified channel */
	if (!info->AutoSelect) 
	{
		for(i=0;i<info->TotalChannels;i++)
		{
			if (info->ChannelInfo[i].Valid == 0)
				continue;

			PackageID = info->ChannelInfo[i].PackageID;
			ChannelID = info->ChannelInfo[i].ChannelID;

			if ((PackageID == info->ForcePackage) && (ChannelID == info->ForceChannel))
			{
				if (EnableChannel(info,PackageID,ChannelID) != 0)
					return 1;   /* Failed User Values */
				info->ChannelInfo[i].Enabled = 1;
				InitEthtoolInfo(info);
				return 0;
			}

		}
		printk("NCSI(%s): Unable to Enable User Config. Default to auto\n",info->dev->name);
	}


	/* If Auto Select or user specified is invalid */
	for(i=0;i<info->TotalChannels;i++)
	{
		if (info->ChannelInfo[i].Valid == 0)
			continue;

		PackageID = info->ChannelInfo[i].PackageID;
		ChannelID = info->ChannelInfo[i].ChannelID;
		if (EnableChannel(info,PackageID,ChannelID) == 0)
		{
			info->ChannelInfo[i].Enabled = 1;
			InitEthtoolInfo(info);
			return 0;
		}
	}
	return 1;
}

void
NCSI_Enable(struct work_struct *data)
{
	NCSI_Enable_Ex(data);	
}

void
NCSI_Disable(struct work_struct *data)
{
	NCSI_IF_INFO *info;
	int i;
	UINT8 PackageID;
	UINT8 ChannelID;

	//info = (NCSI_IF_INFO *)data;
	info = container_of(data, NCSI_IF_INFO, disable_work); 


	for(i=0;i<info->TotalChannels;i++)
	{
		if (info->ChannelInfo[i].Valid == 0)
			continue;

		if (info->ChannelInfo[i].Enabled == 1)
		{
			PackageID = info->ChannelInfo[i].PackageID;
			ChannelID = info->ChannelInfo[i].ChannelID;
			if (DisableChannel(info,PackageID,ChannelID) == 0)
			{
				info->ChannelInfo[i].Enabled = 0;
				InitEthtoolInfo(info);
			}
		}
	}

	return;
}
	

extern int UserPackageID;
extern int UserChannelID;
extern int UserSpeed;
extern int UserDuplex;
extern char UserInterface[];
extern struct workqueue_struct *ncsi_wq;

static void 
NCSI_SetUserLink(struct work_struct *data)
{
	SetUserLinkReq_T *work;
	NCSI_IF_INFO *info;
	int NcsiSpeed;
	int NcsiDuplex;
	int retval;

	if (data == NULL) return;

	work = (SetUserLinkReq_T *)data;
	info = GetInterfaceInfoByName(work->InterfaceName);


	if (work->Speed == 10)
		NcsiSpeed =  LINK_ENABLE_10_MBPS;
	if (work->Speed == 100)
		NcsiSpeed =  LINK_ENABLE_100_MBPS;
	if (work->Speed == 1000)
		NcsiSpeed =  LINK_ENABLE_1000_MBPS;

	if (work-> Duplex== 1)
		NcsiDuplex =  LINK_ENABLE_FULL_DUPLEX;
	if (work-> Duplex== 0)
		NcsiDuplex =  LINK_ENABLE_HALF_DUPLEX;

	if (info != NULL)
	{

		if (work->Speed == 0)	/* Auto Neg */
		{
			printk("NCSI(%s): Forcing  Link for AutoNegotiation\n", work->InterfaceName);
			retval = NCSI_Issue_SetLink(info,(UINT8)work->PackageId,(UINT8)work->ChannelId,1,0,0);
		}
		else
		{
			printk("NCSI(%s): Forcing  Link for %dMbps %s Duplex \n", work->InterfaceName,work->Speed, (work->Duplex)?"Full":"Half");
			retval = NCSI_Issue_SetLink(info,(UINT8)work->PackageId,(UINT8)work->ChannelId,0,NcsiSpeed,NcsiDuplex);
		}
		if (retval == 0)
			printk("NCSI(%s): Forcing Link Success \n", work->InterfaceName);
	}
	else
		printk("NCSI : Setting User Link Failed. Invalid Interface (%s)\n", work->InterfaceName);

	kfree((void *)data);
	
	return;
}

static void 
NCSI_SetUserSettings(struct work_struct *data)
{
	SetUserSettingsReq_T *work;
	NCSI_IF_INFO *info;

	if (data == NULL) return;

	work = (SetUserSettingsReq_T *)data;
	info = GetInterfaceInfoByName(work->InterfaceName);

	if (info != NULL)
	{

		info->vlanID = work->VLANId;

		info->AutoSelect = 0;
		info->ForcePackage = work->PackageId;
		info->ForceChannel = work->ChannelId;
		
		if (NCSI_Enable_Ex((struct work_struct*)&(info->enable_work)) == 1) 
			printk("NCSI(%s): Setting User Config Failed\n", work->InterfaceName);
	}
	else
		printk("NCSI : Setting User Config Failed. Invalid Interface (%s)\n", work->InterfaceName);

	kfree((void *)data);
	
	return;
}


int
SetUserSettings(int ID)
{
	SetUserSettingsReq_T *work = (SetUserSettingsReq_T *)kmalloc(sizeof(SetUserSettingsReq_T), GFP_KERNEL);
	int destLen = 0;

	if (work == NULL)
	{
		printk("NCSI:ERROR: Unable to allocate memory for the work queue item\n");
		return 1;
	}
	
	memset(work, 0, sizeof(SetUserSettingsReq_T));

	INIT_WORK((struct work_struct *)work, NCSI_SetUserSettings);

	destLen = sizeof(work->InterfaceName);
	strncpy(work->InterfaceName, UserInterface, destLen);
	if (work->InterfaceName[destLen - 1] != 0) work->InterfaceName[destLen - 1] = 0;

	work->PackageId = UserPackageID;
	work->ChannelId = UserChannelID;
	work->VLANId = ID;
	
	queue_work(ncsi_wq, (struct work_struct *)work);
	
	return 0;
}

int
SetUserLink(void)
{
	SetUserLinkReq_T *work = (SetUserLinkReq_T *)kmalloc(sizeof(SetUserLinkReq_T), GFP_KERNEL);
	int destLen = 0;

	if (work == NULL)
	{
		printk("NCSI:ERROR: Unable to allocate memory for the work queue item\n");
		return 1;
	}
	
	memset(work, 0, sizeof(SetUserLinkReq_T));

	INIT_WORK((struct work_struct *)work, NCSI_SetUserLink);

	destLen = sizeof(work->InterfaceName);
	strncpy(work->InterfaceName, UserInterface, destLen);
	if (work->InterfaceName[destLen - 1] != 0) work->InterfaceName[destLen - 1] = 0;

	work->PackageId = UserPackageID;
	work->ChannelId = UserChannelID;
	work->Speed = UserSpeed;
	work->Duplex = UserDuplex;
	
	
	queue_work(ncsi_wq, (struct work_struct *)work);
	
	return 0;
}

