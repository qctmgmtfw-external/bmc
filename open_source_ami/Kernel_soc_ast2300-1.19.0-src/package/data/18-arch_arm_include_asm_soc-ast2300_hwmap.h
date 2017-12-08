--- linux-2.6.28.10-pristine/arch/arm/include/asm/soc-ast2300/hwmap.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/include/asm/soc-ast2300/hwmap.h	2011-05-25 14:59:15.830790892 +0800
@@ -0,0 +1,226 @@
+/*
+ *  This file contains the AST2300 SOC Register Base, IRQ and DMA  mappings
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
+#ifndef     _HWREG_AST2300_H_
+#define     _HWMAP_AST2300_H_
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
+#define AST_SPI_FLASH_BASE             0x14000000
+#define AST_SMC_BASE                   0x16000000
+#define AST_AHBC_BASE                   0x1E600000
+#define AST_MAC1_BASE                  0x1E660000
+#define AST_MAC2_BASE                  0x1E680000
+#define AST_USB20_BASE                  0x1E6A0000
+#define AST_IC_BASE                        0x1E6C0000
+#define AST_SDRAMC_BASE                 0x1E6E0000
+#define AST_USB1_BASE                   0x1E6E1000
+#define AST_SCU_BASE                   0x1E6E2000
+#define AST_CRYPTO_BASE                 0x1E6E3000
+#define AST_ADPCM_BASE                  0x1E6E4000
+#define AST_I2S_BASE                   0x1E6E5000
+#define AST_GRAPHIC_BASE                0x1E6E6000
+#define AST_VIDEO_BASE                 0x1E700000
+#define AST_AHB_TO_PBUS_BASE            0x1E720000
+#define AST_MDMA_BASE                   0x1E740000
+#define AST_2D_BASE                     0x1E760000
+#define AST_GPIO_BASE                  0x1E780000
+#define AST_RTC_BASE                       0x1E781000
+#define AST_TIMER_BASE                 0x1E782000
+#define AST_UART1_BASE                  0x1E783000
+#define AST_UART2_BASE                  0x1E78D000
+#define AST_UART3_BASE                  0x1E78E000
+#define AST_UART4_BASE                  0x1E78F000
+#define AST_UART5_BASE                  0x1E784000
+#define AST_WDT_BASE                   0x1E785000
+#define AST_PWM_BASE                       0x1E786000
+#define AST_VUART0_BASE                 0x1E787000
+#define AST_VUART1_BASE                 0x1E788000
+#define AST_LPC_BASE                   0x1E789000
+#define AST_I2C_BASE                   0x1E78A000
+#define AST_PECI_BASE                  0x1E78B000
+#define AST_PCIARBITER_BASE             0x1E78C000
+#define AST_PCI_MASTER1_BASE            0x60000000
+#define AST_PCI_MASTER2_BASE            0x80000000
+
+/*--------------- Virtual address of the IO Registers Region  ------------*/
+#define AST_REGISTER_VA_BASE           IO_ADDRESS(AST_REGISTER_BASE)
+
+#define AST_SPI_FLASH_VA_BASE           IO_ADDRESS(AST_SPI_FLASH_BASE)
+#define AST_SMC_VA_BASE                 IO_ADDRESS(AST_SMC_BASE)
+#define AST_AHBC_VA_BASE                IO_ADDRESS(AST_AHBC_BASE)
+#define AST_MAC1_VA_BASE                IO_ADDRESS(AST_MAC1_BASE)
+#define AST_MAC2_VA_BASE                IO_ADDRESS(AST_MAC2_BASE)
+#define AST_USB20_VA_BASE               IO_ADDRESS(AST_USB20_BASE)
+#define AST_IC_VA_BASE                 IO_ADDRESS(AST_IC_BASE)
+#define AST_SDRAMC_VA_BASE              IO_ADDRESS(AST_SDRAMC_BASE)
+#define AST_USB1_VA_BASE                IO_ADDRESS(AST_USB1_BASE)
+#define AST_SCU_VA_BASE                    IO_ADDRESS(AST_SCU_BASE)
+#define AST_CRYPTO_VA_BASE              IO_ADDRESS(AST_CRYPTO_BASE)
+#define AST_ADPCM_VA_BASE               IO_ADDRESS(AST_ADPCM_BASE)
+#define AST_I2S_VA_BASE                    IO_ADDRESS(AST_I2S_BASE)
+#define AST_GRAPHIC_VA_BASE             IO_ADDRESS(AST_GRAPHIC_BASE)
+#define AST_VIDEO_VA_BASE               IO_ADDRESS(AST_VIDEO_BASE)
+#define AST_AHB_TO_PBUS_VA_BASE         IO_ADDRESS(AST_AHB_TO_PBUS_BASE)
+#define AST_MDMA_VA_BASE                IO_ADDRESS(AST_MDMA_BASE)
+#define AST_2D_VA_BASE                  IO_ADDRESS(AST_2D_BASE)
+#define AST_GPIO_VA_BASE               IO_ADDRESS(AST_GPIO_BASE)
+#define AST_RTC_VA_BASE                IO_ADDRESS(AST_RTC_BASE)
+#define AST_TIMER_VA_BASE              IO_ADDRESS(AST_TIMER_BASE)
+#define AST_UART1_VA_BASE               IO_ADDRESS(AST_UART1_BASE)
+#define AST_UART2_VA_BASE               IO_ADDRESS(AST_UART2_BASE)
+#define AST_UART3_VA_BASE               IO_ADDRESS(AST_UART3_BASE)
+#define AST_UART4_VA_BASE               IO_ADDRESS(AST_UART4_BASE)
+#define AST_UART5_VA_BASE               IO_ADDRESS(AST_UART5_BASE)
+#define AST_WDT_VA_BASE                    IO_ADDRESS(AST_WDT_BASE)
+#define AST_PWM_VA_BASE                IO_ADDRESS(AST_PWM_BASE)
+#define AST_LPC_VA_BASE                 IO_ADDRESS(AST_LPC_BASE)
+#define AST_VUART0_VA_BASE              IO_ADDRESS(AST_VUART0_BASE)
+#define AST_VUART1_VA_BASE              IO_ADDRESS(AST_VUART1_BASE)
+#define AST_I2C_VA_BASE                 IO_ADDRESS(AST_I2C_BASE)
+#define AST_PECI_VA_BASE                   IO_ADDRESS(AST_PECI_BASE)
+#define AST_PCIARBITER_VA_BASE          IO_ADDRESS(AST_PCIARBITER_BASE)
+
+/*****************************************************************
+                     IRQ Assignment
+*****************************************************************/
+#define NR_IRQS                44
+#define MAXIRQNUM           43
+#define BW_CMP_NR_IRQS		32	//This is the number of IRQS backward-compatible to ast2050/2150
+
+#define IRQ_ECC                0
+#define IRQMASK_ECC            (1 << IRQ_ECC)
+#define IRQ_MIC                1
+#define IRQMASK_MIC            (1 << IRQ_MIC)
+#define IRQ_MAC1               2
+#define IRQMASK_MAC1           (1 << IRQ_MAC1)
+#define IRQ_MAC2               3
+#define IRQMASK_MAC2           (1 << IRQ_MAC2)
+#define IRQ_HAC            4
+#define IRQMASK_HAC        (1 << IRQ_HAC)
+#define IRQ_USB20           5
+#define IRQMASK_USB20       (1 << IRQ_USB20)
+#define IRQ_MDMA               6
+#define IRQMASK_MDMA           (1 << IRQ_MDMA)
+#define IRQ_VIDEO           7
+#define IRQMASK_VIDEO       (1 << IRQ_VIDEO)
+#define IRQ_LPC                8
+#define IRQ_VUARTINT0       8
+#define IRQMASK_LPC            (1 << IRQ_LPC)
+#define IRQ_UARTINT0        9
+#define IRQMASK_UARTINT0    (1 << IRQ_UARTINT0)
+#define IRQ_UARTINT4        10
+#define IRQMASK_UARTINT4    (1 << IRQ_UARTINT4)
+
+#define IRQ_I2S            11
+#define IRQMASK_I2S        (1 << IRQ_I2S)
+#define IRQ_I2C                12
+#define IRQMASK_I2C            (1 << IRQ_I2C)
+#define IRQ_USB1           13
+#define IRQMASK_USB1       (1 << IRQ_USB1)
+#define IRQ_ADPCM           14
+#define IRQMASK_ADPDM       (1 << IRQ_ADPCM)
+#define IRQ_PECI               15
+#define IRQMASK_PECI           (1 << IRQ_PECI)
+#define IRQ_TIMERINT0       16
+#define IRQMASK_TIMERINT0   (1 << IRQ_TIMERINT0)
+#define IRQ_TIMERINT1       17
+#define IRQMASK_TIMERINT1   (1 << IRQ_TIMERINT1)
+#define IRQ_TIMERINT2       18
+#define IRQMASK_TIMERINT2   (1 << IRQ_TIMERINT2)
+#define IRQ_FLASH          19
+#define IRQMASK_FLASH       (1 << IRQ_FLASH)
+#define IRQ_GPIO               20
+#define IRQMASK_GPIO           (1 << IRQ_GPIO)
+#define IRQ_SCU            21
+#define IRQMASK_SCU        (1 << IRQ_SCU)
+#define IRQ_RTC_SEC            22
+#define IRQMASK_RTC_SEC        (1 << IRQ_RTC_SEC)
+#define IRQ_RTC_DAY            23
+#define IRQMASK_RTC_DAY        (1 << IRQ_RTC_DAY)
+#define IRQ_RTC_HOUR       24
+#define IRQMASK_RTC_HOUR   (1 << IRQ_RTC_HOUR)
+#define IRQ_RTC_MIN            25
+#define IRQMASK_RTC_MIN        (1 << IRQ_RTC_MIN)
+#define IRQ_RTC_ALARM      26
+#define IRQMASK_RTC_ALARM  (1 << IRQ_RTC_ALARM)
+#define IRQ_WDT            27
+#define IRQMASK_WDT            (1 << IRQ_WDT)
+#define IRQ_TACHO           28
+#define IRQMASK_TACHO       (1 << IRQ_TACHO)
+#define IRQ_2D             29
+#define IRQMASK_2D         (1 << IRQ_2D)
+#define IRQ_PCI            30
+#define IRQMASK_PCI        (1 << IRQ_PCI)
+#define IRQ_AHBC           31
+#define IRQMASK_AHBC       (1 << IRQ_AHBC)
+
+#define IRQ_UARTINT1        32
+#define IRQMASK_UARTINT1    (1 << (IRQ_UARTINT1 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT2        33
+#define IRQMASK_UARTINT2    (1 << (IRQ_UARTINT2 - BW_CMP_NR_IRQS))
+#define IRQ_UARTINT3        34
+#define IRQMASK_UARTINT3    (1 << (IRQ_UARTINT3 - BW_CMP_NR_IRQS))
+
+#define IRQ_SGPIO_MASTER	40
+#define IRQMASK_SGPIO_MASTER	(1 << (IRQ_SGPIO_MASTER - BW_CMP_NR_IRQS))
+
+
+/* Mask of valid system controller interrupts */
+/* VIC means VIC1 */
+#define AST2050_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_ECC | \
+                                         IRQMASK_MIC | \
+                                         (IRQMASK_MAC1 | IRQMASK_MAC2) | \
+                                         IRQMASK_HAC | \
+                                         IRQMASK_USB20 | \
+                                         IRQMASK_MDMA | \
+                                         IRQMASK_VIDEO | \
+                                         IRQMASK_LPC | \
+                                         (IRQMASK_UARTINT0 | IRQMASK_UARTINT4) | \
+                                         IRQMASK_I2S | \
+                                         IRQMASK_I2C | \
+                                         IRQMASK_USB1 | \
+                                         IRQMASK_ADPDM | \
+                                         IRQMASK_PECI | \
+                                         (IRQMASK_TIMERINT0 | IRQMASK_TIMERINT1 | IRQMASK_TIMERINT2) | \
+                                         IRQMASK_FLASH | \
+                                         IRQMASK_GPIO | \
+                                         IRQMASK_SCU | \
+                                        (IRQMASK_RTC_SEC | IRQMASK_RTC_DAY | IRQMASK_RTC_HOUR | IRQMASK_RTC_MIN | IRQMASK_RTC_ALARM) | \
+                                         IRQMASK_WDT | \
+                                         IRQMASK_TACHO | \
+                                         IRQMASK_2D | \
+                                         IRQMASK_PCI | \
+                                         IRQMASK_AHBC \
+                   )
+
+#define EXT_AST2050_VIC_VALID_INTMASK    ( \
+                                         IRQMASK_UARTINT1 | IRQMASK_UARTINT2 | IRQMASK_UARTINT3 | IRQMASK_SGPIO_MASTER \
+					)
+#endif
