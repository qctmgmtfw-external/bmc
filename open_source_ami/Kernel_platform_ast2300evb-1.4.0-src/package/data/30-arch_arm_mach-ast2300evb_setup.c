--- linux-2.6.28.10/arch/arm/mach-ast2300evb/setup.c	1970-01-01 08:00:00.000000000 +0800
+++ linux/arch/arm/mach-ast2300evb/setup.c	2011-07-01 15:14:51.000000000 +0800
@@ -0,0 +1,133 @@
+/*
+ *  linux/arch/arm/mach-ast2300evb/setup.c
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ *
+ */
+#include <linux/mm.h>
+#include <linux/init.h>
+#include <linux/major.h>
+#include <linux/fs.h>
+#include <linux/device.h>
+#include <linux/serial.h>
+#include <linux/tty.h>
+#include <linux/serial_core.h>
+
+#include <asm/io.h>
+#include <asm/pgtable.h>
+#include <asm/page.h>
+#include <asm/mach/map.h>
+#include <asm/setup.h>
+#include <asm/system.h>
+#include <asm/memory.h>
+#include <mach/hardware.h>
+#include <asm/mach-types.h>
+#include <asm/mach/arch.h>
+
+static struct map_desc ast2050evb_std_desc[] __initdata = {
+	{ AST_SMC_VA_BASE,             __phys_to_pfn(AST_SMC_BASE),         	SZ_4K,		MT_DEVICE},
+	{ AST_AHBC_VA_BASE,            __phys_to_pfn(AST_AHBC_BASE),           SZ_64K,		MT_DEVICE},
+	{ AST_MAC1_VA_BASE,            __phys_to_pfn(AST_MAC1_BASE),		    SZ_64K,		MT_DEVICE},
+	{ AST_MAC2_VA_BASE,            __phys_to_pfn(AST_MAC2_BASE),		    SZ_64K,		MT_DEVICE},
+	{ AST_USB20_VA_BASE,           __phys_to_pfn(AST_USB20_BASE),          SZ_64K,		MT_DEVICE},
+	{ AST_IC_VA_BASE,			    __phys_to_pfn(AST_IC_BASE),			    SZ_64K,		MT_DEVICE},
+	{ AST_SDRAMC_VA_BASE,          __phys_to_pfn(AST_SDRAMC_BASE),         SZ_4K,		MT_DEVICE},
+	{ AST_USB1_VA_BASE,            __phys_to_pfn(AST_USB1_BASE),           SZ_4K,		MT_DEVICE},
+	{ AST_SCU_VA_BASE,			    __phys_to_pfn(AST_SCU_BASE),		    SZ_4K,		MT_DEVICE},
+	{ AST_CRYPTO_VA_BASE,          __phys_to_pfn(AST_CRYPTO_BASE),         SZ_4K,		MT_DEVICE},
+	{ AST_ADPCM_VA_BASE,           __phys_to_pfn(AST_ADPCM_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_I2S_VA_BASE,             __phys_to_pfn(AST_I2S_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_GRAPHIC_VA_BASE,         __phys_to_pfn(AST_GRAPHIC_BASE),        SZ_4K,		MT_DEVICE},
+	{ AST_VIDEO_VA_BASE,           __phys_to_pfn(AST_VIDEO_BASE),		    SZ_64K,		MT_DEVICE},
+	{ AST_AHB_TO_PBUS_VA_BASE,    	__phys_to_pfn(AST_AHB_TO_PBUS_BASE),	SZ_64K,		MT_DEVICE},
+	{ AST_MDMA_VA_BASE,            __phys_to_pfn(AST_MDMA_BASE),           SZ_64K,		MT_DEVICE},
+	{ AST_2D_VA_BASE,              __phys_to_pfn(AST_2D_BASE),             SZ_64K,		MT_DEVICE},
+	{ AST_GPIO_VA_BASE,		    __phys_to_pfn(AST_GPIO_BASE),           SZ_4K,		MT_DEVICE},
+	{ AST_RTC_VA_BASE,             __phys_to_pfn(AST_RTC_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_TIMER_VA_BASE,		    __phys_to_pfn(AST_TIMER_BASE),		    SZ_4K,		MT_DEVICE},
+	{ AST_UART1_VA_BASE,           __phys_to_pfn(AST_UART1_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART2_VA_BASE,           __phys_to_pfn(AST_UART2_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART3_VA_BASE,           __phys_to_pfn(AST_UART3_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART4_VA_BASE,           __phys_to_pfn(AST_UART4_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART5_VA_BASE,           __phys_to_pfn(AST_UART5_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_WDT_VA_BASE,			    __phys_to_pfn(AST_WDT_BASE),		    SZ_4K,		MT_DEVICE},
+	{ AST_PWM_VA_BASE,             __phys_to_pfn(AST_PWM_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_VUART0_VA_BASE,          __phys_to_pfn(AST_VUART0_BASE),         SZ_4K,		MT_DEVICE},
+	{ AST_VUART1_VA_BASE,          __phys_to_pfn(AST_VUART1_BASE),         SZ_4K,		MT_DEVICE},
+	{ AST_LPC_VA_BASE,             __phys_to_pfn(AST_LPC_BASE),		    SZ_4K,		MT_DEVICE},
+	{ AST_I2C_VA_BASE,             __phys_to_pfn(AST_I2C_BASE),		    SZ_4K,		MT_DEVICE},
+	{ AST_PECI_VA_BASE,            __phys_to_pfn(AST_PECI_BASE),           SZ_4K,		MT_DEVICE},
+	{ AST_SPI_FLASH_VA_BASE,       __phys_to_pfn(AST_SPI_FLASH_BASE),   	SZ_32M,		MT_DEVICE},
+};
+
+int io_mapping_done = 0;
+
+/*
+static struct uart_port early_serial_ports[] = {
+	{
+		.membase	= (char *)AST_UART1_VA_BASE,
+		.mapbase	= AST_UART1_BASE,
+		.irq		= IRQ_UART1,
+		.flags		= UPF_SKIP_TEST,
+		.iotype		= UPIO_PORT,
+		.regshift	= 2,
+		.uartclk	= CONFIG_UART_CLK,
+		.line		= 0,
+		.type		= PORT_16550A,
+		.fifosize	= 16
+	}
+};
+*/
+
+/*
+extern void printascii(char *Str);
+
+void
+EarlyPrintk(char *Str)
+{
+	printascii(Str);
+}
+*/
+
+void __init ast2050evb_init(void)
+{
+	/* Do any initial hardware setup if needed */
+}
+
+void __init ast2050evb_map_io(void)
+{
+	/* Do any initial hardware setup if needed */
+	iotable_init(ast2050evb_std_desc, ARRAY_SIZE(ast2050evb_std_desc));
+	io_mapping_done = 1;
+//	early_serial_setup(&early_serial_ports[0]);
+}
+
+extern struct sys_timer ast2050_timer;
+extern void ast2050_init_irq(void);
+
+MACHINE_START(AST2300EVB, "AST2300EVB")
+	.phys_io		= AST_REGISTER_BASE,
+	.phys_ram		= 0x40000000,
+	.io_pg_offst	= ((AST_REGISTER_VA_BASE) >> 18) & 0xfffc,
+/* Note: 512 Mb of Register space is mapped during init, so that we can use
+         serial port and check point display to output debug info */
+	.map_io			= ast2050evb_map_io,
+	.timer			= &ast2050_timer,
+	.init_irq		= ast2050_init_irq,
+	.boot_params	= 0x00000100,
+	.init_machine	= ast2050evb_init,
+MACHINE_END
