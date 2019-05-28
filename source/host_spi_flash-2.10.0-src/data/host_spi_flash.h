/*****************************************************************
 *****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 *****************************************************************
 *****************************************************************/

#ifndef __HOST_SPI_FLASH_H__
#define __HOST_SPI_FLASH_H__

#include <linux/version.h>

#define SET_HOST_SPI_ACCESS     0

struct host_spi_flash_hal_ops_t {
	int (*max_read)(void); /* maximum data size to be used to read type functions in spi_transfer */
	int (*transfer)(unsigned char *cmd, unsigned int cmd_len, unsigned char *data, unsigned long data_len, int data_dir);
	int (*burst_read)(unsigned char *cmd, unsigned int cmd_len, unsigned char *data, unsigned long  data_len, int data_dir);
	int (*configure_clock)(unsigned int clock);
    int (*set_addr_mode)(unsigned int addr_mode);
    void (*host_spi_flash_change_mode)(int mode); 
    int max_clock;
};

struct host_spi_flash_hal_t {
	struct host_spi_flash_hal_ops_t *ops;
	struct mtd_info *mtd;
	struct mtd_partition partitions;
};

struct host_spi_flash_dev
{
	struct host_spi_flash_hal_t *phost_spi_flash_hal;
};
struct host_spi_flash_core_ops_t {
};

#endif /* __HOST_SPI_FLASH_H__ */
