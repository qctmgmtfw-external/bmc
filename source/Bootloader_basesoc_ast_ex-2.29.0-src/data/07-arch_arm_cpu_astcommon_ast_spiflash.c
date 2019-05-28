--- u-boot-2013.07/arch/arm/cpu/astcommon/ast_spiflash.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/ast_spiflash.c	2013-12-05 12:17:55.079503948 -0500
@@ -0,0 +1,420 @@
+/*
+ * (C) Copyright 2009-2013
+ * American Megatrends Inc.
+ *
+ * SPI flash controller driver for the AST SoC
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+#include <common.h>
+#include <asm/processor.h>
+#include <asm/byteorder.h>
+#include <environment.h>
+#include "soc_hw.h"
+
+#include "spiflash.h"
+
+#define AST_FMC_CE0_BASE			CONFIG_SYS_FLASH_BASE
+
+#define AST_FMC_REG_BASE		    	0x1e620000
+#define AST_FMC_FLASH_CTRL_REG			0x00
+#define AST_FMC_CTRL_REG_SIZE			0x04
+#define AST_FMC_CE_CTRL_REG			0x04
+#define AST_FMC_CE0_CTRL_REG		0x10
+#define AST_4BYTES_MODE_REG			0x00002000/*bit 13*/
+#define AST_ADDRESS_CYCLE_4BYTE_REG	0x00000001/*bit 0*/
+
+#define AST_SPI_CMD_MASK			0x00FF0000 /* bit[23:16] */
+#define AST_SPI_CMD_SHIFT			16
+
+#define AST_SPI_CLOCK_MASK			0x00000F00 /* bit[11:8] */
+#define AST_SPI_CLOCK_SHIFT			8
+
+#define AST_SPI_DUMMY_MASK			0x000000C0 /* bit[7:6] */
+#define AST_SPI_DUMMY_0				0x00000000
+#define AST_SPI_DUMMY_1				0x00000040
+#define AST_SPI_DUMMY_2				0x00000080
+#define AST_SPI_DUMMY_3				0x000000C0
+
+#define AST_SPI_DATA_SINGLE			0x00000000
+#define AST_SPI_DATA_DUAL			0x00000008
+#define AST_SPI_DUAL_IO		0x00000002
+#define AST_SPI_FULL_DUAL_IO		0x00000003
+#define AST_SPI_DATA_MASK		7
+#define AST_SPI_DUAL_IO_SHIFT		28
+
+
+#define AST_SPI_CE_LOW				0x00000000
+#define AST_SPI_CE_HI				0x00000004
+
+#define AST_SPI_CMD_MODE_MASK		0x00000007 /* bit[2:0] */
+#define AST_SPI_CMD_MODE_NORMAL		0x00000000
+#define AST_SPI_CMD_MODE_FAST		0x00000001
+#define AST_SPI_CMD_MODE_WRITE		0x00000002
+#define AST_SPI_CMD_MODE_USER		0x00000003
+
+#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530)
+ #define AST_SPI_FLASH_SEGMEMT_WRITE	0x00070000 /* bit[18:16] */
+#else
+ #define AST_SPI_LEGACY_MDOE_DISABLE	0x80000000 /* bit[21] */
+ #define AST_SPI_FLASH_SEGMEMT_WRITE	0x001F0000 /* bit[20:16] */
+#endif
+
+#define SPI_FAST_READ_CMD		0x0B
+#define SPI_DREAD_CMD		0x3B
+#define SPI_2READ_CMD		0xBB
+
+#define AST_CE0_CTRL_REGISTER   0x30
+#define AST_CE1_CTRL_REGISTER   0x34
+#define AST_CE2_CTRL_REGISTER   0x38
+#if !defined(CONFIG_AST2500) && !defined(CONFIG_AST2530)
+ #define AST_CE3_CTRL_REGISTER   0x3C
+ #define AST_CE4_CTRL_REGISTER   0x40
+#endif
+#define SZ_8M 0x800000
+#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530)
+ static u32 fmc_ctl_reg[] = {AST_CE0_CTRL_REGISTER, AST_CE1_CTRL_REGISTER, AST_CE2_CTRL_REGISTER};
+ static u32 spi_base[] = { 0x20000000, 0x28000000, 0x2A000000 };
+#else
+ static u32 fmc_ctl_reg[] = {AST_CE0_CTRL_REGISTER, AST_CE1_CTRL_REGISTER, AST_CE2_CTRL_REGISTER, AST_CE3_CTRL_REGISTER, AST_CE4_CTRL_REGISTER};
+ static u32 spi_base[] = { 0x20000000, 0x24000000, 0x26000000, 0x28000000, 0x2A000000 };
+#endif
+static unsigned long fmc_start_size = 0;
+static int *g_fast_read = 0;
+
+void post_spi_init(int bank, struct spi_flash_info *spi_info)
+{
+	u32 val = 0, flash_size;
+	//u32 end_size = 0;
+	udelay(100);
+	if(bank == 0) fmc_start_size = 0;
+	flash_size = (spi_info->size / SZ_8M);
+	if (flash_size == 0) flash_size = 1;
+	spi_base[bank+1] = spi_base[bank] + spi_info->size;
+	val = (((1 << 6) + (fmc_start_size + flash_size)) << 24) + (((1 << 6) +  fmc_start_size) << 16);
+	*((volatile u32 *)(AST_FMC_REG_BASE + fmc_ctl_reg[bank])) = val;
+	fmc_start_size += flash_size;
+	val = (((1 << 6) + (fmc_start_size + 4)) << 24) + (((1 << 6) +  fmc_start_size) << 16);
+	*((volatile u32 *)(AST_FMC_REG_BASE + fmc_ctl_reg[bank+1])) = val;
+	udelay(100);
+}
+static void reset_flash (int bank)
+{
+	u32 reg;
+	u32 ctrl_reg;
+
+	// bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+	ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	reg = *((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg));
+	if (*g_fast_read == 3)//2xI/O Read
+	{
+		reg &= ~(AST_SPI_CMD_MASK | AST_SPI_DUMMY_MASK | AST_SPI_CMD_MODE_MASK);
+		reg |= (SPI_2READ_CMD << AST_SPI_CMD_SHIFT) | AST_SPI_DUMMY_1 | AST_SPI_CE_LOW | AST_SPI_CMD_MODE_FAST | (AST_SPI_FULL_DUAL_IO << AST_SPI_DUAL_IO_SHIFT);
+	}
+	else if (*g_fast_read == 2)//Dual Read
+	{
+		reg &= ~(AST_SPI_CMD_MASK | AST_SPI_DUMMY_MASK | AST_SPI_CMD_MODE_MASK);
+		reg |= (SPI_DREAD_CMD << AST_SPI_CMD_SHIFT) | AST_SPI_DUMMY_1 | AST_SPI_CE_LOW | AST_SPI_CMD_MODE_FAST | (AST_SPI_DUAL_IO << AST_SPI_DUAL_IO_SHIFT);	
+	}	
+	else if (*g_fast_read == 1)//Fast Read
+	{
+		reg &= ~(AST_SPI_CMD_MASK | AST_SPI_DUMMY_MASK | AST_SPI_CMD_MODE_MASK);
+		reg |= (SPI_FAST_READ_CMD << AST_SPI_CMD_SHIFT) | AST_SPI_DUMMY_1 | AST_SPI_CE_LOW | AST_SPI_CMD_MODE_FAST;
+	}
+	else
+	{
+		reg &= (~AST_SPI_CMD_MODE_MASK);
+		reg |= (AST_SPI_CE_LOW | AST_SPI_CMD_MODE_NORMAL);
+	}
+  *((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg)) = reg;
+  
+  reg = *((volatile u32 *)(AST_FMC_REG_BASE + AST_FMC_CE_CTRL_REG));
+  reg &= AST_ADDRESS_CYCLE_4BYTE_REG;
+
+  /*flash >= 16M*/
+  /*enable 32 bit address mode*/
+  if(reg == 0x01)
+  	*((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg))|= AST_4BYTES_MODE_REG;
+
+	return;
+}
+
+static void reset_iomode (int bank)
+{
+	u32 reg;
+	u32 ctrl_reg;
+
+	// bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+	ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	reg = *((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg));
+	reg &= ~(AST_SPI_CMD_MASK | AST_SPI_DUMMY_MASK | (AST_SPI_DATA_MASK << AST_SPI_DUAL_IO_SHIFT));
+
+	if (*g_fast_read == 2)//Dual Read
+		reg |= (AST_SPI_DUAL_IO << AST_SPI_DUAL_IO_SHIFT);		
+	else if (*g_fast_read == 3)//2xI/O Read
+		reg |= (AST_SPI_FULL_DUAL_IO << AST_SPI_DUAL_IO_SHIFT);
+		
+	*((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg)) = reg;	
+
+	return;
+}
+
+static void chip_select_active(int bank)
+{
+	u32 reg;
+	u32 ctrl_reg;
+
+	// bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+	ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	reg = *((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg)); 
+	reg &= (~(AST_SPI_CMD_MODE_MASK | (AST_SPI_DATA_MASK << AST_SPI_DUAL_IO_SHIFT)) );
+	reg |= AST_SPI_CE_LOW | AST_SPI_CMD_MODE_USER;
+	*((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg)) = reg;
+
+	return;
+}
+
+static void chip_select_deactive(int bank)
+{
+	u32 reg;
+	u32 ctrl_reg;
+
+	// bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+	ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	reg = *((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg)); 
+	reg &= (~(AST_SPI_CMD_MODE_MASK | (AST_SPI_DATA_MASK << AST_SPI_DUAL_IO_SHIFT)) );
+	reg |= AST_SPI_CE_HI | AST_SPI_CMD_MODE_USER;
+	*((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg)) = reg;
+
+	return;
+}
+
+static const unsigned char clock_selection_table[] = {
+	0x0F, /* 1 */
+	0x07, /* 2 */
+	0x0E, /* 3 */
+	0x06, /* 4 */
+	0x0D, /* 5 */
+	0x05, /* 6 */
+	0x0C, /* 7 */
+	0x04, /* 8 */
+	0x0B, /* 9 */
+	0x03, /* 10 */
+	0x0A, /* 11 */
+	0x02, /* 12 */
+	0x09, /* 13 */
+	0x01, /* 14 */
+	0x08, /* 15 */
+	0x00, /* 16 */
+};
+
+static int ast_spiflash_configure_clock(int bank, unsigned int clock)
+{
+	u32 reg;
+	u32 cpu_clock;
+	u32 clock_divisor;
+	u32 ctrl_reg;
+#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530)
+	u32 cpu_ratio;
+	u32 axi_ratio;
+#endif
+	// bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+	ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	/* according to AST spec, clock of SPI controller can not exceed 50M Hz */
+	if (clock > (50 * 1000000))
+		clock = 50 * 1000000;
+
+	/* read CPU clock rate and CPU/AHB ratio from SCU */
+	reg = *((volatile u32 *) SCU_HW_STRAPPING_REG);
+	
+	#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530)
+	cpu_ratio = 2;
+
+	switch ((reg & 0x00000E00) >> 9) {
+	case 0x01:
+		axi_ratio = 2;
+		break;
+	case 0x02:
+		axi_ratio = 3;
+		break;
+	case 0x03:
+		axi_ratio = 4;
+		break;		
+	case 0x04:
+		axi_ratio = 5;
+		break;		
+	case 0x05:
+		axi_ratio = 6;
+		break;		
+	case 0x06:
+		axi_ratio = 7;
+		break;		
+	case 0x07:
+		axi_ratio = 8;
+		break;		
+	default:
+		axi_ratio = 2;
+		break;
+	}
+
+	cpu_clock = 792 * 1000000;//Default H-PLL value
+	cpu_clock = cpu_clock / cpu_ratio / axi_ratio;
+	
+	#else
+	switch ((reg & 0x00000300) >> 8) {
+	case 0x00:
+		cpu_clock = 384 * 1000000;
+		break;
+	case 0x01:
+		cpu_clock = 360 * 1000000;
+		break;
+	case 0x02:
+		cpu_clock = 336 * 1000000;
+		break;
+	case 0x03:
+		cpu_clock = 408 * 1000000;
+		break;
+	default:
+		cpu_clock = 408 * 1000000;
+	}
+
+	switch ((reg & 0x00000C00) >> 10) {
+	case 0x01:
+		cpu_clock /= 2;
+		break;
+	case 0x02:
+		cpu_clock /= 4;
+		break;
+	case 0x03:
+		cpu_clock /= 3;
+		break;
+	}
+	#endif
+
+	clock_divisor = 1;
+	while ((cpu_clock / clock_divisor) > clock) {
+		clock_divisor ++;
+		if (clock_divisor == 16)
+			break;
+	}
+
+	reg = *((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg));
+	reg &= ~AST_SPI_CLOCK_MASK;
+	reg |= (clock_selection_table[clock_divisor - 1] << AST_SPI_CLOCK_SHIFT);
+	*((volatile u32 *) (AST_FMC_REG_BASE + ctrl_reg)) = reg;
+
+	return 0;
+}
+
+static int ast_spiflash_transfer(int bank, unsigned char *cmd, int cmdlen, SPI_DIR dir, unsigned char *data, unsigned long datalen,unsigned long flashsize)
+{
+	ulong base;
+	int i;
+
+	/* Set the base address based on bank */
+	base = spi_base[bank];
+
+	//printk("bank = %d, base = %x\n",bank,base); 
+
+	chip_select_active(bank);
+
+	if (cmd[0] == 0xBB)
+	{
+		*((volatile unsigned char *) base) = cmd[0];
+		reset_iomode(bank);
+		for (i = 1; i < cmdlen; i ++)
+			*((volatile unsigned char *) base) = cmd[i];
+	}
+	else if (cmd[0] == 0x3B)
+	{
+		for (i = 0; i < cmdlen; i ++)
+			*((volatile unsigned char *) base) = cmd[i];
+		reset_iomode(bank);			
+	}
+	else
+	{
+	/* issue command */
+	for (i = 0; i < cmdlen; i ++)
+		*((volatile unsigned char *) base) = cmd[i];
+	}
+
+	if (dir == SPI_WRITE) {
+		/* write data to flash */
+		for (i = 0; i < datalen; i ++) {
+			*((volatile unsigned char *) base) = data[i];
+		}
+	} else if (dir == SPI_READ) {
+		/* read data from flash */
+		for (i = 0; i < datalen; i ++) {
+			data[i] = ((volatile unsigned char *) base)[i];
+		}
+	}
+
+	chip_select_deactive(bank);
+
+	reset_flash(bank);
+
+	return 0;
+}
+
+static int ast_spiflash_init(void)
+{
+
+//	printf("AST SPI Init Called\n");
+	#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530)
+	*(volatile u32 *) (AST_FMC_REG_BASE + AST_FMC_FLASH_CTRL_REG) |= (AST_SPI_FLASH_SEGMEMT_WRITE); /* enable Flash Write */	
+	#else
+	*(volatile u32 *) (AST_FMC_REG_BASE + AST_FMC_FLASH_CTRL_REG) |= (AST_SPI_FLASH_SEGMEMT_WRITE | AST_SPI_LEGACY_MDOE_DISABLE); /* enable Flash Write */
+	#endif
+
+	// Reset the flash modules
+	reset_flash(0);	// CE0
+	reset_flash(1);	// CE1
+	reset_flash(2);	// CE2
+
+	return 0;
+}
+
+struct spi_ctrl_driver SPI_CTRL_DRIVER = {
+	.name = "ast_spiflash",
+	.module = THIS_MODULE,
+	.max_read = (32 * 1024 * 1024), /* 32 MB */
+	#ifdef CONFIG_FLASH_OPERATION_MODE_MASK
+	.operation_mode_mask = CONFIG_FLASH_OPERATION_MODE_MASK,
+	#else
+	.operation_mode_mask = 0x00010003, //Default
+	#endif
+	.fast_read = 1,
+	.fast_write = 0,	
+	.spi_transfer = ast_spiflash_transfer,
+	.spi_burst_read = ast_spiflash_transfer,
+	.spi_init = ast_spiflash_init,
+	.spi_configure_clock = ast_spiflash_configure_clock,
+};
+
+void SPI_CTRL_DRIVER_INIT(void)
+{
+	g_fast_read = &SPI_CTRL_DRIVER.fast_read;
+	ast_spiflash_init();
+	init_MUTEX(&SPI_CTRL_DRIVER.lock);
+	register_spi_ctrl_driver(&SPI_CTRL_DRIVER);
+}
