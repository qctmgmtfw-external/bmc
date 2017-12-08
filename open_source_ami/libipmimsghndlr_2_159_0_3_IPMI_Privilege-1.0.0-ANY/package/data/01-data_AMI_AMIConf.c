--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Wed Sep  7 19:52:24 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Wed Sep  7 19:59:54 2011
@@ -694,8 +694,8 @@
     char IfcName[16],tmpstr[16];
     int EthIndex = 0,i,ethcount=0;
     INT8U CurrentIfaceState = 0x00;
-     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
 #ifdef CONFIG_SPX_FEATURE_BOND_SUPPORT
+    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
     BondIface   bond;
     int Ifccount = 0;
     INT8U slave;
@@ -724,23 +724,6 @@
     switch(pAMIIfaceStateReq->Params)
     {
         case AMI_IFACE_STATE_ETH:
-            memset(IfcName,0,sizeof(IfcName));
-            sprintf(IfcName,"bond0");
-            for(i=0;i<sizeof(pBMCInfo->LANConfig.LanIfcConfig)/sizeof(LANIFCConfig_T);i++)
-            {
-                if((strcmp(IfcName,pBMCInfo->LANConfig.LanIfcConfig[i].ifname) == 0 ) && 
-                    (pBMCInfo->LANConfig.LanIfcConfig[i].Up_Status == 1))
-                {
-                    break;
-                }
-            }
-
-            /*Bond is Enabled*/
-            if(i != sizeof(pBMCInfo->LANConfig.LanIfcConfig)/sizeof(LANIFCConfig_T))
-            {
-                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
-                return sizeof(INT8U);
-            }
 
             if(pAMIIfaceStateReq->ConfigData.EthIface.EthIndex == 0xff)
             {
@@ -760,11 +743,25 @@
                 return sizeof(INT8U);
             }
 
-            sprintf(tmpstr,"bond0");
-            if(strcmp(tmpstr,IfcName) == 0)
+            sprintf(tmpstr,"bond");
+            if(strstr(IfcName,tmpstr) != NULL)
             {
                 IPMI_WARNING("Given EthIndex %d  is specific to bonding interface. Use specific parameter for bonding\n",EthIndex);
                 *pRes = OEMCC_INV_PARAM_ONLY_FOR_NON_BONDING;
+                return sizeof(INT8U);
+            }
+
+            /*Check if the given Interface is the slave of any Bond interfaces*/
+            retValue= CheckBondSlave(pAMIIfaceStateReq->ConfigData.EthIface.EthIndex);
+            if(retValue == -1)
+            {
+                *pRes = CC_UNSPECIFIED_ERR;
+                return sizeof(INT8U);
+            }
+
+            if(retValue == 1)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
                 return sizeof(INT8U);
             }
 
