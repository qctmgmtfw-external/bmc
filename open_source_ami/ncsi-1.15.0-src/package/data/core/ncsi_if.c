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

  Module	: NCSI Driver Entry

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#ifdef MODULE


#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/errno.h>
#include <linux/in.h>
#include <linux/mm.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/workqueue.h>
#ifdef CONFIG_SYSCTL
#include <linux/sysctl.h>
#endif
#include <net/protocol.h>
#include <net/sock.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include "ncsi.h"
#include "interfaces.h"


extern int verbose;

struct workqueue_struct *ncsi_wq = NULL;

static int 
IsNetworkInterface(char *deviceName)
{
	char interfaceName[MAX_IF_NAME_LEN+1];
	int i = 0;

	memset(interfaceName, 0, sizeof(interfaceName));

	for (i = 0; i < MAX_NET_IF; i++)
	{
		snprintf(interfaceName, sizeof(interfaceName), "%s%d", "eth", i);
		if (strcmp(deviceName, interfaceName) == 0) return 0;
	}

	return -1;
}

int 
ncsi_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, 
							struct net_device *orig_dev)
{
	ETH_HDR  *EthHdr;
	NCSI_HDR *hdr;
	NCSI_IF_INFO *info;
	int i;
	int paylen;

	if (verbose & SHOW_MESSAGES)
		printk("NCSI(%s): Received Packet\n",dev->name);

	if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL) 
		goto out;

	if (verbose & SHOW_EXT_MESSAGES)
		printk("NCSI(%s): SKB Packet recieved from %s\n",dev->name,skb->dev->name);

	if (!pskb_may_pull(skb, sizeof(NCSI_HDR)))
		goto hdr_error;

	/* Validate all 0xFF in Dest MAC Address - Signature of NCSI */
	EthHdr = (ETH_HDR *)(skb_mac_header(skb));
	for (i=0;i<MAC_ADDR_LEN;i++)
	{	
		if (EthHdr->DestMACAddr[i] != 0xFF)
		{
			printk("NCSI(%s):  Invalid Packet (Dest Mac != All 0xFF)\n",dev->name);
			goto drop;
		}
	}

	if (verbose & SHOW_EXT_MESSAGES)
		printk("NCSI(%s): Valid Packet. Dest MAC Addr All 0xFF\n",dev->name);

	/* Get NCSI header and extract payload length */	
	hdr = (NCSI_HDR *)(skb_network_header(skb));
	paylen = be16_to_cpu(hdr->PayloadLen) + 4; 	/* +4 = CRC32 */

	if (verbose & SHOW_EXT_MESSAGES)
		printk("NCSI(%s): Packet Payload = %d\n",dev->name,paylen);

	if (!pskb_may_pull(skb, sizeof(NCSI_HDR)+paylen))
		goto pkt_error;
	
	info = GetInterfaceInfo(dev);
	if (info == NULL)
	{
		/* Normally during bonding , bond0 will recevie the NCSI responses intead of the actual physical interface */
		/* So to get info, we use the default interface set in the project configuraiton. */
		/* TODO: This will not work with more then one NCSI interfaces. Need a proper fix */
		info = GetInterfaceInfoByName(CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
		if (info == NULL)
		{
			printk("NCSI(%s): ERROR: Unable to get interface Info for \n",CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE);
			goto drop;
		}
	}
	
	/* Copy Data to our internal data */
	info->RecvLen = skb->len;
	memcpy(info->RecvData,skb_network_header(skb),info->RecvLen);

	ProcessNCSI(info);

	kfree_skb(skb);
	return NET_RX_SUCCESS;

	/*------------------------- Error Paths -----------------------------*/
hdr_error:
	printk("NCSI(%s): Unable to get NCSI Header\n",dev->name);
	goto drop;
pkt_error:
	printk("NCSI(%s): Unable to get NCSI PayLoad\n",dev->name);
	goto drop;
drop:
	kfree_skb(skb);
out:
	return NET_RX_DROP;
}

#ifndef NCSI_DEFER_DETECT
static
void
detect_ncsi_fg(char *devname)
{
	struct net_device *dev;
	NCSI_IF_INFO *info;

	if (IsNetworkInterface(devname) != 0)
	{
		if (verbose & SHOW_MESSAGES)
			printk("NCSI: Dev %s is detected as virtual and hence NCSI interface is not available\n", devname);
		return;
	}

	dev = dev_get_by_name(&init_net,devname);
	if (dev == NULL)
	{
		printk("NCSI: ERROR: Unable to get device for %s\n",devname);
		return;
	}

	/* Add the Interface to list */
	info=AddNetInterface(dev);

	if(rtnl_trylock() == 1)
	{
		dev_open(dev);
		rtnl_unlock();
	}
	else
	{
	       dev_open(dev);
	}

	NCSI_Detect_Info(info);
	NCSI_Enable_Info(info);
	

	return;
}
#endif

static int 
ncsi_netdev_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct net_device *dev = ptr;
#ifdef NCSI_DEFER_DETECT
	NCSI_IF_INFO *info;
