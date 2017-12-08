--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/PEFDevice/PEFDevice.c Fri Aug 24 11:18:47 2012
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/PEFDevice/PEFDevice.c Fri Aug 24 15:37:30 2012
@@ -178,12 +178,9 @@
         case ARM_PEF_TEMP_DISABLE:
             IPMI_DBG_PRINT ("ARM_PEF_TMR_DISABLE\n");
             /* Disable PEF Temporarily */
-            if ((pNVRPEFConfig->PEFControl & 0x01) != 0) //PEF Enabled in PEF Configuration Parameters originally
-            {
-            pNVRPEFConfig->PEFControl  &= ~0x01;
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrInterval   = ARM_PEF_TEMP_DISABLE;
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.InitCountDown = ARM_PEF_TEMP_DISABLE;
-            }
+
             // Timer Interval and InitCountdown not set to ARM_PEF_TEMP_DISABLE if originally PEF was disabled
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrArmed      = FALSE;
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.StartTmr      = FALSE;
@@ -195,7 +192,6 @@
             /* Check if PEF Temporary Disable was called earlier and then Disable the PEFTmr */
             if (g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrInterval == ARM_PEF_TEMP_DISABLE)
             {
-            pNVRPEFConfig->PEFControl  |= 0x01;
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TakePEFAction   = TRUE;
             }
             else
@@ -215,10 +211,6 @@
             /* Other values correspond to arm timer */
             IPMI_DBG_PRINT ("DEFAULT\n");
             /* Check if PEF Temporary Disable was called earlier and then set the PEFTmr */
-            if (g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrInterval == ARM_PEF_TEMP_DISABLE)
-            {
-            pNVRPEFConfig->PEFControl  |= 0x01;
-            }
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.InitCountDown   = *pReq;
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrInterval     = *pReq;
             g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrArmed        = TRUE;
