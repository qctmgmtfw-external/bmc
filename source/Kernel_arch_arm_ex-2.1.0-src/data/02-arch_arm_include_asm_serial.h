--- linux-3.14.17/arch/arm/include/asm/serial.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/include/asm/serial.h	2014-08-21 13:52:46.309710774 -0400
@@ -0,0 +1,9 @@
+#ifndef __ASM_SERIAL_H
+#define __ASM_SERIAL_H
+
+//#define BASE_BAUD    (1843200 / 16)
+
+#include <mach/serial.h>
+
+#endif
+
