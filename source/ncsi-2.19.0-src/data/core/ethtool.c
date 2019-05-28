/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.             **
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

  Module	: Ethtool support for Network interface with NCSI

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include "interfaces.h"
#include "ncsi.h"


#define DRV_NAME        "NCSI"
#define DRV_FW		"NCSI 1.0"    

/* 
   The folllowing two should be defined in the network driver. 
   If not defined, NCSI driver will not load 
*/

extern int register_ethtool_override(struct ethtool_ops *ops , int ethindex);
extern int unregister_ethtool_override(int ethindex);


/* ethtool_ops function prototypes */
static void ncsi_get_drvinfo(struct net_device *netdev, struct ethtool_drvinfo *info);
static u32  ncsi_get_link(struct net_device *netdev);
static int  ncsi_nway_reset(struct net_device *netdev);
static int  ncsi_get_settings(struct net_device *netdev, struct ethtool_cmd *cmd);
static int  ncsi_set_settings(struct net_device *netdev, struct ethtool_cmd *cmd);

extern int UserSpeed;
extern int UserDuplex;
extern int UserAutoNeg;

static struct ethtool_ops ncsi_ops =
{
        .get_drvinfo  = ncsi_get_drvinfo,
        .get_link     = ncsi_get_link,
        .nway_reset   = ncsi_nway_reset,
        .get_settings = ncsi_get_settings,
        .set_settings = ncsi_set_settings,
};


static
int 
GetEthIndex(char *deviceName)
{
	char interfaceName[MAX_IF_NAME_LEN+1];
	int i = 0;

	memset(interfaceName, 0, sizeof(interfaceName));

	for (i = 0; i < MAX_NET_IF; i++)
	{
		snprintf(interfaceName, sizeof(interfaceName), "%s%d", "eth", i);
		if (strcmp(deviceName, interfaceName) == 0) 
			return i;
	}

	return -1;
}


int  
NCSI_Net_Driver_Register(NCSI_IF_INFO *info)
{

	int ethindex;

	/* Validate Info */
	if (info == NULL)
	  	return 1;

	/* Validate network device */
	if (info->dev == NULL)
		return 1;

	/* Get and Validate device index */
	ethindex = GetEthIndex(info->dev->name);
	if (ethindex == -1)
		return 1;
	/* Override the ethtool operations */
	info->dev->ethtool_ops = &ncsi_ops;

	return register_ethtool_override(&ncsi_ops , ethindex);


}


int  
NCSI_Net_Driver_DeRegister(NCSI_IF_INFO *info)
{
	int ethindex;

	/* Validate Info */
	if (info == NULL)
	  	return 1;

	/* Validate network device */
	if (info->dev == NULL)
		return 1;

	/* Get and Validate device index */
	ethindex = GetEthIndex(info->dev->name);
	if (ethindex == -1)
		return 1;

	return unregister_ethtool_override(ethindex);

}


static void 
ncsi_get_drvinfo(struct net_device *netdev, struct ethtool_drvinfo *info)
{
	if(snprintf(info->driver,sizeof(info->driver),"%s",DRV_NAME)>=(signed)sizeof(info->driver))
	printk("ncsi_get_drvinfo-source buffer truncated\n");
	sprintf(info->version,"%d.%d",NCSI_DRIVER_MAJOR,NCSI_DRIVER_MINOR);
	if(snprintf(info->fw_version,sizeof(info->fw_version),"%s",DRV_FW)>=(signed)sizeof(info->fw_version))
	printk("ncsi_get_drvinfo-source buffer truncated\n");
    return;
}

static u32
ncsi_get_link(struct net_device *netdev)
{
	NCSI_IF_INFO *info;

	info = GetInterfaceInfo(netdev);
	if (info == NULL)
		return 0;

	return info->LinkStatus;
}

