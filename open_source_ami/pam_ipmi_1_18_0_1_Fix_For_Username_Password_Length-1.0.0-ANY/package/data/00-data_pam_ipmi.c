--- .pristine/pam_ipmi-1.18.0-src/data/pam_ipmi.c Wed Sep 21 14:40:35 2011
+++ source/pam_ipmi-1.18.0-src/data/pam_ipmi.c Wed Sep 21 14:41:04 2011
@@ -94,6 +94,19 @@
 	{
 		TCRIT("Get no Password:");
 	}
+
+  if(strlen(username) > MAX_USER_NAME_LEN-1)
+  {
+        TCRIT("User Name is restricted to %d Bytes\n",MAX_USER_NAME_LEN-1);
+        pam_err = PAM_NEW_AUTHTOK_REQD;
+        goto pamerr;
+  }
+  if((password != NULL) && (strlen(password) > MAX_USER_PWD_LEN-1))
+  {
+        TCRIT("Password is restricted to %d Bytes\n",MAX_USER_PWD_LEN-1);
+        pam_err = PAM_NEW_AUTHTOK_REQD;
+        goto pamerr;
+  }
 	/*Set/Get MDCount Environment variable, to know the 
 	Number of modules it traversed*/
 	GetModuleCountEnv (pamh, &cntr);
