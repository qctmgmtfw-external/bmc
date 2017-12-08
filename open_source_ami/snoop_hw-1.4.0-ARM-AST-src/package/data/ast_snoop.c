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
 * ast_snoop.c
 * ASPEED AST2100/2050/2200/2150 LPC snoop driver
 *
*****************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/kfifo.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "driver_hal.h"
#include "snoop.h"
#include "ast_snoop.h"

#define AST_SNOOP_DRIVER_NAME	"ast_snoop"

static void *ast_snoop_virt_base;
static snoop_core_funcs_t *snoop_core_ops;
static int ast_snoop_hal_id;

struct kfifo *cur_codes;
struct kfifo *pre_codes;

inline uint32_t ast_snoop_read_reg(uint32_t reg)
{
	return ioread32(ast_snoop_virt_base + reg);
}

inline void ast_snoop_write_reg(uint32_t data, uint32_t reg)
{
	iowrite32(data, ast_snoop_virt_base + reg);
}

static void ast_snoop_enable_port(void)
{
	uint32_t reg;

	reg = ast_snoop_read_reg(AST_LPC_HICR5);
	reg |= AST_LPC_HICR5_SNP0_EN;
	ast_snoop_write_reg(reg, AST_LPC_HICR5);
}

static void ast_snoop_disable_port(void)
{
	uint32_t reg;

	reg = ast_snoop_read_reg(AST_LPC_HICR5);
	reg &= ~(AST_LPC_HICR5_SNP0_EN);
	ast_snoop_write_reg(reg, AST_LPC_HICR5);
}

static void ast_snoop_configure_port_addr(void)
{
	uint32_t reg;

	reg = ast_snoop_read_reg(AST_LPC_SNPWADR);
	reg &= ~(AST_LPC_SNPWADR_ADDR0_MASK);
	reg |= (AST_SNOOP_ADDR_0 << AST_LPC_SNPWADR_ADDR0_SHIFT);
	ast_snoop_write_reg(reg, AST_LPC_SNPWADR);
}

static void ast_snoop_enable_interrupt(void)
{
	uint32_t reg;

	reg = ast_snoop_read_reg(AST_LPC_HICR5);
	reg |= AST_LPC_HICR5_SNP0_ENINT;
	ast_snoop_write_reg(reg, AST_LPC_HICR5);
}

static void ast_snoop_disable_interrupt(void)
{
	uint32_t reg;

	reg = ast_snoop_read_reg(AST_LPC_HICR5);
	reg &= ~(AST_LPC_HICR5_SNP0_ENINT);
	ast_snoop_write_reg(reg, AST_LPC_HICR5);
}

static void ast_snoop_init_hw(void)
{
	ast_snoop_disable_interrupt();
	ast_snoop_disable_port();

	ast_snoop_configure_port_addr();

	ast_snoop_enable_port();
	ast_snoop_enable_interrupt();
}

static void ast_snoop_reset(void)
{
}

static snoop_hal_operations_t ast_snoop_hw_ops = {
	.enable_snoop_interrupt = ast_snoop_enable_interrupt,
	.disable_snoop_interrupt = ast_snoop_disable_interrupt,
	.reset_snoop = ast_snoop_reset
};

static hw_hal_t ast_snoop_hw_hal = {
	.dev_type = EDEV_TYPE_SNOOP,
	.owner = THIS_MODULE,
	.devname = AST_SNOOP_DRIVER_NAME,
	.num_instances = AST_SNOOP_PORT_NUM,
	.phal_ops = (void *) &ast_snoop_hw_ops
};

static irqreturn_t ast_snoop_irq_handler(int irq, void *dev_id)
{
	uint32_t reg;
	uint8_t data;

	reg = ast_snoop_read_reg(AST_LPC_HICR6);

	if (reg & AST_LPC_HICR6_SNP0_STR) {
		/* clear interrupt flag */
		reg |= AST_LPC_HICR6_SNP0_STR;
		ast_snoop_write_reg(reg, AST_LPC_HICR6);

		/* read port data */
		reg = ast_snoop_read_reg(AST_LPC_SNPWDR);
		reg = (reg & AST_LPC_SNPWDR_DATA0_MASK) >> AST_LPC_SNPWDR_DATA0_SHIFT;
		data = (uint8_t) reg;

		/* put data into FIFO in snoop core */
		kfifo_put(cur_codes, &data, 1);

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static int ast_snoop_module_init(void)
{
	int ret;

	ast_snoop_hal_id = register_hw_hal_module(&ast_snoop_hw_hal, (void **) &snoop_core_ops);
	if (ast_snoop_hal_id < 0) {
		printk(KERN_WARNING "%s: register HAL HW module failed\n", AST_SNOOP_DRIVER_NAME);
		return ast_snoop_hal_id;
	}

	ast_snoop_virt_base = ioremap(AST_SNOOP_REG_BASE, AST_SNOOP_REG_SIZE);
	if (!ast_snoop_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_SNOOP_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_register_hal;
	}

	IRQ_SET_LEVEL_TRIGGER(AST_SNOOP_IRQ);
	IRQ_SET_HIGH_LEVEL(AST_SNOOP_IRQ);

	ret = request_irq(AST_SNOOP_IRQ, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, ast_snoop_virt_base);
	if (ret) {
		printk(KERN_WARNING "%s: request irq failed\n", AST_SNOOP_DRIVER_NAME);
		goto out_iomap;
	}

	snoop_core_ops->get_snoop_core_data(&cur_codes, &pre_codes, ast_snoop_hal_id);

	ast_snoop_init_hw();

	return 0;

out_iomap:
	iounmap(ast_snoop_virt_base);
out_register_hal:
	unregister_hw_hal_module(EDEV_TYPE_SNOOP, ast_snoop_hal_id);

	return ret;
}

static void ast_snoop_module_exit(void)
{
	ast_snoop_disable_port();
	ast_snoop_disable_interrupt();
	free_irq(AST_SNOOP_IRQ, ast_snoop_virt_base);
	iounmap(ast_snoop_virt_base);
	unregister_hw_hal_module(EDEV_TYPE_SNOOP, ast_snoop_hal_id);
}

module_init(ast_snoop_module_init);
module_exit(ast_snoop_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150 LPC snoop driver");
MODULE_LICENSE("GPL");
