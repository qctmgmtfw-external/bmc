/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

/****************************************************************
 *
 * ast_pwmtach.c
 * ASPEED AST2100/2050/2200/2150/2300/2400 PWM & Fan Tach controller driver
 *
*****************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/ioport.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "driver_hal.h"
#include "pwmtach.h"
#include "ast_pwmtach.h"

#define AST_PWMTACH_DRIVER_NAME		"ast_pwmtach"

static void *ast_pwmtach_virt_base;
static pwmtach_core_funcs_t *pwmtach_core_ops;
static int ast_pwmtach_hal_id;

static unsigned int ast_pwmtach_counter_resolution[AST_PWM_CHANNEL_NUM];
static unsigned int ast_pwmtach_presacle[AST_PWM_CHANNEL_NUM];

inline uint32_t ast_pwmtach_read_reg(uint32_t reg)
{
	return ioread32((void * __iomem)ast_pwmtach_virt_base + reg);
}

inline void ast_pwmtach_write_reg(uint32_t data, uint32_t reg)
{
	iowrite32(data, (void * __iomem)ast_pwmtach_virt_base + reg);
}

static int ast_pwmtach_enable_pwm_control(unsigned char pwm_num)
{
	uint32_t reg;

	if ((pwm_num < 0) || (pwm_num >= AST_PWM_CHANNEL_NUM))
		return -EINVAL;

	#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
		if(pwm_num < AST_PWM_EXT_CHANNEL_NUM)
		{
			reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
			reg |= (AST_PWMTACH_GEN_PWM_EN << pwm_num);
			reg &= ~(AST_PWMTACH_GEN_PWM_TYPE << pwm_num); /* we only use type M */
			ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
		}else{
			reg = ast_pwmtach_read_reg(AST_PWMTACH_EXT_GEN);
			reg |= (AST_PWMTACH_GEN_PWM_EN << (pwm_num - AST_PWM_EXT_CHANNEL_NUM));
			reg &= ~(AST_PWMTACH_GEN_PWM_TYPE << (pwm_num - AST_PWM_EXT_CHANNEL_NUM)); /* we only use type M */
			ast_pwmtach_write_reg(reg, AST_PWMTACH_EXT_GEN);
		}
	#else
		reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
		reg |= (AST_PWMTACH_GEN_PWM_EN << pwm_num);
		reg &= ~(AST_PWMTACH_GEN_PWM_TYPE << pwm_num); /* we only use type M */
		ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
	#endif

	return 0;
}

static unsigned char ast_pwmtach_get_pwm_control_status(unsigned char pwm_num)
{
	return -EINVAL;
}

static unsigned char ast_pwmtach_disable_pwm_control(unsigned char pwm_num)
{
	uint32_t reg;

	if ((pwm_num < 0) || (pwm_num >= AST_PWM_CHANNEL_NUM))
		return -EINVAL;

	#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
		if(pwm_num < AST_PWM_EXT_CHANNEL_NUM)
		{
			reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
			reg &= ~(AST_PWMTACH_GEN_PWM_EN << pwm_num);
			ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
		}else{
			reg = ast_pwmtach_read_reg(AST_PWMTACH_EXT_GEN);
			reg &= ~(AST_PWMTACH_GEN_PWM_EN << (pwm_num - AST_PWM_EXT_CHANNEL_NUM));
			ast_pwmtach_write_reg(reg, AST_PWMTACH_EXT_GEN);
		}
	#else
			reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
			reg &= ~(AST_PWMTACH_GEN_PWM_EN << pwm_num);
			ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
	#endif

	return 0;
}

