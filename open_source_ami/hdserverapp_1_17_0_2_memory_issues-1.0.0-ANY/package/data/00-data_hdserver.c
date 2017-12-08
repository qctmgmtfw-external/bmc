--- .pristine/hdserverapp-1.17.0-src/data/hdserver.c Wed Sep 14 14:30:10 2011
+++ source/hdserverapp-1.17.0-src/data/hdserver.c Wed Sep 14 16:58:28 2011
@@ -832,6 +832,8 @@
 	SetHDiskType (MEDIUM_REMOVABLE, InstanceIx);
 	ProcessDisconnect (InstanceIx);
 	pthread_cancel (ChildId[InstanceIx]);
+	//Joining the thread before exiting
+	pthread_join(ChildId[InstanceIx], NULL);
 	FiniStateMachine(InstanceIx);
 	CleanUpSession (InstanceIx);
 	HDConfigInProgress = 0;
