--- linux-3.14.17/arch/arm/include/asm/glue-cache.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/arch/arm/include/asm/glue-cache.h	2014-08-21 14:07:13.549727056 -0400
@@ -29,7 +29,7 @@
 
 #if defined(CONFIG_CPU_ARM920T) || defined(CONFIG_CPU_ARM922T) || \
     defined(CONFIG_CPU_ARM925T) || defined(CONFIG_CPU_ARM1020) || \
-    defined(CONFIG_CPU_ARM1026)
+    defined(CONFIG_CPU_ARM1026) || defined(CONFIG_CPU_ARM926T)
 # define MULTI_CACHE 1
 #endif
 
