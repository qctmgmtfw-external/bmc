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
 * adc_hw.c
 * ADC driver related 
 *
 * Author: Rama Rao Bisa <ramab@ami.com>
 *
 *****************************************************************/

#ifndef __ADC_H__
#define __ADC_H__


#define READ_ADC_CHANNEL		1
#define READ_ADC_REF_VOLATGE	2
#define READ_ADC_RESOLUTION		3
#define ENABLE_EXT_REF_VOLTAGE		4
#define DISABLE_EXT_REF_VOLTAGE		5
#define ENABLE_ADC_CHANNEL		6
#define DISABLE_ADC_CHANNEL		7

#define PACKED __attribute__ ((packed))

typedef struct
{
	uint8_t 	channel_num;
	uint16_t	channel_value;

} PACKED get_adc_value_t;

typedef struct
{
	int (*adc_read_channel) (uint16_t *adc_value, int channel);
	int (*adc_get_resolution) (uint16_t *adc_resolution);
	int (*adc_get_reference_voltage) (uint16_t *adc_ref_volatge);
	int (*adc_reboot_notifier) (void);
}adc_hal_operations_t;

typedef struct
{
} adc_core_funcs_t;


struct adc_hal
{
	adc_hal_operations_t *padc_hal_ops;
};

struct adc_dev
{
	struct adc_hal *padc_hal;
};


#endif /* !__ADC_H__ */