static int  
ncsi_get_settings(struct net_device *netdev, struct ethtool_cmd *ecmd)
{
	NCSI_IF_INFO *info;

	info = GetInterfaceInfo(netdev);
	if (info == NULL)
		return 0;

	memcpy(ecmd,&(info->ecmd),sizeof(struct ethtool_cmd));

/*TODO : If the dev is closed/down, AEN events are not received by driver and we will miss link up/down events
	 In such case the info.ecmd will not match the correct state of the NCSI.
	 So when get_settings is called, we need to read the Link Informaton from the NCSI device. But 
	 this is big overhead on the system, if a application (bonding) polls continuously every 100ms
	
	 Another option is a have a seperate timer thread for every 3 seconds (SPX time for detecting link status change)
	 which gets the link status and save to info.ecmd
*/

	return 0;
}


static int
ncsi_nway_reset(struct net_device *netdev)
{
	NCSI_IF_INFO *info;
	UINT32 LinkStatus;
	int index;
	int err=-EINVAL;

	info = GetInterfaceInfo(netdev);
	if (info == NULL)
		return err;

	/* If link is up on any of the enabled channel, then return link up */
	for (index = 0; index < MAX_CHANNELS ; index ++)
	{
		if (info->ChannelInfo[index].Enabled == 0)
			continue;

		msleep(1000); //Quanta: //mdelay(1000); //Adding a delay of 1 second before issuing GetLinkStatus command to get the proper link status
		if (NCSI_Issue_GetLinkStatus(info,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID,&LinkStatus) != NCSI_ERR_SUCCESS)
		{
			printk("NCSI(%s): ERROR: GetLinkStatus(%d.%d) Failed \n",netdev->name,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID);
			continue;
		}

		/* Auto-Ngotiation is turned off by someone. So should not turn on */
	//	if (!(LinkStatus & LINK_AUTO_NEG_FLAG))
	//		continue;

		/* Isuse SetLink to Turn on Auto Negotiaiton */
		if (NCSI_Issue_SetLink(info,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID,1/*Auto Neg*/,0,0) != NCSI_ERR_SUCCESS)
		{
			printk("NCSI(%s): ERROR: SetLink AutoNegotiation (%d.%d) Failed \n",netdev->name,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID);
			continue;
		}
		
		printk(KERN_DEBUG "NCSI(%s) : SetLink (%d.%d) Restart AutoNegotiation Success \n",netdev->name,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID);
		err = 0;
	}
	
	/* Autonegotiation might have change the link properties*/
	InitEthtoolInfo(info);

	return err;
}


