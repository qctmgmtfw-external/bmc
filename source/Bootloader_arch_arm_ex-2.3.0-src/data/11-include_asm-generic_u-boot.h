--- uboot_orig/include/asm-generic/u-boot.h	2015-08-25 16:47:29.995702528 +0530
+++ uboot/include/asm-generic/u-boot.h	2015-08-25 16:08:38.842249934 +0530
@@ -114,6 +114,12 @@
 #ifdef CONFIG_HAS_ETH5
 	unsigned char   bi_enet5addr[6];	/* OLD: see README.enetaddr */
 #endif
+#ifdef CONFIG_VIRTUAL_ETH
+        unsigned char   bi_vnetaddr[6]; /* OLD: see README.enetaddr */
+        unsigned char   bi_vnet1addr[6];        /* OLD: see README.enetaddr */
+        unsigned char   bi_vnet2addr[6];        /* OLD: see README.enetaddr */
+        unsigned char   bi_vnet3addr[6];        /* OLD: see README.enetaddr */
+#endif
 
 #if defined(CONFIG_405GP) || defined(CONFIG_405EP) || \
 		defined(CONFIG_405EZ) || defined(CONFIG_440GX) || \
