--- .pristine/libuserauth-1.27.0-src/data/user_auth.c Tue Nov  6 02:42:29 2012
+++ source/libuserauth-1.27.0-src/data/user_auth.c Thu Nov  8 00:05:47 2012
@@ -391,7 +391,7 @@
 {
     int priv;
     GetUsrPriv( username, &priv);
-	switch(priv & 0x07 ) // we shall check with last three bits to identify the privilege
+	switch(priv & 0x0F ) // we shall check with last four bits to identify the privilege
 		{
 		case PRIV_LEVEL_PROPRIETARY:  
 			(*userpriv) = PRIV_LEVEL_ADMIN;//OEM privilege considered as Admin priv.  
