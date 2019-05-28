--- linux.org/arch/arm/mach-astevb/include/mach/hardware.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/hardware.h	2014-01-31 15:42:23.587921284 -0500
@@ -0,0 +1,48 @@
+/*
+ *  linux/include/asm-arm/arch-ast3100evb/hardware.h
+ *
+ *  This file contains the hardware definitions of the AST 2000 EVB
+ *
+ *  Copyright (C) 1999 ARM Limited.
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
+#ifndef __ASM_ARCH_HARDWARE_H
+#define __ASM_ARCH_HARDWARE_H
+
+#include <asm/sizes.h>
+#include <mach/platform.h>
+
+/*****************************************************************
+							IO Mapping
+ All registers (128MB) and flash (max 128MB) should fit within
+ 256MB @ 0xF0000000
+ Mem address Range
+		 Virt = 0xF0000000 to 0xFFFFFFFF
+	     Phys = 0x10000000 to 0x1FFFFFFF
+ IO address Range
+		 Virt = 0xF7000000 to 0xFEFFFFFF
+         Phys = 0x16000000 to 0x1EFFFFFF
+*****************************************************************/
+#define IO_BASE			0xF0000000                 // VA of IO
+#define MEM_ADDRESS(x)  ((x&0x00ffffff)+IO_BASE)
+#define IO_ADDRESS(x)   ((x&0x0fffffff)+IO_BASE)
+#define PHY_ADDRESS(x)  ((x&0x0fffffff)+0x10000000)
+
+#define PCIMEM_SIZE		0x01000000
+#define PCIMEM_BASE		0xf0000000
+
+#endif
