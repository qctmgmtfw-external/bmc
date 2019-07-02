--- linux-2.6.28.10-pristine/arch/arm/mach-ast2300evb/include/mach/astmacdef.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/mach-ast2300evb/include/mach/astmacdef.h	2011-05-25 14:59:15.955665633 +0800
@@ -0,0 +1,31 @@
+/*
+ *  This file contains the AST MAC Hardware description
+ *
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
+#ifndef ASTMAC_DEF_H
+#define ASTMAC_DEF_H
+
+/*****************************************************************
+							Network  Setting
+*****************************************************************/
+#define  MAX_ASTMAC 1
+const unsigned long  astmac_io[MAX_ASTMAC]  = {AST_MAC_VA_BASE};
+const unsigned short astmac_irq[MAX_ASTMAC] = {IRQ_MAC};
+
+#endif