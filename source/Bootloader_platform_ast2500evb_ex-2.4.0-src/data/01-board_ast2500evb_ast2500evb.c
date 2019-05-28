--- uboot/board/ast2500evb/ast2500evb.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot.new/board/ast2500evb/ast2500evb.c	2014-08-18 14:11:10.892269163 +0800
@@ -0,0 +1,165 @@
+/*************************** IMPORTANT ************************************
+ *
+ *          !!!!!!!READ THIS BEFORE EDITING THIS FILE !!!!!!
+ *
+ *  This file is duplicated across AST2300/AST2400/AST3100/AST2500/AST3200
+ *  So any modifications done here should be reflected on other files also 
+ *
+ **************************************************************************
+ */
+
+/*
+ * (C) Copyright 2013
+ *  American Megatrends Inc.
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+
+#include <common.h>
+#include "ast_hw.h"
+#include "i2c.h"
+
+#define AST_FMC_REG_BASE	0x1e620000
+#define AST_FMC_FLASH_CTRL_REG	0x00
+#define AST_CE0_CTRL_REGISTER	0x30
+#define AST_CE1_CTRL_REGISTER	0x34
+#define AST_CE2_CTRL_REGISTER	0x38
+#define AST_CE3_CTRL_REGISTER	0x3C
+#define AST_CE4_CTRL_REGISTER	0x40
+
+#define AST_SCU_REG_BASE	0x1e6e2000
+#define AST_SCU_MULTI_FN_REG	0x88
+
+#define AST_EN_ROMCS0_4_PINS	0x0F000000		// Enable ROMCS[0-4] in SCU
+
+#define SPI_CONTROL_REG			0x04
+
+#if !defined(CONFIG_AST2500) && !defined(CONFIG_AST3200)
+#define AST_CE_SPI_CHIPS		0x000002AA		// Set CE[0-4] as SPI
+#define AST_CE0_SEGMENT_ADDRESS 0x48400000      // 0x20000000 - 0x23FFFFFF
+#define AST_CE1_SEGMENT_ADDRESS 0x4C480000      // 0x24000000 - 0x25FFFFFF
+#define AST_CE2_SEGMENT_ADDRESS 0x504C0000      // 0x26000000 - 0x27FFFFFF
+#define AST_CE3_SEGMENT_ADDRESS 0x54500000      // 0x28000000 - 0x29FFFFFF
+#define AST_CE4_SEGMENT_ADDRESS 0x58540000      // 0x2A000000 - 0x2BFFFFFF  		
+#else
+#define AST_CE_SPI_CHIPS		0x0000002A		// Set CE[0-3] as SPI 
+#define AST_CE0_SEGMENT_ADDRESS 0x50400000      // 0x20000000 - 0x27FFFFFF
+#define AST_CE1_SEGMENT_ADDRESS 0x54500000      // 0x28000000 - 0x29FFFFFF
+#define AST_CE2_SEGMENT_ADDRESS 0x58540000      // 0x2A000000 - 0x2BFFFFFF
+#endif
+
+extern void soc_init(void);
+
+DECLARE_GLOBAL_DATA_PTR;
+
+#if defined(CONFIG_SHOW_BOOT_PROGRESS)
+void show_boot_progress(int progress)
+{
+    printf("Boot reached stage %d\n", progress);
+}
+#endif
+
+/*
+ * Miscellaneous platform dependent initialisations
+ */
+
+int board_early_init_f (void)
+{
+
+	return 0;
+}
+
+int board_init (void)
+{
+    
+	/*Should match with linux mach type for this board */
+	gd->bd->bi_arch_number = 900;
+	
+	/* Should match with linux Makefile.boot entry for params-phys-y */
+	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x0000100;
+	
+	gd->flags = 0;
+
+	icache_enable ();
+	
+#ifdef CONFIG_AST2500	
+	/* Set I2C Bus Number */
+	i2c_set_bus_num(CONFIG_I2C_CHANNEL_ID);
+#endif	
+
+	return 0;
+}
+
+
+int misc_init_r (void)
+{
+	
+#ifdef CONFIG_HDMI_SUPPORT			/* AST 3100 & AST3200? */
+	/* Enable HDMI Recevier if required */
+	extern int hdmi_init(void); 
+	hdmi_init();
+#endif	
+	
+#ifdef CONFIG_AST3100EVB
+	
+	*((volatile ulong*) 0x1e6e2000) = 0x1688A8A8;	/* unlock SCU */
+	
+	/* Set OSC output to 12 MHz for Codec */
+	*(volatile u32 *)(0x1e6e2010) |= 0x00000030;
+	mdelay(1);									/* Wait for SCU14 becomes 0 */
+	*(volatile u32 *)(0x1e6e2010) |= 0x00000039;	
+	mdelay(1);									/* Delay 1ms*/
+	*(volatile u32 *)(0x1e6e2010) |= 0x0000003b;	
+	mdelay(1);									/* Wait for SCU10[6]=1 */
+
+//	printf("OSC Multiplier = 0x%x\n", *(volatile u32 *)(0x1e6e2014));
+
+	/* Set Mux to output the OSC clock */ 	
+	*(volatile u32 *)(0x1e6e202c) |= 0x00080012;	
+
+	/* Enable USB1.1 Host Port 2 function */
+	*(volatile u32 *)(0x1e6e2090) |= 0x00000008;	/* Set Bit 3 */
+	
+	/* Drive GPIO pins for LED, Power On, USB HUB Reset */
+     *(volatile u32 *)(0x1e780004) |= 0x0000000e;	
+     *(volatile u32 *)(0x1e780000) |= 0x0000000f;
+	
+	*((volatile ulong*) SCU_KEY_CONTROL_REG) = 0; /* lock SCU */
+	
+#endif	
+
+	setenv("verify", "n");
+	return (0);
+}
+
+/******************************
+ Routine:
+ Description:
+******************************/
+int dram_init (void)
+{
+	bd_t *bd = gd->bd;
+
+	/* dram_init must store complete ramsize in gd->ram_size */
+	gd->ram_size=CONFIG_SYS_SDRAM_SYS_USED;
+	
+	/* Only one Bank*/
+	bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
+	bd->bi_dram[0].size = CONFIG_SYS_SDRAM_SYS_USED;
+
+	return (0);
+}
