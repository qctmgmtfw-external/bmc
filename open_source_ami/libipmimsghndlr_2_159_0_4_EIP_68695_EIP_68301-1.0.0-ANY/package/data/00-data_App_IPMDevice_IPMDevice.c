--- .pristine/libipmimsghndlr-2.159.0-src/data/App/IPMDevice/IPMDevice.c Wed Sep  7 20:23:25 2011
+++ source/libipmimsghndlr-2.159.0-src/data/App/IPMDevice/IPMDevice.c Thu Sep  8 22:31:42 2011
@@ -76,6 +76,7 @@
 static void GetFirmwareVersion(unsigned int* Major,unsigned int* Minor,unsigned int* Rev)
 {
     char aline[82];
+    int AuxVer;
 
     FILE* fp = fopen(FW_INFO_FILE,"rb");
     if(fp == NULL)
@@ -89,7 +90,7 @@
 
     fgets(aline,79,fp);
 
-    sscanf(aline,"FW_VERSION=%d.%d.%d",Major,Minor,Rev);
+    sscanf(aline,"FW_VERSION=%d.%d.%d.%d",Major,Minor,&AuxVer,Rev);
 
     fclose(fp);
 
