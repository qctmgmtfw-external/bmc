--- .pristine/libipmi-2.38.0-src/data/libipmi_fru.c Wed Sep 14 14:28:18 2011
+++ source/libipmi-2.38.0-src/data/libipmi_fru.c Wed Sep 14 15:27:27 2011
@@ -1156,6 +1156,8 @@
 {
     int i;
 
+    //Chassis type
+	SAFE_FREE(fru_data->ChassisInfo.ChassisType);
     //Chassis Part Num
     SAFE_FREE(fru_data->ChassisInfo.ChassisPartNum);
 
@@ -1169,6 +1171,9 @@
     }
     SAFE_FREE(fru_data->ChassisInfo.CustomFields);
 
+    //InternalInof Data
+    SAFE_FREE(fru_data->InternalInfo.Data);
+    
     // Board MfrName
     SAFE_FREE(fru_data->BoardInfo.BoardMfr);
 
