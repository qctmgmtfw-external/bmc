--- .pristine/ipmimain-2.61.0-src/data/IPMI_Main.c	Thu Aug 14 14:33:33 2014
+++ source/ipmimain-2.61.0-src/data/IPMI_Main.c	Thu Aug 14 14:59:32 2014
@@ -80,6 +80,16 @@
             {
                 close(g_BMCInfo[k].LANConfig.UDPSocket[j]);
             }
+            /*Close the VLAN Sockets*/
+            if(-1 != g_BMCInfo[k].LANConfig.VLANUDPSocket[j])
+            {
+                close(g_BMCInfo[k].LANConfig.VLANUDPSocket[j]);
+            }
+            if(-1 != g_BMCInfo[k].LANConfig.VLANTCPSocket[j])
+            {
+                shutdown(g_BMCInfo[k].LANConfig.VLANTCPSocket[j],SHUT_RDWR);
+                close(g_BMCInfo[k].LANConfig.VLANTCPSocket[j]);
+            }
         }
     }
 
