--- .pristine/libipmitimer-2.8.0-src/data/Timer.c Fri Mar 15 14:47:15 2013
+++ source/libipmitimer-2.8.0-src/data/Timer.c Fri Mar 15 14:57:10 2013
@@ -107,12 +107,6 @@
 
         }*/
         
-        if(BMC_GET_SHARED_MEM(BMCInst)->SysRestartCaused == TRUE)
-        {
-            BootValidTimerTick = 0;
-            BMC_GET_SHARED_MEM(BMCInst)->SysRestartCaused = FALSE;
-        }
-
         if((sBootOptions->BootFlagValidBitClearing & SIXTYSECOND_OVERRIDE) != SIXTYSECOND_OVERRIDE)
         {
             if(((sBootOptions->BootFlags.BootFlagsValid & BOOT_FLAGS_VALID_SET) == BOOT_FLAGS_VALID_SET) 
@@ -126,6 +120,7 @@
         if ((sBootOptions->BootFlags.BootFlagsValid & BOOT_FLAGS_VALID_SET) == 0x00)  
         {
             BootValidTimerTick = 0;
+	    BMC_GET_SHARED_MEM(BMCInst)->SysRestartCaused = FALSE;
         }
 
         if(0 == (CurTimerTick % POWER_STATE_UPDATE_CNT))
