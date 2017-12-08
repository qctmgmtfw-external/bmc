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
 * ast_lpcreset.c
 * ASPEED AST2100/2050/2200/2150 LPC reset driver
 *
*****************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/ide.h>
#include <mach/platform.h>

#include "driver_hal.h"
#include "reset.h"
#include "ast_lpcreset.h"

#define AST_LPCRESET_DRIVER_NAME	"ast_lpcreset"

static void *ast_lpcreset_virt_base;
static int ast_lpcreset_hal_hw_id;
static reset_core_funcs_t *reset_core_ops;

static reset_hal_operations_t ast_lpcreset_ops = {
};

static hw_hal_t ast_lpcreset_hw_hal = {
	.dev_type = EDEV_TYPE_RESET,
    .owner = THIS_MODULE,
    .devname = AST_LPCRESET_DRIVER_NAME,
    .num_instances = AST_LPCRESET_HW_NUM,
    .phal_ops = (void *) &ast_lpcreset_ops
};

inline uint32_t ast_lpcreset_read_reg(uint32_t reg)
{
	return ioread32(ast_lpcreset_virt_base + reg);
}

inline void ast_lpcreset_write_reg(uint32_t data, uint32_t reg)
{
	iowrite32(data, ast_lpcreset_virt_base + reg);
}

static void ast_lpcreset_enable_interrupt(void)
{
	uint32_t reg;

	reg = ast_lpcreset_read_reg(AST_LPC_HICR2);
	reg |= AST_LPC_HICR2_ERRIE;
	ast_lpcreset_write_reg(reg, AST_LPC_HICR2);
}

static void ast_lpcreset_disable_interrupt(void)
{
	uint32_t reg;

	reg = ast_lpcreset_read_reg(AST_LPC_HICR2);
	reg &= ~(AST_LPC_HICR2_ERRIE);
	ast_lpcreset_write_reg(reg, AST_LPC_HICR2);
}

irqreturn_t ast_lpcreset_irq_handler(int irq, void *dev_id)
{
	uint32_t reg;
	int handled;
	int err;

	handled = 0;
	reg = ast_lpcreset_read_reg(AST_LPC_HICR2);

	if (reg & AST_LPC_HICR2_LRST) {
		/* clear interrupt flag */
		reg &= ~(AST_LPC_HICR2_LRST);
		ast_lpcreset_write_reg(reg, AST_LPC_HICR2);

		/* inform common module to handle it */
		err = reset_core_ops->process_reset_intr(ast_lpcreset_hal_hw_id);
		handled = 1;
	}

	if (reg & AST_LPC_HICR2_SDWN) {
		/* clear interrupt flag */
		reg &= ~AST_LPC_HICR2_SDWN;
		ast_lpcreset_write_reg(reg, AST_LPC_HICR2);
		printk(KERN_INFO "LPC SWDN\n");
		handled = 1;
	}

	if (reg & AST_LPC_HICR2_ABRT) {
		/* clear interrupt flag */
		reg &= ~AST_LPC_HICR2_ABRT;
		ast_lpcreset_write_reg(reg, AST_LPC_HICR2);
		printk(KERN_INFO "LPC ABORT\n");
		handled = 1;
	}

	return (handled == 1) ? IRQ_HANDLED : IRQ_NONE;
}

static int __init ast_lpcreset_module_init(void)
{
	int ret;
	uint32_t reg;

	ast_lpcreset_virt_base = ioremap(AST_LPCRESET_REG_BASE, AST_LPCRESET_REG_SIZE);
	if (!ast_lpcreset_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_LPCRESET_DRIVER_NAME);
		return -ENOMEM;
	}

	/* clear interrupt status and disable interrupt of KCS and LPC-reset */
	reg = ast_lpcreset_read_reg(AST_LPC_HICR2);
 	reg &= ~(AST_LPC_HICR2_LRST | AST_LPC_HICR2_SDWN | AST_LPC_HICR2_ABRT | AST_LPC_HICR2_IBFIE1 | AST_LPC_HICR2_IBFIE2 | AST_LPC_HICR2_IBFIE3 | AST_LPC_HICR2_ERRIE);
 	ast_lpcreset_write_reg(reg, AST_LPC_HICR2);

 	/* disable interrupt of snoop */
 	reg = ast_lpcreset_read_reg(AST_LPC_HICR5);
 	reg &= ~(AST_LPC_HICR5_SNP0_EN | AST_LPC_HICR5_SNP0_ENINT | AST_LPC_HICR5_SNP1_EN | AST_LPC_HICR5_SNP1_ENINT);
 	ast_lpcreset_write_reg(reg, AST_LPC_HICR5);

 	/* clear interrupt status of snoop */
 	reg = ast_lpcreset_read_reg(AST_LPC_HICR6);
 	reg |= (AST_LPC_HICR6_SNP0_STR | AST_LPC_HICR6_SNP1_STR);
 	ast_lpcreset_write_reg(reg, AST_LPC_HICR6);

	IRQ_SET_HIGH_LEVEL(AST_LPCRESET_IRQ);
	IRQ_SET_LEVEL_TRIGGER(AST_LPCRESET_IRQ);

    ret = request_irq(AST_LPCRESET_IRQ, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, ast_lpcreset_virt_base);
	if (ret < 0) {
		printk("%s: request irq failed", AST_LPCRESET_DRIVER_NAME);
		goto out_iomap;
	}

	ast_lpcreset_hal_hw_id = register_hw_hal_module(&ast_lpcreset_hw_hal, (void **) &reset_core_ops);
	if (ast_lpcreset_hal_hw_id < 0) {
		printk(KERN_WARNING "%s: register LPC-reset HW module failed\n", AST_LPCRESET_DRIVER_NAME);
		ret = ast_lpcreset_hal_hw_id;
		goto out_irq;
	}

	ast_lpcreset_enable_interrupt();

	return 0;

out_irq:
	free_irq(AST_LPCRESET_IRQ, ast_lpcreset_virt_base);
out_iomap:
	iounmap(ast_lpcreset_virt_base);

	return ret;
}

static void __exit ast_lpcreset_module_exit(void)
{
	ast_lpcreset_disable_interrupt();
	free_irq(AST_LPCRESET_IRQ, ast_lpcreset_virt_base);
	iounmap(ast_lpcreset_virt_base);
	if (0 != unregister_hw_hal_module(EDEV_TYPE_RESET, ast_lpcreset_hal_hw_id)) {
		printk("Failed to unregister LPCRESET HW module\n");
	}
}

module_init(ast_lpcreset_module_init);
module_exit(ast_lpcreset_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150 LPC reset driver");
MODULE_LICENSE("GPL");
