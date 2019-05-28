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

#define  RELOCATE_OFFSET		0x380

#define  IOCTL_IO_READ                 _IOR('j', 0, int)            
#define  IOCTL_IO_WRITE                _IOW('j', 1, int)            
#define  IOCTL_REMAP                   _IOW('j', 2, int)
#define  IOCTL_REAL_IO_READ            _IOR('j', 3, int) 
#define  IOCTL_REAL_IO_WRITE           _IOW('j', 4, int)
#define  IOCTL_BIT_STREAM_BASE         _IOW('j', 5, int)    
#define  IOCTL_TX_BIT_STREAM           _IOW('j', 6, int)    
#define  IOCTL_GET_SOCKET              _IOR('j', 7, int)
#define  IOCTL_AUTOMODE_TRIGGER        _IOW('j', 8, int)    
#define  IOCTL_PASS3_TRIGGER           _IOW('j', 9, int)    
#define  IOCTL_I2C_READ                _IOR('j', 10, int) 
#define  IOCTL_I2C_WRITE               _IOW('j', 11, int)
#define  IOCTL_JTAG_RESET              _IOW('j', 12, int)
#define  IOCTL_JTAG_IDCODE_READ        _IOR('j', 13, int)
#define  IOCTL_JTAG_ERASE_DEVICE       _IOW('j', 14, int)
#define  IOCTL_JTAG_PROGRAM_DEVICE     _IOW('j', 15, int)
#define  IOCTL_JTAG_VERIFY_DEVICE      _IOW('j', 16, int)
#define  IOCTL_JTAG_DEVICE_CHECKSUM    _IOW('j', 17, int)
#define  IOCTL_JTAG_DEVICE_TFR         _IOW('j', 18, int)
#define  IOCTL_JTAG_UPDATE_DEVICE      _IOW('j', 19, int)
//Quanta++
//Need to check the magic number can sync to driver
#define  IOCTL_JTAG_UES_READ           _IOR('j', 20, int)
//Quanta==
#define  IOCTL_JTAG_DEVICE_INDEX	   _IOW('j', 21, int)

//Altera
#define  IOCTL_JTAG_READ_USERCODE      _IOW('j', 22, int)
#define  IOCTL_JTAG_SET_IO             _IOW('j', 23, int)
#define  IOCTL_JTAG_JBC                _IOW('j', 24, int)

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

