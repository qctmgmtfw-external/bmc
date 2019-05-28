--- uboot/include/ast/hwreg.h	2015-05-18 15:46:19.717105380 +0800
+++ uboot.new/include/ast/hwreg.h	2015-05-18 15:47:05.529106601 +0800
@@ -0,0 +1,218 @@
+/*
+ *  This file contains the AST SOC Register locations
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
+#define     _HWREG_AST_H_
+
+
+/* -----------------------------------------------------------------
+ *                  SMC Registers
+ * -----------------------------------------------------------------
+*/
+#define SMC_CE0_SEGMENT_AC_TIMING_REG   (AST_SMC_VA_BASE + 0x00)
+#define SMC_CE0_CTRL_REG                (AST_SMC_VA_BASE + 0x04)
+#define SMC_CE1_CTRL_REG                (AST_SMC_VA_BASE + 0x08)
+#define SMC_CE2_CTRL_REG                (AST_SMC_VA_BASE + 0x0C)
+
+/* -----------------------------------------------------------------
+ *                  AHB Registers
+ * -----------------------------------------------------------------
+*/
+#define AHB_PROTECTION_KEY_REG          (AST_AHBC_VA_BASE + 0x00)
+#define AHB_PRIORITY_CTRL_REG           (AST_AHBC_VA_BASE + 0x80)
+#define AHB_INTERRUPT_CTRL_REG          (AST_AHBC_VA_BASE + 0x88)
+#define AHB_ADDR_REMAP_REG              (AST_AHBC_VA_BASE + 0x8C)
+
+/* -----------------------------------------------------------------
+ *                 SRAM Registers
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
+ *                 SCU Registers
+ *--------------------------------------------------------------------
+ */
+#define SCU_KEY_CONTROL_REG                (AST_SCU_VA_BASE +  0x00)
+#define SCU_SYS_RESET_REG               (AST_SCU_VA_BASE +  0x04)
+#define SCU_CLK_SELECT_REG              (AST_SCU_VA_BASE +  0x08)
+#define SCU_CLK_STOP_REG                (AST_SCU_VA_BASE +  0x0C)
+#define SCU_OSC_COUNT_CTRL_REG         (AST_SCU_VA_BASE +  0x10)
+#define SCU_OSC_COUNT_VALUE_REG         (AST_SCU_VA_BASE +  0x14)
+#define SCU_INTR_CRTL_VALUE_REG         (AST_SCU_VA_BASE +  0x18)
+#define SCU_D2PLL_PARAM_REG             (AST_SCU_VA_BASE +  0x1C)//AST2500A1
+#define SCU_32CLK_ERR_CORRECT_REG       (AST_SCU_VA_BASE +  0x1C)
+#define SCU_M_PLL_PARAM_REG             (AST_SCU_VA_BASE +  0x20)
+#define SCU_H_PLL_PARAM_REG             (AST_SCU_VA_BASE +  0x24)
+#define SCU_MISC_CONTROL_REG            (AST_SCU_VA_BASE +  0x2C)
+#define SCU_MISC_MULTI_PIN3_REG  	(AST_SCU_VA_BASE +  0x88)
+#define SCU_MISC_MULTI_PIN5_REG		    (AST_SCU_VA_BASE +  0x90)
+
+#define SCU_SOC_SCRATCH1_REG            (AST_SCU_VA_BASE +  0x40)
+#define SCU_SOC_SCRATCH2_REG            (AST_SCU_VA_BASE +  0x44)
+#define SCU_MAC_CLOCK_DELAY             (AST_SCU_VA_BASE +  0x48)
+#define SCU_HW_STRAPPING_REG            (AST_SCU_VA_BASE +  0x70)
+#define SCU_PIN_MUX                     (AST_SCU_VA_BASE +  0x74)
+#define SCU_REVISION_ID                 (AST_SCU_VA_BASE +  0x7C)
+#define SCU_F0_REG                      (AST_SCU_VA_BASE +  0xf0)
+#define SCU_MAC_CLK_DELAY_100M          (AST_SCU_VA_BASE +  0xB8)
+#define SCU_MAC_CLK_DELAY_10M           (AST_SCU_VA_BASE +  0xBC)
+#define SCU_SYS_RESET_CTL_REG2          (AST_SCU_VA_BASE +  0xD4)
+#define SCU_D2PLL_EXTENDED_REG          (AST_SCU_VA_BASE +  0x13C)
+
+/* -------------------------------------------------------------------
+ *                     Timer Registers
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
+/* --------------------------------------------------------------------
+ *                         GPIO Registers
+ * --------------------------------------------------------------------
+ */
+#define SIMPLE_GPIO0_DATA_VAL_REG       (AST_GPIO_VA_BASE + 0x70)
+#define EXT_GPIO_CMD_SRC_0_REG		(AST_GPIO_VA_BASE + 0x68)
+#define EXT_GPIO_CMD_SRC_1_REG		(AST_GPIO_VA_BASE + 0x6c)
+#define EXT_GPIO_DATA_REG		(AST_GPIO_VA_BASE + 0x20)
+#define EXT_GPIO_DIR_REG		(AST_GPIO_VA_BASE + 0x24)
+#define GPIO_DATA_REG			(AST_GPIO_VA_BASE + 0x0)
+#define GPIO_DIR_REG			(AST_GPIO_VA_BASE + 0x4)
+
+
+/* -----------------------------------------------------------------
+ *                  Interrupt Controller Register
+ * -----------------------------------------------------------------
+ */
+#define IRQ_STATUS_REG                 (AST_IC_VA_BASE + 0x00)
+#define FIQ_STATUS_REG                 (AST_IC_VA_BASE + 0x04)
+#define RAW_INT_STATUS_REG             (AST_IC_VA_BASE + 0x08)
+#define IRQ_SELECT_REG                 (AST_IC_VA_BASE + 0x0C)
+#define IRQ_ENABLE_REG                 (AST_IC_VA_BASE + 0x10)
+#define IRQ_CLEAR_REG                  (AST_IC_VA_BASE + 0x14)
+#define SOFT_INT_REG                   (AST_IC_VA_BASE + 0x18)
+#define SOFT_INT_CLEAR_REG             (AST_IC_VA_BASE + 0x1C)
+#define PROTECT_ENABLE_REG             (AST_IC_VA_BASE + 0x20)
+
+/*-------------------------------------------------------------
+ *               SSP Controllers Registers
+ *  -----------------------------------------------------------
+ */
+
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
+ *              RTC Register Locations
+ *------------------------------------------------------------------*/
+
+
+/*------------------------------------------------------------------
+ *              WDT Register Locations
+ *------------------------------------------------------------------*/
+#define WDT_CNT_STATUS_REG  (AST_WDT_VA_BASE + 0x00)
+#define WDT_RELOAD_REG     (AST_WDT_VA_BASE + 0x04)
+#define WDT_CNT_RESTART_REG (AST_WDT_VA_BASE + 0x08)
+#define WDT_CONTROL_REG        (AST_WDT_VA_BASE + 0x0C)
+#define WDT_TIMEOUT_STAT_REG (AST_WDT_VA_BASE + 0x10)
+#define WDT_CLR_TIMEOUT_STAT_REG (AST_WDT_VA_BASE + 0x14)
+#define WDT2_CNT_STATUS_REG  (AST_WDT_VA_BASE + 0x20)
+#define WDT2_RELOAD_REG          (AST_WDT_VA_BASE + 0x24)
+#define WDT2_CNT_RESTART_REG (AST_WDT_VA_BASE + 0x28)
+#define WDT2_CONTROL_REG         (AST_WDT_VA_BASE + 0x2C)
+
+#define WDT2_TIMEOUT_STAT_REG      (AST_WDT_VA_BASE + 0x30)
+#define WDT2_CLR_TIMEOUT_STAT_REG  (AST_WDT_VA_BASE + 0x34)
+
+/*------------------------------------------------------------------
+ *              LPC PLUS Controllers Register Locations
+ *------------------------------------------------------------------*/
+#define LPC_PLUS_CALIBRATION_DATA   (AST_LPC_PLUS_VA_BASE + 0x04)
+
+
+/*------------------------------------------------------------------
+ *              LPC Controllers Register Locations
+ *------------------------------------------------------------------*/
+#define LPC_HOST_CNT_REG0	        (AST_LPC_VA_BASE + 0xa0)
+#define LPC_SNP_WADR			(AST_LPC_VA_BASE + 0x90)
+#define LPC_SNP_WDR			(AST_LPC_VA_BASE + 0x94)
+#define LPC_HICR5			(AST_LPC_VA_BASE + 0x80)
+
+
+#endif
