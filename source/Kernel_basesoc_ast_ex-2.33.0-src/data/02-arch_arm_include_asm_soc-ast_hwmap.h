--- linux_orig/arch/arm/include/asm/soc-ast/hwmap.h	1969-12-31 19:00:00.000000000 -0500
+++ linux_mod/arch/arm/include/asm/soc-ast/hwmap.h	2015-12-24 15:05:11.312006697 -0500
@@ -0,0 +1,534 @@
+/*
+ *  This file contains the AST SOC Register Base, IRQ and DMA  mappings
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
+ */
+
+#ifndef     _HWREG_ASTCOMMON_H_
+#define     _HWMAP_ASTCOMMON_H_
+
+
+/********************************************************************
+   Internal Register Mapping Actual = 0x1E600000 to 0x1E78CFFF
+   After Align the base and size    = 0x16000000 to 0x1EFFFFFF
+*********************************************************************/
+
+#define AST_REGISTER_BASE              0x14000000
+#define AST_REGISTER_SIZE              0x0A800000
+
+/*--------------------------- Individual Devices ------------------------- */
+#define AST_SPI_FLASH_BASE             CONFIG_SPX_FEATURE_GLOBAL_FLASH_START
+#define AST_SMC_BASE                   0x16000000
+#define AST_AHBC_BASE                  0x1E600000
+#define AST_FMC_BASE                   0x1E620000
+#define AST_SPI_BASE                   0X1e630000
+#define AST_MIC_BASE                   0x1E640000
+#define AST_MAC1_BASE                  0x1E660000
+#define AST_MAC2_BASE                  0x1E680000
+#define AST_MAC3_BASE                  0x1E670000    /* ???? */
+#define AST_MAC4_BASE                  0x1E690000	 /* ???? */
+#define AST_USB20_BASE                 0x1E6A0000
+#if defined(CONFIG_SOC_AST3200) || defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define AST_USB20_1_BASE               0x1E6A1000	/* Only in AST3200 and maybe in AST2500 too */
+#define AST_USB20_2_BASE               0x1E6A3000	/* Only in AST3200 and maybe in AST2500 too */
+#endif
+#define AST_USB11_BASE                 0x1E6B0000
+#define AST_IC_BASE                    0x1E6C0000
+#define AST_SDRAMC_BASE                0x1E6E0000
+#define AST_USB1_BASE                  0x1E6E1000	 /* Not in AST3100 */
+#define AST_SCU_BASE                   0x1E6E2000
+#define AST_CRYPTO_BASE                0x1E6E3000  
+#define AST_JTAG_BASE                  0x1E6E4000 
+#define AST_GRAPHIC_BASE               0x1E6E6000
+#define AST_DMA_BASE                   0x1E6E7000	 /* Not in AST3100 */
+#define AST_MCTP_BASE                  0x1E6E8000    /* Not in AST3100 */
+#define AST_ADC_BASE                   0x1E6E9000
+#define AST_PS2_BASE                   0x1E6EA000    /* AST3100 Specific */
+#define AST_RFX_BASE                   0x1E6EB000    /* AST3100 Specific */
+#define AST_LPC_PLUS_BASE              0x1E6EC000	 /* Not in AST3100   */
+#ifdef CONFIG_SOC_AST3200
+#define AST_PCIE_BASE                  0x1E6ED000
+#define AST_P2X_BASE                   0x1E6F0000	/* P2X Bridge  */
+#define AST_PCIE_WIN_BASE              0x70000000	/* PCIe Host Bus Bridge */
+#define AST_PCIE_WIN_SIZE              0x10000000
+#else
+#define AST_PCI_MASTER1_BASE           0x60000000    /* Not in AST3200   */
+#define AST_PCI_MASTER2_BASE           0x80000000    /* Not in AST3200   */
+#endif
+#define AST_VIDEO_BASE                 0x1E700000	 /* Not in AST3100   */  
+#define AST_AHB_TO_PBUS_BASE           0x1E720000	 /* SRAM */ 	
+#define AST_MDMA_BASE                  0x1E740000	 /* SD/MMC controller */
+#define AST_2D_BASE                    0x1E760000
+#define AST_GPIO_BASE                  0x1E780000
+#define AST_RTC_BASE                   0x1E781000
+#define AST_TIMER_BASE                 0x1E782000
+#define AST_UART1_BASE                 0x1E783000
+#define AST_UART5_BASE                 0x1E784000
+#define AST_WDT_BASE                   0x1E785000
+#define AST_PWM_BASE                   0x1E786000
+#define AST_VUART0_BASE                0x1E787000    /* Not in AST3100   */
+#define AST_VUART1_BASE                0x1E788000    /* Not in AST3100   */
+#define AST_LPC_BASE                   0x1E789000    /* Not in AST3100   */
+#define AST_I2C_BASE                   0x1E78A000
+#define AST_PECI_BASE                  0x1E78B000    /* Not in AST3100   */
+#define AST_UART2_BASE                 0x1E78D000
+#define AST_UART3_BASE                 0x1E78E000
+#define AST_UART4_BASE                 0x1E78F000
+#define AST_PCIARBITER_BASE            0x1E78C000
+#if defined(CONFIG_SOC_AST2530)
+#define AST_UART6_BASE                 0x1E790000
+#define AST_UART7_BASE                 0x1E791000
+#define AST_UART8_BASE                 0x1E792000
+#define AST_UART9_BASE                 0x1E793000
+#define AST_UART10_BASE                0x1E794000
+#define AST_UART11_BASE                0x1E795000
+#define AST_UART12_BASE                0x1E796000
+#define AST_UART13_BASE                0x1E797000
+#endif
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define AST_UART_UDMA_BASE             0x1E79E000
+#endif
+#ifdef CONFIG_SOC_AST3200
+#define AST_H264_BASE                  0x1E7C0000
+#endif
+
+
+/*--------------- Virtual address of the IO Registers Region  ------------*/
+#define AST_REGISTER_VA_BASE            IO_ADDRESS(AST_REGISTER_BASE)
+
+#define AST_SPI_FLASH_VA_BASE           IO_ADDRESS(AST_SPI_FLASH_BASE)
+#define AST_SMC_VA_BASE                 IO_ADDRESS(AST_SMC_BASE)
+#define AST_FMC_VA_BASE                 IO_ADDRESS(AST_FMC_BASE)
+#define AST_AHBC_VA_BASE                IO_ADDRESS(AST_AHBC_BASE)
+#define AST_MAC1_VA_BASE                IO_ADDRESS(AST_MAC1_BASE)
+#define AST_MAC2_VA_BASE                IO_ADDRESS(AST_MAC2_BASE)
+#define AST_MAC3_VA_BASE                IO_ADDRESS(AST_MAC3_BASE)
+#define AST_MAC4_VA_BASE                IO_ADDRESS(AST_MAC4_BASE)
+#define AST_USB20_VA_BASE               IO_ADDRESS(AST_USB20_BASE)
+#if defined(CONFIG_SOC_AST3200) || defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define AST_USB20_1_VA_BASE             IO_ADDRESS(AST_USB20_1_BASE)
+#define AST_USB20_2_VA_BASE             IO_ADDRESS(AST_USB20_2_BASE)
+#endif
+#define AST_USB11_VA_BASE               IO_ADDRESS(AST_USB11_BASE)
+#define AST_IC_VA_BASE                  IO_ADDRESS(AST_IC_BASE)
+#define AST_SDRAMC_VA_BASE              IO_ADDRESS(AST_SDRAMC_BASE)
+#define AST_USB1_VA_BASE                IO_ADDRESS(AST_USB1_BASE)
+#define AST_SCU_VA_BASE                 IO_ADDRESS(AST_SCU_BASE)
+#define AST_CRYPTO_VA_BASE              IO_ADDRESS(AST_CRYPTO_BASE)
+#define AST_JTAG_VA_BASE                IO_ADDRESS(AST_JTAG_BASE)
+
+#define AST_GRAPHIC_VA_BASE             IO_ADDRESS(AST_GRAPHIC_BASE)
+#define AST_DMA_VA_BASE                 IO_ADDRESS(AST_DMA_BASE)
+#define AST_MCTP_VA_BASE                IO_ADDRESS(AST_MCTP_BASE)
+#define AST_ADC_VA_BASE                 IO_ADDRESS(AST_ADC_BASE)
+#define AST_PS2_VA_BASE                 IO_ADDRESS(AST_PS2_BASE)
+#define AST_RFX_VA_BASE                 IO_ADDRESS(AST_RFX_BASE)
+#ifdef CONFIG_SOC_AST3200
+#define AST_H264_VA_BASE                IO_ADDRESS(AST_H264_BASE)
+#define AST_PCIE_VA_BASE                IO_ADDRESS(AST_PCIE_BASE)
+#define AST_P2X_VA_BASE                 IO_ADDRESS(AST_P2X_BASE)	
+#define AST_PCIE_WIN_VA_BASE            IO_ADDRESS(AST_PCIE_WIN_BASE)
+#endif
+#define AST_LPC_PLUS_VA_BASE            IO_ADDRESS(AST_LPC_PLUS_BASE)
+#define AST_VIDEO_VA_BASE               IO_ADDRESS(AST_VIDEO_BASE)
+#define AST_AHB_TO_PBUS_VA_BASE         IO_ADDRESS(AST_AHB_TO_PBUS_BASE)
+#define AST_MDMA_VA_BASE                IO_ADDRESS(AST_MDMA_BASE)
+#define AST_2D_VA_BASE                  IO_ADDRESS(AST_2D_BASE)
+#define AST_GPIO_VA_BASE                IO_ADDRESS(AST_GPIO_BASE)
+#define AST_RTC_VA_BASE                 IO_ADDRESS(AST_RTC_BASE)
+#define AST_TIMER_VA_BASE               IO_ADDRESS(AST_TIMER_BASE)
+#define AST_UART1_VA_BASE               IO_ADDRESS(AST_UART1_BASE)
+#define AST_UART2_VA_BASE               IO_ADDRESS(AST_UART2_BASE)
+#define AST_UART3_VA_BASE               IO_ADDRESS(AST_UART3_BASE)
+#define AST_UART4_VA_BASE               IO_ADDRESS(AST_UART4_BASE)
+#define AST_UART5_VA_BASE               IO_ADDRESS(AST_UART5_BASE)
+#define AST_WDT_VA_BASE                 IO_ADDRESS(AST_WDT_BASE)
+#define AST_PWM_VA_BASE                 IO_ADDRESS(AST_PWM_BASE)
+#define AST_LPC_VA_BASE                 IO_ADDRESS(AST_LPC_BASE)
+#define AST_VUART0_VA_BASE              IO_ADDRESS(AST_VUART0_BASE)
+#define AST_VUART1_VA_BASE              IO_ADDRESS(AST_VUART1_BASE)
+#define AST_I2C_VA_BASE                 IO_ADDRESS(AST_I2C_BASE)
+#define AST_PECI_VA_BASE                IO_ADDRESS(AST_PECI_BASE)
+#define AST_PCIARBITER_VA_BASE          IO_ADDRESS(AST_PCIARBITER_BASE)
+#if defined(CONFIG_SOC_AST2530)
+#define AST_UART6_VA_BASE               IO_ADDRESS(AST_UART6_BASE)
+#define AST_UART7_VA_BASE               IO_ADDRESS(AST_UART7_BASE)
+#define AST_UART8_VA_BASE               IO_ADDRESS(AST_UART8_BASE)
+#define AST_UART9_VA_BASE               IO_ADDRESS(AST_UART9_BASE)
+#define AST_UART10_VA_BASE              IO_ADDRESS(AST_UART10_BASE)
+#define AST_UART11_VA_BASE              IO_ADDRESS(AST_UART11_BASE)
+#define AST_UART12_VA_BASE              IO_ADDRESS(AST_UART12_BASE)
+#define AST_UART13_VA_BASE              IO_ADDRESS(AST_UART13_BASE)
+#endif
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define AST_UART_UDMA_VA_BASE           IO_ADDRESS(AST_UART_UDMA_BASE)
+#endif
+
+
+/*****************************************************************
+                     IRQ Assignment
+*****************************************************************/
+#ifdef CONFIG_SOC_AST2300
+#define NR_IRQS                         46
+#define MAXIRQNUM                       45
+#endif
+
+#if defined(CONFIG_SOC_AST2400) || defined(CONFIG_SOC_AST1250) 
+#define NR_IRQS                			51	
+#define MAXIRQNUM           			50
+#endif
+
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define NR_IRQS                			64	
+#define MAXIRQNUM           			63
+#endif
+
+#ifdef CONFIG_SOC_AST3100
+#define NR_IRQS                			51
+#define MAXIRQNUM           			50
+#endif
+
+#ifdef CONFIG_SOC_AST3200
+#define AST_VIC_NUM						64
+#define NR_IRQS                			AST_VIC_NUM + ARCH_NR_PCIE
+#define MAXIRQNUM           			(AST_VIC_NUM + ARCH_NR_PCIE - 1)
+#endif
+
+
+#define BW_CMP_NR_IRQS		         32 //This is the number of IRQS backward-compatible to ast2050/2150
+
+//AST_PCIE
+#ifdef CONFIG_AST_PCIE
+#define ARCH_NR_PCIE				4
+#define AST_NUM_MSI_IRQS			0
+#define IRQ_PCIE_CHAIN				IRQ_PCIE
+#define IRQ_PCIE_CHAIN_START        (AST_VIC_NUM)
+
+#define IRQ_PCIE_INTA				(IRQ_PCIE_CHAIN_START)
+#define IRQ_PCIE_INTB				(IRQ_PCIE_CHAIN_START + 1)
+#define IRQ_PCIE_INTC				(IRQ_PCIE_CHAIN_START + 2)
+#define IRQ_PCIE_INTD				(IRQ_PCIE_CHAIN_START + 3)
+#define IRQ_PCIE_MSI0				(IRQ_PCIE_INTD + 1)		// support max 32 MSI
+#else
+#define ARCH_NR_PCIE				0
+#define AST_NUM_MSI_IRQS			0
+#endif //AST_PCIE
+
+#define IRQ_ECC                0
+#define IRQMASK_ECC            (1 << IRQ_ECC)
+#define IRQ_MIC                1
+#define IRQMASK_MIC            (1 << IRQ_MIC)
+#define IRQ_MAC1               2
+#define IRQMASK_MAC1           (1 << IRQ_MAC1)
+#define IRQ_MAC2               3
+#define IRQMASK_MAC2           (1 << IRQ_MAC2)
+#define IRQ_HAC                4 // CRYPTO
+#define IRQMASK_HAC            (1 << IRQ_HAC)
+#define IRQ_USB20              5 // 2.0 Hub/Host
+#define IRQMASK_USB20          (1 << IRQ_USB20)
+#define IRQ_MDMA               6						/* Non AST3100 */
+#define IRQMASK_MDMA           (1 << IRQ_MDMA)
+
+#define IRQ_RFXV0              7						/* AST3100  ???? */
+#define IRQMASK_RFXV0          (1 << IRQ_RFXV0)
+#define IRQ_VIDEO              7						/* Non AST3100 */
+#define IRQMASK_VIDEO          (1 << IRQ_VIDEO)
+
+#define IRQ_LPC                8						/* Non AST3100 */
+#define IRQ_VUARTINT0          8
+#define IRQMASK_LPC            (1 << IRQ_LPC)
+#define IRQ_UARTINT0           9
+#define IRQMASK_UARTINT0       (1 << IRQ_UARTINT0)
+#define IRQ_UARTINT4           10
+#define IRQMASK_UARTINT4       (1 << IRQ_UARTINT4)
+
+#define IRQ_RFXV1              11						/* AST3100 */
+#define IRQMASK_RFXV1          (1 << IRQ_RFXV1)
+
+#define IRQ_I2C                12
+#define IRQMASK_I2C            (1 << IRQ_I2C)
+
+#if defined(CONFIG_SOC_AST3200) || defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define IRQ_USB20_2            13 			
+#define IRQMASK_USB20_2        (1 << IRQ_USB20_2)
+#else
+#define IRQ_USB1               13 // 1.1 HID			/* Non AST3100 */
+#define IRQMASK_USB1           (1 << IRQ_USB1)
+#endif
+#define IRQ_USB11              14 // 1.1 Host
+#define IRQMASK_USB11          (1 << IRQ_USB11)
+
+#define IRQ_PECI               15                         /* Non AST3100 */
+#define IRQMASK_PECI           (1 << IRQ_PECI)
+#define IRQ_TIMERINT0          16
+#define IRQMASK_TIMERINT0      (1 << IRQ_TIMERINT0)
+#define IRQ_TIMERINT1          17
+#define IRQMASK_TIMERINT1      (1 << IRQ_TIMERINT1)
+#define IRQ_TIMERINT2          18
+#define IRQMASK_TIMERINT2      (1 << IRQ_TIMERINT2)
+#define IRQ_FLASH              19 // SMC
+#define IRQMASK_FLASH          (1 << IRQ_FLASH)
+#define IRQ_GPIO               20
+#define IRQMASK_GPIO           (1 << IRQ_GPIO)
+#define IRQ_SCU                21
+#define IRQMASK_SCU            (1 << IRQ_SCU)
+#define IRQ_RTC_ALARM          22
+#define IRQMASK_RTC_ALARM      (1 << IRQ_RTC_ALARM)
+#define IRQ_MAC3               23						/*  ????   */
+#define IRQMASK_MAC3           (1 << IRQ_MAC3)
+#define IRQ_MAC4               24						/*  ????   */
+#define IRQMASK_MAC4           (1 << IRQ_MAC4)
+#define IRQ_CRT                25
+#define IRQMASK_CRT            (1 << IRQ_CRT)
+#define IRQ_SD                 26
+#define IRQMASK_SD             (1 << IRQ_SD)
+#define IRQ_WDT                27
+#define IRQMASK_WDT            (1 << IRQ_WDT)
+#define IRQ_TACHO              28 // PWM
+#define IRQMASK_TACHO          (1 << IRQ_TACHO)
+#define IRQ_2D                 29
+#define IRQMASK_2D             (1 << IRQ_2D)
+#define IRQ_PCI                30 // Sys Wakeup
+#define IRQMASK_PCI            (1 << IRQ_PCI)
+#define IRQ_ADC       		   31
+#define IRQMASK_ADC      	   (1 << IRQ_ADC)
+#define IRQ_UARTINT1           32
+#define IRQMASK_UARTINT1       (1 << (IRQ_UARTINT1 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT2           33
+#define IRQMASK_UARTINT2       (1 << (IRQ_UARTINT2 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT3           34
+#define IRQMASK_UARTINT3       (1 << (IRQ_UARTINT3 - BW_CMP_NR_IRQS))
+#define IRQ_TIMERINT3          35
+#define IRQMASK_TIMERINT3      (1 << (IRQ_TIMERINT3 - BW_CMP_NR_IRQS))
+#define IRQ_TIMERINT4          36
+#define IRQMASK_TIMERINT4      (1 << (IRQ_TIMERINT4 - BW_CMP_NR_IRQS))
+#define IRQ_TIMERINT5          37
+#define IRQMASK_TIMERINT5      (1 << (IRQ_TIMERINT5 - BW_CMP_NR_IRQS))
+#define IRQ_TIMERINT6          38
+#define IRQMASK_TIMERINT6      (1 << (IRQ_TIMERINT6 - BW_CMP_NR_IRQS))
+#define IRQ_TIMERINT7          39
+#define IRQMASK_TIMERINT7      (1 << (IRQ_TIMERINT7 - BW_CMP_NR_IRQS))
+#define IRQ_SGPIO_MASTER       40
+#define IRQMASK_SGPIO_MASTER   (1 << (IRQ_SGPIO_MASTER - BW_CMP_NR_IRQS))
+
+#ifndef CONFIG_SOC_AST3200
+#define IRQ_SGPIO_SLAVE        41
+#define IRQMASK_SGPIO_SLAVE    (1 << (IRQ_SGPIO_SLAVE - BW_CMP_NR_IRQS))
+#else
+#define IRQ_P2X_BRIDGE         41
+#define IRQMASK_P2X_BRIDGE     (1 << (IRQ_P2X_BRIDGE - BW_CMP_NR_IRQS))
+#endif
+
+#define IRQ_MCTP               42                        /* Non AST3100 */
+#define IRQMASK_MCTP           (1 << (IRQ_MCTP - BW_CMP_NR_IRQS))
+#define IRQ_JTAG_MASTER        43
+#define IRQMASK_JTAG_MASTER    (1 << (IRQ_JTAG_MASTER - BW_CMP_NR_IRQS))
+
+#define IRQ_PS2				   44						  /* AST3100 */
+#define IRQMASK_PS2			   (1 << (IRQ_PS2 - BW_CMP_NR_IRQS))
+
+#define IRQ_COPROCESSOR		   45
+#define IRQMASK_COPROCESSOR	   (1 << (IRQ_COPROCESSOR - BW_CMP_NR_IRQS))
+#define IRQ_SOFT_INTR          45
+#define IRQMASK_SOFT_INTR      (1 << (IRQ_SOFT_INTR - BW_CMP_NR_IRQS))
+#define IRQ_MAIL_BOX           46
+#define IRQMASK_MAIL_BOX       (1 << (IRQ_MAIL_BOX - BW_CMP_NR_IRQS))
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define IRQ_AHBC               47
+#define IRQMASK_AHBC           (1 << (IRQ_AHBC - BW_CMP_NR_IRQS))
+#define IRQ_GPIO_L1            48
+#define IRQMASK_GPIO_L1        (1 << (IRQ_GPIO_L1 - BW_CMP_NR_IRQS))
+#define IRQ_GPIO_L3            49
+#define IRQMASK_GPIO_L3        (1 << (IRQ_GPIO_L3 - BW_CMP_NR_IRQS))
+#else
+#define IRQ_GPIO_L1            47
+#define IRQMASK_GPIO_L1        (1 << (IRQ_GPIO_L1 - BW_CMP_NR_IRQS))
+#define IRQ_GPIO_L3            48
+#define IRQMASK_GPIO_L3        (1 << (IRQ_GPIO_L3 - BW_CMP_NR_IRQS))
+#define IRQ_GPIO_M1            49
+#define IRQMASK_GPIO_M1        (1 << (IRQ_GPIO_M1 - BW_CMP_NR_IRQS))
+#endif
+#define IRQ_UART_UDMA_INT      50
+#define IRQMASK_UART_UDMA_INT  (1 << (IRQ_UART_UDMA_INT - BW_CMP_NR_IRQS))
+
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define IRQ_UARTINT5           51
+#define IRQMASK_UARTINT5       (1 << (IRQ_UARTINT5 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT6           52
+#define IRQMASK_UARTINT6       (1 << (IRQ_UARTINT6 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT7           53
+#define IRQMASK_UARTINT7       (1 << (IRQ_UARTINT7 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT8           54
+#define IRQMASK_UARTINT8       (1 << (IRQ_UARTINT8 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT9           55
+#define IRQMASK_UARTINT9       (1 << (IRQ_UARTINT9 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT10          56
+#define IRQMASK_UARTINT10      (1 << (IRQ_UARTINT10 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT11          57
+#define IRQMASK_UARTINT11      (1 << (IRQ_UARTINT11 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT12          58
+#define IRQMASK_UARTINT12      (1 << (IRQ_UARTINT12 - BW_CMP_NR_IRQS))
+#define IRQ_SPI_INT            59
+#define IRQMASK_SPI_INT        (1 << (IRQ_SPI_INT   - BW_CMP_NR_IRQS))
+#endif
+
+#ifdef CONFIG_SOC_AST3200
+#define IRQ_H264               61
+#define IRQMASK_H264           (1 << (IRQ_H264 - BW_CMP_NR_IRQS))
+
+#define IRQ_420_422            62
+#define IRQMASK_420_422        (1 << (IRQ_420_422 - BW_CMP_NR_IRQS))
+#define IRQ_P2X                63
+#define IRQMASK_P2X            (1 << (IRQ_P2X - BW_CMP_NR_IRQS))
+#endif
+
+/* Mask of valid system controller interrupts */
+/* VIC means VIC1 */
+#ifdef CONFIG_SOC_AST3100
+#define AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_ECC | \
+                                         IRQMASK_MIC | \
+                                         (IRQMASK_MAC1 | IRQMASK_MAC2) | \
+                                         IRQMASK_HAC | \
+                                         IRQMASK_USB20 | \
+                                         (IRQMASK_UARTINT0 | IRQMASK_UARTINT4) | \
+                                         (IRQMASK_RFXV0 | IRQMASK_RFXV1) | \
+                                         IRQMASK_I2C | \
+                                         IRQMASK_USB11 | \
+                                         (IRQMASK_TIMERINT0 | IRQMASK_TIMERINT1 | IRQMASK_TIMERINT2) | \
+                                         IRQMASK_FLASH | \
+                                         IRQMASK_GPIO | \
+                                         IRQMASK_SCU | \
+                                         IRQ_RTC_ALARM | \
+                                         (IRQMASK_MAC3 | IRQMASK_MAC4) | \
+                                         IRQMASK_CRT | \
+                                         IRQMASK_SD | \
+                                         IRQMASK_WDT | \
+                                         IRQMASK_TACHO | \
+                                         IRQMASK_2D | \
+                                         IRQMASK_PCI | \
+                                         IRQMASK_ADC \
+                   )
+#define EXT_AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_UARTINT1 | IRQMASK_UARTINT2 | IRQMASK_UARTINT3 | IRQMASK_SGPIO_MASTER | \
+                                         IRQMASK_PS2 \
+					)
+#else
+#ifdef CONFIG_SOC_AST3200
+#define AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_ECC | \
+                                         IRQMASK_MIC | \
+                                         (IRQMASK_MAC1 | IRQMASK_MAC2) | \
+                                         IRQMASK_HAC | \
+                                         IRQMASK_USB20 | \
+                                         IRQMASK_USB20_2 | \
+                                         (IRQMASK_UARTINT0 | IRQMASK_UARTINT4) | \
+                                         (IRQMASK_RFXV0 | IRQMASK_RFXV1) | \
+                                         IRQMASK_I2C | \
+                                         IRQMASK_USB11 | \
+                                         (IRQMASK_TIMERINT0 | IRQMASK_TIMERINT1 | IRQMASK_TIMERINT2) | \
+                                         IRQMASK_FLASH | \
+                                         IRQMASK_GPIO | \
+                                         IRQMASK_SCU | \
+                                         IRQ_RTC_ALARM | \
+                                         (IRQMASK_MAC3 | IRQMASK_MAC4) | \
+                                         IRQMASK_CRT | \
+                                         IRQMASK_SD | \
+                                         IRQMASK_WDT | \
+                                         IRQMASK_TACHO | \
+                                         IRQMASK_2D | \
+                                         IRQMASK_PCI | \
+                                         IRQMASK_ADC \
+                   )
+#define EXT_AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_UARTINT1 | IRQMASK_UARTINT2 | IRQMASK_UARTINT3 | IRQMASK_SGPIO_MASTER | \
+                                         IRQMASK_PS2 | IRQMASK_H264 | IRQMASK_420_422 | IRQMASK_P2X | IRQMASK_P2X_BRIDGE \
+					)
+#else /* Non CONFIG_SOC_AST3100 and CONFIG_SOC_AST3200 */
+
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530)
+#define AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_ECC | \
+                                         IRQMASK_MIC | \
+                                         (IRQMASK_MAC1 | IRQMASK_MAC2) | \
+                                         IRQMASK_HAC | \
+                                         IRQMASK_USB20 | \
+                                         IRQMASK_MDMA | \
+                                         IRQMASK_VIDEO | \
+                                         IRQMASK_LPC | \
+                                         (IRQMASK_UARTINT0 | IRQMASK_UARTINT4) | \
+                                         IRQMASK_I2C | \
+                                         IRQMASK_USB20_2 | \
+                                         IRQMASK_USB11 | \
+                                         IRQMASK_PECI | \
+                                         (IRQMASK_TIMERINT0 | IRQMASK_TIMERINT1 | IRQMASK_TIMERINT2) | \
+                                         IRQMASK_FLASH | \
+                                         IRQMASK_GPIO | \
+                                         IRQMASK_SCU | \
+                                         IRQ_RTC_ALARM | \
+                                         (IRQMASK_MAC3 | IRQMASK_MAC4) | \
+                                         IRQMASK_CRT | \
+                                         IRQMASK_SD | \
+                                         IRQMASK_WDT | \
+                                         IRQMASK_TACHO | \
+                                         IRQMASK_2D | \
+                                         IRQMASK_PCI  \
+                   )
+
+#else /* Non CONFIG_SOC_AST3100, CONFIG_SOC_AST3200 and CONFIG_SOC_AST2500 */
+#define AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_ECC | \
+                                         IRQMASK_MIC | \
+                                         (IRQMASK_MAC1 | IRQMASK_MAC2) | \
+                                         IRQMASK_HAC | \
+                                         IRQMASK_USB20 | \
+                                         IRQMASK_MDMA | \
+                                         IRQMASK_VIDEO | \
+                                         IRQMASK_LPC | \
+                                         (IRQMASK_UARTINT0 | IRQMASK_UARTINT4) | \
+                                         IRQMASK_I2C | \
+                                         IRQMASK_USB1 | \
+                                         IRQMASK_PECI | \
+                                         (IRQMASK_TIMERINT0 | IRQMASK_TIMERINT1 | IRQMASK_TIMERINT2) | \
+                                         IRQMASK_FLASH | \
+                                         IRQMASK_GPIO | \
+                                         IRQMASK_SCU | \
+                                         IRQ_RTC_ALARM | \
+                                         (IRQMASK_MAC3 | IRQMASK_MAC4) | \
+                                         IRQMASK_CRT | \
+                                         IRQMASK_SD | \
+                                         IRQMASK_WDT | \
+                                         IRQMASK_TACHO | \
+                                         IRQMASK_2D | \
+                                         IRQMASK_PCI  \
+                   )
+#endif //CONFIG_SOC_AST2500
+#if !defined(CONFIG_SOC_AST2500) && !defined(CONFIG_SOC_AST2530)
+#define EXT_AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_UARTINT1 | IRQMASK_UARTINT2 | IRQMASK_UARTINT3 | IRQMASK_SGPIO_MASTER | IRQMASK_JTAG_MASTER \
+					)
+#else
+#define EXT_AST_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_UARTINT1 | IRQMASK_UARTINT2 | IRQMASK_UARTINT3 | IRQMASK_SGPIO_MASTER | IRQMASK_JTAG_MASTER | \
+                                         IRQMASK_UARTINT5 | IRQMASK_UARTINT6 | IRQMASK_UARTINT7 | IRQMASK_UARTINT8 | IRQMASK_UARTINT9 | \
+                                         IRQMASK_UARTINT10 | IRQMASK_UARTINT11 | IRQMASK_UARTINT12 | IRQMASK_UART_UDMA_INT | IRQMASK_MAIL_BOX | IRQMASK_MCTP \
+					)
+#endif					
+
+#endif //CONFIG_SOC_AST3200					
+#endif					
+#endif //_HWREG_ASTCOMMON_H_
