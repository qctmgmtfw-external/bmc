--- .pristine/fdserverapp-1.17.0-src/data/fdserver.c	Fri May 10 15:06:15 2013
+++ source/fdserverapp-1.17.0-src/data/fdserver.c	Fri May 10 16:14:38 2013
@@ -971,7 +971,7 @@
 
 				if (InstanceRcvd >= gNumFdInstances)
 				{
-					TWARN ("CD Instance %d is more than specified in the vmedia configuration !!!\n", InstanceRcvd);
+					TWARN ("FD Instance %d is more than specified in the vmedia configuration !!!\n", InstanceRcvd);
 					continue;
 				}
 				if (RunFdChild[InstanceRcvd] == 1)
@@ -1077,9 +1077,9 @@
 					Retval=RecvAuthFromRemote(temp, ipaddress, token, &InstanceRcvd);
 					printf ("InstanceRcvd = %d\n", InstanceRcvd);
 
-					if (InstanceRcvd != cfgVMedia.num_fd)
-					{
-						TWARN ("Invalid FD Instance %d for local FD Redirection !!!\n", InstanceRcvd);
+					if (InstanceRcvd >= gNumFdInstances)
+					{
+						TWARN ("FD Instance %d is more than specified in the vmedia configuration !!!\n", InstanceRcvd);
 						continue;
 					}
 					if (RunFdChild[InstanceRcvd] == 1)
