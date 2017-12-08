--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/PEFDevice/PEFTmr.c Fri Aug 24 11:18:48 2012
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/PEFDevice/PEFTmr.c Fri Aug 24 15:38:01 2012
@@ -61,11 +61,6 @@
         (sysPwrState == IPMI_ACPI_SLEEPING_S1_S3) || (sysPwrState == IPMI_ACPI_G1_SLEPPING_S1_S4) ||
         (sysPwrState == IPMI_ACPI_G3))||(pBMCInfo->PefConfig.PEFTmrMgr.PEFTimerResetFlag == 1))
     {
-        if (g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrInterval == ARM_PEF_TEMP_DISABLE)
-        {
-            pNVRPEFConfig->PEFControl  |= 0x01;
-        }
-
         pBMCInfo->PefConfig.PEFTmrMgr.TmrArmed = FALSE;
         pBMCInfo->PefConfig.PEFTmrMgr.StartTmr = FALSE;
         pBMCInfo->PefConfig.PEFTmrMgr.InitCountDown = 0;
