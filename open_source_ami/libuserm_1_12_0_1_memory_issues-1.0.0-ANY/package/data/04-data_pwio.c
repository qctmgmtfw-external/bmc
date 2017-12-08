--- .pristine/libuserm-1.12.0-src/data/pwio.c Wed Sep 14 14:28:24 2011
+++ source/libuserm-1.12.0-src/data/pwio.c Wed Sep 14 15:42:01 2011
@@ -40,25 +40,28 @@
     
         
 	if (!(pw->pw_name = strdup(pwent->pw_name)))
-		return NULL;
+		goto error_out;
         
 	if (!(pw->pw_passwd = strdup(pwent->pw_passwd)))
-		return NULL;
+		goto error_out;
 #ifdef ATT_AGE
 	if (!(pw->pw_age = strdup(pwent->pw_age)))
-		return NULL;
+		goto error_out;
 #endif
 #ifdef ATT_COMMENT
 	if (!(pw->pw_comment = strdup(pwent->pw_comment)))
-		return NULL;
+		goto error_out;
 #endif
 	if (!(pw->pw_gecos = strdup(pwent->pw_gecos)))
-		return NULL;
+		goto error_out;
 	if (!(pw->pw_dir = strdup(pwent->pw_dir)))
-		return NULL;
+		goto error_out;
 	if (!(pw->pw_shell = strdup(pwent->pw_shell)))
-		return NULL;
+		goto error_out;
 	return pw;
+error_out:
+	free(pw);
+	return NULL;
 }
 
 static void *
