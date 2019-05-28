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
 * ast_snoop.c
 * ASPEED AST2100/2050/2200/2150 LPC snoop driver
 *
*****************************************************************/

#include <linux/version.h>
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

#define AST_SNOOP_REG_VA_BASE         IO_ADDRESS(AST_SNOOP_REG_BASE)
#define AST_SNOOP_DEV_ID_OFFSET       2  

/** Default values in these arrays are for the as **/
static u32 snoop_as_io_base[ AST_SNOOP_PORT_NUM ] = {
#if defined(GROUP_AST1070_COMPANION)
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
#else /* !defined(GROUP_AST1070_COMPANION) */
    AST_SNOOP_REG_VA_BASE,
#endif /* !defined(GROUP_AST1070_COMPANION) */
};
static snoop_core_funcs_t *snoop_core_ops;
static int ast_snoop_hal_id;

struct kfifo *cur_codes[AST_SNOOP_PORT_NUM];
struct kfifo *pre_codes[AST_SNOOP_PORT_NUM];

inline uint32_t ast_snoop_read_reg(int ch_num, uint32_t reg)
{
	return ioread32((void * __iomem)snoop_as_io_base[ch_num] + reg);
}

inline void ast_snoop_write_reg(int ch_num, uint32_t data, uint32_t reg)
{
	iowrite32(data, (void * __iomem)snoop_as_io_base[ch_num] + reg);
}

static void ast_snoop_enable_port(int ch_num)
{
	uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
	reg = ast_snoop_read_reg(ch_num, AST_BMCCC_LPC_80H_CTRL_REG);
	reg |= AST_BMCCC_LPC_80H_EN;
	ast_snoop_write_reg(ch_num, reg, AST_BMCCC_LPC_80H_CTRL_REG);
#else
	reg = ast_snoop_read_reg(ch_num, AST_LPC_HICR5);
	reg |= AST_LPC_HICR5_SNP0_EN;
	ast_snoop_write_reg(ch_num, reg, AST_LPC_HICR5);
#endif
}

static void ast_snoop_disable_port(int ch_num)
{
	uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
	reg = ast_snoop_read_reg(ch_num, AST_BMCCC_LPC_80H_CTRL_REG);
	reg &= ~(AST_BMCCC_LPC_80H_EN);
	ast_snoop_write_reg(ch_num, reg, AST_BMCCC_LPC_80H_CTRL_REG);
#else
	reg = ast_snoop_read_reg(ch_num, AST_LPC_HICR5);
	reg &= ~(AST_LPC_HICR5_SNP0_EN);
	ast_snoop_write_reg(ch_num, reg, AST_LPC_HICR5);
#endif
}

static void ast_snoop_configure_port_addr(int ch_num)
{
	uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
	reg = ast_snoop_read_reg(ch_num, AST_BMCCC_LPC_80H_ADDR0_REG);
	reg &= ~(AST_BMCCC_LPC_ADDR0_MASK);
	reg |= (AST_SNOOP_ADDR_0 << AST_BMCCC_LPC_ADDR0_SHIFT);
	ast_snoop_write_reg(ch_num, reg, AST_BMCCC_LPC_80H_ADDR0_REG);
#else
	reg = ast_snoop_read_reg(ch_num, AST_LPC_SNPWADR);
	reg &= ~(AST_LPC_SNPWADR_ADDR0_MASK);
	reg |= (AST_SNOOP_ADDR_0 << AST_LPC_SNPWADR_ADDR0_SHIFT);
	ast_snoop_write_reg(ch_num, reg, AST_LPC_SNPWADR);
#endif
}

static void ast_snoop_enable_interrupt(int ch_num)
{
	uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
	reg = ast_snoop_read_reg(ch_num, AST_BMCCC_LPC_80H_CTRL_REG);
	reg |= AST_BMCCC_LPC_80H_INT_EN;
	ast_snoop_write_reg(ch_num, reg, AST_BMCCC_LPC_80H_CTRL_REG);
#else
	reg = ast_snoop_read_reg(ch_num, AST_LPC_HICR5);
	reg |= AST_LPC_HICR5_SNP0_ENINT;
	ast_snoop_write_reg(ch_num, reg, AST_LPC_HICR5);
#endif
}

