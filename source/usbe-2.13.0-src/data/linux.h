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

#ifndef ARCH_LINUX_H 
#define ARCH_LINUX_H


#include <linux/version.h>
#include <linux/types.h>		
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/major.h>
/* Changed for Linux from malloc.h to slab.h*/
#include <linux/slab.h>		
#include <linux/interrupt.h>
#include <linux/wait.h>
//#include <linux/tqueue.h>
#include <linux/timer.h>
#include <linux/vmalloc.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/semaphore.h>

#include <asm/unistd.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

typedef struct 
{
	wait_queue_head_t queue;

} Usb_OsSleepStruct;

/* Driver Isr */
irqreturn_t   UsbDriverIsr(int irq, void *dev_id);

/* OS specific data structures defined for uClinux  */
typedef wait_queue_head_t Usb_OsWaitStruct;


/* OS specific data structures for network driver */

#define NUM_TX_BUFS 6

struct NET_DEVICE;
struct NET_PACKET;

typedef struct NET_PACKET
{
	/* OS specific details */
	//struct semaphore lock;
	struct mutex lock;
	struct sk_buff *skb;
	struct net_device *ethdev;
	
	/* OS independent details */
	uint16 Sent;
	uint16 Size;
	uint16 Hits;
	uint8 *Data;

} NET_PACKET;

typedef struct NET_DEVICE
{
	/* OS specific details */
	struct net_device *ethdev;
	struct net_device_stats stats;
	//struct semaphore txon;
	struct mutex txon;
	
	/* OS independent details */
	int Mtu;
	int (*NetTxHandler)(NET_PACKET *Pkt);
	int (*NetTxTimeoutHandler) (void);
	uint8 DevMac[ETH_ALEN];
	uint8 HostMac[ETH_ALEN];
	struct NET_PACKET TxBuff[NUM_TX_BUFS];
	void *Priv;
} NET_DEVICE;



#endif /* ARCH_LINUX_H */

