/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef _JTAG_IOCTL_H_
#define _JTAG_IOCTL_H_

#include <linux/socket.h>
#include <linux/tcp.h>

#define  IOCTL_IO_READ			0x1103
#define  IOCTL_IO_WRITE			0x1104
#define  IOCTL_REMAP			0x1105
#define  IOCTL_REAL_IO_READ		0x1106
#define  IOCTL_REAL_IO_WRITE		0x1107
#define  IOCTL_BIT_STREAM_BASE		0x1108
#define  IOCTL_TX_BIT_STREAM		0x1109
#define  IOCTL_GET_SOCKET		0x1110
#define  IOCTL_AUTOMODE_TRIGGER		0x1111
#define  IOCTL_PASS3_TRIGGER		0x1112
#define  IOCTL_I2C_READ			0x1113
#define  IOCTL_I2C_WRITE		0x1114
#define  IOCTL_JTAG_RESET		0x1120
#define  IOCTL_JTAG_IDCODE_READ		0x1121
#define  IOCTL_JTAG_ERASE_DEVICE		0x1122
#define  IOCTL_JTAG_PROGRAM_DEVICE	0x1123
#define  IOCTL_JTAG_VERIFY_DEVICE		0x1124
#define  IOCTL_JTAG_DEVICE_CHECKSUM	0x1125
#define  IOCTL_JTAG_DEVICE_TFR	    0x1126
#define  IOCTL_JTAG_UPDATE_DEVICE	    0x1127
#define  IOCTL_JTAG_UES_READ        0x1150//Quanta Add from 1150 cause AMI could update ioctl number

#define  RELOCATE_OFFSET		0x380

typedef struct _IO_ACCESS_DATA {
	unsigned char	Type;
	unsigned long	Address;
	unsigned long	Data;
	unsigned long	Value;
	unsigned long	I2CValue;
	int		kernel_socket;
	unsigned long	Input_Buffer_Base;
} IO_ACCESS_DATA, *PIO_ACCESS_DATA;

typedef IO_ACCESS_DATA jtag_ioaccess_data;

#endif /* _JTAG_IOCTL_H_ */

