--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Wed Sep  7 20:06:57 2011
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Wed Sep  7 20:08:04 2011
@@ -509,7 +509,7 @@
         sprintf(FullEth_Network_if,"%s",m_VLANInterface);
     }
 
-    strcpy(ifr.ifr_name, FullEth_Network_if);
+    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
 
     ifr.ifr_hwaddr.sa_family = AF_INET;
     r = ioctl(skfd, SIOCGIFHWADDR, &ifr);
@@ -536,7 +536,7 @@
         sprintf(FullEth_Network_if,"%s",m_VLANInterface);
     }
 */
-    strcpy(ifr.ifr_name, FullEth_Network_if);
+    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
 
     info = (struct sockaddr_in *) &(ifr.ifr_addr);
     info->sin_family = AF_INET;
@@ -567,7 +567,7 @@
     {
         sprintf(FullEth_Network_if,"%s",m_VLANInterface);
 */
-    strcpy(ifr.ifr_name, FullEth_Network_if);
+    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
     info = (struct sockaddr_in *) &(ifr.ifr_broadaddr);
     info->sin_family = AF_INET;
     r = ioctl(skfd, SIOCGIFBRDADDR, &ifr);
@@ -593,7 +593,7 @@
         sprintf(FullEth_Network_if,"%s",m_VLANInterface);
     }
 */
-    strcpy(ifr.ifr_name, FullEth_Network_if);
+    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
 
     info = (struct sockaddr_in *) &(ifr.ifr_netmask);
     info->sin_family = AF_INET;
@@ -1024,6 +1024,11 @@
 	char oneline[80];
 	fpdev = fopen(DEV_FILE, "r");
 
+        if(fpdev == NULL)
+        {
+            TDBG("Error in opening network device file\n");
+            return -1;
+        }
 	memset(cfg,0,sizeof(NWCFGS));
 
 	while(!feof(fpdev))
@@ -2305,9 +2310,10 @@
     }
 
     memset(tmp, 0, MAC_ADDR_LEN);
+    memset(&ifr,0,sizeof(ifr));
     memset(FullEth_Network_if,0,MAX_STR_LENGTH);
     sprintf(FullEth_Network_if,"%s%d",Eth_NetWork_If,0);
-    strcpy(ifr.ifr_name, FullEth_Network_if);
+    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ -1);
     r = ioctl(skfd, SIOCGIFHWADDR, &ifr);
     if ( r < 0 )
     {
@@ -2426,8 +2432,9 @@
 
     memset(tmp, 0, MAC_ADDR_LEN);
     memset(FullEth_Network_if,0,MAX_STR_LENGTH);
+    memset(&ifr,0,sizeof(ifr));
     sprintf(FullEth_Network_if,"%s%d",Eth_NetWork_If,0);
-    strcpy(ifr.ifr_name, FullEth_Network_if);
+    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
     r = ioctl(skfd, SIOCGIFHWADDR, &ifr);
     if ( r < 0 )
     {
@@ -4379,8 +4386,14 @@
 
        if(retry == 2)
        {
-    	   PrevMac= malloc(MAC_ADDR_LEN);
-    	   memcpy(PrevMac,p,MAC_ADDR_LEN);
+            PrevMac= malloc(MAC_ADDR_LEN);
+            if(PrevMac == NULL)
+            {
+                TCRIT("Error in Allocating memory\n");
+                close(s);
+                return -1;
+            }
+           memcpy(PrevMac,p,MAC_ADDR_LEN);
        }
        retry--;
     }
