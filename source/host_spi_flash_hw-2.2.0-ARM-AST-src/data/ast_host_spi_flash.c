/*****************************************************************
 *****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 *****************************************************************
 *****************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#include "driver_hal.h"
#include "host_spi_flash.h"

/* base address of flash */
#define AST_HOST_SPI_FLASH_NAME         "ast_host_spi_flash"
#define AST_HOST_SPI_FLASH_PHYS_ADDR    0x30000000
/*issue commands, allocate full flash size is not necessary */
#define AST_HOST_SPI_FLASH_SIZE         SZ_4K

/* base address of registers */
#define AST_HOST_SPI_REG_BASE           0x1E630000

/* offset of registers */
#define AST_HOST_SPI_CFG_REG            0x00
#ifdef CONFIG_SOC_AST2500
 #define AST_HOST_SPI_CE_CTRL_REG		0x4
 #define AST_HOST_SPI_CTRL_REG			0x10
#else
 #define AST_HOST_SPI_CTRL_REG			0x04
 #define AST_HOST_SPI_MISC_REG			0x10
#endif

#define AST_HOST_SPI_CMD_MASK           0x00FF0000 /* bit[23:16] */
#define AST_HOST_SPI_CMD_SHIFT          16

#define AST_HOST_SPI_DUMMY_MASK         0x000000C0 /* bit[7:6] */
#define AST_HOST_SPI_DUMMY_0            0x00000000
#define AST_HOST_SPI_DUMMY_1            0x00000040
#define AST_HOST_SPI_DUMMY_2            0x00000080
#define AST_HOST_SPI_DUMMY_3            0x000000C0

#define AST_HOSTSPI_INACT_PULS_WID_MASK 0x0F000000
#define AST_HOSTSPI_IO_MODE_MASK        0xF0000000

#define AST_HOST_SPI_CMD_MODE_MASK      0x00000003 /* bit[1:0] */
#define AST_HOST_SPI_CMD_MODE_NORMAL    0x00
#define AST_HOST_SPI_CMD_MODE_FAST      0x01
#define AST_HOST_SPI_CMD_MODE_WRITE     0x02
#define AST_HOST_SPI_CMD_MODE_USER      0x03
#define AST_HOST_SPI_CE_MASK            0x00000004 /* bit 2 */
#define AST_HOST_SPI_CE_LOW             0x00
#define AST_HOST_SPI_CE_HI              0x04

#ifdef CONFIG_SOC_AST2500
 #define AST_HOST_SPI_CFG_EN_FL_WRITE    0x00030000 /* bit[17:16] */
 #define AST_HOST_SPI_ADDR_CYCLE_4B      0x00000001 /* bit 0 */
#else 
 #define AST_HOST_SPI_CFG_EN_FL_WRITE    0x00000001 /* bit 0 */
 #define AST_HOST_SPI_ADDR_CYCLE_4B      0x00002000 /* bit 13 */
#endif 

/* bits of SPI flash status register */
#define SPI_FLASH_SR_WIP                0x01
#define SPI_FLASH_SR_WEL                0x02
#define SPI_FLASH_SR_SRWD               0x07

#define SPI_FLASH_PAGE_SZ               256

/* Define max times to check status register before we give up. */
#define MAX_READY_WAIT_COUNT            100000

#define AST_HOST_SPI_CLOCK_MASK         0x00000F00 /* bit[11:8] */
#define AST_HOST_SPI_CLOCK_SHIFT        8

#define DISABLE_MODE                    0	// Quanta
#define MASTER_MODE                     1
#define BYPASS_MODE                     2
#define LPC_TO_AHB_BRIDGE_MODE          3

#define SPI_FAST_READ_CMD               0x0B

#define BYTE_3_ADDR_MODE                0
#define BYTE_4_ADDR_MODE                1

#define AST_SCU_KEY                     0x1688A8A8
#define AST_SCU_LPC_AHB_MODE_MASK       0x00003000
#define AST_SCU_LPC_AHB_MASTER_MODE     0x00001000
#define AST_SCU_LPC_AHB_BRIDGE_MODE     0x00002000
#define AST_SCU_LPC_AHB_BYPASS_MODE     0x00003000

static void __iomem *ast_host_spi_flash_virt;
static void __iomem *ast_host_spi_flash_mem_virt;
static struct host_spi_flash_core_ops_t *host_spi_flash_core_ops;
static int ast_host_spi_flash_hal_hw_id;
static void ast_host_spi_flash_change_mode(int mode);

