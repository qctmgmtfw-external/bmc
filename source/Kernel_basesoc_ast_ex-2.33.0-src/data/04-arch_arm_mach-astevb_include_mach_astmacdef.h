--- linux.org/arch/arm/mach-astevb/include/mach/astmacdef.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/astmacdef.h	2014-01-31 15:42:23.587921284 -0500
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
