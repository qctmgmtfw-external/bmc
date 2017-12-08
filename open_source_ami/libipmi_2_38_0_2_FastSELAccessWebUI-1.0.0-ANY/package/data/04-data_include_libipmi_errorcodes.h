--- .pristine/libipmi-2.38.0-src/data/include/libipmi_errorcodes.h Thu Sep 15 12:23:01 2011
+++ source/libipmi-2.38.0-src/data/include/libipmi_errorcodes.h Thu Sep 15 14:24:19 2011
@@ -120,6 +120,8 @@
 #define LIBIPMI_E_IPMB_UNKNOWN_ERROR						0x89
 #define LIBIPMI_E_IPMB_REQ_BUFF_TOO_BIG						0x8A
 #define LIBIPMI_E_IPMB_RES_BUFF_TOO_BIG						0x8B
+#define LIBIPMI_E_SEL_CLEARED                                              0x85
+#define LIBIPMI_E_SEL_EMPTY                                                  0x86
 
 
 /* Last RMCP+/RAKP satus code */
