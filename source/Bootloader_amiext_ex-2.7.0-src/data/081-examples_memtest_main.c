--- uboot/examples/memtest/main.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.exam/examples/memtest/main.c	2014-07-28 17:23:30.961408921 -0400
@@ -0,0 +1,192 @@
+/***************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2006, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        6145-F, Northbelt Parkway, Norcross,                **
+ **                                                            **
+ **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************/
+/****************************************************************
+  MG9090 Diagnostics 
+
+  Author	: Samvinesh Christopher
+
+  Module	: Entry point of Memory Test
+
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version [SC]
+
+ *****************************************************************/
+/*
+ * memtester version 4
+ *
+ * Very simple but very effective user-space memory tester.
+ * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
+ * Version 2 by Charles Cazabon <memtest@discworld.dyndns.org>
+ * Version 3 not publicly released.
+ * Version 4 rewrite:
+ * Copyright (C) 2005 Charles Cazabon <memtest@discworld.dyndns.org>
+ * Licensed under the terms of the GNU General Public License version 2 (only).
+ * See the file COPYING for details.
+ *
+ */
+/* 
+ * Modified for U-boot 1.1.4 by American Megatrends Inc
+ */
+
+#define __version__ "4.0.5"
+
+#include <common.h>
+#include <exports.h>
+
+#include "types.h"
+#include "sizes.h"
+#include "tests.h"
+
+#define EXIT_FAIL_NONSTARTER    0x01
+#define EXIT_FAIL_ADDRESSLINES  0x02
+#define EXIT_FAIL_OTHERTEST     0x04
+
+struct test tests[] = {
+    { "Random Value", test_random_value },
+    { "Compare XOR", test_xor_comparison },
+    { "Compare SUB", test_sub_comparison },
+    { "Compare MUL", test_mul_comparison },
+    { "Compare DIV",test_div_comparison },
+    { "Compare OR", test_or_comparison },
+    { "Compare AND", test_and_comparison },
+    { "Sequential Increment", test_seqinc_comparison },
+    { "Solid Bits", test_solidbits_comparison },
+    { "Block Sequential", test_blockseq_comparison },
+    { "Checkerboard", test_checkerboard_comparison },
+    { "Bit Spread", test_bitspread_comparison },
+    { "Bit Flip", test_bitflip_comparison },
+    { "Walking Ones", test_walkbits1_comparison },
+    { "Walking Zeroes", test_walkbits0_comparison },
+    { NULL, NULL }
+};
+
+
+int 
+memtester(char *buf,size_t bufsize, unsigned long loops)
+{
+    ul loop, i;
+    size_t  halflen, count;
+    char *aligned;
+    ulv *bufa, *bufb;
+	int exit_code = 0;
+	unsigned long adjust;
+
+	/* Aligned to DWORD boundary*/
+	if (((unsigned long)buf%4)==0)
+		aligned = buf;
+	else
+	{
+		adjust = 4-(unsigned long)buf % 4;
+		aligned = buf+adjust;
+		if (bufsize <= adjust)
+		{
+			printf("Error: Invalid length\n");
+			return 1;
+		}
+		bufsize -= adjust;
+	}
+ 
+    halflen = bufsize / 2;
+    count = halflen / sizeof(ul);
+    bufa = (ulv *) aligned;
+    bufb = (ulv *) ((size_t) aligned + halflen);
+
+    for(loop=1; ((!loops) || loop <= loops); loop++) 
+	{
+        printf("Loop %lu", loop);
+        if (loops) 
+            printf("/%lu", loops);
+        printf(":\n");
+        printf("  %-20s: ", "Stuck Address");
+        if (!test_stuck_address((unsigned long *)aligned, bufsize / sizeof(ul))) 
+		{
+             printf("ok\n");
+        } else 
+		{
+            exit_code |= EXIT_FAIL_ADDRESSLINES;
+        }
+        for (i=0;;i++) 
+		{
+            if (!tests[i].name) 
+					break;
+            printf("  %-20s: ", tests[i].name);
+            if (!tests[i].fp(bufa, bufb, count)) 
+			{
+                printf("ok\n");
+            } else 
+			{
+                exit_code |= EXIT_FAIL_OTHERTEST;
+            }
+        }
+        printf("\n");
+		if  (tstc())
+		{
+			 getc();
+			 break;
+		}
+    }
+    printf("Done.\n");
+	return exit_code;
+}
+
+
+void putchar(char ch) { 	printf("%c",ch); } 
+void fflush(int fd) {}
+extern unsigned long GetUnsigned(const char *prompt);
+
+
+int 
+main(int argc, char *argv[])
+{
+	unsigned long Start;
+	unsigned long End;
+	unsigned long Loop;
+
+	app_startup(argv);
+
+
+	/* Copyright Message */
+	printf ("Memory Diagnostics 1.0 \n");
+	printf ("Copyright (c) 2005, American Megatrends Inc.\n");
+    printf ("Orginal Code for memtester by Charles Cazbon\n");
+	printf ("U-Boot Port American Megatrends Inc.\n");
+	printf ("\n");
+	printf("memester version " __version__ " (%d-bit)\n", UL_LEN);
+    printf("Copyright (C) 2005 Charles Cazabon.\n");
+	printf("Licensed under the GNU General Public License version 2 (only).\n");	
+	printf ("\n");
+
+	Start = End =0;
+	while ((Start+4) >End)
+	{
+		Start = GetUnsigned("Enter the Start Address : ");
+		End  =  GetUnsigned("Enter the End Address : ");
+		if ((Start+4) > End)
+			printf("ERROR: Start should be alteast 4 bytes less than End\n");
+	}
+	Loop=GetUnsigned("Enter Number of Tests (0 for Infinite) :");
+
+	memtester((char *)Start,End-Start,Loop);
+
+	printf ("Hit any key to exit ... ");
+	while (!tstc())
+		;
+	/* consume input */
+	(void) getc();
+	printf ("\n\n");
+	return (0);
+}
+
+void raise(void) {}
