--- .pristine/libipmimsghndlr-2.159.0-src/data/Storage/SDRDevice/SDR.c Wed Dec 21 21:08:57 2011
+++ source/libipmimsghndlr-2.159.0-src/data/Storage/SDRDevice/SDR.c Wed Dec 21 21:44:13 2011
@@ -81,6 +81,8 @@
 #define ENABLE_SELECTED_EVENT_MSG   0x10
 #define DISABLE_SELECTED_EVENT_MSG  0x20
 
+#define OEM_SDR_NM_REC             0x0D
+
 /**
  * @brief Size of SDR Record
 **/
@@ -647,6 +649,17 @@
             ((void(*)(INT8U *,int))g_PDKHandle[PDK_PROCESSOEMRECORD]) ((INT8U*)&OEM_Recdata,BMCInst);
         }
       }
+      #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+      if(OEM_SDRNM_REC == pSDRRecord->Type)
+      {
+         OEM_NMRec_T* sonm;
+         sonm =(OEM_NMRec_T*) ( (UINT8*)pSDRRecord );
+
+         if(sonm->RecordSubType == OEM_SDR_NM_REC)
+           pBMCInfo->NMConfig.NMDevSlaveAddress = sonm->NMDevSlaveAddress;
+      }
+      #endif
+
      pSDRRecord = SDR_GetNextSDRRec (pSDRRecord,BMCInst);
    }
     