static int  
ncsi_set_settings(struct net_device *netdev, struct ethtool_cmd *ecmd)
{

	NCSI_IF_INFO *info;
	int index;
	int err=-EINVAL,retval;
	UINT32 NcsiSpeed=LINK_ENABLE_100_MBPS;
	UINT32 NcsiDuplex=LINK_ENABLE_FULL_DUPLEX;


	info = GetInterfaceInfo(netdev);
	if (info == NULL)
		return err;

	/* Validate Inputs */
	if (ecmd->speed != SPEED_10 &&
            ecmd->speed != SPEED_100 &&
            ecmd->speed != SPEED_1000 &&
            ecmd->speed != SPEED_10000 &&
            ecmd->speed != SPEED_25000)
                return -EINVAL;
        if (ecmd->duplex != DUPLEX_HALF && ecmd->duplex != DUPLEX_FULL)
                return -EINVAL;
        if (ecmd->autoneg != AUTONEG_DISABLE && ecmd->autoneg != AUTONEG_ENABLE)
                return -EINVAL;

	/* Convert Speed and Duplex to NCSI values */
	if (ecmd->speed == SPEED_10)
		NcsiSpeed =  LINK_ENABLE_10_MBPS;
	if (ecmd->speed == SPEED_100)
		NcsiSpeed =  LINK_ENABLE_100_MBPS;
	if (ecmd->speed == SPEED_1000)
		NcsiSpeed =  LINK_ENABLE_1000_MBPS;
	if (ecmd->speed == SPEED_10000)
		NcsiSpeed =  LINK_ENABLE_10_GBPS;
	if (ecmd->speed == SPEED_25000)
		NcsiSpeed =  LINK_ENABLE_25_GBPS;	
	if (ecmd->duplex== DUPLEX_FULL)
		NcsiDuplex =  LINK_ENABLE_FULL_DUPLEX;
	if (ecmd->duplex== DUPLEX_HALF)
		NcsiDuplex =  LINK_ENABLE_HALF_DUPLEX;


	/* For every enabled channel do the settings */
	for (index = 0; index < MAX_CHANNELS ; index ++)
	{
		if (info->ChannelInfo[index].Enabled == 0)
			continue;

		if (ecmd->autoneg == AUTONEG_ENABLE) 
		{
			retval = NCSI_Issue_SetLink(info,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID,1,0,0);
			if (retval == NCSI_ERR_SUCCESS)
				err = 0;			/* if any one succeeds, it is success */
			else
				printk("NCSI(%s): ERROR: SetLink AutoNegotiation (%d.%d) Failed \n",netdev->name,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID);
		}
		else
		{
			retval = NCSI_Issue_SetLink(info,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID,0,NcsiSpeed,NcsiDuplex);
			if (retval == NCSI_ERR_SUCCESS)
				err = 0;			/* if any one succeeds, it is success */
			else
				printk("NCSI(%s): ERROR: SetLink Force Speed/Duplex (%d.%d) Failed \n",netdev->name,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID);
		}

	}
	mdelay (3000);
	DisplayLinkStatus(info,LINK_STATUS_UP, 1);
	/* Force settings might have change the link properties*/
	InitEthtoolInfo(info);
	return err;

}


