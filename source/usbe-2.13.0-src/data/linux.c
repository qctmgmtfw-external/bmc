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

#include <linux/init.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>	
#include <asm/io.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <coreTypes.h>
#include <asm/atomic.h>
#include "header.h"
#include "arch.h"
#include "helper.h"
#include "dbgout.h"
#include "usb_module.h"
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
#include <linux/kthread.h>
#endif

static DECLARE_COMPLETION(DPC_Semaphore);
static DECLARE_COMPLETION(DPC_Init);
static DECLARE_COMPLETION(DPC_Exited);
static int DPCThreadRunning=1;
static struct timer_list poll_timer;
static int PendingBufs [NUM_TX_BUFS]; 
static int CurrentActiveBufNum;
static NET_DEVICE* GlobalNetDev;


/****************** OS Specific Functions  ***************************/
void
Usb_OsDelay(long msecs)
{
	wait_queue_head_t sleep;
	long timeout;
	int cond = 0;
	
	init_waitqueue_head(&sleep);

	timeout = (HZ*msecs)/1000;
	wait_event_interruptible_timeout(sleep, (cond == 1), timeout);

	return;
}


int
Usb_OsRegisterIsr(char *Name,int Vector,uint8 DevNo,uint8 epnum, uint32 Flags)
{
	unsigned long PtrNo = ((unsigned long)DevNo) | ((unsigned long)(epnum << 8));
	return request_irq(Vector,UsbDriverIsr,Flags,Name,(void *)PtrNo);
}

void
Usb_OsUnregisterIsr(char *Name,int Vector,uint8 DevNo,uint8 epnum, uint32 Flags)
{
	unsigned long PtrNo = ((unsigned long)DevNo) | ((unsigned long)(epnum << 8));
	free_irq(Vector,(void *)PtrNo); 
	return;
}

/********* OS specific implementation of Sleep/Wakeup with timeout **********/
long 
Usb_OsSleepOnTimeout(Usb_OsSleepStruct *Sleep,uint8 *Var,long msecs)
{
	
    long timeout;	/* In jiffies */
    uint8 *volatile Condition = Var;

	/* Convert milliseconds into jiffies*/
	timeout = (HZ*msecs)/1000;



	TDBG_FLAGGED(usbe, DEBUG_ARCH,"Sleeping on Sleep Structure 0x%lx for 0x%lx jiffies\n",
									(unsigned long)Sleep,timeout);

	/* Sleep on the Condition for a wakeup */
    if (msecs)
       return (wait_event_interruptible_timeout(Sleep->queue, (*Condition), timeout ));
    else
    {
        wait_event_interruptible(Sleep->queue, (*Condition));
        return 1;

    }
       

}


void 
Usb_OsWakeupOnTimeout(Usb_OsSleepStruct *Sleep)
{
	
	/* Wakeup Process */
    wake_up(&(Sleep->queue));

	TDBG_FLAGGED(usbe, DEBUG_ARCH,"Woken up Sleep Struture 0x%lx \n",(unsigned long )Sleep);

	return;
}

void 
Usb_OsInitSleepStruct(Usb_OsSleepStruct *Sleep)
{
	init_waitqueue_head(&(Sleep->queue));
	return;
}

/***************** OS Specific Implementation of DPC ***********************/
static
int
DPCDispatchThread(void *proc)
{
	DPC DPCprocess = NULL;

	/* Put in background and release our user space connections */
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	daemonize("USBDPC");
#endif
	/* Notify caller that DPC is Initialized */
	complete(&DPC_Init);
	/* Main DPC Processing starts here */
	DPCprocess = (DPC)proc;
	while (DPCThreadRunning)	
	{	
		wait_for_completion_interruptible(&DPC_Semaphore);
		if (!DPCThreadRunning)
		{
			TWARN("Terminating Kernel Thread\n");
			break;
		}
		if (DPCprocess)
		{
			(*DPCprocess)(NULL);
		}
		else
			TCRIT("WARNING: DPC Process is NULL\n");

	}
	complete(&DPC_Exited);
	return 0;
}

static
void
DPCStarter(void *proc)
{
	/* Start the local kernel thread which dispatches our DPC */
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	kernel_thread(DPCDispatchThread, proc,0);
#else
	kthread_run(DPCDispatchThread,proc,"USBDPC");
#endif

}

