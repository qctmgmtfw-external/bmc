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

  Module	: NCSI Detect and Enable Modules
			
  Revision	: 1.0  

  Changelog : 1.0 - Initial Version [SC]

*****************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <asm/byteorder.h>
#include <linux/delay.h>
#include "ncsi.h"
#include "interfaces.h"

// Quanta +++
extern int UserLinkStatus;
extern int NCSIStatus;
extern int NCSIStateMachine;
// Quanta +++

extern int UserAuto;
extern int UserPackageID;
extern int UserChannelID;
extern int UserSpeed;
extern int UserDuplex;
extern int UserAutoNeg;
extern int UserVetoBit;
extern unsigned int UserCommand[NCSI_COMMAND_SIZE];

extern char UserInterface[];
extern struct workqueue_struct *ncsi_wq;
void NSCI_Detect_Package_Channel (NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID);
int NCSI_Enable_Info(NCSI_IF_INFO *info);

#if defined(CONFIG_SPX_FEATURE_NCSI_GET_LINK_STATUS_FOR_NON_AEN_SUPPORTED_CONTROLLERS) ||\
	defined (CONFIG_SPX_FEATURE_POLL_FOR_ASYNC_RESET) ||\
	defined (NCSI_POLL_LINK_STATUS)
extern int verbose;

extern struct timer_list getLinkStatusTimer;  /* A Kernel Timer to periodically issue GetLinkStatus Command */
UINT8 enabledPkgID;  /* Currently enabled Pkg ID */
UINT8 enabledChID;  /* Currently enabled Channel ID */
extern int isPollTimerInitialized;

/*
 * @Brief : Checks the current link state, if async reset has occurred reconfigures the channel
 *          if AENs are not enabled, alerts all netdevice notifiers
 */
static void
NCSI_CheckStatusAndResetNCSI(struct work_struct *data)
{
    int retval = 0;
    GetLinkStatusReq_T *work;
    NCSI_IF_INFO *info;
    UINT32 linkstate = 0;
	
    if(data == NULL)
        return;

    work = (GetLinkStatusReq_T *) data;
    info = GetInterfaceInfoByName(work->InterfaceName);

    if(info != NULL)
    {
        netif_carrier_on(info->dev);    //make sure we can transmit

	retval = NCSI_Issue_GetLinkStatus(info, work->PackageId, work->ChannelId, &linkstate ); // Quanta change package 0 channel 0, to package/channel in work structure.

#ifdef CONFIG_SPX_FEATURE_POLL_FOR_ASYNC_RESET
	// If the controller reverts back to initial state after an asynchronous reset
	// Re configure the link
        if ( NCSI_INIT_REQUIRED == retval )
        {
		printk(KERN_WARNING"Asynchronous Reset Detected !!!\n");			
		printk (KERN_WARNING"NCSI(%s):(%d.%d) Reset for NCSI Interface..\n",
                work->InterfaceName, work->PackageId, work->ChannelId);

        	/* Detect and Configure the NC-SI Interface that 
	        * pluggable NCSI channels will also work */
        	netif_carrier_on(info->dev); 
		NSCI_Detect_Package_Channel (info, work->PackageId, work->ChannelId);
		// Enabling previously active channel
		NCSI_Enable_Info(info);

		#ifdef CONFIG_SPX_FEATURE_NCSI_FORCE_LAN_SPEED_10G
    			NCSI_Issue_SetLink(info,(UINT8)PackageID,(UINT8)ChannelID,0/*Force Speed*/,LINK_ENABLE_10_GBPS,LINK_ENABLE_FULL_DUPLEX) ;
		#else
			// Setting Auto Negotiate for link speed
			NCSI_Issue_SetLink(info,(UINT8)work->PackageId,(UINT8)work->ChannelId,1,0,0);
		#endif
        }
#endif

#ifndef NCSI_POLL_LINK_STATUS // Quanta do LinkStatus polling no whether AEN support or not.
#ifdef CONFIG_SPX_FEATURE_NCSI_GET_LINK_STATUS_FOR_NON_AEN_SUPPORTED_CONTROLLERS
	// If AEN is not enabled in the interface, alert netdevice notifiers
	if ( !info->AENEnabled && NCSI_ERR_SUCCESS == retval )	
	{
		DisplayLinkStatus(info, linkstate, 0);
	}
#endif
#else
		// alert netdevice notifiers
		if ( NCSI_ERR_SUCCESS == retval && info->LinkStatus != (linkstate & LINK_STATUS_UP))	
		{
			printk("NCSI(%s): Polling(Not AEN) LINK STATUS changed\n",info->dev->name);
			// Copy from case AEN_TYPE_LINK_STATUS_CHANGE of CheckIfAEN() of ncsilib.c
			DisplayLinkStatus(info, linkstate, 1);
			if (info->AutoSelect == 1)
			{
				// Auto failover(AutoSelect==1) might not set Link Status in DisplayLinkStatus() when link down.
				info->LinkStatus = (linkstate & LINK_STATUS_UP);
				
				info->AENEvent = 1;
				info->AENLinkStatus = linkstate;
				info->AENPackageID = work->PackageId;
				info->AENChannelID = work->ChannelId;
				SendWakeEvent(EVENT_ID_DO_AEN);
				//queue_work(ncsi_wq,&info->enable_work);
			}
		}
#endif
    }

    kfree((void *)data);
    return;
}

/* 
 * @ Brief: Create a job on the workqueue for each configured NC-SI interface,
 *          in order to check the link state
 */
