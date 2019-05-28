--- u-boot-2013.07/arch/arm/include/asm/u-boot.h	2013-12-13 13:49:25.444958498 -0500
+++ uboot.new/arch/arm/include/asm/u-boot.h	2013-12-05 12:17:55.087503948 -0500
@@ -54,6 +54,23 @@
 	ulong start;
 	ulong size;
     }			bi_dram[CONFIG_NR_DRAM_BANKS];
+
+	unsigned char   bi_enetaddr[6];		/* OLD: see README.enetaddr */
+#ifdef CONFIG_HAS_ETH1
+	unsigned char   bi_enet1addr[6];	/* OLD: see README.enetaddr */
+#endif
+#ifdef CONFIG_HAS_ETH2
+	unsigned char	bi_enet2addr[6];	/* OLD: see README.enetaddr */
+#endif
+#ifdef CONFIG_HAS_ETH3
+	unsigned char   bi_enet3addr[6];	/* OLD: see README.enetaddr */
+#endif
+#ifdef CONFIG_HAS_ETH4
+	unsigned char   bi_enet4addr[6];	/* OLD: see README.enetaddr */
+#endif
+#ifdef CONFIG_HAS_ETH5
+	unsigned char   bi_enet5addr[6];	/* OLD: see README.enetaddr */
+#endif
 } bd_t;
 #endif
 
