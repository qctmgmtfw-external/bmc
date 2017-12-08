--- .pristine/cdserverapp-1.16.0-src/data/cdserver.c	Fri May 10 15:06:13 2013
+++ source/cdserverapp-1.16.0-src/data/cdserver.c	Fri May 10 16:11:41 2013
@@ -1099,11 +1099,12 @@
 					Retval=RecvAuthFromRemote(temp, ipaddress, token, &InstanceRcvd);
 					printf ("InstanceRcvd = %d\n", InstanceRcvd);
 
-					if (InstanceRcvd != cfgVMedia.num_cd)
-					{
-						TWARN ("Invalid CD Instance %d for local CD Redirection !!!\n", InstanceRcvd);
+					if (InstanceRcvd >= gNumCdInstances)
+					{
+						TWARN ("CD Instance %d is more than specified in the vmedia configuration !!!\n", InstanceRcvd);
 						continue;
 					}
+					
 					if (RunCdChild[InstanceRcvd] == 1)
 					{
 						TWARN ("CD Instance %d is already active !!!\n", InstanceRcvd);