static const unsigned char clock_selection_table[] =
{
    0x0F, /* 1 */
    0x07, /* 2 */
    0x0E, /* 3 */
    0x06, /* 4 */
    0x0D, /* 5 */
    0x05, /* 6 */
    0x0C, /* 7 */
    0x04, /* 8 */
    0x0B, /* 9 */
    0x03, /* 10 */
    0x0A, /* 11 */
    0x02, /* 12 */
    0x09, /* 13 */
    0x01, /* 14 */
    0x08, /* 15 */
    0x00, /* 16 */
};

static int hostspiflash_configure_clock(unsigned int clock)
{
    uint32_t reg;
    uint32_t cpu_clock;
    uint32_t clock_divisor;
#ifdef CONFIG_SOC_AST2500	
		uint32_t cpu_ratio;
		uint32_t axi_ratio;
#endif		
    /* according to AST2300 spec, clock of SPI controller can not exceed 50 MHz */
    if (clock > (50 * 1000000))
        clock = 50 * 1000000;

    /* read CPU clock rate and CPU/AHB ratio from SCU */
    reg = ioread32((void __iomem *)SCU_HW_STRAPPING_REG);
    
    #ifdef CONFIG_SOC_AST2500
    switch ((reg & 0x00000100) >> 8) {
    case 0x00:
    	cpu_ratio = 1;
    	break;
    case 0x01:
    	cpu_ratio = 2;
    	break;
    }
    
    switch ((reg & 0x00000E00) >> 9) {
    case 0x01:
    	axi_ratio = 2;
    	break;
    case 0x02:
    	axi_ratio = 3;
    	break;
    case 0x03:
    	axi_ratio = 4;
    	break;
    case 0x04:
    	axi_ratio = 5;
    	break;
    case 0x05:
    	axi_ratio = 6;
    	break;
    case 0x06:
    	axi_ratio = 7;
    	break;
    case 0x07:
    	axi_ratio = 8;
    	break;
    default:
    	axi_ratio = 2;
    	break;
    }
    
    cpu_clock = 792 * 1000000;//Default H-PLL value
    cpu_clock = cpu_clock / cpu_ratio / axi_ratio;	
    #else
    switch ((reg & 0x00000300) >> 8) {
    case 0x00:
        cpu_clock = 384 * 1000000;
        break;
    case 0x01:
        cpu_clock = 360 * 1000000;
        break;
    case 0x02:
        cpu_clock = 336 * 1000000;
        break;
    case 0x03:
        cpu_clock = 408 * 1000000;
        break;
    default:
        cpu_clock = 408 * 1000000;
    }

    switch ((reg & 0x00000C00) >> 10)
    {
    case 0x01:
        cpu_clock /= 2;
        break;
    case 0x02:
        cpu_clock /= 4;
        break;
    case 0x03:
        cpu_clock /= 3;
        break;
    }
    #endif

    clock_divisor = 1;
    while ((cpu_clock / clock_divisor) > clock)
    {
        clock_divisor ++;
        if (clock_divisor == 16)
            break;
    }

    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
    reg &= ~AST_HOST_SPI_CLOCK_MASK;
    reg |= (clock_selection_table[clock_divisor - 1] << AST_HOST_SPI_CLOCK_SHIFT);
    printk ("Configuring the Host SPI Clock to HCLK/%d \n", clock_divisor);
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);

    return 0;
}

static void reset_flash(void)
{
    uint32_t reg;

#ifdef CONFIG_SPX_FEATURE_HOST_SPI_USE_FAST_READ
    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
    reg &= ~(AST_HOST_SPI_CMD_MASK | AST_HOST_SPI_DUMMY_MASK | AST_HOST_SPI_CMD_MODE_MASK | AST_HOSTSPI_INACT_PULS_WID_MASK | AST_HOSTSPI_IO_MODE_MASK);
    reg |= (SPI_FAST_READ_CMD << AST_HOST_SPI_CMD_SHIFT) | AST_HOST_SPI_DUMMY_1 | AST_HOST_SPI_CE_LOW | AST_HOST_SPI_CMD_MODE_FAST;
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
#else
    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
    reg &= ~(AST_HOST_SPI_CMD_MASK | AST_HOST_SPI_DUMMY_MASK | AST_HOST_SPI_CMD_MODE_MASK | AST_HOSTSPI_INACT_PULS_WID_MASK | AST_HOSTSPI_IO_MODE_MASK);
    reg |= (AST_HOST_SPI_CMD_MODE_NORMAL << AST_HOST_SPI_CMD_SHIFT) | AST_HOST_SPI_CE_LOW | AST_HOST_SPI_CMD_MODE_NORMAL;
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
#endif
}

