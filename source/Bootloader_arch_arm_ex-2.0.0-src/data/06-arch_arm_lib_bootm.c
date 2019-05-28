--- u-boot-2013.07/arch/arm/lib/bootm.c	2013-12-13 13:49:25.392958498 -0500
+++ uboot.new/arch/arm/lib/bootm.c	2013-12-05 12:17:55.071503948 -0500
@@ -121,6 +121,44 @@
 	}
 }
 
+static void setup_enetaddr_tags(bd_t *bd)
+{
+	unsigned long count;
+
+    /* Reload current values from environment */
+    eth_getenv_enetaddr("ethaddr", bd->bi_enetaddr);
+#ifdef CONFIG_HAS_ETH1
+    eth_getenv_enetaddr("eth1addr", bd->bi_enet1addr);
+#endif
+#ifdef CONFIG_HAS_ETH2
+    eth_getenv_enetaddr("eth2addr", bd->bi_enet2addr);
+#endif
+#ifdef CONFIG_HAS_ETH3
+    eth_getenv_enetaddr("eth3addr", bd->bi_enet3addr);
+#endif
+
+
+	params->hdr.tag = ATAG_ENETADDR;
+	params->hdr.size = tag_size (tag_enetaddr);
+	memcpy(params->u.enetaddr.enet0_addr, bd->bi_enetaddr,6);
+	count=1;
+#if defined(CONFIG_HAS_ETH1)
+	memcpy(params->u.enetaddr.enet1_addr, bd->bi_enet1addr,6);
+	count++;
+#endif
+#if defined(CONFIG_HAS_ETH2)
+	memcpy(params->u.enetaddr.enet2_addr, bd->bi_enet2addr,6);
+	count++;
+#endif
+#if defined(CONFIG_HAS_ETH3)
+	memcpy(params->u.enetaddr.enet3_addr, bd->bi_enet3addr,6);
+	count++;
+#endif
+	params->u.enetaddr.enet_count = count;
+	params = tag_next (params);
+}
+
+
 static void setup_commandline_tag(bd_t *bd, char *commandline)
 {
 	char *p;
@@ -217,6 +242,8 @@
 			setup_revision_tag(&params);
 		if (BOOTM_ENABLE_MEMORY_TAGS)
 			setup_memory_tags(gd->bd);
+		if (BOOTM_ENABLE_ENETADDR_TAGS)
+			setup_enetaddr_tags(gd->bd);			
 		if (BOOTM_ENABLE_INITRD_TAG) {
 			if (images->rd_start && images->rd_end) {
 				setup_initrd_tag(gd->bd, images->rd_start,
