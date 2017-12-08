--- .pristine/libncml-1.22.0-src/data/ncml.c Fri Sep 16 13:58:16 2011
+++ source/libncml-1.22.0-src/data/ncml.c Fri Sep 16 20:27:22 2011
@@ -338,6 +338,7 @@
     else
     {  
         strncpy(conf->ServiceName,str,MAX_SERVICE_NAME_SIZE);
+        conf->ServiceName[MAX_SERVICE_NAME_SIZE] = '\0';
         TDBG("%s = %s \n",STR_SERVICE_NAME, conf->ServiceName);
     }
 
@@ -365,6 +366,7 @@
     else
     {  
         strncpy(conf->InterfaceName,str,MAX_SERVICE_IFACE_NAME_SIZE);
+        conf->InterfaceName[MAX_SERVICE_IFACE_NAME_SIZE] = '\0';
         TDBG("%s = %s \n",STR_SERVICE_INTERFACE_NAME, str);
     }
 
