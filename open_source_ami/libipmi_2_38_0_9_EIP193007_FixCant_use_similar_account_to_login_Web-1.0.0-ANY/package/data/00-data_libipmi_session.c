--- .pristine/libipmi-2.38.0-src/data/libipmi_session.c	Thu Nov 20 15:40:52 2014
+++ source/libipmi-2.38.0-src/data/libipmi_session.c	Thu Nov 20 18:56:15 2014
@@ -709,7 +709,7 @@
 		retval = GetUserNameForUserID(pSession, Index, IPMI_Username, AuthEnable, timeout);
 		if(retval == LIBIPMI_E_SUCCESS)
 		{
-			if(memcmp(UserName, IPMI_Username, strlen(UserName)) == 0)
+			if(strncmp(UserName, IPMI_Username, MAX_USERNAME_LEN) == 0)
 			{
 				*UserID = Index;
 				break;
