--- linux-2.6.28.10-pristine/arch/arm/include/asm/soc-ast/dma.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.28.10/arch/arm/include/asm/soc-ast/dma.h	2009-10-06 09:55:46.000000000 -0400
@@ -0,0 +1,27 @@
+/*
+ *  linux/include/asm-arm/arch-xxx/dma.h
+ *
+ *  Copyright (C) 1997,1998 Russell King
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
+#ifndef __ASM_ARCH_DMA_H
+#define __ASM_ARCH_DMA_H
+
+#define MAX_DMA_ADDRESS		0xffffffff
+#define MAX_DMA_CHANNELS	0
+
+#endif /* _ASM_ARCH_DMA_H */
+
