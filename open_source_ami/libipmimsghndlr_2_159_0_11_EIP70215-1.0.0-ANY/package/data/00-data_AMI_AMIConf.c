--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Mon Sep 19 17:22:40 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Mon Sep 19 17:30:04 2011
@@ -1877,6 +1877,7 @@
              SensorInfo.DeassertionEventByte2    = (pSensor.DeassertionEventOccuredByte2 & pSensor.DeassertionEventEnablesByte2);
         }
         pSDRRec = GetSDRRec(pSensor.SDRRec->ID,BMCInst);
+        memset(SensorInfo.SensorName,0,MAX_ID_STR_LEN);
         if(pSensor.SDRRec->Type  == FULL_SDR_REC)       /*Full SDR*/
         {
             FullSDR = (_FAR_ FullSensorRec_T *)pSDRRec;
