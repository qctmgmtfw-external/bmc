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

#ifndef __JTAG_H__
#define __JTAG_H__

#include <linux/types.h>//

typedef struct
{
	uint32_t (*get_hw_device_idcode) (unsigned long *id_code);	
	uint32_t (*set_hw_device_ctl) ( unsigned int cmd, unsigned long *buf, unsigned long data_size ) ;
} jtag_hw_device_operations_t;


typedef struct 
{
	void (*reset_jtag) (void);
} jtag_hal_operations_t;


typedef struct
{
	void (*get_jtag_core_data) ( int dev_id );
} jtag_core_funcs_t;


struct jtag_hal
{
	jtag_hal_operations_t *pjtag_hal_ops;
};


struct jtag_dev
{
	struct jtag_hal *pjtag_hal;
};

typedef struct _JTAG_DEVICE_INFO {
	unsigned char	Device_Name[64];
	unsigned int	Device_ID;
	unsigned long	Device_Column_Length;
	unsigned long	Device_Row_Length;
	unsigned long	Device_All_Bits_Length;
	unsigned long	Device_Bscan_Length;
	unsigned long	Device_Fuses_Length;
} JTAG_DEVICE_INFO;

extern JTAG_DEVICE_INFO	JTAG_device_information;
extern int register_jtag_hw_device_ops(jtag_hw_device_operations_t *pjhwd_ops);
extern int unregister_jtag_hw_device_ops(void);
extern unsigned int* get_jtag_write_buffer(void);
extern unsigned int* get_jtag_read_buffer(void);

#endif

