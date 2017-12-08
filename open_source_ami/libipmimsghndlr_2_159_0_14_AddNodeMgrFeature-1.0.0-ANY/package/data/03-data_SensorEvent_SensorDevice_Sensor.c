--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/Sensor.c Wed Dec 21 21:08:58 2011
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/Sensor.c Wed Dec 21 22:01:10 2011
@@ -176,7 +176,6 @@
     _FAR_ CompactSensorRec_T*   scs;
     _FAR_   FRUDevLocatorRec_T *sfr;
     _FAR_ OEM_FRURec_T*         sof;
-    _FAR_ OEM_NMRec_T*         sonm;
     INT16U 	SharedRecs = 0;
     int		i = 0,len=0,fruvalid=0;
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
@@ -220,8 +219,11 @@
         if (sr->Type == FULL_SDR_REC)
         {
             sfs = (_FAR_ FullSensorRec_T*) sr;
-
+          #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+            if ((sfs->OwnerID == pBMCInfo->IpmiConfig.BMCSlaveAddr)||(sfs->OwnerID == pBMCInfo->NMConfig.NMDevSlaveAddress))
+          #else
             if (sfs->OwnerID == pBMCInfo->IpmiConfig.BMCSlaveAddr)
+          #endif
             {
                 pSenSharedMem->SensorInfo[sfs->SensorNum].IsSensorPresent = TRUE;
                 SR_LoadSDRDefaults (sr, (SensorInfo_T*)&pSenSharedMem->SensorInfo[sfs->SensorNum],BMCInst);
@@ -249,7 +251,11 @@
         {
             scs = (_FAR_ CompactSensorRec_T*) sr;
 
-            if (scs->OwnerID == pBMCInfo->IpmiConfig.BMCSlaveAddr)
+          #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+            if ((scs->OwnerID == pBMCInfo->IpmiConfig.BMCSlaveAddr)||(scs->OwnerID == pBMCInfo->NMConfig.NMDevSlaveAddress))
+          #else
+            if(scs->OwnerID == pBMCInfo->IpmiConfig.BMCSlaveAddr)
+          #endif
             {
                 pSenSharedMem->SensorInfo[scs->SensorNum].IsSensorPresent = TRUE;
                 SR_LoadSDRDefaults (sr, (SensorInfo_T*)&pSenSharedMem->SensorInfo[scs->SensorNum],BMCInst);
@@ -370,13 +376,7 @@
                 fruvalid = 0;
             }
         }
-        else if (sr->Type == OEM_SDRNM_REC)
-        {
-          sonm = (_FAR_ OEM_NMRec_T*)sr;
-
-          if(sonm->RecordSubType == OEM_SDR_NM_REC)
-            pBMCInfo->NMConfig.NMDevSlaveAddress = sonm->NMDevSlaveAddress; 
-        }
+
         /* Get the next record */
         sr = SDR_GetNextSDRRec (sr,BMCInst);
     }
@@ -416,13 +416,16 @@
         {
             sfs = (_FAR_ FullSensorRec_T*) sr;
 
-            if ((sfs->SensorNum == SensorNum) && (pBMCInfo->IpmiConfig.BMCSlaveAddr == sfs->OwnerID)){ return sr; }
+            if ((sfs->SensorNum == SensorNum) && ((pBMCInfo->IpmiConfig.BMCSlaveAddr == sfs->OwnerID) || (pBMCInfo->NMConfig.NMDevSlaveAddress == sfs->OwnerID))  ){ return sr; }
         }
         else if (sr->Type == COMPACT_SDR_REC)
         {
             scs = (_FAR_ CompactSensorRec_T*) sr;
-
-            if (pBMCInfo->IpmiConfig.BMCSlaveAddr == scs->OwnerID)
+          #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+            if ((pBMCInfo->IpmiConfig.BMCSlaveAddr == scs->OwnerID)||(pBMCInfo->NMConfig.NMDevSlaveAddress == scs->OwnerID))
+          #else
+            if(pBMCInfo->IpmiConfig.BMCSlaveAddr == scs->OwnerID)
+          #endif
             {
                 SharedRecs = ipmitoh_u16 (((_FAR_ CompactSensorRec_T*) sr)->RecordSharing) &
                 SHARED_RECD_COUNT;