static void ast_pwmtach_enable_all_pwm_control(void)
{
	uint32_t reg;
	int i;

	#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
		for (i = 0; i < AST_PWM_CHANNEL_NUM; i ++) {
			if(i < AST_PWM_EXT_CHANNEL_NUM)
			{
				reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
				reg |= (AST_PWMTACH_GEN_PWM_EN << i);
				ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
			}else{
				reg = ast_pwmtach_read_reg(AST_PWMTACH_EXT_GEN);
				reg |= (AST_PWMTACH_GEN_PWM_EN << (i - AST_PWM_EXT_CHANNEL_NUM));
				ast_pwmtach_write_reg(reg, AST_PWMTACH_EXT_GEN);
			}
		}
	#else
		reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
		for (i = 0; i < AST_PWM_CHANNEL_NUM; i ++) {
			reg |= (AST_PWMTACH_GEN_PWM_EN << i);
		}
		ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
	#endif

}

static void ast_pwmtach_disable_all_pwm_control(void)
{
	uint32_t reg;
	int i;

	#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
		for (i = 0; i < AST_PWM_CHANNEL_NUM; i ++) {
			if(i < AST_PWM_EXT_CHANNEL_NUM)
			{
				reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
				reg |= (AST_PWMTACH_GEN_PWM_EN << i);
				ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
			}else{
				reg = ast_pwmtach_read_reg(AST_PWMTACH_EXT_GEN);
				reg |= (AST_PWMTACH_GEN_PWM_EN << (i - AST_PWM_EXT_CHANNEL_NUM));
				ast_pwmtach_write_reg(reg, AST_PWMTACH_EXT_GEN);
			}
		}
	#else
		reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
		for (i = 0; i < AST_PWM_CHANNEL_NUM; i ++) {
			reg &= ~(AST_PWMTACH_GEN_PWM_EN << i);
		}
		ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
	#endif
	
}

static int ast_pwmtach_enable_fantach_control(unsigned char ft_num)
{
	uint32_t reg;

	if ((ft_num < 0) || (ft_num >= AST_TACH_CHANNEL_NUM))
		return -EINVAL;

	reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
	reg |= (AST_PWMTACH_GEN_TACH_EN << ft_num);
	ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);

	return 0;
}

static unsigned char ast_pwmtach_get_fantach_control_status(unsigned char pwm_num)
{
	return -EINVAL;
}

static unsigned char ast_pwmtach_disable_fantach_control(unsigned char ft_num)
{
	uint32_t reg;

	if ((ft_num < 0) || (ft_num >= AST_TACH_CHANNEL_NUM))
		return -EINVAL;

	reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
	reg &= ~(AST_PWMTACH_GEN_TACH_EN << ft_num);
	ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);

	return 0;
}

static void ast_pwmtach_enable_all_fantach_control(void)
{
	uint32_t reg;
	int i;

	reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
	for (i = 0; i < AST_TACH_CHANNEL_NUM; i ++) {
		reg |= (AST_PWMTACH_GEN_TACH_EN << i);
	}
	ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
}

static void ast_pwmtach_disable_all_fantach_control(void)
{
	uint32_t reg;
	int i;

	reg = ast_pwmtach_read_reg(AST_PWMTACH_GEN);
	for (i = 0; i < AST_TACH_CHANNEL_NUM; i ++) {
		reg &= ~(AST_PWMTACH_GEN_TACH_EN << i);
	}
	ast_pwmtach_write_reg(reg, AST_PWMTACH_GEN);
}

static void ast_pwmtach_enable_counterresolution(unsigned char pwm_num)
{
}

static unsigned char ast_pwmtach_disable_counterresolution(unsigned char pwm_num)
{
	return 0;
}

static void ast_pwmtach_set_counterresolution(unsigned char pwm_num, unsigned int counterres_value)
{
	if (counterres_value == 0);
		counterres_value = 1;

	ast_pwmtach_counter_resolution[pwm_num] = counterres_value;
}

static unsigned int ast_pwmtach_get_counterresolution(unsigned char pwm_num)
{
	return ast_pwmtach_counter_resolution[pwm_num];
}

