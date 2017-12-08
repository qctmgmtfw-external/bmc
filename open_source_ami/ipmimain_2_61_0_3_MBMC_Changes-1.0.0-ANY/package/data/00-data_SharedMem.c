--- .pristine/ipmimain-2.61.0-src/data/SharedMem.c Fri Sep 23 11:47:45 2011
+++ source/ipmimain-2.61.0-src/data/SharedMem.c Fri Sep 23 12:51:33 2011
@@ -195,7 +195,15 @@
         }
     }
 
-    memcpy(&pBMCInfo->LANConfig.LanIfcConfig[0],g_LANConfigTbl,sizeof(g_LANConfigTbl));
+    if(g_PDKHandle[PDK_GETLANCONFIGURATIONS] != NULL)
+    {
+	    ((int(*)(LANIFCConfig_T*,int))g_PDKHandle[PDK_GETLANCONFIGURATIONS]) (
+	     &pBMCInfo->LANConfig.LanIfcConfig[0],BMCInst);
+    }
+    else
+    {
+        memcpy(&pBMCInfo->LANConfig.LanIfcConfig[0],g_LANConfigTbl,sizeof(g_LANConfigTbl));
+    }
 
     for(i=0;i<MAX_NUM_CHANNELS;i++)
     {