/* Called from AEN which link status changes . Returns link status */
int
GetEthtoolInfoFromLink (NCSI_IF_INFO *info, UINT32 LinkStatus)
{
	struct ethtool_cmd *ecmd;

	ecmd = &(info->ecmd);
	memset(ecmd,0,sizeof(struct ethtool_cmd));
	
	/* Default values */
	ecmd->advertising = 0;
	ecmd->speed =  SPEED_10;
	ecmd->duplex  = DUPLEX_HALF;

	/* NCSI supported speeds */
	ecmd->supported = (SUPPORTED_Autoneg |
            SUPPORTED_10baseT_Half   | SUPPORTED_10baseT_Full   |
            SUPPORTED_100baseT_Half  | SUPPORTED_100baseT_Full  |
            SUPPORTED_1000baseT_Full | SUPPORTED_10000baseT_Full |            
            SUPPORTED_25000baseT_Full);

	/* Not applicable to NCSI. Set some values */
	ecmd->port = 0;
	ecmd->transceiver = 0;


	/* If link is down, return */
	if (!(LinkStatus & LINK_STATUS_UP))
	{
		info->LinkStatus=0;
		netif_carrier_off(info->dev);
		return 0;
	}
	info->LinkStatus=1;
	netif_carrier_on(info->dev);

	/* If Auto negotitaton is enabled */
	if (LinkStatus & LINK_AUTO_NEG_FLAG)
	{
                ecmd->advertising |= ADVERTISED_Autoneg;
        	ecmd->autoneg = AUTONEG_ENABLE;
	}

	/* Get the Speed and Duplex */
	switch (LinkStatus & LINK_SPEED_DUPLEX_MASK) 
	{
		case LINK_10THD:
			ecmd->speed =  SPEED_10;
			ecmd->duplex  = DUPLEX_HALF;
			break;
		case LINK_10TFD:		
			ecmd->speed =  SPEED_10;
			ecmd->duplex  = DUPLEX_FULL;
			break;
		case LINK_100THD:			
			ecmd->speed =  SPEED_100;
			ecmd->duplex  = DUPLEX_HALF;
			break;
		case LINK_100TFD:						
			ecmd->speed =  SPEED_100;
			ecmd->duplex  = DUPLEX_FULL;
			break;
		case LINK_1000THD:						
			ecmd->speed =  SPEED_1000 ;
			ecmd->duplex  = DUPLEX_HALF;
			break;
		case LINK_1000TFD:		
			ecmd->speed =  SPEED_1000 ;
			ecmd->duplex  = DUPLEX_FULL;
			break;
		case LINK_10GT:			
			ecmd->duplex  = DUPLEX_FULL;
			ecmd->speed   =  SPEED_10000;	//??
			break;
		case LINK_100T4:						
			ecmd->speed =  SPEED_100;
			ecmd->duplex  = DUPLEX_FULL;	//??
			break;
		case LINK_25GT:			
			ecmd->duplex  = DUPLEX_FULL;
			ecmd->speed   =  SPEED_25000;	//??
			break;				
		default:
			ecmd->speed = SPEED_10;
			ecmd->duplex = DUPLEX_HALF;
			break;
	}

	//! Change the proc value according to the actual speed.
	UserSpeed = ecmd->speed;
	UserDuplex = ecmd->duplex;
	UserAutoNeg = ecmd->autoneg;
	
	/*! Link Partner values valid SerDes=0 AutoNegFlag=1 and AutoNegComplete=1 */
	if ((!(LinkStatus & LINK_AUTO_NEG_FLAG)) ||(!(LinkStatus & LINK_AUTO_NEG_COMPLETE)))
		return 1;
		

	if (LinkStatus & LINK_PARTNER_1000TFD)	
                ecmd->advertising |= ADVERTISED_1000baseT_Full;
	if (LinkStatus & LINK_PARTNER_1000THD)	
                ecmd->advertising |= ADVERTISED_1000baseT_Half;
	if (LinkStatus & LINK_PARTNER_100TXFD)	
                ecmd->advertising |= ADVERTISED_100baseT_Full;
	if (LinkStatus & LINK_PARTNER_100TXHD)	
                ecmd->advertising |= ADVERTISED_100baseT_Half;
	if (LinkStatus & LINK_PARTNER_10TFD)		
                ecmd->advertising |= ADVERTISED_10baseT_Full;
	if (LinkStatus & LINK_PARTNER_10THD)		
                ecmd->advertising |= ADVERTISED_10baseT_Half;
	if (LinkStatus & LINK_PARTNER_100T4)
                ecmd->advertising |= ADVERTISED_100baseT_Full;	 //??

	return 1;
}


/* Called when a channel is enabled */
void
InitEthtoolInfo(NCSI_IF_INFO *info)
{
	int index;
	//struct ethtool_cmd *ecmd;
	UINT32 LinkStatus;

	//ecmd = &(info->ecmd);
	
	/* If link is up on any of the enabled channel, then return link up */
	for (index = 0; index < MAX_CHANNELS ; index ++)
	{
		if (info->ChannelInfo[index].Enabled == 0)
			continue;

		msleep(1000); //Quanta: //mdelay(1000);  //Adding a delay of 1 second before issuing GetLinkStatus command to get the proper link status
		if (NCSI_Issue_GetLinkStatus(info,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID,&LinkStatus) != NCSI_ERR_SUCCESS)
		{
			printk("NCSI(%s): ERROR: GetLinkStatus(%d.%d) Failed \n",info->dev->name,info->ChannelInfo[index].PackageID,info->ChannelInfo[index].ChannelID);
			continue;
		}

		/* Note: we are returning values only for the first found channel with link up */
		if (LinkStatus & LINK_STATUS_UP)
		{
			GetEthtoolInfoFromLink (info,LinkStatus);
			return;
		}
	}
	
	/* Set the default values */
	GetEthtoolInfoFromLink (info,0);
	return;
}


