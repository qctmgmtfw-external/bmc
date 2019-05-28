--- uboot/examples/memtest/sizes.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.exam/examples/memtest/sizes.h	2014-07-28 17:23:30.961408921 -0400
@@ -0,0 +1,39 @@
+/*
+ * Very simple but very effective user-space memory tester.
+ * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
+ * Version 2 by Charles Cazabon <memtest@discworld.dyndns.org>
+ * Version 3 not publicly released.
+ * Version 4 rewrite:
+ * Copyright (C) 2005 Charles Cazabon <memtest@discworld.dyndns.org>
+ * Licensed under the terms of the GNU General Public License version 2 (only).
+ * See the file COPYING for details.
+ *
+ * This file contains some macro definitions for handling 32/64 bit platforms.
+ *
+ */
+
+//#include <limits.h>
+#define ULONG_MAX 4294967295UL
+
+#define rand32() ((unsigned int) rand() | ( (unsigned int) rand() << 16))
+
+#if (ULONG_MAX == 4294967295UL)
+    #define rand_ul() rand32()
+    #define UL_ONEBITS 0xffffffff
+    #define UL_LEN 32
+    #define CHECKERBOARD1 0x55555555
+    #define CHECKERBOARD2 0xaaaaaaaa
+    #define UL_BYTE(x) ((x | x << 8 | x << 16 | x << 24))
+#elif (ULONG_MAX == 18446744073709551615ULL)
+    #define rand64() (((ul) rand32()) << 32 | ((ul) rand32()))
+    #define rand_ul() rand64()
+    #define UL_ONEBITS 0xffffffffffffffffUL
+    #define UL_LEN 64
+    #define CHECKERBOARD1 0x5555555555555555
+    #define CHECKERBOARD2 0xaaaaaaaaaaaaaaaa
+    #define UL_BYTE(x) (((ul)x | (ul)x<<8 | (ul)x<<16 | (ul)x<<24 | (ul)x<<32 | (ul)x<<40 | (ul)x<<48 | (ul)x<<56))
+#else
+    #error long on this platform is not 32 or 64 bits
+#endif
+
+