static void ast_pwmtach_set_prescale(unsigned char pwm_num, unsigned char prescale_value)
{
	if (prescale_value == 0);
		prescale_value = 1;

	ast_pwmtach_presacle[pwm_num] = prescale_value;
}

static unsigned int ast_pwmtach_get_prescale(unsigned char pwm_num)
{
	return ast_pwmtach_presacle[pwm_num];
}

static void ast_pwmtach_set_dutycycle(unsigned char pwm_num, int dutycycle_value)
{
	uint32_t reg;
	uint32_t reg_base;
	uint32_t rising;
	uint32_t falling;

	/* 1 duty register controls 2 channels, offset of each duty register is 4 bytes */
	#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
		if(pwm_num < AST_PWM_EXT_CHANNEL_NUM)
			reg_base = AST_PWMTACH_DUTY_0 + ((pwm_num / 2) * 4);
		else
			reg_base = AST_PWMTACH_DUTY_2 + (((pwm_num - AST_PWM_EXT_CHANNEL_NUM) / 2) * 4);
	#else
		reg_base = AST_PWMTACH_DUTY_0 + ((pwm_num / 2) * 4);
	#endif
	reg = ast_pwmtach_read_reg(reg_base);
	rising = 0;
	falling = (dutycycle_value * 255) / ast_pwmtach_counter_resolution[pwm_num];

	/* even channel is controlled by bits[15:0], odd channel is controlled by bits[31:16] */
	if (pwm_num & 0x01) { /* odd */
		rising <<= (AST_PWMTACH_DUTY_RISING + AST_PWMTACH_DUTY_ID_SHIFT);
		falling <<= (AST_PWMTACH_DUTY_FALLING + AST_PWMTACH_DUTY_ID_SHIFT);
		reg &= ~(AST_PWMTACH_DUTY_ID_PWM << AST_PWMTACH_DUTY_ID_SHIFT);
	} else { /* even */
		rising <<= AST_PWMTACH_DUTY_RISING;
		falling <<= AST_PWMTACH_DUTY_FALLING;
		reg &= ~AST_PWMTACH_DUTY_ID_PWM;
	}

	reg |= falling | rising;
	ast_pwmtach_write_reg(reg, reg_base);
}

static unsigned int ast_pwmtach_get_dutycycle(unsigned char pwm_num)
{
	uint32_t reg;
	uint32_t reg_base;
	uint32_t rising;
	uint32_t falling;
	uint32_t dutycycle;
	int modulus = 0;

	
	#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
		if(pwm_num < AST_PWM_EXT_CHANNEL_NUM)
			reg_base = AST_PWMTACH_DUTY_0 + ((pwm_num / 2) * 4);
		else
			reg_base = AST_PWMTACH_DUTY_2 + (((pwm_num - AST_PWM_EXT_CHANNEL_NUM) / 2) * 4);
	#else
		reg_base = AST_PWMTACH_DUTY_0 + ((pwm_num / 2) * 4);
	#endif
		
	reg = ast_pwmtach_read_reg(reg_base);
//	printk("pwm %d reg %x\n", pwm_num,reg);
	if (pwm_num & 0x01) { /* odd */
		rising = (reg & 0x00ff0000) >> (AST_PWMTACH_DUTY_RISING + AST_PWMTACH_DUTY_ID_SHIFT);
		falling = (reg & 0xff000000)  >> (AST_PWMTACH_DUTY_FALLING + AST_PWMTACH_DUTY_ID_SHIFT);
	} else { /* even */
		rising = (reg & 0x000000ff)  >> AST_PWMTACH_DUTY_RISING;
		falling = (reg & 0x0000ff00)  >> AST_PWMTACH_DUTY_FALLING;
	}
	
	rising = rising*100 / 255;
	modulus = falling*100 % 255;
	falling = falling*100 / 255;
	if(falling == 0) {
		/* 256 is the pwm max ~ 0x100 but carry bit is masked */
		if(modulus != 0)
			dutycycle = 1;
		else
			dutycycle = 100;
		
	} 
	else 
	{	/* Round value up */
		if(modulus >= 5)
			falling++;
		
		dutycycle = falling - rising;
	}
	
	return dutycycle;
}

