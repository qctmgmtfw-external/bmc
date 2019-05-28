--- u-boot-2013.07/arch/arm/include/asm/setup.h	2013-12-13 13:49:25.488958498 -0500
+++ uboot.new/arch/arm/include/asm/setup.h	2013-12-05 12:17:55.091503948 -0500
@@ -205,6 +205,16 @@
 	u32 fmemclk;
 };
 
+/* ATAG to pass MAC address */
+#define ATAG_ENETADDR	0x41000901
+struct tag_enetaddr {
+	unsigned long enet_count;
+	unsigned char enet0_addr[6];
+	unsigned char enet1_addr[6];
+	unsigned char enet2_addr[6];
+	unsigned char enet3_addr[6];
+};
+
 struct tag {
 	struct tag_header hdr;
 	union {
@@ -227,6 +237,8 @@
 		 * DC21285 specific
 		 */
 		struct tag_memclk	memclk;
+		
+		struct tag_enetaddr	enetaddr;
 	} u;
 };
 
