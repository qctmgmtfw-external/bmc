--- .pristine/fdserverapp-1.17.0-src/data/fdserver.c Wed Sep 14 14:30:09 2011
+++ source/fdserverapp-1.17.0-src/data/fdserver.c Wed Sep 14 16:58:03 2011
@@ -796,6 +796,8 @@
 	}
 	ProcessDisconnect (InstanceIx);
 	pthread_cancel (ChildId[InstanceIx]);	
+	//Joining the thread before exiting
+	pthread_join(ChildId[InstanceIx], NULL);
 	/* If Not a proper disconnect, we still have a pending Response */
 	if (gMachineState[InstanceIx] == STATE_WFRR)
 	{