static void enable_spi_write(void)
{
    uint32_t reg;

    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CFG_REG);
    reg |= AST_HOST_SPI_CFG_EN_FL_WRITE;
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CFG_REG);
}

static void chip_select_active(void)
{
    uint32_t reg;

    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
    reg &= ~(AST_HOST_SPI_CMD_MODE_MASK | AST_HOST_SPI_CE_MASK);
    reg |= (AST_HOST_SPI_CE_LOW | AST_HOST_SPI_CMD_MODE_USER);
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
}

static void chip_select_deactive(void)
{
    uint32_t reg;

    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
    reg &= ~(AST_HOST_SPI_CMD_MODE_MASK | AST_HOST_SPI_CE_MASK);
    reg |= (AST_HOST_SPI_CE_HI | AST_HOST_SPI_CMD_MODE_USER);
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
}

static int ast_host_spi_flash_transfer(unsigned char *cmd, unsigned int cmd_len, unsigned char *data, unsigned long data_len, int data_dir)
{
    int i;

    chip_select_active();

    /* issue command */
    for (i = 0; i < cmd_len; i ++)
        iowrite8(cmd[i], ast_host_spi_flash_mem_virt);

    if (data_dir == 1)
    {
        /* write data to flash */
        for (i = 0; i < data_len; i ++)
        {
            iowrite8(data[i], ast_host_spi_flash_mem_virt);
        }
    }
    else if(data_dir == 0)
    {
        /* read data from flash */
        for (i = 0; i < data_len; i ++)
        {
            data[i] = ioread8(ast_host_spi_flash_mem_virt);
        }
    }

    chip_select_deactive();

    reset_flash();

    return 0;
}

static int ast_host_spi_flash_addr_mode(unsigned int addr_mode)
{
    uint32_t reg;

    #ifdef CONFIG_SOC_AST2500
    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CE_CTRL_REG);
    #else
    reg = ioread32(ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
    #endif

    if (addr_mode == BYTE_4_ADDR_MODE)
    {
        reg |= AST_HOST_SPI_ADDR_CYCLE_4B;
    }
    else
    {
        reg &= ~(AST_HOST_SPI_ADDR_CYCLE_4B);
    }

    #ifdef CONFIG_SOC_AST2500
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CE_CTRL_REG);
    #else
    iowrite32(reg, ast_host_spi_flash_virt + AST_HOST_SPI_CTRL_REG);
    #endif

    return 0;
}

static int ast_host_spi_flash_max_read(void)
{
    return (64 * 1024 * 1024); /* 64 MB */
}

struct host_spi_flash_hal_ops_t ast_host_spi_flash_ops =
{
    .max_read = ast_host_spi_flash_max_read,
    .transfer = ast_host_spi_flash_transfer,
    .burst_read = ast_host_spi_flash_transfer,
    .configure_clock = hostspiflash_configure_clock,
    .set_addr_mode = ast_host_spi_flash_addr_mode,
    .host_spi_flash_change_mode = ast_host_spi_flash_change_mode,
    .max_clock = (int)(CONFIG_SPX_FEATURE_HOST_SPI_CLOCK_FREQUENCY) * 1000 * 1000
};

hw_hal_t ast_host_spi_flash_hw_hal =
{
    .dev_type = EDEV_TYPE_HOST_SPI_FLASH,
    .owner = THIS_MODULE,
    .devname = "ast_host_spi_flash",
    .num_instances = 1,
    .phal_ops = (void *) &ast_host_spi_flash_ops
};

static void ast_host_spi_flash_change_mode(int mode)
{
    uint32_t reg;

    iowrite32(AST_SCU_KEY, (void __iomem *)SCU_KEY_CONTROL_REG); /* unlock SCU */

    reg = ioread32((void __iomem *)SCU_HW_STRAPPING_REG);
    reg &= ~(AST_SCU_LPC_AHB_MODE_MASK);

    switch (mode)
    {
        case DISABLE_MODE:	// Quanta
        break;
    	
        case MASTER_MODE:
            reg |= AST_SCU_LPC_AHB_MASTER_MODE;
        break;

        case BYPASS_MODE:
            reg |= AST_SCU_LPC_AHB_BYPASS_MODE;
        break;

        case LPC_TO_AHB_BRIDGE_MODE:
            reg |= AST_SCU_LPC_AHB_BRIDGE_MODE;
        break;
    }

    iowrite32(reg, (void __iomem *)SCU_HW_STRAPPING_REG);

    iowrite32(0, (void __iomem *)SCU_KEY_CONTROL_REG); /* lock SCU */
}

