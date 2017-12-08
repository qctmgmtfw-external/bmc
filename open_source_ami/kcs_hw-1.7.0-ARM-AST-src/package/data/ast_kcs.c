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

/****************************************************************
 *
 * ast_kcs.c
 * ASPEED AST2100/2050/2200/2150/2300 KCS controller driver
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
#include "kcs.h"
#include "ast_kcs.h"
#include "coreTypes.h"

#define AST_KCS_DRIVER_NAME		"ast_kcs"

static void *ast_kcs_virt_base;
static kcs_core_funcs_t *kcs_core_ops;
static int ast_kcs_hal_id;
bool kcs_enabled_by_user = FALSE;

inline uint32_t ast_kcs_read_reg(uint32_t reg)
{
	return ioread32(ast_kcs_virt_base + reg);
}

inline void ast_kcs_write_reg(uint32_t data, uint32_t reg)
{
	iowrite32(data, ast_kcs_virt_base + reg);
}

static void ast_kcs_enable_channel(void)
{
	uint32_t reg;

	reg = ast_kcs_read_reg(AST_LPC_HICR0);
	reg |= (AST_LPC_HICR0_LPC3E | AST_LPC_HICR0_LPC2E | AST_LPC_HICR0_LPC1E);
	ast_kcs_write_reg(reg, AST_LPC_HICR0);

	#if defined SOC_AST2300
	reg = ast_kcs_read_reg(AST_LPC_HICRB);
	reg |= AST_LPC_HICRB0_KCS4E;
	ast_kcs_write_reg(reg, AST_LPC_HICRB);
	#endif
}

static void ast_kcs_disable_channel(void)
{
	uint32_t reg;

	reg = ast_kcs_read_reg(AST_LPC_HICR0);
	reg &= ~(AST_LPC_HICR0_LPC3E | AST_LPC_HICR0_LPC2E | AST_LPC_HICR0_LPC1E);
	ast_kcs_write_reg(reg, AST_LPC_HICR0);
	
	#if defined SOC_AST2300
	reg = ast_kcs_read_reg(AST_LPC_HICRB);
	reg &= ~(AST_LPC_HICRB0_KCS4E );
	ast_kcs_write_reg(reg, AST_LPC_HICRB);
	#endif
}

static void ast_kcs_configure_io_port_addr(void)
{
	uint32_t reg;

	/* switch the access of LADR12 to LADR1 */
	reg = ast_kcs_read_reg(AST_LPC_HICR4);
	reg &= ~AST_LPC_HICR4_LADR12SEL;
	ast_kcs_write_reg(reg, AST_LPC_HICR4);

	mb();

	/* set I/O port address of channel 1*/
	ast_kcs_write_reg(AST_KCS_ADR1_HI, AST_LPC_LADR12H);
	ast_kcs_write_reg(AST_KCS_ADR1_LO, AST_LPC_LADR12L);

	mb();

	/* switch the access of LADR12 to LADR2 */
	reg = ast_kcs_read_reg(AST_LPC_HICR4);
	reg |= AST_LPC_HICR4_LADR12SEL;
	ast_kcs_write_reg(reg, AST_LPC_HICR4);

	mb();

	/* set I/O port address 2 */
	ast_kcs_write_reg(AST_KCS_ADR2_HI, AST_LPC_LADR12H);
	ast_kcs_write_reg(AST_KCS_ADR2_LO, AST_LPC_LADR12L);

	mb();

	/* enable KCS in channel 3 */
	reg = ast_kcs_read_reg(AST_LPC_HICR4);
	reg |= AST_LPC_HICR4_KCSENBL;
	ast_kcs_write_reg(reg, AST_LPC_HICR4);

	mb();

	/* set I/O port address of channel 3 */
	ast_kcs_write_reg(AST_KCS_ADR3_HI, AST_LPC_LADR3H);
	ast_kcs_write_reg(AST_KCS_ADR3_LO, AST_LPC_LADR3L);

	mb();
	
	#if defined SOC_AST2300
	/* enable KCS in channel 4 */
	reg = ast_kcs_read_reg(AST_LPC_HICRB);
	reg |= AST_LPC_HICRB0_KCS4E;
	ast_kcs_write_reg(reg, AST_LPC_HICRB);

	mb();
	
	/* set I/O port address of channel 4 */
	ast_kcs_write_reg(AST_KCS_ADR4, AST_LPC_LADR4);
	#endif

}

static unsigned char ast_kcs_num_ch(void)
{
	return AST_KCS_CHANNEL_NUM;
}
	