static void ast_snoop_disable_interrupt(int ch_num)
{
	uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
	reg = ast_snoop_read_reg(ch_num, AST_BMCCC_LPC_80H_CTRL_REG);
	reg &= ~(AST_BMCCC_LPC_80H_INT_EN);
	ast_snoop_write_reg(ch_num, reg, AST_BMCCC_LPC_80H_CTRL_REG);
#else
	reg = ast_snoop_read_reg(ch_num, AST_LPC_HICR5);
	reg &= ~(AST_LPC_HICR5_SNP0_ENINT);
	ast_snoop_write_reg(ch_num, reg, AST_LPC_HICR5);
#endif
}

static void ast_snoop_init_hw(int ch_num)
{
	ast_snoop_disable_interrupt(ch_num);
	ast_snoop_disable_port(ch_num);

	ast_snoop_configure_port_addr(ch_num);

	ast_snoop_enable_port(ch_num);
	ast_snoop_enable_interrupt(ch_num);
}

static void ast_snoop_reset(int ch_num)
{
}

static int isInterruptOccurred(int ch_num)
{
	uint32_t reg;
#if defined(GROUP_AST1070_COMPANION)
    reg = ast_snoop_read_reg(ch_num, AST_BMCCC_LPC_80H_CTRL_REG);

    return (reg & AST_BMCCC_LPC_80H_WR_EVNT);
#else
    reg = ast_snoop_read_reg(ch_num, AST_LPC_HICR6);

    return (reg & AST_LPC_HICR6_SNP0_STR);
#endif
}

static void ast_snoop_clear_interrupt_flg(int ch_num)
{
	uint32_t reg=0;

#if defined(GROUP_AST1070_COMPANION)
    reg |= AST_BMCCC_LPC_80H_WR_EVNT;
    ast_snoop_write_reg(ch_num, reg, AST_BMCCC_LPC_80H_DATA_REG);
#else
    reg |= AST_LPC_HICR6_SNP0_STR;
    ast_snoop_write_reg(ch_num, reg, AST_LPC_HICR6);
#endif
}

static uint8_t ast_snoop_read_data(int ch_num)
{
	uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
    reg = ast_snoop_read_reg(ch_num, AST_BMCCC_LPC_80H_DATA_REG);
    reg = (reg & AST_BMCCC_LPC_DATA_MASK) >> AST_BMCCC_LPC_DATA_SHIFT;
#else
    reg = ast_snoop_read_reg(ch_num, AST_LPC_SNPWDR);
    reg = (reg & AST_LPC_SNPWDR_DATA0_MASK) >> AST_LPC_SNPWDR_DATA0_SHIFT;
#endif
    return ((uint8_t) reg);
}

static snoop_hal_operations_t ast_snoop_hw_ops = {
	.enable_snoop_interrupt = ast_snoop_enable_interrupt,
	.disable_snoop_interrupt = ast_snoop_disable_interrupt,
	.reset_snoop = ast_snoop_reset,
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
	  uint8_t data;
    int ch=0;

#if defined(GROUP_AST1070_COMPANION)
    for (ch=0;ch<AST_SNOOP_PORT_NUM;ch++) {
        if (((u32)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET)) == ((u32)dev_id))
            break;
    }

    if (ch == AST_SNOOP_PORT_NUM)
        return IRQ_NONE;
#endif

    if (isInterruptOccurred(ch))
    {
		/* clear interrupt flag */
        ast_snoop_clear_interrupt_flg(ch);

		/* read port data */
		data = (uint8_t) ast_snoop_read_data(ch);

		/* put data into FIFO in snoop core */
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
        memcpy (cur_codes[ch]->kfifo.data+cur_codes[ch]->kfifo.in,(void *)&data,1);

        cur_codes[ch]->kfifo.in = (cur_codes[ch]->kfifo.in + 1) % 512;


        if ( cur_codes[ch]->kfifo.mask == (512-1))
        {
            cur_codes[ch]->kfifo.mask++;
            cur_codes[ch]->kfifo.out = (cur_codes[ch]->kfifo.out + 1) % 512;

        }
        else if ( cur_codes[ch]->kfifo.mask == 512)
            cur_codes[ch]->kfifo.out = (cur_codes[ch]->kfifo.out + 1) % 512;
        else
            cur_codes[ch]->kfifo.mask++;
#else
		cur_codes[ch]->buffer[ cur_codes[ch]->in ] = data;

        cur_codes[ch]->in = (cur_codes[ch]->in + 1) % 512;


        if ( cur_codes[ch]->size == (512-1))
        {
            cur_codes[ch]->size++;
            cur_codes[ch]->out = (cur_codes[ch]->out + 1) % 512;

        }
        else if ( cur_codes[ch]->size == 512)
            cur_codes[ch]->out = (cur_codes[ch]->out + 1) % 512;
        else
            cur_codes[ch]->size ++;
#endif

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static int ast_snoop_module_init(void)
{
	int ret;
    int ch=0;

	extern int snoop_core_loaded;
	if (!snoop_core_loaded)
			return -1;

    ast_snoop_hal_id = register_hw_hal_module(&ast_snoop_hw_hal, (void **) &snoop_core_ops);
    if (ast_snoop_hal_id < 0) {
        printk(KERN_WARNING "%s%d: register HAL HW module failed\n", AST_SNOOP_DRIVER_NAME, ch);
        return ast_snoop_hal_id;
    }

#if defined(GROUP_AST1070_COMPANION)
    for (ch=0;ch<AST_SNOOP_PORT_NUM;ch++)
#endif
    {
#if defined(GROUP_AST1070_COMPANION)
        BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N1_PORT80);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N1_PORT80);
        BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N2_PORT80);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N2_PORT80);
        BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N3_PORT80);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N3_PORT80);
        BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N4_PORT80);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N4_PORT80);
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
        BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N1_PORT80);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N1_PORT80);
        BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N2_PORT80);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N2_PORT80);
        BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N3_PORT80);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N3_PORT80);
        BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N4_PORT80);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N4_PORT80);
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#else /* !defined(GROUP_AST1070_COMPANION) */
        IRQ_SET_LEVEL_TRIGGER(AST_SNOOP_IRQ);
        IRQ_SET_HIGH_LEVEL(AST_SNOOP_IRQ);
