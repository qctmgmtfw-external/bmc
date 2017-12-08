--- .pristine/libipmistack-2.52.0-src/data/BMCInfo.c Fri Sep 23 11:47:20 2011
+++ source/libipmistack-2.52.0-src/data/BMCInfo.c Fri Sep 23 12:54:21 2011
@@ -185,17 +185,6 @@
     {
         pBMCInfo = &g_BMCInfo[i+1];
         sectionname = iniparser_getsecname (d, i);
-        Notfound = 0xFFFF;
-        memset(ConfigParams,0,sizeof(ConfigParams));
-        sprintf(ConfigParams,"%s:Eth",sectionname);
-        str = iniparser_getstring(d,ConfigParams,NULL);
-        if(str == NULL)
-        {
-            IPMI_DBG_PRINT("EthIndex Not found\n");
-            iniparser_freedict(d); /* Freeing the dictionary  */
-            return -1;
-        }
-        strcpy(pBMCInfo->EthIndex,str);
 
         Notfound = 0xFFFF;
         memset(ConfigParams,0,sizeof(ConfigParams));