static void ast_kcs_enable_interrupt(void)
{
	uint32_t reg;

	reg = ast_kcs_read_reg(AST_LPC_HICR2);
	reg |= (AST_LPC_HICR2_IBFIE1 | AST_LPC_HICR2_IBFIE2 | AST_LPC_HICR2_IBFIE3);
	ast_kcs_write_reg(reg, AST_LPC_HICR2);

	#if defined SOC_AST2300
	reg = ast_kcs_read_reg(AST_LPC_HICRB);
	reg |= (AST_LPC_HICRB0_KCS4INTE);
	ast_kcs_write_reg(reg, AST_LPC_HICRB);
	#endif
}

static void ast_kcs_disable_interrupt(void)
{
	uint32_t reg;

	reg = ast_kcs_read_reg(AST_LPC_HICR2);
	reg &= ~(AST_LPC_HICR2_IBFIE1 | AST_LPC_HICR2_IBFIE2 | AST_LPC_HICR2_IBFIE3);
	ast_kcs_write_reg(reg, AST_LPC_HICR2);

	#if defined SOC_AST2300
	reg = ast_kcs_read_reg(AST_LPC_HICRB);
	reg &= ~(AST_LPC_HICRB0_KCS4INTE);
	ast_kcs_write_reg(reg, AST_LPC_HICRB);
	#endif
}

static void ast_kcs_read_status(u8 channel, u8 *status)
{
	if (channel == 3)
	{
		#if defined SOC_AST2300
		*status = (uint8_t) ast_kcs_read_reg(AST_LPC_STR4);
		#endif
	}
	else
		*status = (uint8_t) ast_kcs_read_reg(AST_LPC_STR_CH(channel));
}

static void ast_kcs_write_status(u8 channel, u8 status)
{
	if (channel == 3)
	{
		#if defined SOC_AST2300
		ast_kcs_write_reg(status, AST_LPC_STR4);
		#endif
	}
	else
		ast_kcs_write_reg(status, AST_LPC_STR_CH(channel));

}

static void ast_kcs_read_command(u8 channel, u8 *command)
{
	if (channel == 3)
	{
		#if defined SOC_AST2300
		*command = (uint8_t) ast_kcs_read_reg(AST_LPC_IDR4);
		#endif
	}
	else
		*command = (uint8_t) ast_kcs_read_reg(AST_LPC_IDR_CH(channel));
}

static void ast_kcs_read_data(u8 channel, u8 *data)
{
	if(channel == 3)
	{
		#if defined SOC_AST2300
		*data = (uint8_t) ast_kcs_read_reg(AST_LPC_IDR4);
		#endif
	}
	else
		*data = (uint8_t) ast_kcs_read_reg(AST_LPC_IDR_CH(channel));
	
}

static void ast_kcs_write_data(u8 channel, u8 data)
{
	if (channel == 3)
	{
		#if defined SOC_AST2300
		ast_kcs_write_reg(data, AST_LPC_ODR4);
		#endif
	}
	else
		ast_kcs_write_reg(data, AST_LPC_ODR_CH(channel));	
}

void 
ast_kcs_interrupt_enable_user (void)
{
	kcs_enabled_by_user = TRUE;
	ast_kcs_enable_interrupt();
}

void
ast_kcs_interrupt_disable_user (void)
{
	kcs_enabled_by_user = FALSE;
	ast_kcs_disable_interrupt();
}

static kcs_hal_operations_t ast_kcs_hw_ops = {
	.num_kcs_ch = ast_kcs_num_ch,
	.enable_kcs_interrupt = ast_kcs_enable_interrupt,
	.disable_kcs_interrupt = ast_kcs_disable_interrupt,
	.read_kcs_status = ast_kcs_read_status,
	.write_kcs_status = ast_kcs_write_status,
	.read_kcs_command = ast_kcs_read_command,
	.read_kcs_data_in = ast_kcs_read_data,
	.write_kcs_data_out = ast_kcs_write_data,
	.kcs_interrupt_enable_user = ast_kcs_interrupt_enable_user,
	.kcs_interrupt_disable_user = ast_kcs_interrupt_disable_user
};

static hw_hal_t ast_kcs_hw_hal = {
	.dev_type = EDEV_TYPE_KCS,
	.owner = THIS_MODULE,
	.devname = AST_KCS_DRIVER_NAME,
	.num_instances = AST_KCS_CHANNEL_NUM,
	.phal_ops = (void *) &ast_kcs_hw_ops
};

