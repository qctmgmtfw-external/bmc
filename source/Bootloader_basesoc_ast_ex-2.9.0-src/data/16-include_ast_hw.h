--- u-boot-2013.07/include/ast_hw.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/include/ast_hw.h	2013-12-05 12:17:55.247503948 -0500
@@ -0,0 +1,13 @@
+#ifndef _AST_HW_H_
+#define _AST_HW_H_
+
+#include <config.h>
+
+/* U-Boot does not use MMU. So no mapping */
+#define IO_ADDRESS(x)	(x)
+#define MEM_ADDRESS(x)  (x)
+
+#include <ast/hwmap.h>
+#include <ast/hwreg.h>
+#include <ast/hwdef.h>
+#endif
