--- u-boot_orig/include/ast/hwmap.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot_mod/include/ast/hwmap.h	2015-12-23 19:41:49.680577837 -0500
@@ -0,0 +1,120 @@
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
+#ifndef     _HWREG_AST_H_
+#define     _HWMAP_AST_H_
+
+
+/********************************************************************
+   Internal Register Mapping Actual = 0x16000000 to 0x1E78CFFF
+   After Align the base and size    = 0x16000000 to 0x1EFFFFFF
+*********************************************************************/
+
+#define AST_REGISTER_BASE				0x16000000
+#define AST_REGISTER_SIZE				0x09000000
+
+/*--------------------------- Individual Devices ------------------------- */
+#define AST_SMC_BASE                    0x16000000
+
+#define AST_AHBC_BASE                   0x1E600000
+#define AST_FMC_BASE                    0x1E620000   
+#define AST_SPI_BASE                    0x1E630000   
+#define AST_MIC_BASE                    0x1E640000
+#define AST_MAC1_BASE                   0x1E660000
+#define AST_MAC2_BASE                   0x1E680000
+#define AST_USB20_HUB_BASE              0x1E6A0000
+#define AST_USB20_HOST_BASE             0x1E6A1000
+#define AST_USB11_HOST_BASE             0x1E6B0000
+#define AST_IC_BASE                     0x1E6C0000
+#define AST_SDRAMC_BASE                 0x1E6E0000
+#define AST_USB11_BASE                  0x1E6E1000
+#define AST_SCU_BASE                    0x1E6E2000
+#define AST_ADC_BASE                    0x1E6E9000
+#define AST_LPC_PLUS_BASE               0x1E6EC000
+#define AST_CRT_BASE                    0x1E6E6000
+#define AST_RFX_BASE                    0x1E6EB000
+#define AST_PCIE_BASE                   0x1E6ED000
+#define AST_P2X_BASE                    0x1E6F0000
+#define AST_GPIO_BASE                   0x1E780000
+#define AST_RTC_BASE                    0x1E781000
+#define AST_TIMER_BASE                  0x1E782000
+#define AST_UART1_BASE                  0x1E783000
+#define AST_UART2_BASE                  0x1E784000
+#define AST_WDT_BASE                    0x1E785000
+#define AST_PWM_FAN_BASE                0x1E786000
+#define AST_VUART_BASE                  0x1E787000
+#define AST_PUART_BASE                  0x1E788000
+#define AST_LPC_BASE                    0x1E789000
+#define AST_I2C_BASE                    0x1E78A000
+
+#define AST_PCIE_WIN_BASE               0x70000000
+#define AST_PCIE_WIN_SIZE               0x10000000
+
+/*--------------- Virtual address of the IO Registers Region  ------------*/
+#define AST_REGISTER_VA_BASE            IO_ADDRESS(AST_REGISTER_BASE)
+
+#define AST_SMC_VA_BASE                 IO_ADDRESS(AST_SMC_BASE)
+#define AST_AHBC_VA_BASE                IO_ADDRESS(AST_AHBC_BASE)
+#define AST_FMC_VA_BASE                 IO_ADDRESS(AST_FMC_BASE)  
+#define AST_SPI_VA_BASE                 IO_ADDRESS(AST_SPI_BASE)   
+#define AST_MIC_VA_BASE                 IO_ADDRESS(AST_MIC_BASE)
+#define AST_MAC1_VA_BASE                IO_ADDRESS(AST_MAC1_BASE)
+#define AST_MAC2_VA_BASE                IO_ADDRESS(AST_MAC2_BASE)
+#define AST_USB20_HUB_VA_BASE           IO_ADDRESS(AST_USB20_HUB_BASE)
+#define AST_USB20_HOST_VA_BASE          IO_ADDRESS(AST_USB20_HOST_BASE)
+#define AST_USB11_HOST_VA_BASE          IO_ADDRESS(AST_USB11_HOST_BASE)
+#define AST_IC_VA_BASE                  IO_ADDRESS(AST_IC_BASE)
+#define AST_SDRAMC_VA_BASE              IO_ADDRESS(AST_SDRAMC_BASE)
+#define AST_USB11_VA_BASE               IO_ADDRESS(AST_USB11_BASE)
+#define AST_SCU_VA_BASE                 IO_ADDRESS(AST_SCU_BASE)
+#define AST_ADC_VA_BASE                 IO_ADDRESS(AST_ADC_BASE)
+#define AST_LPC_PLUS_VA_BASE            IO_ADDRESS(AST_LPC_PLUS_BASE)
+#define AST_CRT_VA_BASE                 IO_ADDRESS(AST_CRT_BASE)
+#define AST_RFX_VA_BASE                 IO_ADDRESS(AST_RFX_BASE)
+#define AST_PCIE_VA_BASE                IO_ADDRESS(AST_PCIE_BASE)
+#define AST_P2X_VA_BASE                 IO_ADDRESS(AST_P2X_BASE)
+#define AST_GPIO_VA_BASE                IO_ADDRESS(AST_GPIO_BASE)
+#define AST_RTC_VA_BASE                 IO_ADDRESS(AST_RTC_BASE)
+#define AST_TIMER_VA_BASE               IO_ADDRESS(AST_TIMER_BASE)
+#define AST_UART1_VA_BASE               IO_ADDRESS(AST_UART1_BASE)
+#define AST_UART2_VA_BASE               IO_ADDRESS(AST_UART2_BASE)
+#define AST_WDT_VA_BASE                 IO_ADDRESS(AST_WDT_BASE)
+#define AST_PWM_FAN_VA_BASE             IO_ADDRESS(AST_PWM_FAN_BASE)
+#define AST_VUART_VA_BASE               IO_ADDRESS(AST_VUART_BASE)
+#define AST_PUART_VA_BASE               IO_ADDRESS(AST_PUART_BASE)
+#define AST_LPC_VA_BASE                 IO_ADDRESS(AST_LPC_BASE)
+#define AST_I2C_VA_BASE                 IO_ADDRESS(AST_I2C_BASE)
+
+
+/*****************************************************************
+						    Flash
+*****************************************************************/
+#define CPE_FLASH_BASE                  0x10000000
+#define CPE_FLASH_SZ                    0x00800000		/* Max 16 MB */
+#define CPE_FLASH_VA_BASE               MEM_ADDRESS(CPE_FLASH_BASE)
+
+
+/*****************************************************************
+					  IRQ Assignment
+*****************************************************************/
+#define NR_IRQS							32
+#define MAXIRQNUM                       31
+
+#endif
