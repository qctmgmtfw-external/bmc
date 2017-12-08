--- .pristine/pam_ad-1.15.0-src/data/pam_ad.c Thu Sep 15 15:47:38 2011
+++ source/pam_ad-1.15.0-src/data/pam_ad.c Thu Sep 15 16:00:09 2011
@@ -73,7 +73,7 @@
 	char ip_addr[MAX_IP_LEN];
 	char username[USER_NAME_LEN];
 	char service[SERVICE_NAME_LEN],**grpname;
-        int index=0,i;
+    int grpcnt=0,i;
 
 	TDBG("PAM-ad:pam_sm_authenticate Called\n");
 
@@ -126,18 +126,17 @@
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
 
-        AddUsrtoGenericGrp((char *)user,GRP_AD,grpname,index);
+        //calculating group count
+        for(grpcnt=0;grpname[grpcnt]!= (char *) 0;grpcnt++);
+
+        AddUsrtoGenericGrp((char *)user,GRP_AD,grpname,grpcnt);
 
         FreeFullList(grpname);
 
