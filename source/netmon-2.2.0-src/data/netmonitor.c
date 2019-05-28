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

  Module	: Network Interface Monitor

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#ifdef MODULE

#define NETMON_MAJOR 125

#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include "netmon_io.h"

#ifdef HAVE_UNLOCKED_IOCTL
  #if HAVE_UNLOCKED_IOCTL
	#define USE_UNLOCKED_IOCTL
  #endif
#endif

#ifdef NETMON_DEBUG
int verbose=1;
#else
int verbose=0;
#endif

typedef struct 
{
	struct list_head list;
	char name[IFNAMSIZ+1];
	unsigned char upstatus;
} ifname_list;

static int  if_count=0;
//static int netmonwakeup=NETMON_SLEEP;

static int linkup_wakeup=NETMON_SLEEP;
static int linkdown_wakeup=NETMON_SLEEP;;
static int linkchange_wakeup=NETMON_SLEEP;;
static int linkstate=0;
struct net_device *linkdev=NULL;


static LIST_HEAD(if_list);
static DEFINE_MUTEX(if_list_lock);
static DECLARE_WAIT_QUEUE_HEAD(monitor_event);
static DECLARE_WAIT_QUEUE_HEAD(link_event);

static atomic_t net_event, listener_count;


static
int
AddInterface(char *name)
{
	int ret = 0;
	ifname_list *iface;

	mutex_lock(&if_list_lock);

 	list_for_each_entry (iface, &if_list, list)
        {
                if (strcmp(name,iface->name) == 0)
                {
                        ret = -EEXIST;
                        goto failed;
                }
        }

	iface = (ifname_list *)kmalloc(sizeof(ifname_list),GFP_KERNEL);
	if (!iface)
	{
		ret = -ENOMEM;
		goto failed;
	}
        memset(iface,0,sizeof(ifname_list));
	strcpy(iface->name,name);
	iface->upstatus = 0;
	
	list_add_tail(&(iface->list),&if_list);
	
	if_count++;
failed:
	mutex_unlock(&if_list_lock);
	return ret;
}

static
int
RemoveInterface(char *name)
{
	int ret= -EINVAL;
	ifname_list *iface;

	mutex_lock(&if_list_lock);

 	list_for_each_entry (iface, &if_list, list)
        {
                if (strcmp(name,iface->name) == 0)
		{
			list_del(&(iface->list));
			kfree(iface);		
			if_count--;
			ret= 0;
			break;	
                }
        }

	mutex_unlock(&if_list_lock);
	return ret;
}

static
int
UpdateInterfaceActiveStatus(char *name, unsigned char upstatus)
{
	int ret = 0;
	ifname_list *iface;

	mutex_lock(&if_list_lock);

 	list_for_each_entry (iface, &if_list, list)
        {
                if (strcmp(name,iface->name) == 0)
                {
					iface->upstatus = upstatus;
                }
        }

	mutex_unlock(&if_list_lock);
	return ret;
}

