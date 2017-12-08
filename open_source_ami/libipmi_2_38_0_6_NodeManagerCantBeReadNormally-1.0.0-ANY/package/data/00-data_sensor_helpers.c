--- .pristine/libipmi-2.38.0-src/data/sensor_helpers.c Tue Jan 10 15:30:22 2012
+++ source/libipmi-2.38.0-src/data/sensor_helpers.c Tue Jan 10 16:39:04 2012
@@ -643,6 +643,10 @@
 	return( 0 );
 }
 
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER 
+#define OEM_SDR_NM_REC  0x0D 
+#define NM_FILE_PATH   "/tmp/nm_data" 
+#endif 
 
 int read_sensor_sdrs( IPMI20_SESSION_T *pSession, struct sensor_info **sensor_list,
                       int timeout )
@@ -702,6 +706,43 @@
 
         header = (SDRRecHdr_T *)( sdr_buffer + ( i * max_sdr_len ) );
 
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER 
+		if(OEM_SDRNM_REC == header->Type) 
+		{  
+			OEM_NMRec_T* sonm; 
+			sonm =(OEM_NMRec_T*) ( (UINT8*)header ); 
+			FILE *nmfile; 
+			if(sonm->RecordSubType == OEM_SDR_NM_REC)
+			{ 
+				/* Open the NM file to check file exit*/ 
+				nmfile = fopen( NM_FILE_PATH, "r" ); 
+				if( nmfile != NULL ) 
+					fclose( nmfile ); 
+				else				 
+				{ 
+					/* If nm file does not exist...  */ 
+					if( errno == ENOENT ) 
+					{ 
+						/* ...create it */ 
+						nmfile = fopen( NM_FILE_PATH, "w" ); 		 
+						/* Write the NM slave address to file */ 
+						if( fprintf(nmfile,"%02x", sonm->NMDevSlaveAddress) < 0 ) 
+						{ 
+							TWARN( "Cannot write to NM file: %s\n", strerror( errno ) ); 
+							fclose( nmfile ); 
+							return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) ); 					   
+						} 
+								fclose( nmfile ); 
+						} 
+						else 
+						{	 
+							TWARN( "Cannot write to NM file: %s\n", strerror( errno ) ); 
+							return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) ); 
+					}	 
+				}				
+			} 
+		} 
+#endif 
         /* If the SDR is for a sensor... */
         if( ( header->Type == FULL_SDR_REC ) ||
             ( header->Type == COMPACT_SDR_REC ) )
