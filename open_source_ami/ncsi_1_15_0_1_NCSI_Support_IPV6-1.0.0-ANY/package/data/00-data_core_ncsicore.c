--- .pristine/ncsi-1.15.0-src/data/core/ncsicore.c Tue Nov 29 19:11:58 2011
+++ source/ncsi-1.15.0-src/data/core/ncsicore.c Tue Dec  6 19:09:16 2011
@@ -385,6 +385,13 @@
 			printk("NCSI(%s):%d.%d Enable Bcast Filter Failed\n",dev->name,PackageID, ChannelID);	
 			continue;
 		}
+
+		/* Disable Multicast filter */
+		if (NCSI_Issue_DisableMcastFilter(info,PackageID,ChannelID) != 0)
+		{
+			printk("NCSI(%s):%d.%d Disable Multicast Filter Failed\n",dev->name,PackageID, ChannelID);
+		}
+		
 		/* Setup AEN Messages */
 		if (NCSI_Issue_EnableAEN(info,(UINT8)PackageID,(UINT8)ChannelID,1,1,1) != 0)
 		{
