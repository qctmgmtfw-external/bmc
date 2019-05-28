--- uboot_orig/arch/arm/include/asm/setup.h	2015-08-25 16:47:25.051819348 +0530
+++ uboot/arch/arm/include/asm/setup.h	2015-08-25 15:47:10.794885032 +0530
@@ -205,6 +205,26 @@
 	u32 fmemclk;
 };
 
+/* ATAG to pass MAC address */
+#define ATAG_ENETADDR	0x41000901
+struct tag_enetaddr {
+	unsigned long enet_count;
+	unsigned char enet0_addr[6];
+	unsigned char enet1_addr[6];
+#ifdef CONFIG_HAS_ETH2	
+	unsigned char enet2_addr[6];
+#endif	
+#ifdef CONFIG_HAS_ETH3
+	unsigned char enet3_addr[6];
+#endif	
+#ifdef CONFIG_VIRTUAL_ETH	
+	unsigned char vnet0_addr[6];
+	unsigned char vnet1_addr[6];
+	unsigned char vnet2_addr[6];
+	unsigned char vnet3_addr[6];
+#endif	
+};
+
 struct tag {
 	struct tag_header hdr;
 	union {
@@ -227,6 +247,8 @@
 		 * DC21285 specific
 		 */
 		struct tag_memclk	memclk;
+		
+		struct tag_enetaddr	enetaddr;
 	} u;
 };
 
