--- .pristine/hdserverapp-1.17.0-src/data/hdserver.c	Fri May 10 15:06:16 2013
+++ source/hdserverapp-1.17.0-src/data/hdserver.c	Fri May 10 16:14:18 2013
@@ -1024,7 +1024,7 @@
 				gHdiskType[InstanceRcvd] = HDType;
 				if (InstanceRcvd >= gNumHdInstances)
 				{
-					TWARN ("HD/VF Instance %d is more than specified in the vmedia configuration !!!\n", InstanceRcvd);
+					TWARN ("HD Instance %d is more than specified in the vmedia configuration !!!\n", InstanceRcvd);
 					continue;
 				}
 				if (RunHdChild[InstanceRcvd] == 1)
@@ -1147,11 +1147,13 @@
 					printf ("InstanceRcvd = %d\n", InstanceRcvd);
 					printf ("HDType = 0x%02x\n", HDType);
 					gHdiskType[InstanceRcvd] = HDType;
-					if (InstanceRcvd != cfgVMedia.num_hd)
-					{
-						TWARN ("Invalid HD/VF Instance %d  for HD/VF redirection !!!\n", InstanceRcvd);
+
+					if (InstanceRcvd >= gNumHdInstances)
+					{
+						TWARN ("HD Instance %d is more than specified in the vmedia configuration !!!\n", InstanceRcvd);
 						continue;
 					}
+
 					if (RunHdChild[InstanceRcvd] == 1)
 					{
 						TWARN ("HD Instance %d is already active !!!\n", InstanceRcvd);
