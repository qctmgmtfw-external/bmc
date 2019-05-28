--- linux_org/arch/arm/include/uapi/asm/setup.h	2015-07-13 18:09:07.912270147 +0530
+++ linux/arch/arm/include/uapi/asm/setup.h	2015-07-13 19:03:53.316561587 +0530
@@ -152,7 +152,14 @@
    unsigned char enet1_addr[6];
    unsigned char enet2_addr[6];
    unsigned char enet3_addr[6];
+#ifdef   CONFIG_SPX_FEATURE_VIRTUAL_ETH_NET
+   unsigned char enet4_addr[6];
+   unsigned char enet5_addr[6];
+   unsigned char enet6_addr[6];
+   unsigned char enet7_addr[6];
+#endif   
 };
+extern unsigned long enetaddr[][6];
 
 
 
