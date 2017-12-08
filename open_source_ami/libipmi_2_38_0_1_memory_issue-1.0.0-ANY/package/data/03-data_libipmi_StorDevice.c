--- .pristine/libipmi-2.38.0-src/data/libipmi_StorDevice.c Wed Sep 14 14:28:18 2011
+++ source/libipmi-2.38.0-src/data/libipmi_StorDevice.c Wed Sep 14 15:26:48 2011
@@ -496,7 +496,7 @@
 			break;
 		}
 	}
-
+	free(GetSELResData);
 	/***********************************Get all Sel Entries now*********************************************/
 
 
