--- u-boot-2013.07/oem/ami/fmh/cmd_fmh.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/oem/ami/fmh/cmd_fmh.h	2013-12-05 12:17:54.975503948 -0500
@@ -0,0 +1,22 @@
+#ifndef __AMI_CMD_FMH_H__
+#define __AMI_CMD_FMH_H__
+
+/* For deciding on mac address */
+#define MAC_ADDR_LEN  18 		 /* no : in between */
+#define BLINK_DELAY_COUNT 200000 /* 200 msec */
+#define FIRMWARE_INFO_TYPE    MODULE_FORMAT_FIRMWARE_INFO
+
+/* U-boot's cmd function to list and bootFMH */
+extern int  do_fmh(cmd_tbl_t *, int, int, char * const []);
+extern int  do_bootfmh(cmd_tbl_t *, int, int, char * const []);
+
+/* Actual function implementing listing of FMH*/
+extern int  ListFMH(void);	
+extern int  GetFMHSectorLocationSize(char * Name, unsigned long * Location, unsigned long * Size);
+extern int  BootFMH(unsigned short PathID);
+extern int processMac( char *mac);
+extern int getMacKb(unsigned char *mac );
+extern int gets(char * buff,unsigned int size );
+extern int validateMac( unsigned char *s);
+
+#endif	
