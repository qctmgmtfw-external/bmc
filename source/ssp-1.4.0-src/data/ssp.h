/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * ssp_hw.c
 * SSP driver related 
 *
 *
 *****************************************************************/

#ifndef __SSP_H__
#define __SSP_H__

#define SSP_STATUS_ERROR		(1 << 0)
#define SSP_STATUS_HALT			(1 << 1)
#define SSP_STATUS_RUNNING		(1 << 2)

#define SSP_FIRMWARE_UNLOAD		(1 << 0)
#define SSP_FIRMWARE_FULL		(1 << 1)
#define SSP_FIRMWARE_LITE		(1 << 2)

#define SSP_OEM_DATA_UNKNOW		(1 << 0)
#define SSP_OEM_DATA_READY		(1 << 1)

#define SSP_IOCTL_START				_IOC(_IOC_WRITE, 'S',0x60,0x3FFF)
#define SSP_IOCTL_STOP				_IOC(_IOC_WRITE, 'S',0x61,0x3FFF)
#define SSP_IOCTL_RESET				_IOC(_IOC_WRITE, 'S',0x62,0x3FFF)
#define SSP_IOCTL_LOADFIRMWARE		_IOC(_IOC_WRITE, 'S',0x63,0x3FFF)
#define SSP_IOCTL_METADATA			_IOC(_IOC_READ,  'S',0x64,0x3FFF)
#define SSP_IOCTL_GET_OEMDATA		_IOC(_IOC_READ,  'S',0x65,0x3FFF)
#define SSP_IOCTL_SET_OEMDATA		_IOC(_IOC_WRITE, 'S',0x66,0x3FFF)

#define SSP_MAX_FILE_NAME_LEN		128
#define PACKED __attribute__ ((packed))

#if defined (SOC_PILOT_IV)
/* SSP image is loaded below PCIE_FUNCTION1load on a 8K-byte boundary */
#if defined (CONFIG_SPX_FEATURE_GLOBAL_PCIE_FUNCTION1_SUPPORT) && defined (CONFIG_SPX_FEATURE_GLOBAL_PCIE_FUNCTION1_SHARED_MEM_SIZE)
#define SSP_LOAD_ADDR  (((CONFIG_SPX_FEATURE_GLOBAL_MEMORY_START+CONFIG_SPX_FEATURE_GLOBAL_MEMORY_SIZE-CONFIG_SPX_FEATURE_GLOBAL_PCIE_FUNCTION1_SHARED_MEM_SIZE \
			              -CONFIG_SPX_FEATURE_GLOBAL_SSP_MEMORY_SIZE)/8192)*8192)
#else
#define SSP_LOAD_ADDR  (((CONFIG_SPX_FEATURE_GLOBAL_MEMORY_START+CONFIG_SPX_FEATURE_GLOBAL_MEMORY_SIZE \
			       	      -CONFIG_SPX_FEATURE_GLOBAL_SSP_MEMORY_SIZE)/8192)*8192)		       
#endif
#endif

typedef struct {
	uint32_t 	magic;
	uint32_t	soc_version;
	uint8_t		processor_status;
	uint8_t		firmware_status;
	uint8_t		oem_data_status;
	uint32_t 	crc;
} PACKED ssp_metadata_t;

typedef struct {
	ssp_metadata_t	metadata;
} PACKED ssp_status_t;

typedef struct {
	uint8_t path[SSP_MAX_FILE_NAME_LEN];
	uint32_t to;
} PACKED ssp_fw_t;

typedef struct
{
	int (*ssp_start) (void);
	int (*ssp_stop) (void);
	int (*ssp_reset) (void);
	int (*ssp_loadfirmware) (ssp_fw_t *ssp_fw);
	int (*ssp_set_oem_data) (uint32_t data_len, void* data_ptr);
	int (*ssp_get_oem_data) (void* data_ptr);
	int (*ssp_get_metadata) (ssp_metadata_t** metadata);
	int (*ssp_reboot_notifier) (void);
} ssp_hal_operations_t;

typedef struct
{
} ssp_core_funcs_t;


struct ssp_hal
{
	ssp_hal_operations_t *pssp_hal_ops;
};

struct ssp_dev
{
	struct ssp_hal *pssp_hal;
};


#endif /* !__SSP_H__ */