void
Usb_OsInitDPC(DPC UsbDeferProcess)
{
	/* Start the Thread */
	DPCStarter((void *)UsbDeferProcess);

	/* Wait for DPC to Initialize */
	wait_for_completion_interruptible(&DPC_Init);
	
	return;
}

void
Usb_OsWakeupDPC(void)
{
	complete(&DPC_Semaphore);
	return;
}

void
Usb_OsKillDPC(void)
{
	DPCThreadRunning = 0;
	complete(&DPC_Semaphore);
	/* Wait for DPCDispatchThread to exit */
	wait_for_completion_interruptible(&DPC_Exited);
	return;
}

/* Network Related Stuff */
/* Private structure to be present in both struct net_device and NET_DEVICE */

static void
SendEthernetPacketTimeout (unsigned long Ptr)
{
	struct net_device *dev = (struct net_device*) Ptr;

#if (LINUX_VERSION_CODE < KERNEL_VERSION (2,6,30))
	NET_DEVICE *Dev = dev->priv;
#else
	NET_DEVICE *Dev = (NET_DEVICE *)netdev_priv (dev);;
#endif
	Dev->stats.tx_errors++;
	Dev->NetTxTimeoutHandler ();
	return;
	
}

static void
StartTxTimeoutTimer (void)
{
	init_timer (&poll_timer);
	poll_timer.function = SendEthernetPacketTimeout;
	poll_timer.data = (unsigned long)GlobalNetDev->ethdev;
	poll_timer.expires = jiffies + HZ /10;
	add_timer (&poll_timer);
}

int SendEthernetPacket(struct sk_buff *skb, struct net_device *dev)
{
	int r;
	int len;
	char *data, shortpkt[ETH_ZLEN];
#if (LINUX_VERSION_CODE < KERNEL_VERSION (2,6,30))
	NET_DEVICE *Dev = (NET_DEVICE *)dev->priv;
#else
	NET_DEVICE *Dev = (NET_DEVICE *)netdev_priv (dev);
#endif
	for(r = 0; r < NUM_TX_BUFS; r++)
	{
		if(mutex_trylock(&Dev->TxBuff[r].lock))
			break;
	}

	if(r == NUM_TX_BUFS)
	{
		printk ("ERROR: did not get TX buffer and so dropping\n");
		Dev->stats.tx_dropped++;
		dev_kfree_skb_any(skb);
		return 0;
	}
	PendingBufs [r] = 0x55;
	data = skb->data;
	len = skb->len;
	if (len < ETH_ZLEN)
	{
		memset (shortpkt, 0, ETH_ZLEN);
		memcpy (shortpkt, skb->data, skb->len);
		len = ETH_ZLEN;
		data = shortpkt;
	}

	dev->trans_start = jiffies;
	
	Dev->TxBuff[r].skb = skb; // skb to be freed in Net_TxComplete()
	Dev->TxBuff[r].Sent = 0;
	Dev->TxBuff[r].Data = data;
	Dev->TxBuff[r].Size = len;
	Dev->stats.tx_packets++;
	Dev->stats.tx_bytes += len;
	
	Dev->TxBuff[r].Hits++;

	if(mutex_trylock(&Dev->txon))
	{
		CurrentActiveBufNum = r;
		StartTxTimeoutTimer ();
		Dev->NetTxHandler(&Dev->TxBuff[r]);
	}

	return 0;
}

void Net_TxComplete(NET_PACKET *Pkt)
{

	int r;

#if (LINUX_VERSION_CODE < KERNEL_VERSION (2,6,30))
	NET_DEVICE *Dev = Pkt->ethdev->priv;
#else
	NET_DEVICE *Dev = (NET_DEVICE *)netdev_priv (Pkt->ethdev);
#endif

	if (Pkt->skb)
		dev_kfree_skb_any(Pkt->skb);
	mutex_unlock(&Pkt->lock);
	PendingBufs [CurrentActiveBufNum] = 0;
	mutex_unlock(&Dev->txon);
	del_timer (&poll_timer);

	for (r = 0;  r < NUM_TX_BUFS; r++)
	{
		if (PendingBufs [r]) break;
	}

	if (r == NUM_TX_BUFS) return;

	if(mutex_trylock(&Dev->txon))
	{
		CurrentActiveBufNum = r;
		StartTxTimeoutTimer ();
		Dev->NetTxHandler(&Dev->TxBuff[r]);
	}

	return;
	
}

