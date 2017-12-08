--- .pristine/libipmi-2.38.0-src/data/libipmi_sensor.c Tue Jan 10 15:30:22 2012
+++ source/libipmi-2.38.0-src/data/libipmi_sensor.c Tue Jan 10 16:35:32 2012
@@ -373,6 +373,9 @@
 
 /* ------------------ High level functions ------------------ */
 #define SDR_FILE_PATH   "/tmp/sdr_data"
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER 
+#define NM_FILE_PATH   "/tmp/nm_data" 
+#endif
 
 LIBIPMI_API uint16
 LIBIPMI_HL_ReadSensorFromSDR( IPMI20_SESSION_T *pSession, uint8 *sdr_buffer,
@@ -393,6 +396,26 @@
     *discrete = (u8)0;
 
     record_type = header->Type;
+
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER 
+		uint8 nm_slave_addr = 0; 
+		FILE *nmfile; 
+		
+		/* Open NM file */ 
+		nmfile = fopen( NM_FILE_PATH, "r" ); 
+		if(nmfile != NULL) 
+		{ 
+			/* read NM slave address*/ 
+			fscanf(nmfile, "%hhx", &nm_slave_addr); 
+			fclose( nmfile ); 
+		} 
+		else 
+		{ 
+			TWARN( "Cannot open NM file: %s\n", strerror( errno ) ); 
+		} 
+				 
+#endif 
+
 	if( record_type == 0x01 )
     {
         FullSensorRec_T *record;
@@ -401,7 +424,11 @@
         record = (FullSensorRec_T *)sdr_buffer;
         if( record->EventTypeCode > 0x01 )
             *discrete = record->EventTypeCode;
-        if(record->OwnerID != 0x20)
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+		if(record->OwnerID != 0x20 && record->OwnerID !=  nm_slave_addr)
+#else
+		if(record->OwnerID != 0x20)
+#endif
         {
             pAMIGetChNumReq.SensorOwnerID = record->OwnerID;
             wRet = IPMICMD_AMIGetChNum (pSession,&pAMIGetChNumReq,&pAMIGetChNumRes,
@@ -427,7 +454,11 @@
         record = (CompactSensorRec_T *)sdr_buffer;
         if( record->EventTypeCode > 0x01 )
             *discrete = record->EventTypeCode;
-        if(record->OwnerID != 0x20)
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+		if(record->OwnerID != 0x20 && record->OwnerID !=  nm_slave_addr)
+#else
+		if(record->OwnerID != 0x20)
+#endif
         {
             pAMIGetChNumReq.SensorOwnerID = record->OwnerID;
             wRet = IPMICMD_AMIGetChNum (pSession,&pAMIGetChNumReq,&pAMIGetChNumRes,
