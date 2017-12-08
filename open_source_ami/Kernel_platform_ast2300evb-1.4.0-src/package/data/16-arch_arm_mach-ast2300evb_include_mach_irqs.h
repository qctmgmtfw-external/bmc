--- linux-2.6.28.10-pristine/arch/arm/mach-ast2300evb/include/mach/irqs.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/mach-ast2300evb/include/mach/irqs.h	2011-05-25 14:59:15.967665756 +0800
@@ -0,0 +1,5 @@
+#include <asm/soc-ast/irqs.h>
+#include <mach/platform.h>
+
+/* This may be changed depending upon programming of each device */
+#define IRQ_EDGE_INTR_LIST			0
