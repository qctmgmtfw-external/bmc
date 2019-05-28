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

/*
 * driver hal header
 *
 * Copyright (C) 2009-2015 American Megatrends Inc.
 *
 * Author : Jothiram Selvam <jothirams@ami.com>
 *
 * This provides driver independent hardware abstraction layer header functions
 */

#ifndef DRIVER_HAL_H
#define DRIVER_HAL_H

#define MAX_DEV_INSTANCES 255

typedef enum
{
	EDEV_TYPE_KCS,
	EDEV_TYPE_RESET,
	EDEV_TYPE_SNOOP,
	EDEV_TYPE_PECI,
	EDEV_TYPE_ADC,
	EDEV_TYPE_PWMTACH,
	EDEV_TYPE_GPIO,
	EDEV_TYPE_I2C,
	EDEV_TYPE_USB,
	EDEV_TYPE_VIDEO,
	EDEV_TYPE_WATCHDOG,
	EDEV_TYPE_HOST_SPI_FLASH,
	EDEV_TYPE_CIPHER,
	EDEV_TYPE_BT,
	EDEV_TYPE_ACPI,
	EDEV_TYPE_JTAG,
	EDEV_TYPE_MCTP_PCIE,
	EDEV_TYPE_SSP,
	EDEV_TYPE_MAILBOX,
} ehal_dev_type;

typedef struct
{
	struct module *owner;
	char name[48];
	ehal_dev_type dev_type;
	int (*register_hal_module) (unsigned char num_instances, void *phal_ops, void **phw_data);
	int (*unregister_hal_module) (void *phw_data);
	void *pcore_funcs;	
} core_hal_t;

typedef struct
{
	struct list_head list;
	core_hal_t *core_hal;
	struct list_head hw_list;	
} core_hal_list_t;

typedef struct
{
	ehal_dev_type dev_type;
	struct module *owner;	
	const char *devname;
	unsigned char num_instances;
	void *phal_ops;	
} hw_hal_t;

typedef struct
{
	struct list_head list;
	int dev_id;
	unsigned char minor_num;
	unsigned char *opencount;
	hw_hal_t *hw_hal;
	void *pdrv_data;
} hw_hal_list_t;

typedef struct
{
	int inst_num;
	void *pdrv_data;
} hw_info_t;

extern int register_core_hal_module (core_hal_t *pcore_hal);
extern int unregister_core_hal_module (ehal_dev_type dev_type);
extern int register_hw_hal_module (hw_hal_t *phw_hal, void **pcore_funcs);
extern int unregister_hw_hal_module (ehal_dev_type dev_type, unsigned char dev_id);

int hw_open (ehal_dev_type dev_type, unsigned char minor_num, unsigned char *popen_count, hw_info_t *phw_info);
int hw_close (ehal_dev_type dev_type, unsigned char minor_num, unsigned char *popen_count);
void* hw_intr (ehal_dev_type dev_type, int dev_id);

#endif /* DRIVER_HAL_H */