static
void CheckLinkStatus ( NCSI_IF_INFO *info )
{
	GetLinkStatusReq_T *work = NULL;

	if(netif_running(info->dev))
    	{	
		if ( info->EnabledChannelID != -1 && info->EnabledPackageID != -1 )
		{
			work = (GetLinkStatusReq_T *) kmalloc(sizeof(GetLinkStatusReq_T), GFP_ATOMIC);

		        if(work == NULL)
       			{
		            printk(KERN_WARNING"getLinkStatusTimerFn(): NCSI(%s) ERROR: Unable to allocate memory for the work queue item\n", 
					info->dev->name);
		            return;
       			}
				
			memset(work, 0, sizeof(GetLinkStatusReq_T));
		        INIT_WORK((struct work_struct *)work, NCSI_CheckStatusAndResetNCSI);
			
		        strncpy(work->InterfaceName, info->dev->name, sizeof(work->InterfaceName));

			work->PackageId = info->EnabledPackageID;
		        work->ChannelId = info->EnabledChannelID;
		        queue_work(ncsi_wq, (struct work_struct *)work);
		}
	}
}

static void
getLinkStatusTimerFn(unsigned long data)
{
	InvokeCallbackForEachInterface(CheckLinkStatus);

	// Respawning the timer
	getLinkStatusTimer.expires = jiffies + (CONFIG_SPX_FEATURE_NCSI_TIMER_DEALAY_FOR_GET_LINK_STATUS * HZ);
	add_timer(&getLinkStatusTimer);
}

#endif

