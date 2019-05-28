--- linux_org/arch/arm/kernel/atags_parse.c	2015-07-13 18:09:04.784254636 +0530
+++ linux/arch/arm/kernel/atags_parse.c	2015-07-13 20:22:08.747844973 +0530
@@ -139,7 +139,12 @@
 
 __tagtable(ATAG_CMDLINE, parse_tag_cmdline);
 
+#ifdef CONFIG_SPX_FEATURE_VIRTUAL_ETH_NET
+unsigned long enetaddr[8][6];
+EXPORT_SYMBOL (enetaddr);
+#else
 unsigned long enetaddr[4][6];
+#endif
 
 static int __init parse_tag_enetaddr(const struct tag *tag)
 {
@@ -151,6 +156,16 @@
        memcpy(enetaddr[2],tag->u.enetaddr.enet2_addr,6);
    if (tag->u.enetaddr.enet_count > 3)
        memcpy(enetaddr[3],tag->u.enetaddr.enet3_addr,6);
+#ifdef CONFIG_SPX_FEATURE_VIRTUAL_ETH_NET
+   if (tag->u.enetaddr.enet_count > 4)
+       memcpy(enetaddr[4],tag->u.enetaddr.enet4_addr,6);
+   if (tag->u.enetaddr.enet_count > 5)
+       memcpy(enetaddr[5],tag->u.enetaddr.enet5_addr,6);
+   if (tag->u.enetaddr.enet_count > 6)
+       memcpy(enetaddr[6],tag->u.enetaddr.enet6_addr,6);
+   if (tag->u.enetaddr.enet_count > 7)
+       memcpy(enetaddr[7],tag->u.enetaddr.enet7_addr,6);
+#endif   
    return 0;
 }
 __tagtable(ATAG_ENETADDR, parse_tag_enetaddr);
