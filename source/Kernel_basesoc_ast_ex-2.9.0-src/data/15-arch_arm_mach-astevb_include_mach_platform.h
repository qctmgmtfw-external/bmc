--- linux.org/arch/arm/mach-astevb/include/mach/platform.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/platform.h	2014-01-31 16:35:28.971859979 -0500
@@ -0,0 +1,33 @@
+/*
+ *  linux/include/asm-arm/arch-astvb/platform.h
+ *
+ *  This file contains the AST SOC specific values
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
+#ifndef     _PLATFORM_ASTEVB_H_
+#define     _PLATFORM_ASTEVB_H_
+
+#include <mach/hardware.h>
+
+#include <asm/soc-ast/hwmap.h>
+#include <asm/soc-ast/hwreg.h>
+#include <asm/soc-ast/hwdef.h>
+
+#endif
