--- uboot/include/ast/hwreg.h	2017-11-17 16:08:25.915053342 +0800
+++ uboot/include/ast/hwreg_new.h	2017-11-17 16:09:07.511053342 +0800
@@ -192,14 +192,21 @@
 #define WDT_CONTROL_REG        (AST_WDT_VA_BASE + 0x0C)
 #define WDT_TIMEOUT_STAT_REG (AST_WDT_VA_BASE + 0x10)
 #define WDT_CLR_TIMEOUT_STAT_REG (AST_WDT_VA_BASE + 0x14)
+
 #define WDT2_CNT_STATUS_REG  (AST_WDT_VA_BASE + 0x20)
 #define WDT2_RELOAD_REG          (AST_WDT_VA_BASE + 0x24)
 #define WDT2_CNT_RESTART_REG (AST_WDT_VA_BASE + 0x28)
 #define WDT2_CONTROL_REG         (AST_WDT_VA_BASE + 0x2C)
-
 #define WDT2_TIMEOUT_STAT_REG      (AST_WDT_VA_BASE + 0x30)
 #define WDT2_CLR_TIMEOUT_STAT_REG  (AST_WDT_VA_BASE + 0x34)
 
+#define WDT3_CNT_STATUS_REG  (AST_WDT_VA_BASE + 0x40)
+#define WDT3_RELOAD_REG          (AST_WDT_VA_BASE + 0x44)
+#define WDT3_CNT_RESTART_REG (AST_WDT_VA_BASE + 0x48)
+#define WDT3_CONTROL_REG         (AST_WDT_VA_BASE + 0x4C)
+#define WDT3_TIMEOUT_STAT_REG      (AST_WDT_VA_BASE + 0x50)
+#define WDT3_CLR_TIMEOUT_STAT_REG  (AST_WDT_VA_BASE + 0x54)
+
 /*------------------------------------------------------------------
  *              LPC PLUS Controllers Register Locations
  *------------------------------------------------------------------*/
