--- include/configs/ast2500evb.h	1970-01-01 08:00:00.000000000 +0800
+++ include/configs/ast2500evb.h	2014-08-15 10:45:03.203419523 +0800
@@ -0,0 +1,50 @@
+/*-------------------------------------------------------------------------------------------------------------------*/
+/* ----------------------------------------------AST2500EVB Configuration -------------------------------------------*/
+/*-------------------------------------------------------------------------------------------------------------------*/
+#ifndef __AST2500EVB_H__
+#define __AST2500EVB_H__
+
+#include <configs/board.cfg>
+#include <configs/ast2.cfg>
+#include <configs/common.cfg>
+
+/* File Systems to be suported */
+#define CONFIG_FS_FAT		1
+#define CONFIG_CMD_FAT		1
+#define CONFIG_CMD_EXT2		1
+#define CONFIG_CMD_EXT4		1
+#define CONFIG_FS_EXT2		1
+#define CONFIG_FS_EXT4		1
+#define CONFIG_FS_GENERIC	1
+
+/* Overerride defaults */
+#undef DEBUG
+#undef CONFIG_SYS_ALT_MEMTEST
+
+
+
+/* Override ethernet address (if needed)*/
+#define	CONFIG_ETHADDR					00:00:00:00:00:00	/* Ethernet MAC0 Address 		*/
+#define	CONFIG_ETH1ADDR					00:00:00:00:00:00	/* Ethernet MAC1 Address 		*/
+
+/* Fixed values for board */
+#define	CONFIG_BOARD_NAME				"AST2500EVB"
+#define	CONFIG_CONS_INDEX				5 		
+#define	CONFIG_AST2500					1				
+#define CONFIG_AST2500EVB				1
+	
+/* ECC Support */
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_MEMORY_ECC_ENABLE
+#define CONFIG_SYS_SDRAM_ECC_USED	(CONFIG_SYS_SDRAM_LEN >> 3)
+#define CONFIG_SYS_SDRAM_SYS_USED	(CONFIG_SYS_SDRAM_LEN - CONFIG_SYS_SDRAM_ECC_USED)
+#define CONFIG_DRAM_ECC
+#endif
+
+
+/* BigPhys Area*/
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_FRAME_BUFFER_SUPPORT
+#define	CONFIG_BIGPHYSAREA			"8192"
+#else
+#define	CONFIG_BIGPHYSAREA			"6144"
+#endif
+
+/* Yafu Image downloading locations */
+#define YAFU_IMAGE_UPLOAD_LOCATION	(0x82000000)  
+#define YAFU_TEMP_SECTOR_BUFFER     (0x83000000)
+
+/* Defaut Memory Test Range	*/
+#define CONFIG_SYS_MEMTEST_START  	0x81000000 	
+#define CONFIG_SYS_MEMTEST_END    	0x85000000 
+
+/*-------------------------------------------------------------------------------------------------------------------*/
+#endif 
