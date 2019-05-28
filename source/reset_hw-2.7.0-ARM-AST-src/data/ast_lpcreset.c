/****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
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
#include <linux/delay.h>

#include "driver_hal.h"
#include "reset.h"
#include "ast_lpcreset.h"

#define AST_LPCRESET_DRIVER_NAME    "ast_lpcreset"

#define AST_LPCRESET_REG_VA_BASE       IO_ADDRESS(AST_LPCRESET_REG_BASE)

/** Default values in these arrays are for the as **/
static u32 lpc_reset_as_io_base[ AST_LPCRESET_HW_NUM ] =
{
#if defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
    AST_BMCCC_LPC0_VA_BASE(0),
    AST_BMCCC_LPC1_VA_BASE(0),
    AST_BMCCC_LPC2_VA_BASE(0),
    AST_BMCCC_LPC3_VA_BASE(0),
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
    AST_BMCCC_LPC0_VA_BASE(1),
    AST_BMCCC_LPC1_VA_BASE(1),
    AST_BMCCC_LPC2_VA_BASE(1),
    AST_BMCCC_LPC3_VA_BASE(1),
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#else /* !(defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)) */

    AST_LPCRESET_REG_VA_BASE, 

#endif /* !(defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)) */
};

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

inline uint32_t ast_lpcreset_read_reg(u8 ch, uint32_t reg)
{
    return ioread32((void * __iomem)lpc_reset_as_io_base[ch] + reg);
}

inline void ast_lpcreset_write_reg(u8 ch, uint32_t data, uint32_t reg)
{
    iowrite32(data, (void * __iomem)lpc_reset_as_io_base[ch] + reg);
}

#if 0
static void ast_lpcreset_enable_interrupt(void)
{
    uint32_t reg;
    int ch=0;

    for(ch=0;ch<AST_LPCRESET_HW_NUM;ch++)
    {
        reg = ast_lpcreset_read_reg(ch, AST_LPC_HICR2);
        reg |= AST_LPC_HICR2_ERRIE;
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR2);
    }
}
#endif

static void ast_lpcreset_disable_interrupt(void)
{
    uint32_t reg;
    int ch=0;

    for(ch=0;ch<AST_LPCRESET_HW_NUM;ch++)
    {
        reg = ast_lpcreset_read_reg(ch, AST_LPC_HICR2);
        reg &= ~(AST_LPC_HICR2_ERRIE);
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR2);
    }
}

irqreturn_t ast_lpcreset_irq_handler(int irq, void *dev_id)
{
    uint32_t reg = 0;
    int handled = 0;
    int ch = 0;

#if defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
    switch(irq)
    {
        case IRQ_BMCCC0_N1_KCS:
            ch=0;
            break;

        case IRQ_BMCCC0_N2_KCS:
            ch=1;
            break;

        case IRQ_BMCCC0_N3_KCS:
            ch=2;
            break;

        case IRQ_BMCCC0_N4_KCS:
            ch=3;
            break;

#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
        case IRQ_BMCCC1_N1_KCS:
            ch=4;
            break;

        case IRQ_BMCCC1_N2_KCS:
            ch=5;
            break;

        case IRQ_BMCCC1_N3_KCS:
            ch=6;
            break;

        case IRQ_BMCCC1_N4_KCS:
            ch=7;
            break;
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */

        default:
            printk(KERN_WARNING "%s: Unknow Interrupt Number %d\n", AST_LPCRESET_DRIVER_NAME, irq);
    }
#endif

    handled = 0;
    reg = ast_lpcreset_read_reg(ch, AST_LPC_HICR2);

    if (reg & AST_LPC_HICR2_LRST) {
        /* clear interrupt flag */
        reg &= ~(AST_LPC_HICR2_LRST);
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR2);
        printk (KERN_INFO "LPC RESET\n");
        if (reset_core_ops)
        {
            /* inform common module to handle it */
            reset_core_ops->process_reset_intr(ast_lpcreset_hal_hw_id);
        }

        handled = 1;
    }

    if (reg & AST_LPC_HICR2_SDWN) {
        /* clear interrupt flag */
        reg &= ~AST_LPC_HICR2_SDWN;
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR2);
        printk(KERN_INFO "LPC SWDN\n");
        handled = 1;
    }

    if (reg & AST_LPC_HICR2_ABRT) {
        /* clear interrupt flag */
        reg &= ~AST_LPC_HICR2_ABRT;
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR2);
        printk(KERN_INFO "LPC ABORT\n");
        handled = 1;
    }

    return (handled == 1) ? IRQ_HANDLED : IRQ_NONE;
}