#define AST_PWMTACH_READ_TIMEOUT 1000

static int ast_pwmtach_trigger_read_fanspeed(unsigned char tachnum)
{
	int32_t reg;
	unsigned int timeout_count;

	/* read tach value by rising edge */

	/* set to low */
	ast_pwmtach_write_reg(0, AST_PWMTACH_TRIGGER);
	timeout_count = 0;
	do {
		if (ast_pwmtach_read_reg(AST_PWMTACH_TRIGGER) == 0)
			break;
		timeout_count ++;
	} while (timeout_count < AST_PWMTACH_READ_TIMEOUT);
	if (timeout_count == AST_PWMTACH_READ_TIMEOUT)
		return -EFAULT;

	barrier();

	/* set to high */
	reg = 1 << tachnum;
	ast_pwmtach_write_reg(reg, AST_PWMTACH_TRIGGER);
	timeout_count = 0;
	do {
		if (ast_pwmtach_read_reg(AST_PWMTACH_TRIGGER) == reg)
			break;
		timeout_count ++;
	} while (timeout_count < AST_PWMTACH_READ_TIMEOUT);
	if (timeout_count == AST_PWMTACH_READ_TIMEOUT)
		return -EFAULT;

	return 0;
}

static int ast_pwmtach_can_read_fanspeed(unsigned char tachnum)
{
	int32_t status;

	status = ast_pwmtach_read_reg(AST_PWMTACH_RESULT);
	if ((status & AST_PWMTACH_RESULT_STATUS) || (status == AST_PWMTACH_NO_FAN_VALUE1) || (status == AST_PWMTACH_NO_FAN_VALUE2))
		return 1;
	return 0;
}

static unsigned int ast_pwmtach_get_current_speed(unsigned char tachnum)
{
	uint32_t reg;

	reg = ast_pwmtach_read_reg(AST_PWMTACH_RESULT) & AST_PWMTACH_RESULT_VALUE;
	if ((reg == AST_PWMTACH_NO_FAN_VALUE1) || (reg == AST_PWMTACH_NO_FAN_VALUE2) || (reg == 0))
		return 0;
	return (AST_PWMTACH_CLOCK_RATE * 60) / (2 * reg * 4);
}

static unsigned int ast_pwmtach_get_num_of_pwms(void)
{
	return AST_PWM_CHANNEL_NUM;
}

static unsigned int ast_pwmtach_get_num_of_tachs(void)
{
	return AST_TACH_CHANNEL_NUM;
}

static unsigned int ast_pwmtach_get_pwm_clk(void)
{
	return AST_PWMTACH_CLOCK_RATE;
}

/*
 * @fn ast_pwmtach_set_tach_property
 * @brief Generic function to set a property of tach.
 * @param[in] property - Property to set and this can be different for different platform.
 * @param[in] tach_num - tach number according to property.
 * @param[in] value - appropriate value to set
 * @retval 0 on success, otherwise -1.
 */
static int ast_pwmtach_set_tach_property (unsigned char property, unsigned char tach_num, unsigned int value)
{
    return -1;
}

/*
 * @fn ast_pwmtach_get_tach_property
 * @brief Generic function to get a property of tach.
 * @param[in] property - Property to get and this can be different for different platform.
 * @param[in] tach_num - tach number according to property.
 * @retval return appropriate value, 0xFFFFFFFF on failure.
 */
static unsigned int ast_pwmtach_get_tach_property (unsigned char property, unsigned char tach_num)
{
    return 0xFFFFFFFF;
}

/*
 * @fn ast_pwmtach_set_pwm_property
 * @brief Generic function to set a property of pwm.
 * @param[in] property - Property to set and this can be different for different platform.
 * @param[in] pwm_num - pwm number according to property.
 * @param[in] value - appropriate value to set
 * @retval 0 on success, otherwise -1.
 */
