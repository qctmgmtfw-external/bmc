--- u-boot-2013.07/arch/arm/cpu/astcommon/ast_kcs.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/ast_kcs.c	2013-12-05 12:17:55.083503948 -0500
@@ -0,0 +1,150 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+
+ ******************************************************************
+ * 
+ * KCS.c
+ * KCS Functions.
+ *
+ *  Author: Rama Rao Bisa <ramab@ami.com>
+ *			Modified by Anurag Bhatia for Pilot-II porting		
+ ******************************************************************/
+#include <common.h>
+#include <config.h>
+
+#include "soc_hw.h"
+
+/* LPC Base Address */ 
+#define AST_LPC_BASE		0x1E789000
+#define LPC_BASE_ADDR       AST_LPC_BASE
+
+/* Register Definitions */
+#define HICR0				0x00
+#define HICR1				0x04
+#define HICR2				0x08
+#define HICR3				0x0C
+#define HICR4				0x10
+#define LADR3H				0x14
+#define LADR3L				0x18
+#define LADR12H				0x1C
+#define LADR12L				0x20
+
+#define BIT0	0x01
+#define BIT1	0x02
+#define BIT2	0x04
+#define BIT3	0x08
+#define BIT4	0x10
+#define BIT5	0x20
+#define BIT6	0x40
+#define BIT7	0x80
+
+#define	KCS_1_CHANNEL			0
+#define	KCS_2_CHANNEL			1
+#define KCS_3_CHANNEL			2
+
+#define WRITE_U8(reg,data)			*((volatile unsigned long *)(reg)) = (data)
+#define READ_U8(reg)					*((volatile unsigned long *)(reg))
+
+
+#define DISABLE_KCS_INTRS(CHANNEL)				 				\
+do {                    									  	\
+	u8 temp;                    				               	\
+    switch (CHANNEL) {                                     	  	\
+    case KCS_1_CHANNEL:										  	\
+		temp = READ_U8(lpc_base + HICR2);					  	\
+		temp &= (~BIT3);								  		\
+		WRITE_U8(lpc_base + HICR2,temp);				  		\
+	break;   												  	\
+    case KCS_2_CHANNEL:										  	\
+		temp = READ_U8(lpc_base + HICR2);					  	\
+		temp &= (~BIT2);									  	\
+		WRITE_U8(lpc_base + HICR2,temp);				  		\
+	break;   												  	\
+    }                                                      	  	\
+} while (0)
+
+
+
+#define ENABLE_KCS_ADDRESS_SELECT(CHANNEL)     				    \
+do {                    									 	\
+	unsigned char temp;                    				      	\
+    switch (CHANNEL) {                                     	  	\
+    case KCS_1_CHANNEL:										  	\
+		temp = READ_U8(lpc_base + HICR4);					  	\
+		temp |= BIT2;										  	\
+		temp &= ~BIT0;										  	\
+		WRITE_U8 (lpc_base+ HICR4,temp);					  	\
+	break;   												  	\
+    case KCS_2_CHANNEL:										  	\
+		temp = READ_U8(lpc_base + HICR4);					  	\
+		temp |= BIT7;										  	\
+		WRITE_U8 (lpc_base+ HICR4,temp);				  		\
+	break;   												  	\
+    }                                                  		  	\
+} while (0)
+
+#define SET_KCS_ADDRESS(CHANNEL, ADDR) 						              			\
+do {                    												  			\
+	int temp = ADDR;                   				              		  			\
+    switch (CHANNEL) {                                     				  			\
+    case KCS_1_CHANNEL:													  			\
+		WRITE_U8 (lpc_base + LADR3H, (unsigned char)(temp >> 8));		  			\
+		WRITE_U8 (lpc_base + LADR3L, (unsigned char)(temp & 0xFA));					\
+	break;   															 			\
+    case KCS_2_CHANNEL:													  			\
+		WRITE_U8 (lpc_base +LADR12H, (unsigned char)(temp >> 8));		  			\
+		WRITE_U8 (lpc_base +LADR12L, (unsigned char)(temp));		  				\
+	break;   															  			\
+    }                                                      				  			\
+} while (0)
+
+#define ENABLE_KCS_CHANNEL(CHANNEL)				      			\
+do {                    									  	\
+	unsigned char temp;                    				      	\
+    switch (CHANNEL) {                                     	  	\
+    case KCS_1_CHANNEL:									  		\
+		temp = READ_U8(lpc_base + HICR0);						\
+		temp |= BIT7;									  		\
+		WRITE_U8 (lpc_base + HICR0,temp);						\
+	break;   												  	\
+    case KCS_2_CHANNEL:										  	\
+		temp = READ_U8(lpc_base + HICR0);						\
+		temp |= BIT6;										  	\
+		WRITE_U8 (lpc_base + HICR0,temp);				  		\
+	break;   											  		\
+    }                                                     		\
+} while (0)
+
+
+
+
+volatile unsigned char   *lpc_base;
+void InitializeKCSHardware(void)
+{
+	static int m_kcs1baseaddr = 0xCA2;
+	static int m_kcs2baseaddr = 0xCA4;
+	lpc_base				= (unsigned char *) LPC_BASE_ADDR;
+
+	ENABLE_KCS_ADDRESS_SELECT  (KCS_1_CHANNEL);
+	SET_KCS_ADDRESS				(KCS_1_CHANNEL, m_kcs1baseaddr);
+	ENABLE_KCS_CHANNEL 			(KCS_1_CHANNEL);
+	DISABLE_KCS_INTRS(0);
+
+	ENABLE_KCS_ADDRESS_SELECT  (KCS_2_CHANNEL);
+	SET_KCS_ADDRESS				(KCS_2_CHANNEL, m_kcs2baseaddr);
+	ENABLE_KCS_CHANNEL 			(KCS_2_CHANNEL);
+	DISABLE_KCS_INTRS(1);
+	return;
+}
+
