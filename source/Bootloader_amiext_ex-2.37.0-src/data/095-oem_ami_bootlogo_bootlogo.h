--- u-boot-2013.07/oem/ami/bootlogo/cmd_bootlogo.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot.new/oem/ami/bootlogo/bootlogo.h	2015-01-16 16:05:03.000000000 +0800
@@ -0,0 +1,53 @@
+#ifndef __BOOTLOGO_H__
+#define __BOOTLOGO_H__
+
+#define BOOT_LOGO_TAG		0x494458
+#define BOOT_LOGO_MAGIC		0x4F474F4C
+
+#define MAX_LOGO_WIDTH		800
+#define MAX_LOGO_HEIGHT		600
+#define MAX_LOGO_COLORS		256
+
+#define LOGO_TYPE_CLUT		1
+
+struct boot_logos_t {
+    unsigned int magic; 
+    unsigned int count; //num of logos
+} __attribute__((packed));
+
+struct info_data_t {
+    unsigned int size;
+    unsigned int offset;
+} __attribute__((packed));
+
+struct info_clut_t {
+    unsigned int size;
+    unsigned int offset;
+} __attribute__((packed));
+
+struct checksum_t {
+    unsigned int info;
+    unsigned int data;
+    unsigned int clut;
+} __attribute__((packed));
+
+struct logo_info_t {
+    unsigned int tag;
+    unsigned int index;
+    unsigned int type;
+    unsigned int width;
+    unsigned int height;
+    unsigned int max_color;
+    struct info_data_t data;
+    struct info_clut_t clut;
+    struct checksum_t chksum;
+} __attribute__((packed));
+
+struct color_t {
+    unsigned char red;
+    unsigned char green;
+    unsigned char blue;
+    unsigned char alpha;
+} __attribute__((packed));
+
+#endif
