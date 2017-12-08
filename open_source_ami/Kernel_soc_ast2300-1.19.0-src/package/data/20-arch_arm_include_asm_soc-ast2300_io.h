--- linux-2.6.28.10-pristine/arch/arm/include/asm/soc-ast2300/io.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/include/asm/soc-ast2300/io.h	2011-05-25 14:59:15.834790817 +0800
@@ -0,0 +1,20 @@
+/*
+ * linux/include/asm-arm/arch-xxx/io.h
+ *
+ *  Copyright (C) 2001  MontaVista Software, Inc.
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License version 2 as
+ * published by the Free Software Foundation.
+ */
+
+#ifndef __ASM_ARM_ARCH_IO_H
+#define __ASM_ARM_ARCH_IO_H
+
+#define IO_SPACE_LIMIT      0x0000ffff      //0xffffffff -> this is org vincent
+#define __io(a)				((void __iomem *)((a)))
+#define __mem_pci(a)		(a)
+
+extern void EarlyPrintk(char *);
+
+#endif
