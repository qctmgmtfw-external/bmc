--- .pristine/libvmedia-1.13.0-src/data/vmedia_instance.c Tue Sep 13 11:59:49 2011
+++ source/libvmedia-1.13.0-src/data/vmedia_instance.c Tue Sep 13 12:10:22 2011
@@ -153,7 +153,7 @@
 	}
 
 	// check if someone is using floppy, hdisk or vflash redirection
-	if( curr_token.floppy_in_use || curr_token.hdisk_in_use )
+	if( curr_token.floppy_in_use || curr_token.hdisk_in_use || curr_token.cdrom_in_use)
 	{	
 		// compare the session token with that of alredy existing token registered by floppy/hdisk/vflash user.
 		if( memcmp( token, curr_token.token, MAX_TOKEN_LEN) != 0 )
@@ -285,7 +285,7 @@
 	}
 
 	// check if someone is using cdrom,hdisk or vflash redirection
-	if( curr_token.cdrom_in_use || curr_token.hdisk_in_use )
+	if( curr_token.cdrom_in_use || curr_token.hdisk_in_use || curr_token.floppy_in_use  )
 	{	
 		// compare the session token with that of alredy existing token registered by cdrom user.
 		if( memcmp( token, curr_token.token, MAX_TOKEN_LEN) != 0 )
@@ -416,7 +416,7 @@
 	}
 
 	// check if someone is using cdrom,floppy or vflash redirection
-	if( curr_token.cdrom_in_use || curr_token.floppy_in_use )
+	if( curr_token.cdrom_in_use || curr_token.floppy_in_use || curr_token.hdisk_in_use )
 	{	
 		// compare the session token with that of alredy existing token registered by cdrom user.
 		if( memcmp( token, curr_token.token, MAX_TOKEN_LEN) != 0 )
