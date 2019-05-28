/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
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
 * adc_hw.h
 * ADC driver hardware layer header file for AST SoC
 *
 * Author: Samvinesh Christopher
 *
 *****************************************************************/
#ifndef _ADC_HW_H_
#define _ADC_HW_H_


#define ADC_CHANNEL0_NUM			0x00
#define ADC_CHANNEL15_NUM			0x0F
#define ADC_REF_VOLTAGE_IN_MVOLTS		2500
#define ADC_NUM_BITS_FOR_RESOLUTION		10
#define ADC_REGS_SIZE				128

#define AST_ADC_REG_BASE			0x1E6E9000
#define AST_SCU_UNLOCK_MAGIC			0x1688A8A8
#define AST_ADC_REG_SIZE			SZ_4K
#define AST_ADC_IRQ				12

#define AST_ADC_ENGINE_CONTROL_REG		0x00
#define AST_ADC_CLOCK_CONTROL_REG		0x0C
#define AST_ADC_DATA_REG			0x10

#define AST_ADC_CHANNEL_ENABLE			16

//ADC_CLOCK_CONTROL_REGISTER
#define AST_ADC_DIVISOR_OF_ADC_CLOCK		0x3ff
#define AST_ADC_CLOCK_INVERSE			16

#define ADC_HW_DEV_NAME				"adc_hw"

typedef struct {
	void	*ast_adc_addr;
}  __attribute__((packed)) adc_dev_t;

typedef struct 
{
	int	(*adc_read_channel) (uint16_t *adc_value, int channel);
	int	(*adc_get_resolution) (uint16_t *adc_resolution);
	int	(*adc_get_reference_voltage) (uint16_t *adc_ref_volatge);
	int	(*adc_reboot_notifier) (void);
} ADC_HW;


#endif /* _ADC_HW_H_ */

