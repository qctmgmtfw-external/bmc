--- linux-2.6.28.10-pristine/arch/arm/mach-ast2300evb/include/mach/platform.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/mach-ast2300evb/include/mach/platform.h	2011-05-26 16:59:12.014821000 +0800
@@ -0,0 +1,46 @@
+/*
+ *  linux/include/asm-arm/arch-ast2300evb/platform.h
+ *
+ *  This file contains the AST2300 SOC specific values
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
+
+#ifndef     _PLATFORM_AST2300EVB_H_
+#define     _PLATFORM_AST2300EVB_H_
+
+#include <mach/hardware.h>
+
+#include <asm/soc-ast2300/hwmap.h>
+#include <asm/soc-ast2300/hwreg.h>
+#include <asm/soc-ast2300/hwdef.h>
+
+//#include <asm/soc-ast2050/mapdesc.h>
+
+/*****************************************************************
+						    Flash
+*****************************************************************/
+//#define AST_FLASH_BASE                  0x10000000
+//#define AST_FLASH_SZ                    0x01000000		/* 16 MB */
+//#define AST_FLASH_VA_BASE               MEM_ADDRESS(AST_FLASH_BASE)
+
+/* Ractrends MTD Map driver require the following defines */
+//#define CPE_FLASH_BASE					 AST_FLASH_BASE
+//#define CPE_FLASH_SZ					 AST_FLASH_SZ
+
+#endif