static int ast_pwmtach_set_pwm_property (unsigned char property, unsigned char pwm_num, unsigned int value)
{
    return -1;
}

/*
 * @fn ast_pwmtach_get_pwm_property
 * @brief Generic function to get a property of pwm.
 * @param[in] property - Property to get and this can be different for different platform.
 * @param[in] pwm_num - pwm number according to property.
 * @retval return appropriate value, 0xFFFFFFFF on failure.
 */
static unsigned int ast_pwmtach_get_pwm_property (unsigned char property, unsigned char pwm_num)
{
    return 0xFFFFFFFF;
}


static pwmtach_hal_operations_t ast_pwmtach_hw_ops = {
	.enable_pwm_control = ast_pwmtach_enable_pwm_control,
	.disable_pwm_control = ast_pwmtach_disable_pwm_control,
	.enable_all_pwm_control = ast_pwmtach_enable_all_pwm_control,
	.disable_all_pwm_control = ast_pwmtach_disable_all_pwm_control,
	.enable_fantach_control = ast_pwmtach_enable_fantach_control,
	.disable_fantach_control = ast_pwmtach_disable_fantach_control,
	.enable_all_fantach_control = ast_pwmtach_enable_all_fantach_control,
	.disable_all_fantach_control = ast_pwmtach_disable_all_fantach_control,
	.enable_counterresolution = ast_pwmtach_enable_counterresolution,
	.disable_counterresolution = ast_pwmtach_disable_counterresolution,
	.get_current_speed = ast_pwmtach_get_current_speed,
	.set_prescale = ast_pwmtach_set_prescale,
	.get_prescale = ast_pwmtach_get_prescale,
	.set_dutycycle = ast_pwmtach_set_dutycycle,
	.set_counterresolution = ast_pwmtach_set_counterresolution,
	.get_counterresolution = ast_pwmtach_get_counterresolution,
	.trigger_read_fanspeed = ast_pwmtach_trigger_read_fanspeed,
	.can_read_fanspeed = ast_pwmtach_can_read_fanspeed,
	.get_num_of_pwms = ast_pwmtach_get_num_of_pwms,
	.get_num_of_tachs = ast_pwmtach_get_num_of_tachs,
	.get_pwm_clk = ast_pwmtach_get_pwm_clk,
	.get_dutycycle = ast_pwmtach_get_dutycycle,
	.get_pwm_control_status = ast_pwmtach_get_pwm_control_status,
	.get_fantach_control_status = ast_pwmtach_get_fantach_control_status,
	.set_tach_property = ast_pwmtach_set_tach_property,
	.get_tach_property = ast_pwmtach_get_tach_property,
	.set_pwm_property = ast_pwmtach_set_pwm_property,
	.get_pwm_property = ast_pwmtach_get_pwm_property,
};

static hw_hal_t ast_pwmtach_hw_hal = {
	.dev_type = EDEV_TYPE_PWMTACH,
	.owner = THIS_MODULE,
	.devname = AST_PWMTACH_DRIVER_NAME,
	.num_instances = AST_TACH_CHANNEL_NUM,
	.phal_ops = (void *) &ast_pwmtach_hw_ops
};

