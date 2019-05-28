--- u-boot-2013.07/arch/arm/cpu/astcommon/ast_kcs.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/ast_kcs.h	2013-12-05 12:17:55.079503948 -0500
@@ -0,0 +1,31 @@
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
+ ******************************************************************/
+/* AST KCS Registers offsets used for KCS - YAFU Upgrade */
+
+#ifdef SOC_KCS_H
+#define SOC_KCS_H
+
+
+#define KCS_IDR3			0x24
+#define KCS_ODR3			0x30	
+#define KCS_STATUS_3		0x3C	
+
+#define KCS_IDR2			0x28
+#define KCS_ODR2			0x34
+#define KCS_STATUS_2		0x40
+
+#define KCS_IDR1			0x2C
+#define KCS_ODR1			0x38
+#define KCS_STATUS_1		0x44
+
+#endif
