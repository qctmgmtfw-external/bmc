--- linux.org/arch/arm/mach-astevb/include/mach/io.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/io.h	2014-01-31 15:42:23.587921284 -0500
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
+#ifndef __ASM_ARCH_IO_H
+#define __ASM_ARCH_IO_H
+
+#define IO_SPACE_LIMIT      0x0000ffff      //0xffffffff -> this is org vincent
+#define __io(a)				((void __iomem *)((a)))
+#define __mem_pci(a)		(a)
+
+extern void EarlyPrintk(char *);
+
+#endif
