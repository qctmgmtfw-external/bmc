--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMIDevice.h Thu Sep 22 14:03:19 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMIDevice.h Thu Sep 22 16:01:13 2011
@@ -462,7 +462,8 @@
 		INT16U		ModuleFlags;
 		uint32		ModuleLoadAddress;
 		uint32		ModuleChecksum;
-		INT8U		ModuleReserved[8];
+		INT8U		ModuleAuxVer[2];
+		INT8U		ModuleReserved[6];
 		INT16U		EndSignature;
 	}PACKED  FlashMH;
 
