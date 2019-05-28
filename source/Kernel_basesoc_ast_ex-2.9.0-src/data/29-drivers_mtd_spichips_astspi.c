--- linux.old/drivers/mtd/spichips/astspi.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-3.14.17-ami/drivers/mtd/spichips/astspi.c	2014-08-21 17:10:39.781933682 -0400
@@ -0,0 +1,374 @@
+/*
+ * (C) Copyright 2009
+ * American Megatrends Inc.
+ *
+ * SPI flash controller driver for the AST SoC
+ *
+ * See file CREDITS for list of people who contributed to this
+ * project.
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
+#include "spiflash.h"
+
+#define AST_FMC_REG_BASE		AST_FMC_VA_BASE /* 0x1E620000 */
+#define AST_FMC_FLASH_CTRL_REG		0x00
+
+#define AST_FMC_CE0_CTRL_REG		0x10
+#define AST_FMC_CTRL_REG_SIZE           0x04 
+
+#define AST_SPI_CMD_MASK		0x00FF0000 /* bit[23:16] */
+#define AST_SPI_CMD_SHIFT		16
+
+#define AST_SPI_CLOCK_MASK		0x00000F00 /* bit[11:8] */
+#define AST_SPI_CLOCK_SHIFT		8
+
+#define AST_SPI_DUMMY_MASK		0x000000C0 /* bit[7:6] */
+#define AST_SPI_DUMMY_0			0x00000000
+#define AST_SPI_DUMMY_1			0x00000040
+#define AST_SPI_DUMMY_2			0x00000080
+#define AST_SPI_DUMMY_3			0x000000C0
+
+#define AST_SPI_DATA_SINGLE		0x00000000
+#define AST_SPI_DATA_DUAL		0x00000008
+#define AST_SPI_DUAL_IO		0x00000002
+#define AST_SPI_FULL_DUAL_IO		0x00000003
+#define AST_SPI_DATA_MASK		7
+#define AST_SPI_DUAL_IO_SHIFT		28
+
+#define AST_SPI_CE_LOW			0x00000000
+#define AST_SPI_CE_HI			0x00000004
+
+#define AST_SPI_CMD_MODE_MASK		0x00000007 /* bit[2:0] */
+#define AST_SPI_CMD_MODE_NORMAL		0x00000000
+#define AST_SPI_CMD_MODE_FAST		0x00000001
+#define AST_SPI_CMD_MODE_WRITE		0x00000002
+#define AST_SPI_CMD_MODE_USER		0x00000003
+
+#define SPI_FAST_READ_CMD		0x0B
+#define SPI_DREAD_CMD		0x3B
+#define SPI_2READ_CMD		0xBB
+
+extern unsigned long ractrends_spiflash_flash_size[MAX_SPI_BANKS];
+static int *g_fast_read = 0;
+
+static void reset_flash(int bank)
+{
+	uint32_t reg;
+	uint32_t ctrl_reg;
+
+        // bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+        ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+	
+	reg = ioread32((void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
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
+	iowrite32(reg, (void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+	
+}
+
+static void reset_iomode (int bank)
+{
+	uint32_t reg;
+	uint32_t ctrl_reg;
+
+	// bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+	ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	reg = ioread32((void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+	reg &= ~(AST_SPI_CMD_MASK | AST_SPI_DUMMY_MASK | (AST_SPI_DATA_MASK << AST_SPI_DUAL_IO_SHIFT));
+
+	if (*g_fast_read == 2)//Dual Read
+		reg |= (AST_SPI_DUAL_IO << AST_SPI_DUAL_IO_SHIFT);		
+	else if (*g_fast_read == 3)//2xI/O Read
+		reg |= (AST_SPI_FULL_DUAL_IO << AST_SPI_DUAL_IO_SHIFT);
+		
+	iowrite32(reg, (void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+
+	return;
+}
+
+static void chip_select_active(int bank)
+{
+	uint32_t reg;
+	uint32_t ctrl_reg;
+
+        // bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+        ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	reg = ioread32((void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+	reg &= (~(AST_SPI_CMD_MODE_MASK | (AST_SPI_DATA_MASK << AST_SPI_DUAL_IO_SHIFT)) );
+	reg |= (AST_SPI_CE_LOW | AST_SPI_CMD_MODE_USER);
+	iowrite32(reg, (void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+}
+
+static void chip_select_deactive(int bank)
+{
+	uint32_t reg;
+	uint32_t ctrl_reg;
+
+        // bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+        ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	reg = ioread32((void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+	reg &= (~(AST_SPI_CMD_MODE_MASK | (AST_SPI_DATA_MASK << AST_SPI_DUAL_IO_SHIFT)) );
+	reg |= (AST_SPI_CE_HI | AST_SPI_CMD_MODE_USER);
+	iowrite32(reg, (void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+}
+
+static int astspiflash_transfer(int bank, unsigned char *cmd, int cmdlen, SPI_DIR dir, unsigned char *data, unsigned long datalen)
+{
+	ulong base;
+	int i;
+	ulong offset = 0;
+	
+	for(i = 0; i < bank; i++) 
+	{
+		offset += ractrends_spiflash_flash_size[i];
+	}
+
+	base = AST_SPI_FLASH_VA_BASE + offset;
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
+
+	reset_flash(bank);
+	return 0;
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
+static int astspiflash_configure_clock(int bank, unsigned int clock)
+{
+	uint32_t reg;
+	uint32_t cpu_clock;
+	uint32_t clock_divisor;
+	uint32_t ctrl_reg;
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+	uint32_t cpu_ratio;
+	uint32_t axi_ratio;
+#endif		
+        // bank = 0,CE0 (0x10) ; bank = 1,CE1 (0x14) ; bank = 2,CE2 (0x18)
+        ctrl_reg = AST_FMC_CE0_CTRL_REG + (bank * AST_FMC_CTRL_REG_SIZE);
+
+	/* according to AST spec, clock of SPI controller can not exceed 50M Hz */
+	if (clock > (50 * 1000000))
+		clock = 50 * 1000000;
+
+	/* read CPU clock rate and CPU/AHB ratio from SCU */
+	reg = ioread32((void __iomem *)SCU_HW_STRAPPING_REG);
+	
+	#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+	switch ((reg & 0x00000100) >> 8) {
+	case 0x00:
+		cpu_ratio = 1;
+		break;
+	case 0x01:
+		cpu_ratio = 2;
+		break;
+	}
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
+	reg = ioread32((void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+	reg &= ~AST_SPI_CLOCK_MASK;
+	reg |= (clock_selection_table[clock_divisor - 1] << AST_SPI_CLOCK_SHIFT);
+	iowrite32(reg, (void __iomem *)AST_FMC_REG_BASE + ctrl_reg);
+
+	return 0;
+}
+
+struct spi_ctrl_driver astspi_driver = {
+	.name = "astspiflash",
+	.module = THIS_MODULE,
+	.max_read = (64 * 1024 * 1024), /* 32 MB */
+	#ifdef CONFIG_FLASH_OPERATION_MODE_MASK
+	.operation_mode_mask = CONFIG_FLASH_OPERATION_MODE_MASK,
+	#else
+	.operation_mode_mask = 0x00010003, //Default
+	#endif
+	.fast_read = 1,
+	.fast_write = 0,	
+	.spi_transfer = astspiflash_transfer,
+	.spi_burst_read = astspiflash_transfer,
+	.spi_configure_clock = astspiflash_configure_clock,
+};
+
+static int astspi_init(void)
+{
+	sema_init(&astspi_driver.lock, 1);
+	register_spi_ctrl_driver(&astspi_driver);
+
+	g_fast_read = &astspi_driver.fast_read;
+
+	reset_flash(0);	// CE0
+
+	reset_flash(1);	// CE1
+
+	reset_flash(2);	// CE2
+	#if !defined(CONFIG_SOC_AST2500) && !defined(CONFIG_SOC_AST2530)
+	reset_flash(3);	// CE3
+	#endif
+
+	return 0;
+}
+
+static void astspi_exit(void)
+{
+	unregister_spi_ctrl_driver(&astspi_driver);
+}
+
+module_init(astspi_init);
+module_exit(astspi_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("AST SOC SPI flash controller driver");
