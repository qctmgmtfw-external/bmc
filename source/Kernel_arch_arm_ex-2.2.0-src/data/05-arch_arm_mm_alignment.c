--- linux-3.14.17/arch/arm/mm/alignment.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/arch/arm/mm/alignment.c	2014-08-21 14:03:19.941722670 -0400
@@ -80,7 +80,7 @@
 static unsigned long ai_word;
 static unsigned long ai_dword;
 static unsigned long ai_multi;
-static int ai_usermode;
+static int ai_usermode=3;	/* Fixup and Warn */
 
 core_param(alignment, ai_usermode, int, 0600);
 
