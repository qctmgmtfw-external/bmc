--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/PEFDevice/PEFTmr.c Thu Apr 26 21:17:10 2012
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/PEFDevice/PEFTmr.c Thu Apr 26 21:38:14 2012
@@ -56,10 +56,10 @@
     UNLOCK_BMC_SHARED_MEM(BMCInst);
 
     /* Disable and reset PEF timer whenever System enters a sleep state / is powered down / is reset*/
-    if ((sysPwrState == IPMI_ACPI_S1) || (sysPwrState == IPMI_ACPI_S2) || (sysPwrState == IPMI_ACPI_S3) ||
+    if(((sysPwrState == IPMI_ACPI_S1) || (sysPwrState == IPMI_ACPI_S2) || (sysPwrState == IPMI_ACPI_S3) ||
         (sysPwrState == IPMI_ACPI_S4) || (sysPwrState == IPMI_ACPI_S5) || (sysPwrState == IPMI_ACPI_S4_S5) ||
         (sysPwrState == IPMI_ACPI_SLEEPING_S1_S3) || (sysPwrState == IPMI_ACPI_G1_SLEPPING_S1_S4) ||
-        (sysPwrState == IPMI_ACPI_G3))
+        (sysPwrState == IPMI_ACPI_G3))||(pBMCInfo->PefConfig.PEFTmrMgr.PEFTimerResetFlag == 1))
     {
         if (g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrInterval == ARM_PEF_TEMP_DISABLE)
         {
@@ -71,6 +71,7 @@
         pBMCInfo->PefConfig.PEFTmrMgr.InitCountDown = 0;
         pBMCInfo->PefConfig.PEFTmrMgr.TmrInterval = 0;
         pBMCInfo->PefConfig.PEFTmrMgr.TakePEFAction = TRUE;
+        pBMCInfo->PefConfig.PEFTmrMgr.PEFTimerResetFlag = 0;
         return;
     }
 
@@ -195,13 +196,13 @@
     sysPwrState = BMC_GET_SHARED_MEM(BMCInst)->m_ACPISysPwrState & ACPI_SYS_PWR_STATE_MASK;
     UNLOCK_BMC_SHARED_MEM(BMCInst);
       
-    if(((sysPwrState == IPMI_ACPI_S0) && ((PresysPwrState == IPMI_ACPI_S4)||(PresysPwrState == IPMI_ACPI_S5)))||(pBMCInfo->PefConfig.PEFTmrMgr.ResetFlag == 1))
+    if(((sysPwrState == IPMI_ACPI_S0) && ((PresysPwrState == IPMI_ACPI_S4)||(PresysPwrState == IPMI_ACPI_S5)))||(pBMCInfo->PefConfig.PEFTmrMgr.StartDlyResetFlag == 1))
     {
         EnableCountStartupDly = TRUE;
         EnableCountAlertStartupDly = TRUE;
         pBMCInfo->PefConfig.PEFTmrMgr.StartDlyTmr = pPEFConfig->PEFStartupDly;
         pBMCInfo->PefConfig.PEFTmrMgr.AlertStartDlyTmr = pPEFConfig->PEFAlertStartupDly;
-        pBMCInfo->PefConfig.PEFTmrMgr.ResetFlag = 0;
+        pBMCInfo->PefConfig.PEFTmrMgr.StartDlyResetFlag = 0;
     }
     
     if(EnableCountStartupDly == TRUE && pBMCInfo->PefConfig.PEFTmrMgr.StartDlyTmr > 0)
