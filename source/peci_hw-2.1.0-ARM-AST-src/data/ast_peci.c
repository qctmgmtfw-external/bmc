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
 * ast_peci.c
 * ASPEED AST2100/2050/2200/2150 PECI controller driver
 *
 ****************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/delay.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "driver_hal.h"
#include "peci.h"
#include "ast_peci.h"

#define AST_PECI_DRIVER_NAME		"ast_peci"

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args)
#else
#define dbgprint(fmt, args...)
#endif

static void *ast_peci_virt_base;
static peci_core_funcs_t *peci_core_ops;
static int ast_peci_hal_id;
static struct completion ast_peci_done;

inline uint32_t ast_peci_read_reg(uint32_t reg)
{
	return ioread32((void * __iomem)ast_peci_virt_base + reg);
}

inline void ast_peci_write_reg(uint32_t data, uint32_t reg)
{
	iowrite32(data, (void *__iomem)ast_peci_virt_base + reg);
}

static void ast_peci_enable(void)
{
	uint32_t reg;

	reg = ast_peci_read_reg(AST_PECI_CTRL);
	reg |= AST_PECI_CTRL_ENABLE_PECI;
	ast_peci_write_reg(reg, AST_PECI_CTRL);
}

static void ast_peci_enable_intr(void)
{
	uint32_t reg;

	reg = ast_peci_read_reg(AST_PECI_ICR);
	reg |= AST_PECI_IRQ_DONE | AST_PECI_IRQ_WRITE_FCS_ABORT | AST_PECI_IRQ_WRITE_FCS_BAD | AST_PECI_IRQ_BUS_CONTENT;
	ast_peci_write_reg(reg, AST_PECI_ICR);
}

static int ast_peci_check_tx_fcs(void)
{
	uint32_t reg;
	uint8_t hw_fcs;
	uint8_t cap_fcs;

	reg = ast_peci_read_reg(AST_PECI_HW_FCS);
	reg = (reg & AST_PECI_HW_FCS_WR_FCS_MASK) >> AST_PECI_HW_FCS_WR_FCS_SHIFT;
	hw_fcs = (uint8_t) reg;

	reg = ast_peci_read_reg(AST_PECI_CAP_FCS);
	reg = (reg & AST_PECI_CAP_FCS_WR_FCS_MASK) >> AST_PECI_CAP_FCS_WR_FCS_SHIFT;
	cap_fcs = (uint8_t) reg;

	return (cap_fcs == hw_fcs) ? 0 : -1;
}

static int ast_peci_check_rx_fcs(void)
{
	uint32_t reg;
	uint8_t hw_fcs;
	uint8_t cap_fcs;

	reg = ast_peci_read_reg(AST_PECI_HW_FCS);
	reg = (reg & AST_PECI_HW_FCS_RD_FCS_MASK) >> AST_PECI_HW_FCS_RD_FCS_SHIFT;
	hw_fcs = (uint8_t) reg;

	reg = ast_peci_read_reg(AST_PECI_CAP_FCS);
	reg = (reg & AST_PECI_CAP_FCS_RD_FCS_MASK) >> AST_PECI_CAP_FCS_RD_FCS_SHIFT;
	cap_fcs = (uint8_t) reg;

	return (cap_fcs == hw_fcs) ? 0 : -1;
}

static void ast_peci_prepare_transfer_data(unsigned char target, unsigned char tx_len, unsigned char rx_len, unsigned char *buf, unsigned char rx_offset)
{
	uint32_t reg;
	int reg_num;
	int remainder;
	int i;

	reg = target | ((tx_len << AST_PECI_RW_LEN_WRITE_SHIFT) & AST_PECI_RW_LEN_WRITE_MASK) |
			((rx_len << AST_PECI_RW_LEN_READ_SHIFT) & AST_PECI_RW_LEN_READ_MASK);
	ast_peci_write_reg(reg, AST_PECI_RW_LEN); /* target address and command/data length */

	remainder = tx_len % 4;
	if (remainder != 0)
		tx_len += (4 - remainder); /* pad remainder, let tx_len be a multiple of 4 */

	reg_num = 0;
	while (tx_len > 0) {
		reg = 0;
		for (i = 0; i < 4; i ++) {
			reg |= ((uint32_t) buf[(4 * reg_num) + i]) << (8 * i);
		}
		ast_peci_write_reg(reg, AST_PECI_W_DATA_0 + (4 * reg_num)); /* command code and data */
		reg_num ++;
		tx_len -= 4;
	}
}

