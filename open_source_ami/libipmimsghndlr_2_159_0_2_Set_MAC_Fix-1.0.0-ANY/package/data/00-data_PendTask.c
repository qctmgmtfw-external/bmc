--- .pristine/libipmimsghndlr-2.159.0-src/data/PendTask.c Wed Sep  7 19:50:20 2011
+++ source/libipmimsghndlr-2.159.0-src/data/PendTask.c Wed Sep  7 19:51:23 2011
@@ -1083,6 +1083,15 @@
 
 	/* --------------------- End of U-Boot section --------------------------*/
 
+        /*If the bond interface is enabled, reboot is needed*/
+        if(CheckBondSlave(data->EthIndex) == 1)
+        {
+            TDBG("Given index is slave of bond interface\n");
+            SetPendStatus(PEND_OP_SET_MAC_ADDRESS, PEND_STATUS_COMPLETED);
+            reboot(LINUX_REBOOT_CMD_RESTART);
+            return 0;
+        }
+
 	/* ------- This section sets the MAC Address Temporarily in Linux environment --------- */
 	/* ------- Upon next reboot, the new mac will take effect permanently 	      --------- */
 
