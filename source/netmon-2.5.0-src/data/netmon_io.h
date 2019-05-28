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

  Module	: Network Interface Monitor Ioctl defines

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#ifndef __NETMON_IO__
#define __NETMON_IO__

#include <linux/netdevice.h>	/* Needed for IFNAMSIZ */
#include <linux/ioctl.h>

/*Ioctls */
#define NETMON_GET_INTERFACE_COUNT 	 _IOC(_IOC_READ, 'N', 0x00, 0x3FFFF)
#define NETMON_GET_INTERFACE_LIST	 _IOC(_IOC_READ, 'N', 0x01, 0x3FFFF)
#define NETMON_WAIT_FOR_INTERFACE_CHANGE _IOC(_IOC_READ, 'N', 0x02, 0x3FFFF)
#define NETMON_WAIT_FOR_LINK_UP 	 _IOC(_IOC_READ, 'N', 0x03, 0x3FFFF)
#define NETMON_WAIT_FOR_LINK_DOWN	 _IOC(_IOC_READ, 'N', 0x04, 0x3FFFF)
#define NETMON_WAIT_FOR_LINK_CHANGE	 _IOC(_IOC_READ, 'N', 0x05, 0x3FFFF)


#define NETMON_WAKE_UP 1
#define NETMON_SLEEP 0

typedef struct
{
	int linkstate;
	char ifname[IFNAMSIZ];
} LINK_CHANGE_INFO;


typedef struct
{
	int count;
	char *ifname;   /* List of ASCIIZ ifnames at each IFNAMSIZE+1 boundary */
	unsigned char *ifupstatus;
} INTERFACE_LIST;



#endif  /* NETMON_IOCTL_H */

/****************************************************************************/
