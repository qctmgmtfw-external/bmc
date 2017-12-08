--- linux-2.6.28.10-pristine/arch/arm/include/asm/soc-ast2300/hwreg.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/include/asm/soc-ast2300/hwreg.h	2011-05-25 14:59:15.832790821 +0800
@@ -0,0 +1,208 @@
+/*
+ *  This file contains the AST2300 SOC Register locations
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
+#define     _HWREG_AST2300_H_
+
+
+/* -----------------------------------------------------------------
+ *      	         SMC Registers
+ * -----------------------------------------------------------------
+*/
+#define SMC_CE0_SEGMENT_AC_TIMING_REG   (AST_SMC_VA_BASE + 0x00)
+#define SMC_CE0_CTRL_REG                (AST_SMC_VA_BASE + 0x04)
+#define SMC_CE1_CTRL_REG                (AST_SMC_VA_BASE + 0x08)
+#define SMC_CE2_CTRL_REG                (AST_SMC_VA_BASE + 0x0C)
+
+/* -----------------------------------------------------------------
+ *      	         AHB Registers
+ * -----------------------------------------------------------------
+*/
+#define AHB_PROTECTION_KEY_REG          (AST_AHBC_VA_BASE + 0x00)
+#define AHB_PRIORITY_CTRL_REG           (AST_AHBC_VA_BASE + 0x80)
+#define AHB_INTERRUPT_CTRL_REG          (AST_AHBC_VA_BASE + 0x88)
+#define AHB_ADDR_REMAP_REG              (AST_AHBC_VA_BASE + 0x8C)
+
+/* -----------------------------------------------------------------
+ *      	        SRAM Registers
+ * -----------------------------------------------------------------
+ */
+
+ /* -------------------------------------------------------------
+ *                  SDRAM Registers
+ * -------------------------------------------------------------
+ */
+#define SDRAM_PROTECTION_KEY_REG            (AST_SDRAMC_VA_BASE + 0x00)
+#define SDRAM_CONFIG_REG                    (AST_SDRAMC_VA_BASE + 0x04)
+#define SDRAM_GRAP_MEM_PROTECTION_REG       (AST_SDRAMC_VA_BASE + 0x08)
+#define SDRAM_REFRESH_TIMING_REG            (AST_SDRAMC_VA_BASE + 0x0C)
+#define SDRAM_NSPEED_REG1                   (AST_SDRAMC_VA_BASE + 0x10)
+#define SDRAM_LSPEED_REG1                   (AST_SDRAMC_VA_BASE + 0x14)
+#define SDRAM_NSPEED_REG2                   (AST_SDRAMC_VA_BASE + 0x18)
+#define SDRAM_LSPEED_REG2                   (AST_SDRAMC_VA_BASE + 0x1C)
+#define SDRAM_NSPEED_DELAY_CTRL_REG         (AST_SDRAMC_VA_BASE + 0x20)
+#define SDRAM_LSPEED_DELAY_CTRL_REG         (AST_SDRAMC_VA_BASE + 0x24)
+#define SDRAM_MODE_SET_CTRL_REG             (AST_SDRAMC_VA_BASE + 0x28)
+#define SDRAM_MRS_EMRS2_MODE_SET_REG        (AST_SDRAMC_VA_BASE + 0x2C)
+#define SDRAM_MRS_EMRS3_MODE_SET_REG        (AST_SDRAMC_VA_BASE + 0x30)
+#define SDRAM_PWR_CTRL_REG                  (AST_SDRAMC_VA_BASE + 0x34)
+#define SDRAM_PAGE_MISS_LATENCY_MASK_REG    (AST_SDRAMC_VA_BASE + 0x38)
+#define SDRAM_PRIORITY_GROUP_SET_REG        (AST_SDRAMC_VA_BASE + 0x3C)
+#define SDRAM_MAX_GRANT_LENGTH_REG1         (AST_SDRAMC_VA_BASE + 0x40)
+#define SDRAM_MAX_GRANT_LENGTH_REG2         (AST_SDRAMC_VA_BASE + 0x44)
+#define SDRAM_MAX_GRANT_LENGTH_REG3         (AST_SDRAMC_VA_BASE + 0x48)
+#define SDRAM_ECC_CTRL_STATUS_REG           (AST_SDRAMC_VA_BASE + 0x50)
+#define SDRAM_ECC_SEGMENT_EN_REG            (AST_SDRAMC_VA_BASE + 0x54)
+#define SDRAM_ECC_SCRUB_REQ_MASK_CTRL_REG   (AST_SDRAMC_VA_BASE + 0x58)
+#define SDRAM_ECC_ADDR_FIRST_ERR_REG        (AST_SDRAMC_VA_BASE + 0x5C)
+#define SDRAM_IO_BUFF_MODE_REG              (AST_SDRAMC_VA_BASE + 0x60)
+#define SDRAM_DLL_CTRL_REG1                 (AST_SDRAMC_VA_BASE + 0x64)
+#define SDRAM_DLL_CTRL_REG2                 (AST_SDRAMC_VA_BASE + 0x68)
+#define SDRAM_DLL_CTRL_REG3                 (AST_SDRAMC_VA_BASE + 0x6C)
+#define SDRAM_TEST_CTRL_STATUS_REG          (AST_SDRAMC_VA_BASE + 0x70)
+#define SDRAM_TEST_START_ADDR_LENGTH_REG    (AST_SDRAMC_VA_BASE + 0x74)
+#define SDRAM_TEST_FAIL_DQ_BIT_REG          (AST_SDRAMC_VA_BASE + 0x78)
+#define SDRAM_TEST_INIT_VALUE_REG           (AST_SDRAMC_VA_BASE + 0x7C)
+#define AST2100_COMPATIBLE_SCU_PASSWORD     (AST_SDRAMC_VA_BASE + 0x100)
+#define AST2100_COMPATIBLE_SCU_MPLL_PARA    (AST_SDRAMC_VA_BASE + 0x120)
+
+/*-------------------------------------------------------------------
+ *				    SCU Registers
+ *--------------------------------------------------------------------
+ */
+#define SCU_KEY_CONTROL_REG			    (AST_SCU_VA_BASE +  0x00)
+#define SCU_SYS_RESET_REG               (AST_SCU_VA_BASE +  0x04)
+#define SCU_CLK_SELECT_REG              (AST_SCU_VA_BASE +  0x08)
+#define SCU_CLK_STOP_REG                (AST_SCU_VA_BASE +  0x0C)
+#define SCU_OSC_COUNT_CTRL_REG			(AST_SCU_VA_BASE +  0x10)
+#define SCU_OSC_COUNT_VALUE_REG         (AST_SCU_VA_BASE +  0x14)
+#define SCU_INTR_CRTL_VALUE_REG         (AST_SCU_VA_BASE +  0x18)
+#define SCU_32CLK_ERR_CORRECT_REG       (AST_SCU_VA_BASE +  0x1C)
+#define SCU_M_PLL_PARAM_REG             (AST_SCU_VA_BASE +  0x20)
+#define SCU_H_PLL_PARAM_REG             (AST_SCU_VA_BASE +  0x24)
+
+#define SCU_SOC_SCRATCH1_REG            (AST_SCU_VA_BASE +  0x40)
+#define SCU_SOC_SCRATCH2_REG            (AST_SCU_VA_BASE +  0x44)
+#define SCU_HW_STRAPPING_REG            (AST_SCU_VA_BASE +  0x70)
+#define SCU_MULTIFUNCTION_PIN_REGISTER	(AST_SCU_VA_BASE +  0x74)
+#define SCU_HW_REVISION_CODE			(AST_SCU_VA_BASE +  0x7C)
+
+/* -------------------------------------------------------------------
+ *  					Timer Registers
+ * -------------------------------------------------------------------
+ */
+#define TIMER1_COUNT_REG                (AST_TIMER_VA_BASE + 0x00)
+#define TIMER1_RELOAD_REG               (AST_TIMER_VA_BASE + 0x04)
+#define TIMER1_FIRST_MATCH_REG          (AST_TIMER_VA_BASE + 0x08)
+#define TIMER1_SEC_MATCH_REG            (AST_TIMER_VA_BASE + 0x0C)
+
+#define TIMER2_COUNT_REG                (AST_TIMER_VA_BASE + 0x10)
+#define TIMER2_RELOAD_REG               (AST_TIMER_VA_BASE + 0x14)
+#define TIMER2_FIRST_MATCH_REG          (AST_TIMER_VA_BASE + 0x18)
+#define TIMER2_SEC_MATCH_REG            (AST_TIMER_VA_BASE + 0x1C)
+
+#define TIMER3_COUNT_REG                (AST_TIMER_VA_BASE + 0x20)
+#define TIMER3_RELOAD_REG               (AST_TIMER_VA_BASE + 0x24)
+#define TIMER3_FIRST_MATCH_REG          (AST_TIMER_VA_BASE + 0x28)
+#define TIMER3_SEC_MATCH_REG            (AST_TIMER_VA_BASE + 0x2C)
+
+#define TIMER_CONTROL_REG               (AST_TIMER_VA_BASE + 0x30)
+
+/* -----------------------------------------------------------------
+ * 				 Interrupt Controller Register
+ * -----------------------------------------------------------------
+ */
+#define IRQ_STATUS_REG	                (AST_IC_VA_BASE + 0x00)
+#define FIQ_STATUS_REG	                (AST_IC_VA_BASE + 0x04)
+#define RAW_INT_STATUS_REG	            (AST_IC_VA_BASE + 0x08)
+#define IRQ_SELECT_REG	                (AST_IC_VA_BASE + 0x0C)
+#define IRQ_ENABLE_REG	                (AST_IC_VA_BASE + 0x10)
+#define IRQ_DISABLE_REG					(AST_IC_VA_BASE + 0x14)
+#define SOFT_INT_REG					(AST_IC_VA_BASE + 0x18)
+#define SOFT_INT_CLEAR_REG				(AST_IC_VA_BASE + 0x1C)
+#define PROTECT_ENABLE_REG				(AST_IC_VA_BASE + 0x20)
+#define INTERRUPT_SENS_REG				(AST_IC_VA_BASE + 0x24)
+#define INTERRUPT_BOTH_EDGE_TRIGGER_REG (AST_IC_VA_BASE + 0x28)
+#define INTERRUPT_EVENT_REG             (AST_IC_VA_BASE + 0x2C)
+#define EDGE_TRIGGERED_INTCLEAR_REG     (AST_IC_VA_BASE + 0x38)
+
+/* Interrupt Controllers */
+#define ASPEED_VIC_STATUS_OFFSET        0x00
+
+/* Interrupt Controllers 2 */
+#define ASPEED_VIC2_STATUS_OFFSET       0x84
+
+
+/* -----------------------------------------------------------------
+ * 				 Interrupt Controller Register for IRQ 32 - 44
+ * -----------------------------------------------------------------
+ */
+#define EXT_IRQ_SELECT_REG	            (AST_IC_VA_BASE + 0x9C)
+#define EXT_IRQ_ENABLE_REG 				(AST_IC_VA_BASE + 0xA4)
+#define EXT_IRQ_DISABLE_REG				(AST_IC_VA_BASE + 0xAC)
+#define EXT_EDGE_TRIGGERED_INTCLEAR_REG (AST_IC_VA_BASE + 0xDC)
+#define EXT_INTERRUPT_EVENT_REG         (AST_IC_VA_BASE + 0xD4)
+#define EXT_INTERRUPT_SENS_REG			(AST_IC_VA_BASE + 0xC4)
+
+
+/* --------------------------------------------------------------------
+ * 				 Interrupt Controller 2 Register
+ * --------------------------------------------------------------------
+ */
+#define VIC2_STATUS              (AST_GPIO_VA_BASE + 0x38)
+#define VIC2_RAW_STATUS          (AST_GPIO_VA_BASE + 0x38)
+#define VIC2_ENABLE_SET          (AST_GPIO_VA_BASE + 0x28)
+#define VIC2_ENABLE_CLEAR        (AST_GPIO_VA_BASE + 0x28)
+#define VIC2_SENSE               (AST_GPIO_VA_BASE + 0x30)
+#define VIC2_BOTH_EDGE		     (AST_GPIO_VA_BASE + 0x34)
+#define VIC2_EVENT               (AST_GPIO_VA_BASE + 0x2C)
+#define VIC2_EDGE_CLEAR          (AST_GPIO_VA_BASE + 0x38)
+
+/*---------------------------------------------------------------
+ *   GPIO Controllers Register
+ *  ------------------------------------------------------------
+ */
+#define GPIO_DATA_REG                   (AST_GPIO_VA_BASE + 0x00)
+#define GPIO_DIRECTION_REG              (AST_GPIO_VA_BASE + 0x04)
+
+/*---------------------------------------------------------------
+ *   I2C Controllers Register
+ *  ------------------------------------------------------------
+ */
+
+/*------------------------------------------------------------------
+ *   DMA Controllers Registers
+ *  ----------------------------------------------------------------
+ */
+
+/*------------------------------------------------------------------
+ *				 RTC Register Locations
+ *------------------------------------------------------------------*/
+
+
+/*------------------------------------------------------------------
+ *				 WDT Register Locations
+ *------------------------------------------------------------------*/
+#define WDT_CNT_STATUS_REG  (AST_WDT_VA_BASE + 0x00)
+#define WDT_RELOAD_REG		(AST_WDT_VA_BASE + 0x04)
+#define WDT_CNT_RESTART_REG (AST_WDT_VA_BASE + 0x08)
+#define WDT_CONTROL_REG		(AST_WDT_VA_BASE + 0x0C)
+
+#endif