#endif

	/* Handle only ethX devices. Don't handle others */
	if (dev->type != ARPHRD_ETHER)
		return NOTIFY_DONE;

	switch (event) 
	{
#ifdef NCSI_DEFER_DETECT
		case NETDEV_REGISTER:
			if (verbose & SHOW_MESSAGES)
				printk("NCSI: NetDev Register Event for %s\n",dev->name);

			/* Check if device is virtual interface device and if so don't use for NCSI */
			if (IsNetworkInterface(dev->name) != 0)
			{
				if (verbose & SHOW_MESSAGES)
					printk("NCSI: Dev %s is detected as virtual and hence NCSI interface is not available\n", dev->name);

				break;
			}

			/* Add the Interface to list */
			info=AddNetInterface(dev);
			/* If interface is not yet UP, we make it UP */
			dev_open(dev);
			/* Schedule work to detect and enable NCSI */
			if (info != NULL)
			{
				queue_work(ncsi_wq,&info->detect_work);
				queue_work(ncsi_wq,&info->enable_work);
			}
			break;
		case NETDEV_UNREGISTER:
			if (verbose & SHOW_MESSAGES)
				printk("NCSI: NetDev UnRegister Event for %s\n",dev->name);

			/* Check if device is virtual interface device and if so don't use for NCSI */
			if (IsNetworkInterface(dev->name) != 0)
			{
				if (verbose & SHOW_MESSAGES)
					printk("NCSI: Dev %s is detected as virtual and hence NCSI interface is not available\n", dev->name);

				break;
			}

			/* Get Interface Info */
			info=GetInterfaceInfo(dev);	
			if (info != NULL)
				queue_work(ncsi_wq,&info->disable_work);
			/* Remove the interface from list */
			RemoveNetInterface(dev);
			break;
#else
		case NETDEV_REGISTER:
			if (verbose & SHOW_MESSAGES)
				printk("INFO: NetDev Register Event for %s\n",dev->name);
			break;

		case NETDEV_UNREGISTER:
			if (verbose & SHOW_MESSAGES)
				printk("INFO: NetDev UnRegister Event for %s\n",dev->name);
			break;
#endif


		case NETDEV_CHANGEADDR:
			if (verbose & SHOW_MESSAGES)
				printk("NCSI : NetDev Addr Change Event for %s\n",dev->name);
			NCSI_Change_MacAddr(dev);
			break;

#if 0
		case NETDEV_UP:
			printk("INFO: NetDev Up Event for %s\n",dev->name);
			break;
		case NETDEV_DOWN:
			printk("INFO: NetDev Down Event for %s\n",dev->name);
			break;
		case NETDEV_CHANGEMTU:
			printk("INFO: NetDev MTU  Change Event for %s\n",dev->name);
			break;
		case NETDEV_CHANGENAME:
			printk("INFO: NetDev Name Change Event for %s\n",dev->name);
			break;
		case NETDEV_CHANGE:
			printk("INFO: NetDev State Change Event for %s\n",dev->name);
			break;
		case NETDEV_GOING_DOWN:	
			printk("INFO: NetDev Going Down Event for %s\n",dev->name);
			break;
		case NETDEV_REBOOT:	
			printk("INFO: NetDev Reboot Event for %s\n",dev->name);
			break;
		case NETDEV_FEAT_CHANGE:
			printk("INFO: NetDev Feature Change Event for %s\n",dev->name);
			break;
		case NETDEV_BONDING_FAILOVER:
			printk("INFO: NetDev Bonding Failover Event for %s\n",dev->name);
			break;
		default:
			printk("INFO: WARNING: NetDev Unhandled (%ld) Event for %s\n",event,dev->name);
			break;
#endif
	}
	return NOTIFY_DONE;
}

static struct notifier_block ncsi_netdev_notifier = 
{
	.notifier_call = ncsi_netdev_event,
};

static struct packet_type ncsi_packet_type = 
{
	.type =	__constant_htons(ETH_P_NCSI),
	.func =	ncsi_rcv,
};

extern int CreateProcSys(void);
extern int RemoveProcSys(void);


static int
init_ncsi_module(void)
{
	int i;
	char interfaceName[MAX_IF_NAME_LEN+1];

	printk("NCSI Interface Driver Version %d.%d\n",NCSI_DRIVER_MAJOR,NCSI_DRIVER_MINOR);
	printk("Copyright (c) 2007 American Megatrends Inc.\n");

	ncsi_wq = create_workqueue("NCSI");
	if (ncsi_wq == NULL)
	{
		printk("NCSI: ERROR: Unable to create Work queue \n");
		return 1;
	}

	if (CreateProcSys())
		return 1;

	dev_add_pack(&ncsi_packet_type);
	
	register_netdevice_notifier(&ncsi_netdev_notifier);

#ifndef NCSI_DEFER_DETECT
	for (i=0;i<MAX_NET_IF;i++)
	{
		sprintf(interfaceName,"eth%d",i);
		detect_ncsi_fg(interfaceName);
	}
#endif

	return 0;
}


static void
exit_ncsi_module(void)
{
	RemoveProcSys();

	unregister_netdevice_notifier(&ncsi_netdev_notifier);
	
	dev_remove_pack(&ncsi_packet_type);
	
	flush_workqueue(ncsi_wq);
	destroy_workqueue(ncsi_wq);
	RemoveAllNetInterfaces();

	printk("NCSI Interface driver unloaded sucessfully.\n");
	return;
}

module_init (init_ncsi_module);
module_exit (exit_ncsi_module);

MODULE_AUTHOR("Samvinesh Christopher- American Megatrends Inc");
MODULE_DESCRIPTION("NCSI Interface");
MODULE_LICENSE("GPL");

#endif

