--- linux-2.6.28.10/drivers/mtd/spichips/astspi.c	1970-01-01 08:00:00.000000000 +0800
+++ linux/drivers/mtd/spichips/astspi.c	2011-07-01 15:13:41.000000000 +0800
@@ -0,0 +1,230 @@
+/*
+ * (C) Copyright 2009
+ * American Megatrends Inc.
+ *
+ * SPI flash controller driver for the AST2300 SoC
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
+#define AST_SMC_REG_BASE		AST_SMC_VA_BASE /* 0x16000000 */
+#define AST_SMC_FLASH_CTRL_REG		0x00
+#define AST_SMC_CE2_CTRL_REG		0x0C
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
+
+#define AST_SPI_RUN_FAST_READ_MODE
+
+static void reset_flash(void)
+{
+	uint32_t reg;
+
+	#ifdef AST_SPI_RUN_FAST_READ_MODE
+	reg = ioread32(AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+	reg &= ~(AST_SPI_CMD_MASK | AST_SPI_DUMMY_MASK | AST_SPI_CMD_MODE_MASK);
+	reg |= (SPI_FAST_READ_CMD << AST_SPI_CMD_SHIFT) | AST_SPI_DUMMY_1 | AST_SPI_CE_LOW | AST_SPI_CMD_MODE_FAST;
+	iowrite32(reg, AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+	#else
+	reg = ioread32(AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+	reg &= (~AST_SPI_CMD_MODE_MASK);
+	reg |= (AST_SPI_CE_LOW | AST_SPI_CMD_MODE_NORMAL);
+	iowrite32(reg, AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+	#endif
+}
+
+static void chip_select_active(void)
+{
+	uint32_t reg;
+
+	reg = ioread32(AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG) & (~AST_SPI_CMD_MODE_MASK);
+	reg |= (AST_SPI_CE_LOW | AST_SPI_CMD_MODE_USER);
+	iowrite32(reg, AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+}
+
+static void chip_select_deactive(void)
+{
+	uint32_t reg;
+
+	reg = ioread32(AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG) & (~AST_SPI_CMD_MODE_MASK);
+	reg |= (AST_SPI_CE_HI | AST_SPI_CMD_MODE_USER);
+	iowrite32(reg, AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+}
+
+static int astspiflash_transfer(int bank, unsigned char *cmd, int cmdlen, SPI_DIR dir, unsigned char *data, unsigned long datalen)
+{
+	ulong base;
+	int i;
+
+	base = AST_SPI_FLASH_VA_BASE;
+
+	chip_select_active();
+
+	/* issue command */
+	for (i = 0; i < cmdlen; i ++)
+		*((volatile unsigned char *) base) = cmd[i];
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
+	chip_select_deactive();
+
+	reset_flash();
+
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
+
+	/* according to AST2300 spec, clock of SPI controller can not exceed 50M Hz */
+	if (clock > (50 * 1000000))
+		clock = 50 * 1000000;
+
+	/* read CPU clock rate and CPU/AHB ratio from SCU */
+	reg = ioread32(SCU_HW_STRAPPING_REG);
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
+
+	clock_divisor = 1;
+	while ((cpu_clock / clock_divisor) > clock) {
+		clock_divisor ++;
+		if (clock_divisor == 16)
+			break;
+	}
+
+	reg = ioread32(AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+	reg &= ~AST_SPI_CLOCK_MASK;
+	reg |= (clock_selection_table[clock_divisor - 1] << AST_SPI_CLOCK_SHIFT);
+	iowrite32(reg, AST_SMC_REG_BASE + AST_SMC_CE2_CTRL_REG);
+
+	return 0;
+}
+
+struct spi_ctrl_driver astspi_driver = {
+	.name = "astspiflash",
+	.module = THIS_MODULE,
+	.max_read = (32 * 1024 * 1024), /* 32 MB */
+	.fast_read = 1,
+	.spi_transfer = astspiflash_transfer,
+	.spi_burst_read = astspiflash_transfer,
+	.spi_configure_clock = astspiflash_configure_clock,
+};
+
+static int astspi_init(void)
+{
+	init_MUTEX(&astspi_driver.lock);
+	register_spi_ctrl_driver(&astspi_driver);
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
+MODULE_DESCRIPTION("AST2300 SOC SPI flash controller driver");
