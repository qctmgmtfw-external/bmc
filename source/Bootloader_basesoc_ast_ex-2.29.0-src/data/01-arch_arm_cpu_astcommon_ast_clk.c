--- u-boot-2013.07/arch/arm/cpu/astcommon/ast_clk.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/ast_clk.c	2013-12-05 12:17:55.079503948 -0500
@@ -0,0 +1,30 @@
+
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2013, American Megatrends Inc.        **
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
+#include <config.h>
+#include <common.h>
+
+
+int
+get_serial_clock(void)
+{
+	unsigned long MiscCtrl;	
+	MiscCtrl = *((volatile unsigned long *) (0x1e6e202c));
+	if (MiscCtrl & 0x1000)
+		return  CONFIG_AST_UART_CLKIN/13;
+	return CONFIG_AST_UART_CLKIN;
+}
+