#endif /* !defined(GROUP_AST1070_COMPANION) */

#if defined(GROUP_AST1070_COMPANION)
        switch(ch) {
            case 0:
                ret = request_irq(IRQ_BMCCC0_N1_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 1:
                ret = request_irq(IRQ_BMCCC0_N2_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 2:
                ret = request_irq(IRQ_BMCCC0_N3_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 3:
                ret = request_irq(IRQ_BMCCC0_N4_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
            case 4:
                ret = request_irq(IRQ_BMCCC1_N1_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 5:
                ret = request_irq(IRQ_BMCCC1_N2_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 6:
                ret = request_irq(IRQ_BMCCC1_N3_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 7:
                ret = request_irq(IRQ_BMCCC1_N4_PORT80, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
        }
#else /* !defined(GROUP_AST1070_COMPANION) */
        ret = request_irq(AST_SNOOP_IRQ, ast_snoop_irq_handler, IRQF_SHARED, AST_SNOOP_DRIVER_NAME, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
#endif /* !defined(GROUP_AST1070_COMPANION) */
        if (ret) {
            printk(KERN_WARNING "%s%d: request irq failed\n", AST_SNOOP_DRIVER_NAME,ch);
            goto out_register_hal;
        }

        snoop_core_ops->get_snoop_core_data(&cur_codes[ch], &pre_codes[ch], ast_snoop_hal_id, ch);
        ast_snoop_init_hw(ch);
    }

	return 0;

out_register_hal:
	unregister_hw_hal_module(EDEV_TYPE_SNOOP, ast_snoop_hal_id);

	return ret;
}

static void ast_snoop_module_exit(void)
{
    int ch=0;
#if defined(GROUP_AST1070_COMPANION)
    for (ch=0;ch<AST_SNOOP_PORT_NUM;ch++)
#endif
    {
        ast_snoop_disable_port(ch);
        ast_snoop_disable_interrupt(ch);
#if defined(GROUP_AST1070_COMPANION)
        switch(ch) {
            case 0:
                free_irq(IRQ_BMCCC0_N1_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 1:
                free_irq(IRQ_BMCCC0_N2_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 2:
                free_irq(IRQ_BMCCC0_N3_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 3:
                free_irq(IRQ_BMCCC0_N4_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
            case 4:
                free_irq(IRQ_BMCCC1_N1_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 5:
                free_irq(IRQ_BMCCC1_N2_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 6:
                free_irq(IRQ_BMCCC1_N3_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
            case 7:
                free_irq(IRQ_BMCCC1_N4_PORT80, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
                break;
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
        }
#else /* !defined(GROUP_AST1070_COMPANION) */
        free_irq(AST_SNOOP_IRQ, (void *)(snoop_as_io_base[ch]+AST_SNOOP_DEV_ID_OFFSET));
#endif /* !defined(GROUP_AST1070_COMPANION) */
    }
	unregister_hw_hal_module(EDEV_TYPE_SNOOP, ast_snoop_hal_id);
}

module_init(ast_snoop_module_init);
module_exit(ast_snoop_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150 LPC snoop driver");
MODULE_LICENSE("GPL");