int __init ast_host_spi_flash_init(void)
{
    int rc;

    if (!request_mem_region(AST_HOST_SPI_REG_BASE, SZ_4K, "ast_host_spi_flash"))
    {
        return -EBUSY;
    }

    ast_host_spi_flash_virt = ioremap(AST_HOST_SPI_REG_BASE, SZ_4K);
    if (!ast_host_spi_flash_virt)
    {
        rc = -ENOMEM;
        goto out_release_mem_region_spi;
    }

    if (!request_mem_region(AST_HOST_SPI_FLASH_PHYS_ADDR, AST_HOST_SPI_FLASH_SIZE, AST_HOST_SPI_FLASH_NAME))
    {
        rc = -ENODEV;
        goto out_iounmap_spi;
    }

    ast_host_spi_flash_mem_virt = ioremap(AST_HOST_SPI_FLASH_PHYS_ADDR, AST_HOST_SPI_FLASH_SIZE);
    if (!ast_host_spi_flash_mem_virt)
    {
        rc = -ENOMEM;
        goto out_release_mem_region;
    }

#ifndef CONFIG_SPX_FEATURE_GLOBAL_ENABLE_LPC_TO_AHB_BRIDGE
    ast_host_spi_flash_change_mode(MASTER_MODE);
#endif
    enable_spi_write();

    reset_flash();

    ast_host_spi_flash_hal_hw_id = register_hw_hal_module(&ast_host_spi_flash_hw_hal, (void **) &host_spi_flash_core_ops);
    if (ast_host_spi_flash_hal_hw_id < 0)
    {
#ifndef CONFIG_SPX_FEATURE_GLOBAL_ENABLE_LPC_TO_AHB_BRIDGE
        //ast_host_spi_flash_change_mode(BYPASS_MODE);DISABLE_MODE
        ast_host_spi_flash_change_mode(DISABLE_MODE);	// Quanta, Quanta doesn't use bypass, GPIO I0~I3 used as GPIO
#endif
        goto out_iounmap;
    }

    //Need to change mode to bypass so host can access bios spi
//    ast_host_spi_flash_change_mode(BYPASS_MODE);//Raincer-- Quanta use External MUX, not use Bypass cause clock frequency
    return 0;

out_iounmap:
    iounmap(ast_host_spi_flash_mem_virt);
out_release_mem_region:
    release_mem_region(AST_HOST_SPI_FLASH_PHYS_ADDR, AST_HOST_SPI_FLASH_SIZE);
out_iounmap_spi:
    iounmap(ast_host_spi_flash_virt);
out_release_mem_region_spi:
    release_mem_region(AST_HOST_SPI_REG_BASE, SZ_4K);

    return rc;
}

void __exit ast_host_spi_flash_exit(void)
{
    unregister_hw_hal_module(EDEV_TYPE_HOST_SPI_FLASH, ast_host_spi_flash_hal_hw_id);

#ifndef CONFIG_SPX_FEATURE_GLOBAL_ENABLE_LPC_TO_AHB_BRIDGE
    //ast_host_spi_flash_change_mode(BYPASS_MODE);
    ast_host_spi_flash_change_mode(DISABLE_MODE);	// Quanta, Quanta doesn't use bypass, GPIO I0~I3 used as GPIO
#endif

    if (ast_host_spi_flash_mem_virt)
    {
        iounmap(ast_host_spi_flash_mem_virt);
        release_mem_region(AST_HOST_SPI_FLASH_PHYS_ADDR, AST_HOST_SPI_FLASH_SIZE);
    }

    if (ast_host_spi_flash_virt)
    {
        iounmap(ast_host_spi_flash_virt);
        release_mem_region(AST_HOST_SPI_REG_BASE, SZ_4K);
    }
}

module_init(ast_host_spi_flash_init);
module_exit(ast_host_spi_flash_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("Common module of host SPI flash driver for AST series SoC");
MODULE_LICENSE("GPL");
