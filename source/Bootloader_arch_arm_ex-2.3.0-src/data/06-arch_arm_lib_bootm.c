--- uboot_orig/arch/arm/lib/bootm.c	2015-08-25 16:47:24.839824364 +0530
+++ uboot/arch/arm/lib/bootm.c	2015-08-25 15:45:22.593265343 +0530
@@ -121,6 +121,55 @@
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
+#ifdef CONFIG_VIRTUAL_ETH    
+    eth_getenv_enetaddr("vethaddr", bd->bi_vnetaddr);
+    eth_getenv_enetaddr("veth1addr", bd->bi_vnet1addr);
+    eth_getenv_enetaddr("veth2addr", bd->bi_vnet2addr);
+    eth_getenv_enetaddr("veth3addr", bd->bi_vnet3addr);
+#endif    
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
+#if defined(CONFIG_VIRTUAL_ETH)
+	memcpy(params->u.enetaddr.vnet0_addr, bd->bi_vnetaddr,6);
+	memcpy(params->u.enetaddr.vnet1_addr, bd->bi_vnet1addr,6);
+	memcpy(params->u.enetaddr.vnet2_addr, bd->bi_vnet2addr,6);
+	memcpy(params->u.enetaddr.vnet3_addr, bd->bi_vnet3addr,6);
+	count = count+4;
+#endif
+	params->u.enetaddr.enet_count = count;
+	params = tag_next (params);
+}
+
+
 static void setup_commandline_tag(bd_t *bd, char *commandline)
 {
 	char *p;
@@ -217,6 +266,8 @@
 			setup_revision_tag(&params);
 		if (BOOTM_ENABLE_MEMORY_TAGS)
 			setup_memory_tags(gd->bd);
+		if (BOOTM_ENABLE_ENETADDR_TAGS)
+			setup_enetaddr_tags(gd->bd);			
 		if (BOOTM_ENABLE_INITRD_TAG) {
 			if (images->rd_start && images->rd_end) {
 				setup_initrd_tag(gd->bd, images->rd_start,
