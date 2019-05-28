--- uboot/examples/memtest/tests.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.exam/examples/memtest/tests.c	2014-07-28 17:23:30.961408921 -0400
@@ -0,0 +1,435 @@
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
+ * This file contains the functions for the actual tests, called from the
+ * main routine in memtester.c.  See other comments in that file.
+ *
+ */
+
+//#include <sys/types.h>
+//#include <stdio.h>
+//#include <stdlib.h>
+//#include <limits.h>
+//#include "types.h"
+//#include "sizes.h"
+
+#include <common.h>
+#include <exports.h>
+
+#include "types.h"
+#include "sizes.h"
+#include "tests.h"
+
+extern void fflush(int);
+extern void putchar(char);
+extern unsigned long rand(void);
+
+char progress[] = "-\\|/";
+#define PROGRESSLEN 4
+#define PROGRESSOFTEN 2500
+
+/* Function definitions. */
+
+int compare_regions(ulv *bufa, ulv *bufb, size_t count) {
+    int r = 0;
+    size_t i;
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+
+    for (i = 0; i < count; i++, p1++, p2++) {
+        if (*p1 != *p2) {
+            printf( "FAILURE: 0x%08lx != 0x%08lx at offset 0x%08lx.\n", 
+                (ul) *p1, (ul) *p2, (ul) i);
+            /* printf("Skipping to next test..."); */
+            r = -1;
+        }
+    }
+    return r;
+}
+
+int test_stuck_address(ulv *bufa, size_t count) {
+    ulv *p1 = bufa;
+    unsigned int j;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (j = 0; j < 16; j++) {
+	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+        p1 = (ulv *) bufa;
+        printf("setting %3u", j);
+       fflush(stdout);
+        for (i = 0; i < count; i++) {
+            *p1 = ((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1);
+            *p1++;
+        }
+        printf("\b\b\b\b\b\b\b\b\b\b\b");
+        printf("testing %3u", j);
+        fflush(stdout);
+        p1 = (ulv *) bufa;
+        for (i = 0; i < count; i++, p1++) {
+            if (*p1 != (((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1))) {
+                printf( 
+                    "FAILURE: possible bad address line at offset 0x%08lx.\n", 
+                    (ul) i);
+                printf("Skipping to next test...\n");
+                fflush(stdout);
+                return -1;
+            }
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
+
+int test_random_value(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    ul j = 0;
+    size_t i;
+
+	putchar(' ');
+    fflush(stdout);
+    for (i = 0; i < count; i++) {
+        *p1++ = *p2++ = rand_ul();
+		if (!(i % PROGRESSOFTEN)) {
+			putchar('\b');
+			putchar(progress[++j % PROGRESSLEN]);
+            fflush(stdout);
+		}
+    }
+	printf("\b \b");
+    fflush(stdout);
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_xor_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    size_t i;
+    ul q = rand_ul();
+
+    for (i = 0; i < count; i++) {
+        *p1++ ^= q;
+        *p2++ ^= q;
+    }
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_sub_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    size_t i;
+    ul q = rand_ul();
+
+    for (i = 0; i < count; i++) {
+        *p1++ -= q;
+        *p2++ -= q;
+    }
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_mul_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    size_t i;
+    ul q = rand_ul();
+
+    for (i = 0; i < count; i++) {
+        *p1++ *= q;
+        *p2++ *= q;
+    }
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_div_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    size_t i;
+    ul q = rand_ul();
+
+    for (i = 0; i < count; i++) {
+        if (!q) {
+            q++;
+        }
+        *p1++ /= q;
+        *p2++ /= q;
+    }
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_or_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    size_t i;
+    ul q = rand_ul();
+
+    for (i = 0; i < count; i++) {
+        *p1++ |= q;
+        *p2++ |= q;
+    }
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_and_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    size_t i;
+    ul q = rand_ul();
+
+    for (i = 0; i < count; i++) {
+        *p1++ &= q;
+        *p2++ &= q;
+    }
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_seqinc_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    size_t i;
+    ul q = rand_ul();
+
+    for (i = 0; i < count; i++) {
+        *p1++ = *p2++ = (i + q);
+    }
+    return compare_regions(bufa, bufb, count);
+}
+
+int test_solidbits_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    unsigned int j;
+    ul q;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (j = 0; j < 64; j++) {
+	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+        q = (j % 2) == 0 ? UL_ONEBITS : 0;
+        printf("setting %3u", j);
+        fflush(stdout);
+        p1 = (ulv *) bufa;
+        p2 = (ulv *) bufb;
+        for (i = 0; i < count; i++) {
+            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
+        }
+        printf("\b\b\b\b\b\b\b\b\b\b\b");
+        printf("testing %3u", j);
+        fflush(stdout);
+        if (compare_regions(bufa, bufb, count)) {
+            return -1;
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
+
+int test_checkerboard_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    unsigned int j;
+    ul q;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (j = 0; j < 64; j++) {
+	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+        q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
+        printf("setting %3u", j);
+        fflush(stdout);
+        p1 = (ulv *) bufa;
+        p2 = (ulv *) bufb;
+        for (i = 0; i < count; i++) {
+            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
+        }
+        printf("\b\b\b\b\b\b\b\b\b\b\b");
+        printf("testing %3u", j);
+        fflush(stdout);
+        if (compare_regions(bufa, bufb, count)) {
+            return -1;
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
+
+int test_blockseq_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    unsigned int j;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (j = 0; j < 256; j++) {
+	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+        p1 = (ulv *) bufa;
+        p2 = (ulv *) bufb;
+        printf("setting %3u", j);
+        fflush(stdout);
+        for (i = 0; i < count; i++) {
+            *p1++ = *p2++ = (ul) UL_BYTE(j);
+        }
+        printf("\b\b\b\b\b\b\b\b\b\b\b");
+        printf("testing %3u", j);
+        fflush(stdout);
+        if (compare_regions(bufa, bufb, count)) {
+            return -1;
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
+
+int test_walkbits0_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    unsigned int j;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (j = 0; j < 64; j++) {
+	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+        p1 = (ulv *) bufa;
+        p2 = (ulv *) bufb;
+        printf("setting %3u", j);
+        fflush(stdout);
+        for (i = 0; i < count; i++) {
+            if (j < UL_LEN) { /* Walk it up. */
+                *p1++ = *p2++ = 0x00000001 << j;
+            } else { /* Walk it back down. */
+                *p1++ = *p2++ = 0x00000001 << (UL_LEN * 2 - j - 1);
+            }
+        }
+        printf("\b\b\b\b\b\b\b\b\b\b\b");
+        printf("testing %3u", j);
+        fflush(stdout);
+        if (compare_regions(bufa, bufb, count)) {
+            return -1;
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
+
+int test_walkbits1_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    unsigned int j;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (j = 0; j < UL_LEN * 2; j++) {
+	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+        p1 = (ulv *) bufa;
+        p2 = (ulv *) bufb;
+        printf("setting %3u", j);
+        fflush(stdout);
+        for (i = 0; i < count; i++) {
+            if (j < UL_LEN) { /* Walk it up. */
+                *p1++ = *p2++ = UL_ONEBITS ^ (0x00000001 << j);
+            } else { /* Walk it back down. */
+                *p1++ = *p2++ = UL_ONEBITS ^ (0x00000001 << (UL_LEN * 2 - j - 1));
+            }
+        }
+        printf("\b\b\b\b\b\b\b\b\b\b\b");
+        printf("testing %3u", j);
+        fflush(stdout);
+        if (compare_regions(bufa, bufb, count)) {
+            return -1;
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
+
+int test_bitspread_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    unsigned int j;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (j = 0; j < UL_LEN * 2; j++) {
+	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+        p1 = (ulv *) bufa;
+        p2 = (ulv *) bufb;
+        printf("setting %3u", j);
+        fflush(stdout);
+        for (i = 0; i < count; i++) {
+            if (j < UL_LEN) { /* Walk it up. */
+                *p1++ = *p2++ = (i % 2 == 0)
+                    ? (0x00000001 << j) | (0x00000001 << (j + 2))
+                    : UL_ONEBITS ^ ((0x00000001 << j)
+                                    | (0x00000001 << (j + 2)));
+            } else { /* Walk it back down. */
+                *p1++ = *p2++ = (i % 2 == 0)
+                    ? (0x00000001 << (UL_LEN * 2 - 1 - j)) | (0x00000001 << (UL_LEN * 2 + 1 - j))
+                    : UL_ONEBITS ^ (0x00000001 << (UL_LEN * 2 - 1 - j)
+                                    | (0x00000001 << (UL_LEN * 2 + 1 - j)));
+            }
+        }
+        printf("\b\b\b\b\b\b\b\b\b\b\b");
+        printf("testing %3u", j);
+        fflush(stdout);
+        if (compare_regions(bufa, bufb, count)) {
+            return -1;
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
+
+int test_bitflip_comparison(ulv *bufa, ulv *bufb, size_t count) {
+    ulv *p1 = bufa;
+    ulv *p2 = bufb;
+    unsigned int j, k;
+    ul q;
+    size_t i;
+
+	printf("           ");
+    fflush(stdout);
+    for (k = 0; k < UL_LEN; k++) {
+        q = 0x00000001 << k;
+        for (j = 0; j < 8; j++) {
+    	    printf("\b\b\b\b\b\b\b\b\b\b\b");
+            q = ~q;
+            printf("setting %3u", k * 8 + j);
+            fflush(stdout);
+            p1 = (ulv *) bufa;
+            p2 = (ulv *) bufb;
+            for (i = 0; i < count; i++) {
+                *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
+            }
+            printf("\b\b\b\b\b\b\b\b\b\b\b");
+            printf("testing %3u", k * 8 + j);
+            fflush(stdout);
+            if (compare_regions(bufa, bufb, count)) {
+                return -1;
+            }
+        }
+    }
+    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
+    fflush(stdout);
+    return 0;
+}
