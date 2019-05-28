--- u-boot/oem/ami/ncsi/ncsi.c		2015-07-01 21:35:05.671261508 +0800
+++ uboot.new/oem/ami/ncsi/ncsi.c	2015-07-01 21:35:33.895261811 +0800
@@ -172,7 +172,7 @@
 	
 	for (i = 0; i < NCSI_RETRIES; i++)
 	{
-		counter=0x10000;
+		counter = 0x8000;  //counter=0x10000;  // Quanta, to speed up bootup time
 		ReceivedNCSILen = 0;
 		if (-1 == eth_send(pBuf, Size))
 		{
@@ -209,6 +209,7 @@
 				return NCSI_ERR_SUCCESS;
 			}
 		}
+		udelay(1000*10);  // Quanta
 		if (verbose & SHOW_MESSAGES)
 			printf ("NCSI: Resending Command (Retry = %d)...\n",i+1);
 	}