static void ast_pwmtach_init_hw(void)
{
	uint32_t reg;

	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)SCU_KEY_CONTROL_REG); /* unlock SCU */
	reg = ioread32((void * __iomem)SCU_SYS_RESET_REG);
	reg &= ~(0x200); /* stop the reset */
	iowrite32(reg, (void * __iomem)SCU_SYS_RESET_REG);
	iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG); /* lock SCU */

	/* enable clock and and set all tacho/pwm to type M */
	ast_pwmtach_write_reg(AST_PWMTACH_GEN_CLK, AST_PWMTACH_GEN);

	/* set clock division and period of type M/N */
	/* 0xFF11 --> 24000000 / (2 * 2 * 256) = 23437.5 Hz */
	ast_pwmtach_write_reg(0xFF11FF11, AST_PWMTACH_CLK);

	/* initialize all channels to the highest speed (0x100) */
	ast_pwmtach_write_reg(0, AST_PWMTACH_DUTY_0);
	ast_pwmtach_write_reg(0, AST_PWMTACH_DUTY_1);
	#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
		/* initialize all channels to the highest speed (0x100) */
		ast_pwmtach_write_reg(0, AST_PWMTACH_DUTY_2);
		ast_pwmtach_write_reg(0, AST_PWMTACH_DUTY_3);
	#endif
	/* initialize all channels to lowest speed (0x01)*/
//	ast_pwmtach_write_reg(0x01000100, AST_PWMTACH_DUTY_0);
//	ast_pwmtach_write_reg(0x01000100, AST_PWMTACH_DUTY_1);


	/* Set tacho type M/N Setting */
	/* 0x1000 = 4096 */
	ast_pwmtach_write_reg(0x10000001, AST_PWMTACH_TYPE_M_0);
	ast_pwmtach_write_reg(0x10000001, AST_PWMTACH_TYPE_N_0);

	/* Full Range to do measure */
	/* 0x1000 = 4096 */
	ast_pwmtach_write_reg(0x10000000, AST_PWMTACH_TYPE_M_1);
	ast_pwmtach_write_reg(0x10000000, AST_PWMTACH_TYPE_N_1);

	ast_pwmtach_write_reg(0, AST_PWMTACH_SRC);
}

static int ast_pwmtach_module_init(void)
{
	int i;
	int ret;

	extern int pwmtach_core_loaded;
	if (!pwmtach_core_loaded)
			return -1;

	ast_pwmtach_hal_id = register_hw_hal_module(&ast_pwmtach_hw_hal, (void **) &pwmtach_core_ops);
	if (ast_pwmtach_hal_id < 0) {
		printk(KERN_WARNING "%s: register HAL HW module failed\n", AST_PWMTACH_DRIVER_NAME);
		return ast_pwmtach_hal_id;
	}

	if (!request_mem_region(AST_PWMTACH_REG_BASE, AST_PWMTACH_REG_SIZE, AST_PWMTACH_DRIVER_NAME)) {
		printk(KERN_WARNING "%s: request memory region failed\n", AST_PWMTACH_DRIVER_NAME);
		ret = -EBUSY;
		goto out_register_hal;
	}

	ast_pwmtach_virt_base = ioremap(AST_PWMTACH_REG_BASE, AST_PWMTACH_REG_SIZE);
	if (!ast_pwmtach_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_PWMTACH_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_mem_region;
	}

	ast_pwmtach_init_hw();

	for (i = 0; i < AST_PWM_CHANNEL_NUM; i ++) {
		ast_pwmtach_counter_resolution[i] = 255; /* maximum counter resolution */
		ast_pwmtach_presacle[i] = 1; /* minimum prescale */
	}

	return 0;

out_mem_region:
	release_mem_region(AST_PWMTACH_REG_BASE, AST_PWMTACH_REG_SIZE);
out_register_hal:
	unregister_hw_hal_module(EDEV_TYPE_PWMTACH, ast_pwmtach_hal_id);

	return ret;
}

static void ast_pwmtach_module_exit(void)
{
	iounmap(ast_pwmtach_virt_base);
	release_mem_region(AST_PWMTACH_REG_BASE, AST_PWMTACH_REG_SIZE);
	unregister_hw_hal_module(EDEV_TYPE_PWMTACH, ast_pwmtach_hal_id);
}

module_init(ast_pwmtach_module_init);
module_exit(ast_pwmtach_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150/2300/2400 PWM & Fan Tach controller driver");
MODULE_LICENSE("GPL");