static int 
netmon_netdev_event(struct notifier_block *this, unsigned long event, void *ptr)
{
#if (LINUX_VERSION_CODE >KERNEL_VERSION(3,4,11))
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
#else
	struct net_device *dev = ptr;
#endif

	/* Handle only Ethernet devices. Don't handle others */
	if (dev->type != ARPHRD_ETHER)
		return NOTIFY_DONE;

	//Allow only physical interfaces that can get IP. Do not allow virtual interfaces!!!!!
	if (strncmp(dev->name,"eth", 3) && strncmp(dev->name, "bond", 4)  && strncmp(dev->name, "lo", 2) && strncmp(dev->name, "usb", 3))
		return NOTIFY_DONE;

	switch (event) 
	{
		case NETDEV_REGISTER:
			if (verbose)
				printk("NETMON: NetDev Register Event for %s\n",dev->name);
			AddInterface(dev->name);
			atomic_inc(&net_event);
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_UNREGISTER:
			if (verbose)
				printk("NETMON: NetDev UnRegister Event for %s\n",dev->name);
			RemoveInterface(dev->name);
			atomic_inc(&net_event);
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_UP:
			if (verbose)
				printk("NETMON: NetDev Up Event for %s\n",dev->name);
			UpdateInterfaceActiveStatus(dev->name, 1);
			atomic_inc(&net_event);
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_DOWN:
			if (verbose)
				printk("NETMON: NetDev Down Event for %s\n",dev->name);
			UpdateInterfaceActiveStatus(dev->name, 0);
			atomic_inc(&net_event);
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_LINK_DOWN:
			if (verbose)
				printk("NETMON: NetDev Link Down Event for %s\n",dev->name);
			linkdev=dev;
			linkstate=0;
			linkdown_wakeup= NETMON_WAKE_UP;
			linkchange_wakeup= NETMON_WAKE_UP;
			wake_up_interruptible_all(&link_event);
			break;
		case NETDEV_LINK_UP:
			if (verbose)
				printk("NETMON: NetDev Link Up Event for %s\n",dev->name);
			linkdev=dev;
			linkstate=1;
			linkup_wakeup= NETMON_WAKE_UP;
			linkchange_wakeup= NETMON_WAKE_UP;
			wake_up_interruptible_all(&link_event);
			break;
		case NETDEV_CHANGE:
			if (verbose)
				printk("NETMON: NetDev Link Change Event for %s\n",dev->name);
			linkdev = dev;
			/* netif_carrier_ok() => 1 (carrier connected) else 0 */
			linkstate = netif_carrier_ok(dev);

			if(linkstate) /* link UP */
				linkup_wakeup = NETMON_WAKE_UP;
			else	
				linkdown_wakeup = NETMON_WAKE_UP;

			linkchange_wakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&link_event);
			break;
#if 0		// Change to 0 after debugging and testing is completed 
		case NETDEV_CHANGEADDR:
			printk("NETMON: NetDev Addr Change Event for %s\n",dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_CHANGEMTU:
			printk("NETMON: NetDev MTU  Change Event for %s\n",dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_CHANGENAME:
			printk("NETMON: NetDev Name Change Event for %s\n",dev->name);
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_CHANGE:
			printk("NETMON: NetDev State Change Event for %s\n",dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_GOING_DOWN:	
			printk("NETMON: NetDev Going Down Event for %s\n",dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_REBOOT:	
			printk("NETMON: NetDev Reboot Event for %s\n",dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_FEAT_CHANGE:
			printk("NETMON: NetDev Feature Change Event for %s\n",dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
		case NETDEV_BONDING_FAILOVER:
			printk("NETMON: NetDev Bonding Failover Event for %s\n",dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
		default:
			printk("NETMON: WARNING: NetDev Unhandled (%ld) Event for %s\n",event,dev->name);
			netmonwakeup = NETMON_WAKE_UP;
			wake_up_interruptible_all(&monitor_event);
			break;
#endif
	}
	return NOTIFY_DONE;
}


static long     
NetmonIoctlUnlocked(struct file *file, unsigned int cmd,unsigned long arg)
{
	INTERFACE_LIST *iface_arg;
	char *ifname;
	char *ifupstatus;
	LINK_CHANGE_INFO linkinfo;
	
 	switch (cmd)
        {
                case NETMON_GET_INTERFACE_COUNT:
			if (__copy_to_user((void *)arg,(void*)&if_count,sizeof(int)))
			{		
				printk("ERROR: NETMON: Get Interface Count Failed\n");
				return -EFAULT;
			}
			break;

                case NETMON_GET_INTERFACE_LIST:
			{
				ifname_list *iface;

				iface_arg=(INTERFACE_LIST *)arg;

				if (__copy_to_user((void *)&iface_arg->count,(void*)&if_count,sizeof(int)))	
				{
					printk("ERROR: NETMON: Get Interface List Failed. Unable to copy length\n");
					return -EFAULT;
				}
			
				ifname=iface_arg->ifname;
				ifupstatus = iface_arg->ifupstatus;
		 		list_for_each_entry (iface, &if_list, list)
				{
					if (__copy_to_user((void *)ifname, (void *)iface->name,IFNAMSIZ+1))
					{
						printk("ERROR: NETMON: Get Interface List Failed. Unable to interface %s\n",iface->name);
						return -EFAULT;
					}
					if (__copy_to_user((void *)ifupstatus,(void *)&iface->upstatus,sizeof(unsigned char)))	
					{
						printk("ERROR: NETMON: Get Interface List Failed. Unable to interface %s up status\n",iface->name);
						return -EFAULT;
					}
					ifname+=IFNAMSIZ+1;
					ifupstatus+=1;
				}				
			}
			break;

		case NETMON_WAIT_FOR_INTERFACE_CHANGE:

			/*EAGAIN is returned to avoid awakening same listener twice for one event*/
			if((atomic_read(&net_event) > 0) && (atomic_read(&listener_count)> 0))
			{
				return -EAGAIN;
			}
			atomic_inc (&listener_count);
			
			wait_event_interruptible(monitor_event,(atomic_read(&net_event) > 0));
			if ((atomic_read (&listener_count) > 0))
			{
				atomic_dec (&listener_count);
			}
			if ((atomic_read (&listener_count) == 0))
			{
				atomic_dec(&net_event);
			}
			break;

		case NETMON_WAIT_FOR_LINK_CHANGE:
			wait_event_interruptible(link_event,(linkchange_wakeup == NETMON_WAKE_UP));
			linkchange_wakeup = NETMON_SLEEP;
			linkinfo.linkstate=linkstate;
			if(linkdev != NULL)
				memcpy(linkinfo.ifname,linkdev->name,IFNAMSIZ);
			if (__copy_to_user((void *)arg,(void*)&linkinfo,sizeof(linkinfo)))
			{		
				printk("ERROR: NETMON: Wait for Link Change Failed\n");
				return -EFAULT;
			}
			break;

		case NETMON_WAIT_FOR_LINK_UP:
			wait_event_interruptible(link_event,(linkup_wakeup == NETMON_WAKE_UP));
			linkup_wakeup = NETMON_SLEEP;
			if (__copy_to_user((void *)arg,(void*)&linkdev->name,IFNAMSIZ))
			{		
				printk("ERROR: NETMON: Wait for Link Up Failed\n");
				return -EFAULT;
			}
			break;

		case NETMON_WAIT_FOR_LINK_DOWN:
			wait_event_interruptible(link_event,(linkdown_wakeup == NETMON_WAKE_UP));
			linkdown_wakeup = NETMON_SLEEP;
			if (__copy_to_user((void *)arg,(void*)&linkdev->name,IFNAMSIZ))
			{		
				printk("ERROR: NETMON: Wait for Link Down Failed\n");
				return -EFAULT;
			}
			break;

                default:
                        printk("ERROR: Netmon: Unknown ioctl\n");
                        return(-EINVAL);
        }

	return 0;
}

#ifndef USE_UNLOCKED_IOCTL 
static int     
NetmonIoctl(struct inode * inode, struct file * file, unsigned int cmd,unsigned long arg)
{
	return NetmonIoctlUnlocked(file,cmd,arg);
}
#endif

#if 0
static
ssize_t
NetmonRead(struct file * file , char * buf, size_t count, loff_t *ppos)
{
	ifname_list *iface;

	mutex_lock(&if_list_lock);
	printk("Number of Interfaces = %d\n",if_count);

 	list_for_each_entry (iface, &if_list, list)
        {
		printk("Interface : [%s]\n",iface->name);
        }

	mutex_unlock(&if_list_lock);
        return (-EIO);
}
#endif




static struct notifier_block netmon_netdev_notifier = 
{
	.notifier_call = netmon_netdev_event,
};

struct file_operations netmon_fops =
{
#ifdef USE_UNLOCKED_IOCTL 
       	.unlocked_ioctl  =     NetmonIoctlUnlocked,
#else
       	.ioctl  	=      NetmonIoctl, 
#endif
//	.read	=	NetmonRead,
};



static int
init_netmon_module(void)
{
	printk("Network Interface Monitor Version %d.%d.%d\n",PKG_MAJOR,PKG_MINOR,PKG_AUX);
	printk("Copyright (c) 2009-2015 American Megatrends Inc.\n");
        if (register_chrdev(NETMON_MAJOR, "netmon",  &netmon_fops) < 0)
        {
                printk("ERROR: Unable to register Netmon driver\n");
                return -EBUSY;
        }

	atomic_set(&net_event,0);
	atomic_set(&listener_count,0);
	register_netdevice_notifier(&netmon_netdev_notifier);

	return 0;
}


static void
exit_netmon_module(void)
{
	unregister_netdevice_notifier(&netmon_netdev_notifier);
        unregister_chrdev(NETMON_MAJOR,"netmon");
	return;
}

module_init (init_netmon_module);
module_exit (exit_netmon_module);

MODULE_AUTHOR("Samvinesh Christopher- American Megatrends Inc");
MODULE_DESCRIPTION("Network Interface Monitor");
MODULE_LICENSE("GPL");

#endif