static irqreturn_t ast_kcs_irq_handler(int irq, void *dev_id)
{
	uint32_t reg;
	int ch;
	int ret;
	uint8_t status;
	int handled;

	reg = ast_kcs_read_reg(AST_LPC_HICR6);

	if (reg & (AST_LPC_HICR6_SNP0_STR | AST_LPC_HICR6_SNP1_STR)) { /* snoop interrupt is occured */
		return IRQ_NONE; /* handled by snoop driver */
	}

#if defined SOC_AST2300
    reg = ast_kcs_read_reg(LPC_iBTCR2);
    if (reg & LPC_iBTCR2_H2BI)
    {
		return IRQ_NONE;
    }
#endif
	reg = ast_kcs_read_reg(AST_LPC_HICR2);

	if (reg & (AST_LPC_HICR2_LRST | AST_LPC_HICR2_SDWN | AST_LPC_HICR2_ABRT)) { /* LRESET | SDWN | ABRT interrupt is occured */
		return IRQ_NONE; /* handled by LPC-reset driver */
	}
	for (ch = 0; ch < AST_KCS_CHANNEL_NUM; ch ++) {
		ast_kcs_read_status(ch, &status);
		if (status & 0x02) { /* Command or Data_In register has been written by system-side software */
			handled = 1;
			ret = kcs_core_ops->process_kcs_intr(ast_kcs_hal_id, ch);
			if (ret != 0) {
				printk(KERN_WARNING "KCS core IRQ handler failed\n");
			}
		}
		/* when the KCS core IRQ handler reads IDR, IDF is cleared automatically */
	}

	return (handled == 1) ? IRQ_HANDLED : IRQ_NONE;
}

static void ast_kcs_init_hw(void)
{
	uint32_t reg;
	int ch;
	u8 ch_num, status;
	
	#ifdef SOC_AST2300
	reg = ast_kcs_read_reg(AST_LPC_HICR5);
	reg &= ~(0x00000500); /* clear bit 10 & 8 to disable LPC flash cycle */
	ast_kcs_write_reg(reg, AST_LPC_HICR5);
	#endif

	ast_kcs_disable_interrupt();
	ast_kcs_disable_channel();

	ast_kcs_configure_io_port_addr();

	/* clear OBF */
	for (ch = 0; ch < AST_KCS_CHANNEL_NUM; ch ++) {
		if(ch == 3)
		{
			#ifdef SOC_AST2300
			reg = ast_kcs_read_reg(AST_LPC_STR4);
			reg &= ~AST_LPC_STR_OBFA;
			ast_kcs_write_reg(reg, AST_LPC_STR4);
			#endif
		}
		else
		{		
			reg = ast_kcs_read_reg(AST_LPC_STR_CH(ch));
			reg &= ~AST_LPC_STR_OBFA;
			ast_kcs_write_reg(reg, AST_LPC_STR_CH(ch));
		}
	}
	for (ch_num = 0; ch_num < AST_KCS_CHANNEL_NUM; ++ch_num)
	{
		ast_kcs_read_status (ch_num, &status);
		status = status | ERROR_STATE;
		ast_kcs_write_status (ch_num, status);
	}

	ast_kcs_enable_channel();
	//ast_kcs_enable_interrupt();
}

static int ast_kcs_module_init(void)
{
	int ret;

	ast_kcs_hal_id = register_hw_hal_module(&ast_kcs_hw_hal, (void **) &kcs_core_ops);
	if (ast_kcs_hal_id < 0) {
		printk(KERN_WARNING "%s: register KCS HAL module failed\n", AST_KCS_DRIVER_NAME);
		return ast_kcs_hal_id;
	}

	if (!request_mem_region(AST_KCS_REG_BASE, AST_KCS_REG_SIZE, AST_KCS_DRIVER_NAME)) {
		printk(KERN_WARNING "%s: request memory region failed\n", AST_KCS_DRIVER_NAME);
		ret = -EBUSY;
		goto out_register_hal;
	}

	ast_kcs_virt_base = ioremap(AST_KCS_REG_BASE, AST_KCS_REG_SIZE);
	if (!ast_kcs_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_KCS_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_mem_region;
	}

	IRQ_SET_LEVEL_TRIGGER(AST_KCS_IRQ);
	IRQ_SET_HIGH_LEVEL(AST_KCS_IRQ);

	ret = request_irq(AST_KCS_IRQ, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, ast_kcs_virt_base);
	if (ret) {
		printk(KERN_WARNING "%s: request irq failed\n", AST_KCS_DRIVER_NAME);
		goto out_iomap;
	}

	ast_kcs_init_hw();

	return 0;

out_iomap:
	iounmap(ast_kcs_virt_base);
out_mem_region:
	release_mem_region(AST_KCS_REG_BASE, AST_KCS_REG_SIZE);
out_register_hal:
	unregister_hw_hal_module(EDEV_TYPE_KCS, ast_kcs_hal_id);

	return ret;
}

static void ast_kcs_module_exit(void)
{
	free_irq(AST_KCS_IRQ, ast_kcs_virt_base);
	iounmap(ast_kcs_virt_base);
	release_mem_region(AST_KCS_REG_BASE, AST_KCS_REG_SIZE);
	unregister_hw_hal_module(EDEV_TYPE_KCS, ast_kcs_hal_id);
}

module_init(ast_kcs_module_init);
module_exit(ast_kcs_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150/2300 KCS controller driver");
MODULE_LICENSE("GPL");