static void ast_peci_trigger_data_transfer(void)
{
	uint32_t reg;

	/* fire command by rising edge */
	reg = ast_peci_read_reg(AST_PECI_CMD);
	reg &= ~AST_PECI_CMD_FIRE;
	ast_peci_write_reg(reg, AST_PECI_CMD);

	do {
		reg = ast_peci_read_reg(AST_PECI_CMD);
	} while (reg & AST_PECI_CMD_FIRE);

	reg = ast_peci_read_reg(AST_PECI_CMD);
	reg |= AST_PECI_CMD_FIRE;
	ast_peci_write_reg(reg, AST_PECI_CMD);
}

static void ast_peci_read_data(unsigned char *rx_buf, unsigned int rx_len)
{
	uint32_t reg;
	int remainder;
	int reg_num;
	int i;

	remainder = rx_len % 4;
	if (remainder != 0)
		rx_len += (4 - remainder); /* pad remainder, let rx_len be a multiple of 4 */

	reg_num = 0;
	while (rx_len > 0) {
		reg = ast_peci_read_reg(AST_PECI_R_DATA_0 + (4 * reg_num));
		for (i = 0; i < 4; i ++) {
			rx_buf[(4 * reg_num) + i] = (uint8_t) ((reg >> (8 * i)) & 0x000000FF);
		}
		reg_num ++;
		rx_len -= 4;
	}
}

#define AST_SCU_UNLOCK_MAGIC	0x1688A8A8

static void ast_peci_init_hw(void)
{
	uint32_t reg;

	/* reset the PECI controller */
	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)SCU_KEY_CONTROL_REG); /* unlock SCU */
	reg = ioread32((void * __iomem)SCU_SYS_RESET_REG);
	reg |= (0x00000400); /* start to reset */
	iowrite32(reg, (void * __iomem) SCU_SYS_RESET_REG);
	udelay(3);
	reg &= ~(0x00000400); /* stop */
	iowrite32(reg, (void * __iomem)SCU_SYS_RESET_REG);
	iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG); /* lock SCU */

	/* PECI Control Setting, FIX clock and sampling, Read_Mode = Point sampling */
	reg = ast_peci_read_reg(AST_PECI_CTRL);
	reg &= ~(AST_PECI_CTRL_CLK_MASK | AST_PECI_CTRL_READ_MODE_MASK | AST_PECI_CTRL_SAMPLING_MASK);
	reg |= AST_PECI_CTRL_ENABLE_CLK |
			((AST_PECI_CTRL_CLK_8 << AST_PECI_CTRL_CLK_SHIFT) & AST_PECI_CTRL_CLK_MASK) |
			((AST_PECI_CTRL_READ_MODE_POINT << AST_PECI_CTRL_READ_MODE_SHIFT) & AST_PECI_CTRL_READ_MODE_MASK) |
			((AST_PECI_CTRL_SAMPLE_POINT_8 << AST_PECI_CTRL_SAMPLING_SHIFT) & AST_PECI_CTRL_SAMPLING_MASK);
	ast_peci_write_reg(reg, AST_PECI_CTRL);

	/* PECI Timing Setting, FIX negotiation timing */
	reg = ast_peci_read_reg(AST_PECI_TIMING);
	reg &= ~(AST_PECI_TIMING_MSG_MASK | AST_PECI_TIMING_ADDR_MASK);
	reg |= ((AST_PECI_MSG_TIMING_PERIOD << AST_PECI_TIMING_MSG_SHIFT) & AST_PECI_TIMING_MSG_MASK) |
			((AST_PECI_ADDR_TIMING_PERIOD << AST_PECI_TIMING_ADDR_SHIFT) & AST_PECI_TIMING_ADDR_MASK);
	ast_peci_write_reg(reg, AST_PECI_TIMING);

	/* select timming negotiation result */
	reg = ast_peci_read_reg(AST_PECI_ICR);
	reg &= ~(AST_PECI_IRQ_TIMING_SEL_MASK);
	reg |= ((AST_PECI_IRQ_TIMING_FIRST_BIT << AST_PECI_IRQ_TIMING_SEL_SHIFT) & AST_PECI_IRQ_TIMING_SEL_MASK);
	ast_peci_write_reg(reg, AST_PECI_ICR);

	ast_peci_enable_intr();
	ast_peci_enable();
}

