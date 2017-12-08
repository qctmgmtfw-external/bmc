--- .pristine/libipmimsghndlr-2.159.0-src/data/PendTask.c Fri Sep 16 14:05:48 2011
+++ source/libipmimsghndlr-2.159.0-src/data/PendTask.c Fri Sep 16 18:18:53 2011
@@ -64,6 +64,8 @@
 #include <sys/reboot.h>
 #include <linux/reboot.h>
 #include<sys/prctl.h>
+#include <sys/types.h>
+#include <sys/stat.h>
 /*--------------------------------------------------------------------
 * Global Variables
 *--------------------------------------------------------------------*/
@@ -1400,6 +1402,7 @@
 {
 	BMCInfo_t *pBMCInfo = &g_BMCInfo[BMCInst];
 	RestartService_T *pReq = (RestartService_T *) pData;
+	struct stat buf;
 	int i;
 	
 	if(pReq->ServiceName == IPMI)
@@ -1421,9 +1424,15 @@
 	{
 		//remove tmp/sdr_data file, to reflect the appropriate SDR change in webUI
 		unlink("/var/tmp/sdr_data");
-
-		//restart the webserver to avoid the websession hang
-		safe_system("/etc/init.d/webgo.sh restart");		
+		if(stat("/etc/init.d/webgo.sh",&buf) == 0)
+		{
+			//restart the webserver to avoid the websession hang
+			safe_system("/etc/init.d/webgo.sh restart");		
+		}
+		else
+		{
+			safe_system("/etc/init.d/lighttpd.sh restart");
+		}
 	}
 	else if(pReq->ServiceName == REBOOT)
 	{	
