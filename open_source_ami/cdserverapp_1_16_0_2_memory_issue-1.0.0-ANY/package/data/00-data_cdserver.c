--- .pristine/cdserverapp-1.16.0-src/data/cdserver.c Wed Sep 14 14:30:09 2011
+++ source/cdserverapp-1.16.0-src/data/cdserver.c Wed Sep 14 16:57:33 2011
@@ -818,6 +818,8 @@
 	}
 	ProcessDisconnect (InstanceIx);
 	pthread_cancel (ChildId[InstanceIx]);
+	//joining the pthread until thread exit wait
+	pthread_join(ChildId[InstanceIx], NULL);
 	/* If Not a proper disconnect, we still have a pending Response */
 	if (gMachineState[InstanceIx] == STATE_WFRR)
 	{
