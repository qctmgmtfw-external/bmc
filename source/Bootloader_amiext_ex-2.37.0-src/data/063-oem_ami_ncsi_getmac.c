--- u-boot-2013.07/oem/ami/ncsi/getmac.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/oem/ami/ncsi/getmac.c	2013-12-05 12:17:54.975503948 -0500
@@ -0,0 +1,74 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2006, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        6145-F, Northbelt Parkway, Norcross,                **
+ **                                                            **
+ **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************/
+/****************************************************************
+
+  Author	: Samvinesh Christopher
+
+  Module	: NCSI - Get MAC Address from U-boot
+			
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version [SC]
+
+*****************************************************************/
+#include <common.h>
+#ifdef CONFIG_NCSI_SUPPORT
+#include <exports.h>
+#include "types.h"
+#include <net.h>
+
+int 
+GetDefaultPort(void)
+{
+	char PortNum[8];
+	if (getenv_f("ncsiport",PortNum,sizeof(PortNum)) == -1)
+		return 0;
+	if ((PortNum[0] < '0') || (PortNum[0] > '9'))
+		return 0;
+	return PortNum[0]-'0';
+}
+
+
+/* Get MAC Address of the current interface */
+void 
+GetMACAddr(UINT8 *MACAddr)
+{
+	struct eth_device *dev;
+
+	memset(MACAddr,0,6);
+
+	dev = eth_get_dev();
+	if (dev == NULL)
+		return;
+
+//	memcpy(MACAddr,&dev->enetaddr[0],6);
+// 	For some reason, it is stored backward in MACAddr. Why?
+	MACAddr[5] = dev->enetaddr[0];
+	MACAddr[4] = dev->enetaddr[1];
+	MACAddr[3] = dev->enetaddr[2];
+	MACAddr[2] = dev->enetaddr[3];
+	MACAddr[1] = dev->enetaddr[4];
+	MACAddr[0] = dev->enetaddr[5];
+
+	#if NCSI_DEBUG
+	printf("NC-SI (%s) MAC Address  = %02X:%02X:%02X:%02X:%02X:%02X\n",
+						dev->name,
+						MACAddr[5],MACAddr[4],MACAddr[3],
+					  	MACAddr[2],MACAddr[1],MACAddr[0]);
+	#endif
+	return;
+}
+
+#endif
+	