static int __init ast_lpcreset_module_init(void)
{
    int ret;
    uint32_t reg;
    int ch=0;
	extern int reset_core_loaded;
	if (!reset_core_loaded)
			return -1;

    for(ch=0;ch<AST_LPCRESET_HW_NUM;ch++)
    {
        /* clear interrupt status and disable interrupt of KCS and LPC-reset */
        reg = ast_lpcreset_read_reg(ch, AST_LPC_HICR2);
        reg &= ~(AST_LPC_HICR2_LRST | AST_LPC_HICR2_SDWN | AST_LPC_HICR2_ABRT | AST_LPC_HICR2_IBFIE1 | AST_LPC_HICR2_IBFIE2 | AST_LPC_HICR2_IBFIE3 | AST_LPC_HICR2_ERRIE);
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR2);

        reg = ast_lpcreset_read_reg(ch, AST_LPC_IBTCR1);
        reg &= ~(AST_LPC_IBTCR1_H2B_INTR);
        ast_lpcreset_write_reg(ch, reg, AST_LPC_IBTCR1);

        /* disable interrupt of snoop */
        reg = ast_lpcreset_read_reg(ch, AST_LPC_HICR5);
        reg &= ~(AST_LPC_HICR5_SNP0_EN | AST_LPC_HICR5_SNP0_ENINT | AST_LPC_HICR5_SNP1_EN | AST_LPC_HICR5_SNP1_ENINT);
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR5);

        /* clear interrupt status of snoop */
        reg = ast_lpcreset_read_reg(ch, AST_LPC_HICR6);
        reg |= (AST_LPC_HICR6_SNP0_STR | AST_LPC_HICR6_SNP1_STR | AST_LPC_HICR6_PME_INTR | AST_LPC_HICR6_BAUD_INTR);
        ast_lpcreset_write_reg(ch, reg, AST_LPC_HICR6);
    }

    ast_lpcreset_hal_hw_id = register_hw_hal_module(&ast_lpcreset_hw_hal, (void **) &reset_core_ops);
    if (ast_lpcreset_hal_hw_id < 0) {
        printk(KERN_WARNING "%s: register LPC-reset HW module failed\n", AST_LPCRESET_DRIVER_NAME);
        ret = ast_lpcreset_hal_hw_id;
        goto out_irq;
    }

#if defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
    ret = request_irq(IRQ_BMCCC0_N1_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC0_VA_BASE(0)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N1 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_irq_init;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N1_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N1_KCS);
	}

    ret = request_irq(IRQ_BMCCC0_N2_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC1_VA_BASE(0)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N2 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_request_BMCCC0_N1_KCS;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N2_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N2_KCS);
	}

    ret = request_irq(IRQ_BMCCC0_N3_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC2_VA_BASE(0)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N3 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_request_BMCCC0_N2_KCS;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N3_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N3_KCS);
	}

    ret = request_irq(IRQ_BMCCC0_N4_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC3_VA_BASE(0)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N4 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_request_BMCCC0_N3_KCS;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N4_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N4_KCS);
	}

#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
    ret = request_irq(IRQ_BMCCC1_N1_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC0_VA_BASE(1)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N1 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_request_BMCCC0_N4_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N1_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N1_KCS);
	}

    ret = request_irq(IRQ_BMCCC1_N2_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC1_VA_BASE(1)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N2 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_request_BMCCC1_N1_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N2_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N2_KCS);
	}

    ret = request_irq(IRQ_BMCCC1_N3_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC2_VA_BASE(1)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N3 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_request_BMCCC1_N2_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N3_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N3_KCS);
	}

    ret = request_irq(IRQ_BMCCC1_N4_KCS, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_BMCCC_LPC3_VA_BASE(1)+1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N4 request irq failed\n", AST_LPCRESET_DRIVER_NAME);
        goto out_request_BMCCC1_N3_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N4_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N4_KCS);
	}
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */

#else
    IRQ_SET_HIGH_LEVEL(AST_LPCRESET_IRQ);
    IRQ_SET_LEVEL_TRIGGER(AST_LPCRESET_IRQ);

    ret = request_irq(AST_LPCRESET_IRQ, ast_lpcreset_irq_handler, IRQF_SHARED, AST_LPCRESET_DRIVER_NAME, (void *)(AST_LPCRESET_REG_VA_BASE+1));
    if (ret < 0) {
        printk("%s: request irq failed", AST_LPCRESET_DRIVER_NAME);
        goto out_irq_init;
    }
#endif

    //ast_lpcreset_enable_interrupt();

    return 0;

out_irq:
#if defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
out_request_BMCCC1_N3_KCS:
    free_irq(IRQ_BMCCC1_N3_KCS, (void *)(AST_BMCCC_LPC2_VA_BASE(1)+1));
out_request_BMCCC1_N2_KCS:
    free_irq(IRQ_BMCCC1_N2_KCS, (void *)(AST_BMCCC_LPC1_VA_BASE(1)+1));
out_request_BMCCC1_N1_KCS:
    free_irq(IRQ_BMCCC1_N1_KCS, (void *)(AST_BMCCC_LPC0_VA_BASE(1)+1));
out_request_BMCCC0_N4_KCS:
    free_irq(IRQ_BMCCC0_N4_KCS, (void *)(AST_BMCCC_LPC3_VA_BASE(0)+1));
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
out_request_BMCCC0_N3_KCS:
    free_irq(IRQ_BMCCC0_N3_KCS, (void *)(AST_BMCCC_LPC2_VA_BASE(0)+1));
out_request_BMCCC0_N2_KCS:
    free_irq(IRQ_BMCCC0_N2_KCS, (void *)(AST_BMCCC_LPC1_VA_BASE(0)+1));
out_request_BMCCC0_N1_KCS:
    free_irq(IRQ_BMCCC0_N1_KCS, (void *)(AST_BMCCC_LPC0_VA_BASE(0)+1));
#else
    free_irq(AST_LPCRESET_IRQ, (void *)(AST_LPCRESET_REG_VA_BASE+1));
#endif
out_irq_init:

    return ret;
}

static void __exit ast_lpcreset_module_exit(void)
{
    ast_lpcreset_disable_interrupt();
#if defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
    free_irq(IRQ_BMCCC0_N4_KCS, (void *)(AST_BMCCC_LPC3_VA_BASE(0)+1));
    free_irq(IRQ_BMCCC0_N3_KCS, (void *)(AST_BMCCC_LPC2_VA_BASE(0)+1));
    free_irq(IRQ_BMCCC0_N2_KCS, (void *)(AST_BMCCC_LPC1_VA_BASE(0)+1));
    free_irq(IRQ_BMCCC0_N1_KCS, (void *)(AST_BMCCC_LPC0_VA_BASE(0)+1));
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
    free_irq(IRQ_BMCCC1_N4_KCS, (void *)(AST_BMCCC_LPC3_VA_BASE(1)+1));
    free_irq(IRQ_BMCCC1_N3_KCS, (void *)(AST_BMCCC_LPC2_VA_BASE(1)+1));
    free_irq(IRQ_BMCCC1_N2_KCS, (void *)(AST_BMCCC_LPC1_VA_BASE(1)+1));
    free_irq(IRQ_BMCCC1_N1_KCS, (void *)(AST_BMCCC_LPC0_VA_BASE(1)+1));
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#else
    free_irq(AST_LPCRESET_IRQ, (void *)(AST_LPCRESET_REG_VA_BASE+1));
#endif
    if (0 != unregister_hw_hal_module(EDEV_TYPE_RESET, ast_lpcreset_hal_hw_id)) {
        printk("Failed to unregister LPCRESET HW module\n");
    }
}

module_init(ast_lpcreset_module_init);
module_exit(ast_lpcreset_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150 LPC reset driver");
MODULE_LICENSE("GPL");
