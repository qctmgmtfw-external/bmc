--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Thu Feb 23 11:46:25 2012
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Mon Mar  5 18:45:20 2012
@@ -103,7 +103,7 @@
  */
 static const INT8U NwIfcStateConfigParamLenght [] = {
                             2, /*Eth Interface*/
-                            6, /*Bond Interface*/
+                            7, /*Bond Interface*/
 };
 
 /*
@@ -1024,12 +1024,13 @@
 
             EthIndex = pAMIIfaceStateReq->ConfigData.BondIface.BondIndex;
 
-            TDBG("Enable:%d\nBondIndex:%d\nBondMode:%d\nmiiinterval:%d\nSlave:%d\n",
+            TDBG("Enable:%d\nBondIndex:%d\nBondMode:%d\nmiiinterval:%d\nSlave:%d\nAutoConf:%d\n",
                                                             pAMIIfaceStateReq->ConfigData.BondIface.Enable,
                                                             pAMIIfaceStateReq->ConfigData.BondIface.BondIndex,
                                                             pAMIIfaceStateReq->ConfigData.BondIface.BondMode,
                                                             pAMIIfaceStateReq->ConfigData.BondIface.MiiInterval,
-                                                            pAMIIfaceStateReq->ConfigData.BondIface.Slaves);
+                                                            pAMIIfaceStateReq->ConfigData.BondIface.Slaves,
+                                                            pAMIIfaceStateReq->ConfigData.BondIface.AutoConf);
             /*Validate the bonding mode*/
             if( pAMIIfaceStateReq->ConfigData.BondIface.BondMode > MAX_BOND_MODE)
             {
@@ -1280,6 +1281,7 @@
             pAMIIfaceStateRes->ConfigData.BondIface.BondMode = bond.BondMode;
             pAMIIfaceStateRes->ConfigData.BondIface.MiiInterval = bond.MiiInterval;
             pAMIIfaceStateRes->ConfigData.BondIface.Slaves = bond.Slaves;
+            pAMIIfaceStateRes->ConfigData.BondIface.AutoConf = bond.AutoConf;
             return sizeof(INT8U) +sizeof(BondIface);
 #else
             *pRes = CC_PARAM_NOT_SUPPORTED;
