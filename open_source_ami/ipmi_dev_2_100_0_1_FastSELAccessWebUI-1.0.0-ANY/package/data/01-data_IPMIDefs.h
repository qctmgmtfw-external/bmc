--- .pristine/ipmi_dev-2.100.0-src/data/IPMIDefs.h Thu Sep 15 22:48:04 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMIDefs.h Thu Sep 15 22:58:35 2011
@@ -195,6 +195,9 @@
 #define OEMCC_INV_MIN_IFC_COUNT_DISABLED				0x83
 #define OEMCC_BOND_NOT_ENABLED  						0x80
 #define OEMCC_ACTIVE_SLAVE_LINK_DOWN                    0x81
+#define OEMCC_SEL_EMPTY_REPOSITORY                         0x86
+#define OEMCC_SEL_CLEARED                                           0x85
+
 
 #define	CC_POWER_LIMIT_OUT_OF_RANGE						0x84
 #define	CC_CORRECTION_TIME_OUT_OF_RANGE				    0x85
