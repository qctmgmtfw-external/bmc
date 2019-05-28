--- linux/arch/arm/mach-astevb/setup.c	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/arch/arm/mach-astevb/setup.c	2015-02-16 18:12:09.635794385 +0800
@@ -0,0 +1,268 @@
+/*********************************************************************************  
+ *************** IMPORTANT NOTE:  READ THIS BEFORE EDITING THIS FILE  ************
+ *********************************************************************************
+ *	This file is same across AST2300/AST2400/AST3100 (except MACHINE_START Values)       *
+ *	Make sure the changes are done on the files for all SOC mentioned above      *  
+ *********************************************************************************
+*/
+/*
+ *  linux/arch/arm/mach-astevb/setup.c
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
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"14M"
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
+#include <linux/dma-contiguous.h>
+#include <linux/platform_device.h>
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
+static struct map_desc astevb_std_desc[] __initdata = {
+	{ AST_SMC_VA_BASE,             __phys_to_pfn(AST_SMC_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_FMC_VA_BASE,             __phys_to_pfn(AST_FMC_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_AHBC_VA_BASE,            __phys_to_pfn(AST_AHBC_BASE),           SZ_64K,		MT_DEVICE},
+	{ AST_MAC1_VA_BASE,            __phys_to_pfn(AST_MAC1_BASE),		   SZ_64K,		MT_DEVICE},
+	{ AST_MAC2_VA_BASE,            __phys_to_pfn(AST_MAC2_BASE),		   SZ_64K,		MT_DEVICE},
+#if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT >= 3)	
+	{ AST_MAC3_VA_BASE,           __phys_to_pfn(AST_MAC3_BASE),            SZ_64K,      MT_DEVICE},  
+	{ AST_MAC4_VA_BASE,           __phys_to_pfn(AST_MAC4_BASE),            SZ_64K,      MT_DEVICE},    
+#endif    	
+#if defined(CONFIG_SOC_AST3100) || defined(CONFIG_SOC_AST3200) || defined(CONFIG_SOC_AST2500)
+	{ AST_USB20_VA_BASE,           __phys_to_pfn(AST_USB20_BASE),          SZ_4K,		MT_DEVICE},
+#else
+    { AST_USB20_VA_BASE,           __phys_to_pfn(AST_USB20_BASE),          SZ_8K,		MT_DEVICE},
+#endif
+#if defined(CONFIG_SOC_AST3200) || defined(CONFIG_SOC_AST2500)
+	{ AST_USB20_1_VA_BASE,         __phys_to_pfn(AST_USB20_1_BASE),        SZ_4K,		MT_DEVICE},
+	{ AST_USB20_2_VA_BASE,         __phys_to_pfn(AST_USB20_2_BASE),        SZ_4K,		MT_DEVICE},
+#endif
+#ifdef CONFIG_SOC_AST3200
+	{ AST_PCIE_VA_BASE,            __phys_to_pfn(AST_PCIE_BASE),           SZ_4K,		MT_DEVICE},
+#endif
+	{ AST_USB11_VA_BASE,           __phys_to_pfn(AST_USB11_BASE),          SZ_64K,		MT_DEVICE},
+	{ AST_IC_VA_BASE,			   __phys_to_pfn(AST_IC_BASE),			   SZ_64K,		MT_DEVICE},
+	{ AST_SDRAMC_VA_BASE,          __phys_to_pfn(AST_SDRAMC_BASE),         SZ_4K,		MT_DEVICE},
+	{ AST_SCU_VA_BASE,			   __phys_to_pfn(AST_SCU_BASE),		       SZ_4K,		MT_DEVICE},
+	{ AST_CRYPTO_VA_BASE,          __phys_to_pfn(AST_CRYPTO_BASE),         SZ_4K,		MT_DEVICE},
+	{ AST_JTAG_VA_BASE,		       __phys_to_pfn(AST_JTAG_BASE),		   SZ_4K,		MT_DEVICE}, 
+	{ AST_GRAPHIC_VA_BASE,         __phys_to_pfn(AST_GRAPHIC_BASE),        SZ_4K,		MT_DEVICE},
+	{ AST_ADC_VA_BASE,             __phys_to_pfn(AST_ADC_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_AHB_TO_PBUS_VA_BASE,     __phys_to_pfn(AST_AHB_TO_PBUS_BASE),	   SZ_64K,		MT_DEVICE},
+	{ AST_MDMA_VA_BASE,            __phys_to_pfn(AST_MDMA_BASE),           SZ_64K,		MT_DEVICE},
+	{ AST_2D_VA_BASE,              __phys_to_pfn(AST_2D_BASE),             SZ_64K,		MT_DEVICE},
+	{ AST_GPIO_VA_BASE,            __phys_to_pfn(AST_GPIO_BASE),           SZ_4K,		MT_DEVICE},
+	{ AST_RTC_VA_BASE,             __phys_to_pfn(AST_RTC_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_TIMER_VA_BASE,		   __phys_to_pfn(AST_TIMER_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART1_VA_BASE,           __phys_to_pfn(AST_UART1_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART2_VA_BASE,           __phys_to_pfn(AST_UART2_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART3_VA_BASE,           __phys_to_pfn(AST_UART3_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART4_VA_BASE,           __phys_to_pfn(AST_UART4_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_UART5_VA_BASE,           __phys_to_pfn(AST_UART5_BASE),          SZ_4K,		MT_DEVICE},
+	{ AST_WDT_VA_BASE,			    __phys_to_pfn(AST_WDT_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_PWM_VA_BASE,             __phys_to_pfn(AST_PWM_BASE),            SZ_4K,		MT_DEVICE},
+	{ AST_I2C_VA_BASE,             __phys_to_pfn(AST_I2C_BASE),		       SZ_4K,		MT_DEVICE},
+	{ AST_SPI_FLASH_VA_BASE,       __phys_to_pfn(AST_SPI_FLASH_BASE),      SZ_64M,		MT_DEVICE},
+
+#if !defined(CONFIG_SOC_AST3100) && !defined(CONFIG_SOC_AST3200)
+	{ AST_DMA_VA_BASE,	           __phys_to_pfn(AST_DMA_BASE),	           SZ_4K,		MT_DEVICE},
+	{ AST_MCTP_VA_BASE,		       __phys_to_pfn(AST_MCTP_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_LPC_PLUS_VA_BASE,	       __phys_to_pfn(AST_LPC_PLUS_BASE),	   SZ_4K,		MT_DEVICE},
+	{ AST_VIDEO_VA_BASE,	       __phys_to_pfn(AST_VIDEO_BASE),		   SZ_128K,	MT_DEVICE},
+	{ AST_PECI_VA_BASE,		       __phys_to_pfn(AST_PECI_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_VUART0_VA_BASE,	       __phys_to_pfn(AST_VUART0_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_VUART1_VA_BASE,	       __phys_to_pfn(AST_VUART1_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_LPC_VA_BASE,		       __phys_to_pfn(AST_LPC_BASE),		       SZ_4K,		MT_DEVICE},
+	{ AST_USB1_VA_BASE,		       __phys_to_pfn(AST_USB1_BASE),		   SZ_4K,		MT_DEVICE},
+	
+#endif
+
+#if defined(CONFIG_SOC_AST2530)
+	{ AST_UART6_VA_BASE,	       __phys_to_pfn(AST_UART6_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART7_VA_BASE,	       __phys_to_pfn(AST_UART7_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART8_VA_BASE,	       __phys_to_pfn(AST_UART8_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART9_VA_BASE,	       __phys_to_pfn(AST_UART9_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART10_VA_BASE,	       __phys_to_pfn(AST_UART10_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART11_VA_BASE,	       __phys_to_pfn(AST_UART11_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART12_VA_BASE,	       __phys_to_pfn(AST_UART12_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART13_VA_BASE,	       __phys_to_pfn(AST_UART13_BASE),		   SZ_4K,		MT_DEVICE},
+	{ AST_UART_UDMA_VA_BASE,       __phys_to_pfn(AST_UART_UDMA_BASE),	   SZ_4K,		MT_DEVICE},
+#endif
+
+#if defined(CONFIG_SOC_AST3100) || defined(CONFIG_SOC_AST3200)
+	{ AST_PS2_VA_BASE,             __phys_to_pfn(AST_PS2_BASE),             SZ_4K,		MT_DEVICE},
+	{ AST_RFX_VA_BASE,             __phys_to_pfn(AST_RFX_BASE),             SZ_4K,		MT_DEVICE},
+#endif
+#ifdef CONFIG_SOC_AST3200
+	{ AST_H264_VA_BASE,            __phys_to_pfn(AST_H264_BASE),            SZ_4K,		MT_DEVICE},
+#endif
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
+static inline void ast_arch_idle(void)
+{
+        /*
+         * This should do all the clock switching
+         * and wait for interrupt tricks
+         */
+        cpu_do_idle();
+}
+
+static void ast_arch_reset(enum reboot_mode reboot_mode, const char *cmd)
+{
+        /*
+         * Use WDT to restart system
+         */
+	*(volatile unsigned long *) (WDT2_RELOAD_REG) = 0x10;
+	*(volatile unsigned long *) (WDT2_CNT_RESTART_REG) = 0x4755;
+	*(volatile unsigned long *) (WDT2_CONTROL_REG) = 0x03;
+}
+
+#if defined(CONFIG_SOC_AST3100) || defined(CONFIG_SOC_AST3200)
+void ast_ehci_init(void);
+void ast_uhci_init(void);
+#endif
+
+#ifdef CONFIG_SPX_FEATURE_PACKAGES_SSP_SUPPORT
+static struct platform_device ssp_device = {
+	.name	= "ssp-device",
+	.dev	= {
+		.coherent_dma_mask	= 0xFF000000UL,
+	},
+};
+#endif
+
+void __init astevb_init(void)
+{
+	/* Register idle and restart handler */
+	arm_pm_idle = ast_arch_idle;
+	arm_pm_restart = ast_arch_reset;
+	/* Do any initial hardware setup if needed */
+
+#ifdef CONFIG_SPX_FEATURE_PACKAGES_SSP_SUPPORT
+	platform_device_register(&ssp_device);
+#endif
+
+#if defined(CONFIG_SOC_AST3100) || defined(CONFIG_SOC_AST3200)
+	/* Register EHCI and UHCI Host devices */
+	ast_ehci_init();
+	ast_uhci_init();
+#endif
+
+}
+
+#ifdef CONFIG_SPX_FEATURE_PACKAGES_SSP_SUPPORT
+#define DMA_ALLOC_BASE "0x86400000"
+#define DMA_ALLOC_SIZE "14M"
+static void __init ssp_reserve_cma(void)
+{
+	int ret;
+	phys_addr_t ssp_base = 0;
+	unsigned long ssp_size = 0;
+
+	ssp_base = memparse(DMA_ALLOC_BASE, NULL);
+	ssp_size = memparse(DMA_ALLOC_SIZE, NULL);
+	printk("SSP reserved %ld @ %p - %p\n", ssp_size, (void*)ssp_base, (void*)ssp_base + ssp_size);
+	ret = dma_declare_contiguous(&ssp_device.dev, ssp_size, ssp_base, 0);
+	if(ret)
+		printk("%s: dma_declare_contiguous failed %d\n", __func__, ret);
+}
+#endif
+
+void __init astevb_map_io(void)
+{
+	/* Do any initial hardware setup if needed */
+	iotable_init(astevb_std_desc, ARRAY_SIZE(astevb_std_desc));
+	io_mapping_done = 1;
+//	early_serial_setup(&early_serial_ports[0]);
+}
+
+extern void ast_timer_init(void);
+extern void ast_init_irq(void);
+
+#ifdef CONFIG_SOC_AST3100
+MACHINE_START(AST3100EVB, "AST3100EVB")
+#endif
+#ifdef CONFIG_SOC_AST2300
+MACHINE_START(AST2300EVB, "AST2300EVB")
+#endif
+#ifdef CONFIG_SOC_AST2400
+MACHINE_START(AST2400EVB, "AST2400EVB")
+#endif
+#ifdef CONFIG_SOC_AST2500
+MACHINE_START(AST2500EVB, "AST2500EVB")
+#endif
+#ifdef CONFIG_SOC_AST2530
+MACHINE_START(AST2530EVB, "AST2530EVB")
+#endif
+#ifdef CONFIG_SOC_AST3200
+MACHINE_START(AST3200EVB, "AST3200EVB")
+#endif
+	/* Note: 512 Mb of Register space is mapped during init, so that we can use
+         serial port and check point display to output debug info */
+	.map_io		= astevb_map_io,
+	.init_time	= ast_timer_init,
+	.init_irq	= ast_init_irq,
+	.atag_offset	= 0x100,
+	.init_machine	= astevb_init,
+#ifdef CONFIG_SPX_FEATURE_PACKAGES_SSP_SUPPORT
+.reserve		= ssp_reserve_cma,
+#endif
+MACHINE_END
