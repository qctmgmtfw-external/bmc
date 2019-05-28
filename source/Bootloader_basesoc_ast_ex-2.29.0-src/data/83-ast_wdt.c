--- uboot/arch/arm/cpu/astcommon/ast_wdt.c	2017-11-17 16:15:46.955053342 +0800
+++ uboot/arch/arm/cpu/astcommon/ast_wdt_new.c	2017-11-17 16:16:05.571053342 +0800
@@ -27,7 +27,30 @@
 
 int Enable_watchdog(unsigned long *wdt_timeout_status)
 {
-#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
+#if defined(CONFIG_SPX_FEATURE_SELECT_WDT3)
+       /* Setting the Watchdog Timer to prevent dead locks during booting */
+       /* Disable the Watchdog Timer */
+       *((volatile unsigned long *)(WDT3_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+
+       /* Set the Timeout value to max possible 5 Minutes */
+       *((volatile unsigned long *)(WDT3_RELOAD_REG)) = WDT_TIMEOUT;
+
+       /* Set the Trigger value to restart Timer freshly */
+       *((volatile unsigned long *)(WDT3_CNT_RESTART_REG)) = WDT_TRIGGER;
+
+       /* Set the Watchdog clock to 1MHz clock */
+       *((volatile unsigned long *)(WDT3_CONTROL_REG)) |= (WDT_CLOCK);
+
+       /* Reading the Watchdog Timeout event counter*/
+       *wdt_timeout_status = *((volatile unsigned long *)(WDT3_TIMEOUT_STAT_REG)) >> 8;
+
+       /* Clearing the Watchdog Timeout event counter*/
+       *((volatile unsigned long *)(WDT3_CLR_TIMEOUT_STAT_REG)) = WDT_CLR_EVT_COUNTER;
+
+       /* Enable back the Watchdog timer to start the Watchdog */
+       /* Also set the Flag to reset the CPU on Timeout */
+       *((volatile unsigned long *)(WDT3_CONTROL_REG)) |= (WDT_RESET_SYSTEM | WDT_ENABLE);
+#elif defined(CONFIG_SPX_FEATURE_SELECT_WDT2)
        /* Setting the Watchdog Timer to prevent dead locks during booting */
        /* Disable the Watchdog Timer */
        *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
@@ -42,7 +65,7 @@
        *((volatile unsigned long *)(WDT2_CONTROL_REG)) |= (WDT_CLOCK);
 
        /* Reading the Watchdog Timeout event counter*/
-       *wdt_timeout_status = *((volatile unsigned long *)(WDT2_TIMEOUT_STAT_REG));
+       *wdt_timeout_status = *((volatile unsigned long *)(WDT2_TIMEOUT_STAT_REG)) >> 8;
 
        /* Clearing the Watchdog Timeout event counter*/
        *((volatile unsigned long *)(WDT2_CLR_TIMEOUT_STAT_REG)) = WDT_CLR_EVT_COUNTER;
@@ -96,7 +119,9 @@
 int Disable_watchdog(void)
 {
 
-#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
+#if defined(CONFIG_SPX_FEATURE_SELECT_WDT3)
+       *((volatile unsigned long *)(WDT3_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+#elif defined(CONFIG_SPX_FEATURE_SELECT_WDT2)
        *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
 #else
        *((volatile unsigned long *)(WDT_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
