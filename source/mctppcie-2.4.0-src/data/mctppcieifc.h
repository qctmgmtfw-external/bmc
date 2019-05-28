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

#ifndef __MCTP_PCIE_IFC_H__
#define __MCTP_PCIE_IFC_H__

/* Device Properties */
#define MCTP_PCIE_MAJOR			104
#define MCTP_PCIE_MINOR			0
#define MCTP_PCIE_MAX_DEVICES 	255
#define MCTP_PCIE_DEV_NAME		"MCTP_over_PCIE"

#define MCTP_PCIE_DOWNSTREAM_CMD		1
#define MCTP_PCIE_UPSTREAM_CMD			2

typedef struct 
{
	unsigned char ds_pcie_vdm_buffer[768];
	unsigned char ds_wr_cntr;

}__attribute__((packed)) mctp_downstream;


// These defines are common for the kernel module and applications

typedef enum __mctp_status__{
	MCTP_STATUS_SUCCESS = 0,
	MCTP_STATUS_ERROR,
	MCTP_STATUS_OVERFLOW,
	MCTP_STATUS_LINK_DOWN
}mctp_status;

typedef struct __mctp_iodata__ {
	mctp_status status;
	u32 len;
	u8 data[768];
}mctp_iodata;


typedef struct __mctp_payload__ {
	mctp_status status;
	u32 len;
	u8  *data;
	u8  routing;	// Routing type
	u8  res1[3];	// Alignment
	unsigned int target;		// BDF
}mctp_payload;


typedef struct __mctp_capabilities__ {
	mctp_status status;
	u32 state;
	u32 outsize;
	u32 insize;
	u32 mtu;
}mctp_capabilities;


#define	MCTP_IOCTL_KEY			'm'

#define	MCTP_PCIE_IOCTL_READ			_IOWR(MCTP_IOCTL_KEY, 1, mctp_iodata)
#define	MCTP_PCIE_IOCTL_WRITE			_IOWR(MCTP_IOCTL_KEY, 2, mctp_iodata)
#define	MCTP_PCIE_IOCTL_CAPABILITIES	_IOWR(MCTP_IOCTL_KEY, 3, mctp_capabilities)
#define	MCTP_PCIE_IOCTL_SEND_PAYLOAD	_IOWR(MCTP_IOCTL_KEY, 4, mctp_payload)



#endif


