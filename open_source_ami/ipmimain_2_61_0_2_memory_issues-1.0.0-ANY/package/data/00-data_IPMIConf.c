--- .pristine/ipmimain-2.61.0-src/data/IPMIConf.c Wed Sep 14 14:29:15 2011
+++ source/ipmimain-2.61.0-src/data/IPMIConf.c Wed Sep 14 16:29:29 2011
@@ -155,6 +155,7 @@
         if(buff == NULL)
         {
             IPMI_ERROR("Error in allocating memory to hold IPMI Configurations \n");
+            fclose(srcfd);
             return -1;
         }
 
@@ -169,6 +170,7 @@
         if(createfd == NULL)
         {
             IPMI_ERROR("Unable to create %s\n",IPMIConfFile);
+            fclose(srcfd);
             free(buff);
             return -1;
         }
@@ -176,6 +178,8 @@
         if(fsize != fwrite(buff,sizeof(char),fsize,createfd))
         {
             IPMI_ERROR("Error in writing IPMI Configurations to %s",IPMIConfFile);
+            fclose(srcfd);
+            fclose(createfd);
             free(buff);
             return -1;
         }
@@ -381,17 +385,20 @@
     {
         IPMI_DBG_PRINT("Configuration SOL_IFC_PORT is not found\n");
     }
-    strsize = strlen(str);
-    pBMCInfo->IpmiConfig.pSOLPort = malloc(strsize);
-    if(pBMCInfo->IpmiConfig.pSOLPort != NULL)
-    {
-        strncpy(pBMCInfo->IpmiConfig.pSOLPort,str,strsize);
-        IPMI_DBG_PRINT_1("SOL Port Configuration is %s \n",pBMCInfo->IpmiConfig.pSOLPort);
-        pBMCInfo->IpmiConfig.pSOLPort[strsize] = '\0';
-    }
-    else
-    {
-        IPMI_ERROR("Error in Getting SOL Port Configuration \n");
+    else
+    {
+	    strsize = strlen(str);
+	    pBMCInfo->IpmiConfig.pSOLPort = malloc(strsize);
+	    if(pBMCInfo->IpmiConfig.pSOLPort != NULL)
+	    {
+	        strncpy(pBMCInfo->IpmiConfig.pSOLPort,str,strsize);
+	        IPMI_DBG_PRINT_1("SOL Port Configuration is %s \n",pBMCInfo->IpmiConfig.pSOLPort);
+	        pBMCInfo->IpmiConfig.pSOLPort[strsize] = '\0';
+	    }
+	    else
+	    {
+	        IPMI_ERROR("Error in Getting SOL Port Configuration \n");
+	    }
     }
 
     memset(ConfigParams,0,sizeof(ConfigParams));
@@ -401,19 +408,21 @@
     {
         IPMI_DBG_PRINT("Configuration SERIAL_IFC_PORT is not found\n");
     }
-    strsize = strlen(str);
-    pBMCInfo->IpmiConfig.pSerialPort = malloc(strsize);
-    if(pBMCInfo->IpmiConfig.pSerialPort != NULL)
-    {
-        strncpy(pBMCInfo->IpmiConfig.pSerialPort,str,strsize);
-        pBMCInfo->IpmiConfig.pSerialPort[strsize]='\0';
-        IPMI_DBG_PRINT_1("Serial Port Configuration is %s \n",pBMCInfo->IpmiConfig.pSerialPort);
-    }
-    else
-    {
-        IPMI_ERROR("Error in Getting Serial Port Configuration \n");
-    }
-
+    else
+    {
+	    strsize = strlen(str);
+	    pBMCInfo->IpmiConfig.pSerialPort = malloc(strsize);
+	    if(pBMCInfo->IpmiConfig.pSerialPort != NULL)
+	    {
+	        strncpy(pBMCInfo->IpmiConfig.pSerialPort,str,strsize);
+	        pBMCInfo->IpmiConfig.pSerialPort[strsize]='\0';
+	        IPMI_DBG_PRINT_1("Serial Port Configuration is %s \n",pBMCInfo->IpmiConfig.pSerialPort);
+	    }
+	    else
+	    {
+	        IPMI_ERROR("Error in Getting Serial Port Configuration \n");
+	    }
+    }
     Notfound = 0xFFFF;
     memset(ConfigParams,0,sizeof(ConfigParams));
     sprintf(ConfigParams,"%s:TOTAL_MAX_SESSION",sectionname);