struct net_device_stats* GetEthernetStats(struct net_device *dev)
{

#if (LINUX_VERSION_CODE < KERNEL_VERSION (2,6,30))
		NET_DEVICE *Dev = (NET_DEVICE *)dev->priv;
#else
		NET_DEVICE *Dev = (NET_DEVICE *)netdev_priv (dev);
#endif
	
	return &Dev->stats;
}

static const struct net_device_ops amiusbeth_netdev_ops = {
	.ndo_get_stats          = GetEthernetStats,
	.ndo_start_xmit         = SendEthernetPacket,
	.ndo_change_mtu      = NULL,
};


static void SetupEthernet(struct net_device *dev)
{
	dev->netdev_ops = &amiusbeth_netdev_ops,

	ether_setup(dev);
	dev->tx_queue_len = 0;
	dev->flags &= ~IFF_MULTICAST;
	random_ether_addr(dev->dev_addr);
}

int Net_OSRegisterDriver(NET_DEVICE *Dev)
{
	int r;
	struct net_device *ethdev;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION (2,6,30))	
		struct NET_DEVICE *TempPriv;
#endif

	if((ethdev = alloc_netdev(sizeof(NET_DEVICE *), "usb%d", SetupEthernet)) == NULL)
		return -ENOMEM;
	
	ethdev->mtu = Dev->Mtu;

	GlobalNetDev = Dev;
	if((r = register_netdev(ethdev)) != 0)
	{
		printk ("Error: Failed registering network device\n");
		free_netdev(ethdev);
		return r;
	}
	Dev->ethdev = ethdev;
#if (LINUX_VERSION_CODE < KERNEL_VERSION (2,6,30))	
	ethdev->priv = Dev;

	memcpy(Dev->DevMac, ethdev->dev_addr, ETH_ALEN);
	
	for(r = 0; r < NUM_TX_BUFS; r++)
	{
		init_MUTEX(&Dev->TxBuff[r].lock);
		Dev->TxBuff[r].ethdev = ethdev;
	}
	
	init_MUTEX(&Dev->txon);

#else
	TempPriv = netdev_priv (ethdev);
	
	TempPriv->ethdev = ethdev;
	memcpy(TempPriv->DevMac, ethdev->dev_addr, ETH_ALEN);
	
	for(r = 0; r < NUM_TX_BUFS; r++)
	{
		mutex_init(&TempPriv->TxBuff[r].lock);
		TempPriv->TxBuff[r].ethdev = ethdev;
	}
	
	mutex_init(&TempPriv->txon);

	TempPriv->NetTxHandler = Dev->NetTxHandler;
	TempPriv->NetTxTimeoutHandler = Dev->NetTxTimeoutHandler;

	TempPriv->Mtu = Dev->Mtu;
	TempPriv->Priv = Dev->Priv;
	
#endif

	netif_start_queue (ethdev);
	return 0;

}

void Net_OSUnregisterDriver(NET_DEVICE *Dev)
{
	int r;
	
	netif_stop_queue(Dev->ethdev);
	
	unregister_netdev(Dev->ethdev);
	free_netdev(Dev->ethdev);
	
	Dev->ethdev = NULL;

	printk("Hits :");
	for(r = 0; r < NUM_TX_BUFS; r++)
	{
		printk(" %d", Dev->TxBuff[r].Hits);
	}
	printk("\n");
}




int Net_OSPushPacket(NET_DEVICE *Dev, uint8 *Data, int Len)
{
	struct sk_buff *skb;

	if((skb = dev_alloc_skb(Len+2)) == NULL)
	{
		printk ("Low Memory, Packet dropped\n");
		Dev->stats.rx_dropped++;
		return -ENOMEM;
	}
	Dev->ethdev->last_rx = jiffies;

	skb_reserve (skb,2);
	memcpy(skb_put(skb, Len), Data, Len);
	skb->dev = Dev->ethdev;
	skb->protocol = eth_type_trans(skb, Dev->ethdev);
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	
	switch(netif_rx(skb))
	{
	case NET_RX_SUCCESS:
		Dev->stats.rx_packets++;
		Dev->stats.rx_bytes += Len;
		return 0;
		
	case NET_RX_DROP:
		Dev->stats.rx_dropped++;
		return -EBUSY;
		
//	case NET_RX_CN_LOW:
//	case NET_RX_CN_MOD:
//	case NET_RX_CN_HIGH:
	default:
		break;
	}
	
	return -EBUSY;
}