static int ast_peci_send_cmd(peci_cmd_t *cmd)
{
	unsigned long rx_offset;
	unsigned long expire = msecs_to_jiffies(AST_PECI_TIMEOUT);
	int ret;

	rx_offset = (FEEDBACK_LENGTH + cmd->write_len);

	init_completion(&ast_peci_done);

	ast_peci_prepare_transfer_data(cmd->target, cmd->write_len, cmd->read_len, cmd->write_buffer, rx_offset);
	ast_peci_trigger_data_transfer();

	ret = wait_for_completion_timeout(&ast_peci_done, expire);
	if (ret == 0) {
		dbgprint(KERN_WARNING "%s: timeout\n", AST_PECI_DRIVER_NAME);
		ast_peci_init_hw();
		return -ETIMEDOUT;
	}

	ast_peci_read_data(cmd->read_buffer, cmd->read_len);

	cmd->status = 0;

	/* check Tx FCS */
	ret = ast_peci_check_tx_fcs();
	if (ret != 0)
		cmd->status = -1;

	/* check Rx FCS */
	if (cmd->read_len > 0) {
		ret = ast_peci_check_rx_fcs();
		if (ret != 0)
			cmd->status = -1;
	}

	return 0;
}

static peci_hal_operations_t ast_peci_hw_ops = {
	.send_peci_cmd = ast_peci_send_cmd,
};

static hw_hal_t ast_peci_hw_hal = {
	.dev_type = EDEV_TYPE_PECI,
	.owner = THIS_MODULE,
	.devname = AST_PECI_DRIVER_NAME,
	.num_instances = AST_PECI_CHANNEL_NUM,
	.phal_ops = (void *) &ast_peci_hw_ops
};



static irqreturn_t ast_peci_irq_handler(int irq, void *dev_id)
{
	uint32_t reg;

	reg = ast_peci_read_reg(AST_PECI_CMD);
	reg &= ~AST_PECI_CMD_FIRE;
	ast_peci_write_reg(reg, AST_PECI_CMD); /* back to low level */

	reg = ast_peci_read_reg(AST_PECI_ISR);
	ast_peci_write_reg(reg, AST_PECI_ISR); /* clear interrupt status */

	if (reg & AST_PECI_IRQ_DONE) {
		complete(&ast_peci_done);
	}

	if (reg & AST_PECI_IRQ_BUS_CONTENT) {
		dbgprint(KERN_WARNING "%s: bus content\n", AST_PECI_DRIVER_NAME);
	}

	if (reg & AST_PECI_IRQ_WRITE_FCS_BAD) {
		dbgprint(KERN_WARNING "%s: write FCS bad\n", AST_PECI_DRIVER_NAME);
	}

	if (reg & AST_PECI_IRQ_WRITE_FCS_ABORT) {
		dbgprint(KERN_WARNING "%s: write FCS abort\n", AST_PECI_DRIVER_NAME);
	}

	return IRQ_HANDLED;
}

static int __init ast_peci_module_init(void)
{
	int ret;

	extern int peci_core_loaded;
	if (!peci_core_loaded)
			return -1;

	if (!request_mem_region(AST_PECI_REG_BASE, AST_PECI_REG_SIZE, AST_PECI_DRIVER_NAME)) {
		printk(KERN_WARNING "%s: request memory region failed\n", AST_PECI_DRIVER_NAME);
		return -EBUSY;
	}

	ast_peci_virt_base = ioremap(AST_PECI_REG_BASE, AST_PECI_REG_SIZE);
	if (!ast_peci_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_PECI_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_mem_region;
	}

	IRQ_SET_LEVEL_TRIGGER(AST_PECI_IRQ);
	IRQ_SET_HIGH_LEVEL(AST_PECI_IRQ);

	ret = request_irq(AST_PECI_IRQ, ast_peci_irq_handler, IRQF_SHARED, AST_PECI_DRIVER_NAME, ast_peci_virt_base);
	if (ret) {
		printk(KERN_WARNING "%s: request irq failed\n", AST_PECI_DRIVER_NAME);
		goto out_iomap;
	}

	ast_peci_init_hw();

	ast_peci_hal_id = register_hw_hal_module(&ast_peci_hw_hal, (void **) &peci_core_ops);
	if (ast_peci_hal_id < 0) {
		printk(KERN_WARNING "%s: register PECI HAL HW module failed\n", AST_PECI_DRIVER_NAME);
		ret = ast_peci_hal_id;
		goto out_irq;
	}

	return 0;

out_irq:
	free_irq(AST_PECI_IRQ, ast_peci_virt_base);
out_iomap:
	iounmap(ast_peci_virt_base);
out_mem_region:
	release_mem_region(AST_PECI_REG_BASE, AST_PECI_REG_SIZE);

	return ret;
}

static void __exit ast_peci_module_exit(void)
{
	free_irq(AST_PECI_IRQ, ast_peci_virt_base);
	iounmap(ast_peci_virt_base);
	release_mem_region(AST_PECI_REG_BASE, AST_PECI_REG_SIZE);
	unregister_hw_hal_module(EDEV_TYPE_PECI, ast_peci_hal_id);
}

module_init(ast_peci_module_init);
module_exit(ast_peci_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150 PECI controller driver");
MODULE_LICENSE("GPL");
