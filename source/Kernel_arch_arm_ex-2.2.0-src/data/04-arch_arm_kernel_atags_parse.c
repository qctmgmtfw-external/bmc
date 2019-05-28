--- linux-3.14.17/arch/arm/kernel/atags_parse.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/arch/arm/kernel/atags_parse.c	2014-08-21 14:01:55.561721086 -0400
@@ -139,6 +139,26 @@
 
 __tagtable(ATAG_CMDLINE, parse_tag_cmdline);
 
+unsigned long enetaddr[4][6];
+
+static int __init parse_tag_enetaddr(const struct tag *tag)
+{
+   if (tag->u.enetaddr.enet_count > 0)
+       memcpy(enetaddr[0],tag->u.enetaddr.enet0_addr,6);
+   if (tag->u.enetaddr.enet_count > 1)
+       memcpy(enetaddr[1],tag->u.enetaddr.enet1_addr,6);
+   if (tag->u.enetaddr.enet_count > 2)
+       memcpy(enetaddr[2],tag->u.enetaddr.enet2_addr,6);
+   if (tag->u.enetaddr.enet_count > 3)
+       memcpy(enetaddr[3],tag->u.enetaddr.enet3_addr,6);
+   return 0;
+}
+__tagtable(ATAG_ENETADDR, parse_tag_enetaddr);
+
+
+
+
+
 /*
  * Scan the tag table for this tag, and call its parse function.
  * The tag table is built by the linker from all the __tagtable
