--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_network_imp.js Fri Feb  3 16:19:05 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_network_imp.js Tue Feb  7 15:43:18 2012
@@ -175,7 +175,7 @@
 		chkV4IPSource.checked = false;
 	}
 	loadV4Info(index);
-	doV4DHCP();
+	//doV4DHCP();
 
 	chkV6Enable.checked = LANCFG_DATA[index].v6Enable ? true : false;
 	if (getbits (LANCFG_DATA[index].v6IPSource,3,0) == 
@@ -186,12 +186,13 @@
 		chkV6IPSource.checked = false;
 	}
 	loadV6Info(index);
-	enableV6();
+	//enableV6();
 
 	chkVLANEnable.checked = (LANCFG_DATA[index].vlanEnable == 1) ? true : false;
 	txtVLANId.value = LANCFG_DATA[index].vlanID;
 	txtVLANPriority.value = LANCFG_DATA[index].vlanPriority;
-	enableVLAN();
+
+	enableLAN();
 }
 
 /*
