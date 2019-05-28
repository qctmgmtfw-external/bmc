--- uboot_old/arch/arm/cpu/astcommon/ast_wdt.c	2014-12-09 08:39:47.585971894 -0500
+++ uboot/arch/arm/cpu/astcommon/ast_wdt.c	2014-12-09 08:37:09.617010840 -0500
@@ -0,0 +1,108 @@
+
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+
+ *******************************************************************/
+#if defined(CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING) || defined(CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY)
+#include "soc_hw.h"
+
+#define WDT_ENABLE              0x01
+#define WDT_TIMEOUT             0x11E1A300              // 5 Minutes (300 secs * 1MHz)
+#define WDT_TRIGGER             0x00004755              // Trigger Passcode
+#define WDT_CLOCK               0x10                    // 1MHz Clock for WDT
+#define WDT_RESET_SYSTEM        0x02                    // Enable System-Reset-On-Timeout
+#define WDT_CLR_EVT_COUNTER     0x76                    // Clear the Timeout Counter & Boot Source code
+#define WDT2_SECOND_BOOT_CODE   0x80                    // Enable Second Boot Code upon reset
+
+int Enable_watchdog(unsigned long *wdt_timeout_status)
+{
+#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
+       /* Setting the Watchdog Timer to prevent dead locks during booting */
+       /* Disable the Watchdog Timer */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+
+       /* Set the Timeout value to max possible 5 Minutes */
+       *((volatile unsigned long *)(WDT2_RELOAD_REG)) = WDT_TIMEOUT;
+
+       /* Set the Trigger value to restart Timer freshly */
+       *((volatile unsigned long *)(WDT2_CNT_RESTART_REG)) = WDT_TRIGGER;
+
+       /* Set the Watchdog clock to 1MHz clock */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) |= (WDT_CLOCK);
+
+       /* Reading the Watchdog Timeout event counter*/
+       *wdt_timeout_status = *((volatile unsigned long *)(WDT2_TIMEOUT_STAT_REG));
+
+       /* Clearing the Watchdog Timeout event counter*/
+       *((volatile unsigned long *)(WDT2_CLR_TIMEOUT_STAT_REG)) = WDT_CLR_EVT_COUNTER;
+
+#ifdef CONFIG_SPX_FEATURE_SELECT_CS1
+       /* Enabling the Second Boot code upon reset */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) |= (WDT2_SECOND_BOOT_CODE);
+#else
+       /* Enabling the First Boot code upon reset */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT2_SECOND_BOOT_CODE);
+#endif
+
+       /* Enable back the Watchdog timer to start the Watchdog */
+       /* Also set the Flag to reset the CPU on Timeout */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) |= (WDT_RESET_SYSTEM | WDT_ENABLE);
+#else
+       /* Setting the Watchdog Timer to prevent dead locks during booting */
+       /* Disable the Watchdog Timer */
+       *((volatile unsigned long *)(WDT_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+
+       /* Set the Timeout value to max possible 5 Minutes */
+       *((volatile unsigned long *)(WDT_RELOAD_REG)) = WDT_TIMEOUT;
+
+       /* Set the Trigger value to restart Timer freshly */
+       *((volatile unsigned long *)(WDT_CNT_RESTART_REG)) = WDT_TRIGGER;
+
+       /* Set the Watchdog clock to 1MHz clock */
+       *((volatile unsigned long *)(WDT_CONTROL_REG)) |= (WDT_CLOCK);
+
+       /* Reading the Watchdog Timeout event counter*/
+       *wdt_timeout_status = *((volatile unsigned long *)(WDT_TIMEOUT_STAT_REG)) >> 8;
+
+       /* Clearing the Watchdog Timeout event counter*/
+       *((volatile unsigned long *)(WDT_CLR_TIMEOUT_STAT_REG)) = WDT_CLR_EVT_COUNTER;
+
+       /* Enable back the Watchdog timer to start the Watchdog */
+       /* Also set the Flag to reset the CPU on Timeout */
+       *((volatile unsigned long *)(WDT_CONTROL_REG)) |= (WDT_RESET_SYSTEM | WDT_ENABLE);
+#endif
+
+       return 0;
+
+}
+
+/**
+* @fn Disable_watchdog
+* @brief Disables watchdog reset.
+* @param[in] void.
+* @retval  0 - on success.
+*/
+int Disable_watchdog(void)
+{
+
+#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+#else
+       *((volatile unsigned long *)(WDT_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+#endif
+
+return 0;
+}
+
+#endif
