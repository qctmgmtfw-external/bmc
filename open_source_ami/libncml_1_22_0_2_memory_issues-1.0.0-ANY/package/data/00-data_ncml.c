--- .pristine/libncml-1.22.0-src/data/ncml.c Wed Sep 14 14:27:47 2011
+++ source/libncml-1.22.0-src/data/ncml.c Wed Sep 14 15:54:23 2011
@@ -501,7 +501,7 @@
     iniparser_setstr(d, temp, tempval);
     
     //InterfaceName
-    if(conf->InterfaceName == NULL)
+    if(conf->InterfaceName[0] == '\0')
     {
         TDBG("%s is NULL\n", STR_SERVICE_INTERFACE_NAME);
     }   
@@ -640,12 +640,14 @@
         if(buff == NULL)
         {
             TCRIT("Error in allocating memory to hold service Configurations \n");
+            fclose(srcfd);
             return -1;
         }
 
         if(fsize != fread(buff,sizeof(char),fsize,srcfd))
         {
             TCRIT("Error in reading service configurations from %s \n",DEFAULT_SERVICE_CONF_FILE);
+            fclose(srcfd);
             free(buff);
             return -1;
         }
@@ -654,6 +656,7 @@
         if(createfd == NULL)
         {
             TCRIT("Unable to create %s\n",SERVICE_CONF_FILE);
+            fclose(srcfd);
             free(buff);
             return -1;
         }
@@ -661,6 +664,8 @@
         if(fsize != fwrite(buff,sizeof(char),fsize,createfd))
         {
             TCRIT("Error in writing service configurations to %s",SERVICE_CONF_FILE);
+            fclose(srcfd);
+            fclose(createfd);
             free(buff);
             return -1;
         }
