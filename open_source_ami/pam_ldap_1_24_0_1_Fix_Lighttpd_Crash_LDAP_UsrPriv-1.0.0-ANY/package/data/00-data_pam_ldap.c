--- .pristine/pam_ldap-1.24.0-src/data/pam_ldap.c Thu Sep 15 13:14:07 2011
+++ source/pam_ldap-1.24.0-src/data/pam_ldap.c Thu Sep 15 16:00:20 2011
@@ -66,7 +66,7 @@
 	char ip_addr[MAX_IP_LEN];
 	char username[USER_NAME_LEN];
 	char service[SERVICE_NAME_LEN],**grpname;
-        int index=0,i;
+        int grpcnt,i;
 
 	TDBG("PAM-ldap:pam_sm_authenticate Called\n");
 
@@ -127,18 +127,17 @@
             if(privilege == (unsigned int)g_Grpsprv[i])
             {
                 grpname=AddToList(grpname, Lan1ChGrps[i]);
-                index++;
                 grpname=AddToList(grpname, Lan2ChGrps[i]);
-                index++;
                 grpname=AddToList(grpname, Lan3ChGrps[i]);
-                index++;
                 grpname=AddToList(grpname, SerialChGrps[i]);
-                index++;
                 break;
             }
         }
 
-        AddUsrtoGenericGrp((char *)user,GRP_LDAP,grpname,index);
+        //calculating group count
+        for(grpcnt=0;grpname[grpcnt]!= (char *) 0;grpcnt++);
+
+        AddUsrtoGenericGrp((char *)user,GRP_LDAP,grpname,grpcnt);
 
         FreeFullList(grpname);
 
