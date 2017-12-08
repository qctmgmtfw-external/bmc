/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2010, American Megatrends Inc.        **
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
 * ast_watchdog.c
 * ASPEED AST2050/2100/2150/2200 watchdog timer driver
 *
 ****************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/bitops.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "driver_hal.h"
#include "watchdog_core.h"

#define AST_WATCHDOG_REG_BASE   0x1E785000
#define AST_WATCHDOG_REG_LEN    SZ_4K

/* registers offset */
#define AST_WATCHDOG_CNT        0x00
#define AST_WATCHDOG_VALUE      0x04
#define AST_WATCHDOG_RESTART    0x08
#define AST_WATCHDOG_CTRL       0x0C
#define AST_WATCHDOG_TIMEOUT    0x10
#define AST_WATCHDOG_CLEAR      0x14
#define AST_WATCHDOG_WIDTH      0x18

/* bits of watchdog control register */
#define AST_WATCHDOG_CTRL_CLOCK         0x10
#define AST_WATCHDOG_CTRL_EXTERNAL      0x08
#define AST_WATCHDOG_CTRL_INTERRUPT     0x04
#define AST_WATCHDOG_CTRL_RESET         0x02
#define AST_WATCHDOG_CTRL_ENABLE        0x01

#define AST_WATCHDOG_RELOAD_MAGIC       0x4755

#define AST_WATCHDOG_DRIVER_NAME "ast_watchdog"
#define AST_WATCHDOG_COUNT_PER_SEDOND 1000000 /* we use external 1MHz clock source */

static void *ast_watchdog_virt_base;
static struct watchdog_core_funcs_t *watchdog_core_ops;
static int ast_watchdog_hal_id;

static void ast_watchdog_set_value(int value)
{
	iowrite32(value * AST_WATCHDOG_COUNT_PER_SEDOND, ast_watchdog_virt_base + AST_WATCHDOG_VALUE);
}

static void ast_watchdog_count(void)
{
	iowrite32(AST_WATCHDOG_RELOAD_MAGIC, ast_watchdog_virt_base + AST_WATCHDOG_RESTART);
}

static void ast_watchdog_enable(void)
{
	/* we use external 1MHz clock source */
	iowrite32(AST_WATCHDOG_CTRL_CLOCK | AST_WATCHDOG_CTRL_RESET | AST_WATCHDOG_CTRL_ENABLE, ast_watchdog_virt_base + AST_WATCHDOG_CTRL);
}

static void ast_watchdog_disable(void)
{
	iowrite32(ioread32(ast_watchdog_virt_base + AST_WATCHDOG_CTRL) & ~AST_WATCHDOG_CTRL_ENABLE, ast_watchdog_virt_base + AST_WATCHDOG_CTRL);
}

static struct watchdog_hal_ops_t ast_ops = {
	.set_value = ast_watchdog_set_value,
	.count = ast_watchdog_count,
	.enable = ast_watchdog_enable,
	.disable = ast_watchdog_disable
};

static hw_hal_t ast_hal = {
	.dev_type = EDEV_TYPE_WATCHDOG,
	.owner = THIS_MODULE,
	.devname = AST_WATCHDOG_DRIVER_NAME,
	.num_instances = 1,
	.phal_ops = (void *) &ast_ops
};

static int __init ast_watchdog_init(void)
{
	int ret;

	ast_watchdog_hal_id = register_hw_hal_module(&ast_hal, (void **) &watchdog_core_ops);
	if (ast_watchdog_hal_id < 0) {
		printk(KERN_WARNING "%s: register HAL HW module failed\n", AST_WATCHDOG_DRIVER_NAME);
		return ast_watchdog_hal_id;
	}

	if (!request_mem_region(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN, AST_WATCHDOG_DRIVER_NAME)) {
		printk(KERN_ERR AST_WATCHDOG_DRIVER_NAME ": Can't request memory region\n");
		ret = -EBUSY;
		goto out_hal_register;
	}

	ast_watchdog_virt_base = ioremap(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN);
	if (!ast_watchdog_virt_base) {
		ret = -ENOMEM;
		goto out_mem_region;
	}

	return 0;

out_mem_region:
	release_mem_region(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN);
out_hal_register:
	unregister_hw_hal_module(EDEV_TYPE_WATCHDOG, ast_watchdog_hal_id);

	return ret;
}

static void __exit ast_watchdog_exit(void)
{
	iounmap(ast_watchdog_virt_base);
	release_mem_region(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN);
	unregister_hw_hal_module(EDEV_TYPE_WATCHDOG, ast_watchdog_hal_id);
}

module_init(ast_watchdog_init);
module_exit(ast_watchdog_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2050/AST2100/2150/2200 watchdog timer driver");
MODULE_LICENSE("GPL");
