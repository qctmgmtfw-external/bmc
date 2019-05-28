--- linux-3.14.17/arch/arm/include/uapi/asm/setup.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/arch/arm/include/uapi/asm/setup.h	2014-08-21 13:57:53.693716545 -0400
@@ -143,6 +143,19 @@
 	__u32 fmemclk;
 };
 
+
+/* ATAG to pass MAC address and count to OS - AMI Extension */
+#define ATAG_ENETADDR 0x41000901
+struct tag_enetaddr {
+   unsigned long enet_count;
+   unsigned char enet0_addr[6];
+   unsigned char enet1_addr[6];
+   unsigned char enet2_addr[6];
+   unsigned char enet3_addr[6];
+};
+
+
+
 struct tag {
 	struct tag_header hdr;
 	union {
@@ -165,6 +178,11 @@
 		 * DC21285 specific
 		 */
 		struct tag_memclk	memclk;
+
+		/*
+		 * AMI Extension 
+		 */
+		struct tag_enetaddr enetaddr;
 	} u;
 };
 