static
int
EnableChannel(NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
	UINT8   VlanMode;

	/* Issue Select Package with Hw Arbit Enable */
	if (NCSI_Issue_SelectPackage(info,PackageID,0) != 0)
	{
		printk( "NCSI(%s):%d.%d Select Package (Hw Arbit Enable)Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
	{
		printk( "NCSI(%s):%d.%d Clear Init State Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	/* Setup VLANID */
	if(info->vlanID) 	//This call is to bring up the VLAN H/W filetring
	{
		if (NCSI_Issue_SetVLANFilter(info,PackageID,ChannelID,info->vlanID,1) != 0)
		{
			printk( "NCSI(%s):%d.%d Setting VLAN TAG %d Failed\n",info->dev->name,PackageID, ChannelID,info->vlanID);
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
		printk( "NCSI(%s):%d.%d VLAN filter enable failed\n",info->dev->name,PackageID, ChannelID);
	}

	/* Issue an Enable TX for the channel */
	if (NCSI_Issue_ChannelCommands(info,CMD_ENABLE_CHANNEL_TX,PackageID,ChannelID) != 0)
	{
		printk( "NCSI(%s):%d.%d Enable Channel Tx Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}
		
	/* Enable the channel */
	if (NCSI_Issue_ChannelCommands(info,CMD_ENABLE_CHANNEL,PackageID,ChannelID) != 0)
	{
		printk( "NCSI(%s):%d.%d Enable Channel Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	printk( "NCSI(%s): Channel %d.%d Enabled\n", info->dev->name, PackageID, ChannelID);

	info->EnabledPackageID = PackageID;
	info->EnabledChannelID = ChannelID;	

#if defined(CONFIG_SPX_FEATURE_NCSI_GET_LINK_STATUS_FOR_NON_AEN_SUPPORTED_CONTROLLERS) ||\
	defined(CONFIG_SPX_FEATURE_POLL_FOR_ASYNC_RESET ) ||\
	defined(NCSI_POLL_LINK_STATUS)
	
	info->AENEnabled = CheckAENSupport(info, PackageID, ChannelID);

#ifndef NCSI_POLL_LINK_STATUS // Quanta Start Link Status Timer no whether AEN support or not
#if defined(CONFIG_SPX_FEATURE_NCSI_GET_LINK_STATUS_FOR_NON_AEN_SUPPORTED_CONTROLLERS) &&\
        !defined(CONFIG_SPX_FEATURE_POLL_FOR_ASYNC_RESET )
	
	if ( info->AENEnabled )
		return 0;

#endif
#endif

	/* Initiate the timer only once; This will run always until the NCSI driver gets unloaded */
	if( !isPollTimerInitialized )  
	{
		printk("NCSI(%s): Initializing the Timer for getting the link state(Pkg:%d, Ch:%d).\n", info->dev->name, PackageID, ChannelID);
		init_timer(&getLinkStatusTimer);
		getLinkStatusTimer.data = (unsigned long)info;
		getLinkStatusTimer.function = getLinkStatusTimerFn;
		getLinkStatusTimer.expires = jiffies + (CONFIG_SPX_FEATURE_NCSI_TIMER_DEALAY_FOR_GET_LINK_STATUS * HZ);
		add_timer(&getLinkStatusTimer);
		isPollTimerInitialized = 1;
	}
	else {
		printk("NCSI(%s): Timer already initialized for getting the link state \n", info->dev->name );
	}
#endif

	return 0;
}

static
int
DisableChannel(NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
	/* Issue Select Package with Hw Arbit Enable */
	if (NCSI_Issue_SelectPackage(info,PackageID,0) != 0)
	{
		printk( "NCSI(%s):%d.%d Select Package (Hw Arbit Enable)Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
	{
		printk( "NCSI(%s):%d.%d Clear Init State Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	/* Issue an DISABLE TX for the channel */
	if (NCSI_Issue_ChannelCommands(info,CMD_DISABLE_CHANNEL_TX,PackageID,ChannelID) != 0)
	{
		printk( "NCSI(%s):%d.%d Disable Channel Tx Failed\n",
					info->dev->name,PackageID, ChannelID);
		return 1;
	}
		
	/* Disable the channel */
	if (NCSI_Issue_DisableChannel(info,PackageID,ChannelID,0) != 0)
	{
		printk( "NCSI(%s):%d.%d Disable Channel Failed\n",
					info->dev->name,PackageID, ChannelID);	
		return 1;
	}

	printk( "NCSI(%s): Channel %d.%d Disabled\n", info->dev->name, PackageID, ChannelID);


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
	int reportlink=0;

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

	if(!netif_carrier_ok(dev))
	{
		reportlink = 1;
		netif_carrier_on(dev);
	}

	printk( "NCSI(%s): Changing MAC Addr to  %02X:%02X:%02X:%02X:%02X:%02X\n",dev->name,
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

	
	for (index = 0; index < info->TotalChannels ; index ++)
	{
		/*  
		 * Comment out enabled checking to set MAC address for all channel.
		 * When bonding is enabled and using eth0's MAC address.
		 * If other NCSI channel is still using its own MAC, 
		 * then connection will loss after change NCSI channel.
		 */
		 /*
		if (info->ChannelInfo[index].Enabled == 0)
			continue;
		*/
		if (NCSI_Issue_SetMacAddress(info,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID,
					MACAddr,1,0) != NCSI_ERR_SUCCESS)
		{
			printk("NCSI(%s): ERROR: Change Mac Address(%d.%d) Failed \n",dev->name,info->ChannelInfo[index].PackageID,
								info->ChannelInfo[index].ChannelID);
		}
		else
		{
			printk( "NCSI(%s): Change  Mac Address(%d.%d) Passed \n",dev->name,info->ChannelInfo[index].PackageID,
								info->ChannelInfo[index].ChannelID);
		}
	
	}

	if(reportlink)
		InitEthtoolInfo(info);

	if (!(flags & IFF_UP))	
		dev_close(dev);

	return 0;
}

void NSCI_Detect_Channel (NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
    int i;
    UINT32 Ver1,Ver2,Ver3;
    UINT8 Major;
    struct net_device *dev;
    
    dev  = info->dev;
    
    /* Issue Cleear Init State  for each channel */
    if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
    {
        return;
    }

    /* Get Version ID and verify it is > 1.0  */
    if (NCSI_Issue_GetVersionID(info,PackageID,ChannelID,&Ver1,&Ver2,&Ver3) != 0)
    {
        printk("NCSI(%s):%d.%d Get Version IDFailed\n",dev->name,PackageID, ChannelID); 
        return;
    }

    printk("Manufacturer ID :: (0x%08lx)\n", Ver3);	
    Major = (Ver1 >> 24) & 0xFF;
    if ((Major & 0xF0)== 0xF0)
        Major = Major & 0x0F;
    if (Major < 1)
    {
        printk( "NCSI(%s):%d.%d Version(0x%08lx) is < 1.0  Not supported\n",
                dev->name,PackageID, ChannelID,Ver1);
        return;
    }

    for(i = 0; i < info->TotalChannels; i++)
    {
        if ((PackageID == info->ChannelInfo[i].PackageID) && 
                (ChannelID == info->ChannelInfo[i].ChannelID))
        {
            printk ( "NCSI(%s):%d.%d ChannelID and PackageID found in %d\n", 
                    dev->name, PackageID, ChannelID, i);
            break;
        }
    }
    
    if (i == info->TotalChannels)
    {
        i = info->TotalChannels;
        info->TotalChannels++;
    }
    
    /* Get Capabilities and set ArbitSupport */
    if (NCSI_Issue_GetCapabilities(info,PackageID,ChannelID, 
                &info->ChannelInfo[i].Caps) != 0)
    {
        printk( "NCSI(%s):%d.%d Get Capabilities Failed\n",dev->name,PackageID, ChannelID);  
        return;
    }
    
    if (info->ChannelInfo[i].Caps & HW_ARBITRATION_SUPPORT)
        info->ChannelInfo[i].ArbitSupport = 1;
    else
        info->ChannelInfo[i].ArbitSupport = 0;

    info->ChannelInfo[i].PackageID = PackageID;
    info->ChannelInfo[i].ChannelID = ChannelID;
    info->IANA_ManID = Ver3;	
    printk( "NCSI(%s):Found NC-SI at Package:Channel (%d:%d)\n", dev->name,PackageID,ChannelID);

    return;
}

void NCSI_Configure_Channel (NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
    UINT8 MACAddr[6];
    struct net_device *dev;
    int retval = 0, i;
#ifndef CONFIG_SPX_FEATURE_NCSI_FORCE_LAN_SPEED_10G
    int NcsiSpeed = 0, NcsiDuplex = 0;
#endif    

    
    dev  = info->dev;
    /* Get MAC Address to use */
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
        return;
    }
    
    /* Issue Cleear Init State to enter into init state  */
    if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
    {
        printk("NCSI(%s):%d.%d Clear Init State Failed\n",dev->name,PackageID, ChannelID);  
        return;
    }

    /* Issue a Reset Channel to clear all previous config */
    if (NCSI_Issue_ResetChannel(info,PackageID,ChannelID) != 0)
    {
        printk("NCSI(%s):%d.%d Reset Channel Failed\n",dev->name,PackageID, ChannelID); 
        return;
    }

    /* Issue Cleear Init State to enter into init state  */
    if (NCSI_Issue_ClearInitialState(info,PackageID,ChannelID) != 0)
    {
        printk("NCSI(%s):%d.%d Clear Init State Failed\n",dev->name,PackageID, ChannelID);  
        return;
    }

	switch (info->IANA_ManID)
	{
	
	case VENDOR_ID_INTEL:	//For Intel Management Controller
	    printk( "NCSI(%s):%d.%d %d Set Intel Management Control\n",info->dev->name,PackageID, ChannelID,info->VetoBit);
	    // Enable/Disable Keep Phy Link Up feature for Intel GbE Controller
        if (NCSI_Issue_OEM_SetIntelManagementControlCommand(info,PackageID, ChannelID, info->VetoBit) != 0)
        {
	    	printk( "NCSI(%s):%d.%d Set Intel Management Control Failed\n",info->dev->name,PackageID, ChannelID); 
        }
	break;
		
	default:
		printk( ">>>>>>>>>NCSI Management Control is not supported<<<<<<<<<<<<\n");
	break;
	}	
	
		
    /* Setup MAC Address */
    if (NCSI_Issue_SetMacAddress(info,PackageID,ChannelID,MACAddr,1,0) != 0)
    {
        printk( "NCSI(%s):%d.%d Set Mac Address Failed\n",dev->name,PackageID, ChannelID);   
        return;
    }

#if 0
    /* Setup VLANID */
    printk("NCSI(%s):%d.%d Setting VLAN in NCSI with VLAN TAG %d ...\n",dev->name,PackageID,ChannelID,CFG_PROJ_VLAN_ID);
    if (NCSI_Issue_SetVLANFilter(info,PackageID,ChannelID,CFG_PROJ_VLAN_ID,1) != 0)
    {
        printk("NCSI(%s):%d.%d Setting VLAN TAG %d Failed\n",dev->name,PackageID, ChannelID,CFG_PROJ_VLAN_ID);
        return;
    }
    
    /* Enable VLANID filter */
    if (NCSI_Issue_EnableVLAN(info,PackageID,ChannelID,VLAN_MODE_ANY_VLAN_NON_VLAN) != 0)
    {
        printk("NCSI(%s):%d.%d VLAN filter enable failed\n",dev->name,PackageID, ChannelID);    
        return;
    }
    printk("NCSI(%s):%d.%d Setting VLAN in NCSI is completed with VLAN TAG %d\n",dev->name,PackageID,ChannelID,CFG_PROJ_VLAN_ID);
#endif

    /* Enable Broaccast filter */
    if (NCSI_Issue_EnableBcastFilter(info,PackageID,ChannelID,1,1,1,1) != 0)
    {
        printk("NCSI(%s):%d.%d Enable Bcast Filter Failed\n",dev->name,PackageID, ChannelID);   
        return;
    }
    
    /* Disable Multicast filter */
    if (NCSI_Issue_DisableMcastFilter(info,PackageID,ChannelID) != 0)
    {
        printk("NCSI(%s):%d.%d Disable Multicast Filter Failed\n",dev->name,PackageID, ChannelID);
    }

    /* Setup AEN Messages */
    if (NCSI_Issue_EnableAEN(info,(UINT8)PackageID,(UINT8)ChannelID,1,1,1) != 0)
    {
#ifndef CONFIG_SPX_FEATURE_NCSI_GET_LINK_STATUS_FOR_NON_AEN_SUPPORTED_CONTROLLERS
        printk("NCSI(%s):%d.%d Enable AEN Failed\n",dev->name,PackageID, ChannelID);
        //return; //continue;//Quanta AEN Enable command in NCSI spec is Conditional (C) if enable failed should not be stop
#endif
    }

#ifdef CONFIG_SPX_FEATURE_NCSI_FORCE_LAN_SPEED_10G
    retval = NCSI_Issue_SetLink(info,(UINT8)PackageID,(UINT8)ChannelID,0/*Force Speed*/,LINK_ENABLE_10_GBPS,LINK_ENABLE_FULL_DUPLEX) ;
#else
    /* Enable Auto Negotiation */
    if (UserSpeed == 0)   /* Auto Neg */
    {
    	UINT32 LinkStatus;  // Quanta
        // Quanta ---
        /* Check Auto Negotiation is enabled by Share NIC PHY.
           If not, NCSI driver should enable Negotiation. */
        msleep(1000); //Quanta:
        if (NCSI_Issue_GetLinkStatus(info,(UINT8)PackageID,(UINT8)ChannelID,&LinkStatus) != 0)
        {
           retval = NCSI_Issue_SetLink(info,(UINT8)PackageID,(UINT8)ChannelID,1,0,0);
        }
    }
    else
    {
        if (UserSpeed == 10)
            NcsiSpeed =  LINK_ENABLE_10_MBPS;
        if (UserSpeed == 100)
            NcsiSpeed =  LINK_ENABLE_100_MBPS;
        if (UserSpeed == 1000)
            NcsiSpeed =  LINK_ENABLE_1000_MBPS;

        if (UserDuplex == 1)
            NcsiDuplex =  LINK_ENABLE_FULL_DUPLEX;
        if (UserDuplex == 0)
            NcsiDuplex =  LINK_ENABLE_HALF_DUPLEX;

        retval = NCSI_Issue_SetLink(info,(UINT8)PackageID,(UINT8)ChannelID,UserAutoNeg,NcsiSpeed,NcsiDuplex);
    }
#endif

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
        }
    }

    for(i = 0; i < info->TotalChannels; i++)
    {
        if ((PackageID == info->ChannelInfo[i].PackageID) && 
                (ChannelID == info->ChannelInfo[i].ChannelID))
        {
            /* Set this channel info is setup */ 
             info->ChannelInfo[i].Valid = 1;
        }
    }
    return;
}

void NSCI_Detect_Package_Channel (NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
    /* Issue Select Package with Hw Arbit Disable*/
    if (NCSI_Issue_SelectPackage(info,PackageID,1) != 0)
    {
        return;
    }
    
    NSCI_Detect_Channel (info, PackageID, ChannelID);
    NCSI_Configure_Channel (info, PackageID, ChannelID);
    
    /* Deselect previusly selected package */
    NCSI_Issue_DeSelectPackage(info,PackageID);
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

	dev  = info->dev;

//	printk("NCSI(%s):NCSI_Detect_Info\n", info->dev->name);
	
	if (!netif_carrier_ok(info->dev)) 
	{
		if (rtnl_trylock())
		{
		   netdev_link_up(info->dev);
		   rtnl_unlock();
		}
		else
		{
		   netdev_link_up(info->dev);
		}
		netif_carrier_on(info->dev);
//		printk("NCSI(%s):NCSI_Detect_Info: netif_carrier_on\n",  info->dev->name);
	}

	/* Get MAC Address to use */
	/*NCSI Need in the reverse order */
	//memcpy(&MACAddr[0],dev->dev_addr,6);
	MACAddr[0] = dev->dev_addr[5];
	MACAddr[1] = dev->dev_addr[4];
	MACAddr[2] = dev->dev_addr[3];
	MACAddr[3] = dev->dev_addr[2];
	MACAddr[4] = dev->dev_addr[1];
	MACAddr[5] = dev->dev_addr[0];
	printk( "NCSI(%s): MAC Addr = %02X:%02X:%02X:%02X:%02X:%02X\n",dev->name,
		MACAddr[5],MACAddr[4],MACAddr[3],MACAddr[2],MACAddr[1],MACAddr[0]);
					
	if ((MACAddr[0] == 0) && (MACAddr[1] == 0) && (MACAddr[2] == 0) &&
	    (MACAddr[3] == 0) && (MACAddr[4] == 0) && (MACAddr[5] == 0))
	{
		printk("NCSI(%s): Error! Mac Address is 0. Cannot enable NCSI\n",dev->name);
		return;
	}

	info->TotalChannels = 0;
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
		for (ChannelID = 0; ChannelID < MAX_CHANNEL_ID; ChannelID++)
		{
			NSCI_Detect_Channel (info, PackageID, ChannelID);
		}
		
		/* Deselect previusly selected package */
		NCSI_Issue_DeSelectPackage(info,PackageID);
	}

	if (info->TotalChannels == 0)
		printk( "NCSI(%s): No NCSI Interfaces detected\n", dev->name);

	PrevPackageID = -1;
	/* Configure the detected channels */
	for(i=0;i<info->TotalChannels;i++)
	{
		PackageID = info->ChannelInfo[i].PackageID;
		ChannelID = info->ChannelInfo[i].ChannelID;

		/* Issue Select Package with Hw Arbit Disable*/
		if (NCSI_Issue_SelectPackage(info,PackageID,1) != 0)
			continue;
		
		NCSI_Configure_Channel (info, PackageID, ChannelID);
		
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
		printk( "NCSI(%s):Multiple Packages Found\n",dev->name);
		for(i=0;i<info->TotalChannels;i++)
		{
			if (info->ChannelInfo[i].Valid == 0)
				continue;
			if (info->ChannelInfo[i].ArbitSupport == 0)
			{
				PackageID = info->ChannelInfo[i].PackageID;
				ChannelID = info->ChannelInfo[i].ChannelID;
				printk( "NCSI(%s):WARNING: Channel (%d:%d) does not support Arbitration\n",
							dev->name,PackageID,ChannelID);
			}	
		}
	}

	if ((info->TotalChannels) && (ValidPackages))
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
	UINT32 LinkStatus;
	
	if(!info->TotalChannels)
		return 1;

	if (!netif_carrier_ok(info->dev)) 
	{
		if (rtnl_trylock())
		{
		   netdev_link_up(info->dev);
		   rtnl_unlock();
		}
		else
		{
		   netdev_link_up(info->dev);
		}
		netif_carrier_on(info->dev);
//		printk("NCSI(%s):NCSI_Enable_Info: netif_carrier_on\n",  info->dev->name);
	}

	/* Check for User Specified channel */
	if (!info->AutoSelect) //0
	{
		for(i=0;i<info->TotalChannels;i++)
		{
			if (info->ChannelInfo[i].Valid == 0)
				continue;

			PackageID = info->ChannelInfo[i].PackageID;
			ChannelID = info->ChannelInfo[i].ChannelID;
			if ((ChannelID != info->ForceChannel) || (PackageID != info->ForcePackage)){//other than user requested channel
				if (info->ChannelInfo[i].Enabled == 1)//any other chn enabled
				{
					if (DisableChannel(info,PackageID,ChannelID) == 0){
						info->ChannelInfo[i].Enabled = 0;
						continue;
					}
				}
			}
		}
		
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
		printk( "NCSI(%s): Unable to Enable User Config. Default to auto\n",info->dev->name);
	}
	
	/* If Auto Select or user specified is invalid */
	if (info->AutoSelect == 1)
	{
		netif_carrier_on(info->dev);//make sure we can transmit
		//disable every channel first
		for(i=0;i<info->TotalChannels;i++)
		{
			if (info->ChannelInfo[i].Valid == 0)
				continue;
			PackageID = info->ChannelInfo[i].PackageID;
			ChannelID = info->ChannelInfo[i].ChannelID;
			DisableChannel(info,PackageID,ChannelID);
			info->ChannelInfo[i].Enabled = 0;
		}
	}
	
	for(i=0;i<info->TotalChannels;i++)
	{
		if (info->ChannelInfo[i].Valid == 0)
			continue;

		PackageID = info->ChannelInfo[i].PackageID;
		ChannelID = info->ChannelInfo[i].ChannelID;
		if(info->AutoSelect)
		{
			msleep(1000); //Quanta //mdelay(1000);  //Adding a delay of 1 second before issuing GetLinkStatus command to get the proper link status
			/* Get Link Status to check if Channel can be enabled */	
			if (NCSI_Issue_GetLinkStatus(info,(UINT8)PackageID,(UINT8)ChannelID,&LinkStatus) != 0)
			{
				continue;
			}
			if (!(LinkStatus & 0x01))
			{
				continue;
			}
		}
		if (EnableChannel(info,PackageID,ChannelID) == 0)
		{
			info->ChannelInfo[i].Enabled = 1;
			if (rtnl_trylock())
			{
				netdev_link_up(info->dev);
				rtnl_unlock();
			}
			else
			{
				netdev_link_up(info->dev);
			}
			InitEthtoolInfo(info);
			info->NcsiStateMachine = NCSI_CHANNEL_ENABLE_READY;
			return 0;
		}
	}

	if (info->AutoSelect)
	{
		// there is no connected NC-SI port or error in enabling
		for(i=0;i<info->TotalChannels;i++)
		{
			if (info->ChannelInfo[i].Valid == 0)
				continue;
			PackageID = info->ChannelInfo[i].PackageID;
			ChannelID = info->ChannelInfo[i].ChannelID;
			EnableChannel(info,PackageID,ChannelID);//enable every channel to receive aen
		}
		if (rtnl_trylock())
		{
			netdev_link_down(info->dev);
			rtnl_unlock();
		}
		else
		{
			netdev_link_down(info->dev);
		}
		InitEthtoolInfo(info);
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

//Quanta+++
#ifdef CONFIG_SPX_FEATURE_NCSI_AUTO_FAILOVER
void
NCSI_AenWork(struct work_struct *data)
{
	NCSI_IF_INFO *info;
	int i, activeIdx;
	UINT8 PackageID;
	UINT8 ChannelID;
	UINT32 LinkStatus;
	
	info = container_of(data, NCSI_IF_INFO, aen_work);
	
	info->AENEvent = 0;
	
	if (!(info->AENLinkStatus & LINK_STATUS_UP)) { // For Link down
		
		/* to see which port is link up. */
		for (activeIdx = 0; activeIdx < info->TotalChannels; activeIdx++) {
			if (info->ChannelInfo[activeIdx].Valid == 0)
				continue;
			
			PackageID = info->ChannelInfo[activeIdx].PackageID;
			ChannelID = info->ChannelInfo[activeIdx].ChannelID;
			
			//if ((PackageID == info->AENPackageID) && (ChannelID == info->AENChannelID))
			//	continue;
			
			/* Get Link Status to check if Channel can be enabled */
			if (NCSI_Issue_GetLinkStatus(info,(UINT8)PackageID,(UINT8)ChannelID,&LinkStatus) != 0) {
				continue;
			}
			
			if (LinkStatus & 0x01) { // now the link status is up.
				break;
			}
		}
		
		if (activeIdx >= info->TotalChannels) { // the link status on all of port is link down.
			for (i = 0; i < info->TotalChannels; i++) {
				if (info->ChannelInfo[i].Valid == 0)
					continue;
				
				PackageID = info->ChannelInfo[i].PackageID;
				ChannelID = info->ChannelInfo[i].ChannelID;

				if ((PackageID == info->AENPackageID) && (ChannelID == info->AENChannelID))
					continue;
				
				EnableChannel(info,PackageID,ChannelID);
				info->ChannelInfo[i].Enabled = 1;
			}
		}
		else {
			
			/* 1. Disable the other ports */
			for (i = 0; i < info->TotalChannels; i++) {
				if (info->ChannelInfo[i].Valid == 0)
					continue;
				
				PackageID = info->ChannelInfo[i].PackageID;
				ChannelID = info->ChannelInfo[i].ChannelID;
				
				if (i == activeIdx)
					continue;
				
				DisableChannel(info,PackageID,ChannelID);
				info->ChannelInfo[i].Enabled = 0;
			}
			
			/* 2. Enable active port firstly */
			PackageID = info->ChannelInfo[activeIdx].PackageID;
			ChannelID = info->ChannelInfo[activeIdx].ChannelID;
			EnableChannel(info, PackageID, ChannelID);
			info->ChannelInfo[activeIdx].Enabled = 1;
		}
	}
	else { // For Link up
		for(i=0;i<info->TotalChannels;i++)
		{
			if (info->ChannelInfo[i].Valid == 0)
				continue;

			PackageID = info->ChannelInfo[i].PackageID;
			ChannelID = info->ChannelInfo[i].ChannelID;

			if ((PackageID == info->AENPackageID) && (ChannelID == info->AENChannelID))
				continue;
			
			//DisableChannel(info,PackageID,ChannelID);
			//info->ChannelInfo[i].Enabled = 0;
		}
	}

	return;
}

void
NCSI_CheckLinkWork(struct work_struct *data)
{
	NCSI_IF_INFO *info;
	int channelIdx = 0;
	UINT8 PackageID;
	UINT8 ChannelID;
	UINT32 LinkStatus;
	
	msleep(5000);
	
	info = container_of(data, NCSI_IF_INFO, check_link_work);
	
	if ((info->StopIssueNCSICmd == 0) && (info->AENEvent == 0)) {
		for (channelIdx = 0; channelIdx < info->TotalChannels; channelIdx++) {
			if (info->ChannelInfo[channelIdx].Valid == 0)
				continue;
			
			if (info->ChannelInfo[channelIdx].Enabled == 1)
				break;
		}
		
		if (channelIdx >= info->TotalChannels) {
			
			printk("[ Info ] NCSI, all port are link down\n");
			
			for (channelIdx = 0; channelIdx < info->TotalChannels; channelIdx++) {
				if (info->ChannelInfo[channelIdx].Valid == 0)
					continue;
				
				if ((info->StopIssueNCSICmd == 0) && (info->AENEvent == 0))
					break;
				
				PackageID = info->ChannelInfo[channelIdx].PackageID;
				ChannelID = info->ChannelInfo[channelIdx].ChannelID;
				
				/* Get Link Status to check if Channel can be enabled */
				if (NCSI_Issue_GetLinkStatus(info,(UINT8)PackageID,(UINT8)ChannelID,&LinkStatus) != 0) {
					continue;
				}
				
				if (LinkStatus & 0x01) { // now the link status is up.
					EnableChannel(info,PackageID,ChannelID);
					info->ChannelInfo[channelIdx].Enabled = 1;
					printk("[ WARN ] NCSI, all NIC port were already disconnected. but this NIC port (%d) is link-up\n", channelIdx);
					break;
				}
			}
		}
	}
	
	return;
}
#endif
//Quanta+++

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
			printk( "NCSI(%s): Forcing  Link for AutoNegotiation\n", work->InterfaceName);
			retval = NCSI_Issue_SetLink(info,(UINT8)work->PackageId,(UINT8)work->ChannelId,1,0,0);
		}
		else
		{
			printk( "NCSI(%s): Forcing  Link for %dMbps %s Duplex \n", work->InterfaceName,work->Speed, (work->Duplex)?"Full":"Half");
			retval = NCSI_Issue_SetLink(info,(UINT8)work->PackageId,(UINT8)work->ChannelId,work->AutoNeg,NcsiSpeed,NcsiDuplex);
		}
		if (retval == 0)
			printk( "NCSI(%s): Forcing Link Success \n", work->InterfaceName);
	}
	else
		printk( "NCSI : Setting User Link Failed. Invalid Interface (%s)\n", work->InterfaceName);

	kfree((void *)data);
	
	return;
}

#ifdef CONFIG_SPX_FEATURE_NCSI_MANUAL_DETECT
static void 
NCSI_SetUserDetect(struct work_struct *data)
{
    NCSI_IF_INFO *info = NULL;
    char interfaceName[MAX_IF_NAME_LEN+1];
    int i;
    
    for (i = 0; i < MAX_NET_IF; i++)
    {
        sprintf(interfaceName,"eth%d",i);
        if (strstr (CONFIG_SPX_FEATURE_NCSI_INTERFACE_NAMES, interfaceName) != NULL)
        {
            printk ("NCSI(%s): User Detect NCSI Packages and Channels\n", interfaceName);
            info = GetInterfaceInfoByName(interfaceName);

            if (info != NULL)
            {
                if(rtnl_trylock() == 1)
                {
                    dev_open(info->dev);
                    rtnl_unlock();
                }
                else
                {
                       dev_open(info->dev);
                }
                
                NCSI_Detect_Info(info);
                NCSI_Enable_Info(info);
            }
            
        }
    }
    return;
}
#endif

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
		if( info->AutoSelect == work->AutoSelect &&
		 info->AutoSelect == 1 && info->vlanID == work->VLANId)
		{
			printk( "NCSI: Same mode, do nothing\n");
			kfree((void *)data);
			return;
		}
		info->vlanID = work->VLANId;
		info->AutoSelect = work->AutoSelect;
		if(!info->AutoSelect)
		{
			info->ForcePackage = work->PackageId;
			info->ForceChannel = work->ChannelId;
#ifdef CONFIG_SPX_FEATURE_NCSI_RESET_INTERFACE_IN_USER_SETTINGS
			printk ( "NCSI(%s):(%d.%d) Reset for NCSI Interface.....\n", 
					work->InterfaceName, work->PackageId, work->ChannelId);
			/* Detect and Configure the NC-SI Interface that 
			 * pluggable NCSI channels will also work */
			netif_carrier_on(info->dev);//make sure we can transmit
			NSCI_Detect_Package_Channel (info, work->PackageId, work->ChannelId);
#endif
		}

		if (NCSI_Enable_Ex((struct work_struct*)&(info->enable_work)) == 1) 
			printk("NCSI(%s): Setting User Config Failed\n", work->InterfaceName);
	}
	else
		printk("NCSI : Setting User Config Failed. Invalid Interface (%s)\n", work->InterfaceName);

	kfree((void *)data);
	
	return;
}

static void 
NCSI_SetUserVetoBit(struct work_struct *data)
{
	SetUserVetoBitReq_T *work;
	NCSI_IF_INFO *info;
	UINT32 Ver1,Ver2;
	static UINT32 Ver3 = 0;

	if (data == NULL) return;

	work = (SetUserVetoBitReq_T *)data;
	info = GetInterfaceInfoByName(work->InterfaceName);

	if (info != NULL)
	{
		printk ( "NCSI(%s):(%d.%d.%d) Reset for NCSI Interface..\n", 
					work->InterfaceName, work->PackageId, work->ChannelId, work->VetoBit);

		netif_carrier_on(info->dev);//make sure we can transmit
		if (Ver3 == 0){ // read VersionID only once 
			/* Get Version ID and verify it is > 1.0  */
			if (NCSI_Issue_GetVersionID(info,work->PackageId,work->ChannelId,&Ver1,&Ver2,&Ver3) != 0)
			{
				printk( "NCSI(%s):%d.%d Get Version IDFailed\n",work->InterfaceName,work->PackageId, work->ChannelId);
				InitEthtoolInfo(info);
				kfree((void *)data);
				Ver3 = 0;
				return;
			}
			printk("Manufacturer ID :: (0x%08lx)\n", Ver3);
		}

		switch (Ver3)
		{
		
		case 0x57010000:	//For Intel Management Controller
			printk( "NCSI(%s):%d.%d %d Set Intel Management Control\n",work->InterfaceName,work->PackageId, work->ChannelId,work->VetoBit);
			// Enable/Disable Keep Phy Link Up feature for Intel GbE Controller
			if (NCSI_Issue_OEM_SetIntelManagementControlCommand(info,work->PackageId, work->ChannelId, work->VetoBit) != 0)
			{
				printk( "NCSI(%s):%d.%d.%d Set Intel Management Control Failed\n",work->InterfaceName,work->PackageId, work->ChannelId, work->VetoBit);
			}
		break;
			
		default:
			printk(">>>>>>>>>NCSI Management Control is not supported<<<<<<<<<<<<\n");
		break;
		}	
		InitEthtoolInfo(info);
	}

	kfree((void *)data);
	
	return;
}

static void 
NCSI_SendUserCommand(struct work_struct *data)
{
	SendUserCommandReq_T *work;
	NCSI_IF_INFO *info;

	if (data == NULL) return;

	work = (SendUserCommandReq_T *)data;
	info = GetInterfaceInfoByName(work->InterfaceName);

	if (info != NULL)
	{
		if (NCSI_Issue_UserCommand(info,work->PackageId, work->ChannelId, work->Command, work->Data, work->Length) != 0)
		{
			printk ("NCSI(%s): %d.%d Send User Command Failed\n", work->InterfaceName, work->PackageId, work->ChannelId);
		}
	}

	kfree((void *)data);
	
	return;
}



int
SendUserCommand(void)
{
	SendUserCommandReq_T *work = (SendUserCommandReq_T *)kmalloc(sizeof(SendUserCommandReq_T), GFP_KERNEL);
	int destLen = 0;

	if (work == NULL)
	{
		printk("NCSI:ERROR: Unable to allocate memory for the work queue item\n");
		return 1;
	}
	
	memset(work, 0, sizeof(SendUserCommandReq_T));

	INIT_WORK((struct work_struct *)work, NCSI_SendUserCommand);

	destLen = sizeof(work->InterfaceName);
	strncpy(work->InterfaceName, UserInterface, destLen);
	if (work->InterfaceName[destLen - 1] != 0) work->InterfaceName[destLen - 1] = 0;
	

	work->PackageId = UserPackageID;
	work->ChannelId = UserChannelID;
	work->Length = UserCommand[1] - 1;

	work->Command = UserCommand[0];
	memcpy (&work->Data[0], &UserCommand[2], sizeof (unsigned int) * (work->Length));
	memset (UserCommand, 0, NCSI_COMMAND_SIZE);
	
	queue_work(ncsi_wq, (struct work_struct *)work);
	
	return 0;
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

	work->AutoSelect = UserAuto;
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
	work->AutoNeg = UserAutoNeg;
	
	
	queue_work(ncsi_wq, (struct work_struct *)work);
	
	return 0;
}

#ifdef CONFIG_SPX_FEATURE_NCSI_MANUAL_DETECT
int
SetUserDetect(void)
{
    UserDetectReq_T *work = (UserDetectReq_T *)kmalloc(sizeof(UserDetectReq_T), GFP_KERNEL);
 
    if (work == NULL)
    {
        printk("NCSI:ERROR: Unable to allocate memory for the work queue item\n");
        return 1;
    }
    
    memset(work, 0, sizeof(UserDetectReq_T));
    INIT_WORK((struct work_struct *)work, NCSI_SetUserDetect);
    queue_work(ncsi_wq, (struct work_struct *)work);
    
    return 0;
}
#endif

int
SetUserVetoBit(void)
{
	SetUserVetoBitReq_T *work = (SetUserVetoBitReq_T *)kmalloc(sizeof(SetUserVetoBitReq_T), GFP_KERNEL);
	int destLen = 0;

	if (work == NULL)
	{
		printk("NCSI:ERROR: Unable to allocate memory for the work queue item\n");
		return 1;
	}
	
	memset(work, 0, sizeof(SetUserVetoBitReq_T));

	work->PackageId = UserPackageID;
	work->ChannelId = UserChannelID;

	INIT_WORK((struct work_struct *)work, NCSI_SetUserVetoBit);
	
	destLen = sizeof(work->InterfaceName);
	strncpy(work->InterfaceName, UserInterface, destLen);
	if (work->InterfaceName[destLen - 1] != 0) work->InterfaceName[destLen - 1] = 0;

	work->VetoBit = UserVetoBit;
	
	queue_work(ncsi_wq, (struct work_struct *)work);
	
	return 0;
}

// Quanta ---
#ifdef CONFIG_SPX_FEATURE_NCSI_AUTO_FAILOVER
int ReceiveAEN(void)
{
	NCSI_IF_INFO *info;
	
	info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
	if (info != NULL) {
		queue_work(ncsi_wq,&info->aen_work);
		flush_workqueue(ncsi_wq);
	}
	else {
		printk("NCSI : Receive AEN package, not find the NCSI interface.\n");
	}
	
	return 0;
}
#endif

int EnabledChannel(void)
{
	NCSI_IF_INFO *info;
	
	info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
	if (info != NULL) {
		queue_work(ncsi_wq,&info->enable_work);
		flush_workqueue(ncsi_wq);
	}
	else {
		printk("NCSI : Detect and Enable Channel, not find the NCSI interface.\n");
	}
	
	return 0;
}

int DetectedChannel(void)
{
	NCSI_IF_INFO *info;
	
	info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
	if (info != NULL) {
		queue_work(ncsi_wq,&info->detect_work);
		flush_workqueue(ncsi_wq);
	}
	else {
		printk("NCSI : Detect and Enable Channel, not find the NCSI interface.\n");
	}
	
	return 0;
}

int GetNCSIstaMachine(void)
{
	NCSI_IF_INFO *info;
	
	info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
	NCSIStateMachine = info->NcsiStateMachine;
	return 0;
}

int ClearNCSIstaMachine(void)
{
	NCSI_IF_INFO *info;
	
	info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
	info->NcsiStateMachine = NCSI_NONE;
	return 0;
}

int UpdateNICSelectionSta(int InformChangedGPIO)
{
	NCSI_IF_INFO *info;
	
	info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
	info->StopIssueNCSICmd = (InformChangedGPIO == 0) ? 0 : 1;
	return 0;
}

#ifdef CONFIG_SPX_FEATURE_NCSI_AUTO_FAILOVER
int CheckNcsiLinkStatus(void)
{
	NCSI_IF_INFO *info;
	
	info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
	if (info != NULL) {
		queue_work(ncsi_wq,&info->check_link_work);
	}
	else {
		printk("NCSI : Check network link status again, but not find the NCSI interface.\n");
	}
	
	return 0;
}
#endif
// --- Quanta

