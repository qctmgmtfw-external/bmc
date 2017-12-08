--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Thu Jan 12 11:49:47 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Mon Jan 16 17:43:13 2012
@@ -342,16 +342,21 @@
 /************************************************************************
 function : isFamilyEnabled
 brief : checks for the family type (i.e. AF_INET or AF_INET6) availability from the interfaces file.
+param : IfaceFileAbsPath : interface file's absolute path
 param : ifname : interface name to be tested.
 param : family : family type to be checked.
 ************************************************************************/
-static int isFamilyEnabled(char *ifname, int family)
+static int isFamilyEnabled(char *IfaceFileAbsPath, char *ifname, int family)
 {
     FILE*   fpinterfaces;
     int     ipfound = 0;
     char    oneline [100];
 
-    fpinterfaces = fopen(NETWORK_IF_FILE,"r");
+    if(IfaceFileAbsPath == NULL)
+        fpinterfaces = fopen(NETWORK_IF_FILE,"r");
+    else
+        fpinterfaces = fopen(IfaceFileAbsPath,"r");
+            
     if (fpinterfaces == NULL)
     {
         //no file just return NULL entires
@@ -626,12 +631,24 @@
     int retValue = 0;
     INT8S  FullEth_Network_if[MAX_STR_LENGTH];
 
-    sprintf(FullEth_Network_if,"%s",Ifcnametable[EthIndex].Ifcname);
+    if(m_NwCfgInfo[EthIndex].VLANID)
+    {
+        TDBG("nwcfg.c : nwReadNWCfg_IPv6 : In VLAN mode");
+        sprintf(FullEth_Network_if,"%s.%d",Ifcnametable[EthIndex].Ifcname,m_NwCfgInfo[EthIndex].VLANID);
+    }
+    else
+    {
+        TDBG("nwcfg.c : nwReadNWCfg_IPv6 : In LAN mode");
+        sprintf(FullEth_Network_if,"%s",Ifcnametable[EthIndex].Ifcname);
+    }    
 
     /* do memset for cfg struct to avoid junk values */
     memset (cfg,0,sizeof(NWCFG6_STRUCT));
 
-    cfg->enable = (unsigned char) isFamilyEnabled(FullEth_Network_if, AF_INET6);
+    if(m_NwCfgInfo[EthIndex].VLANID)
+        cfg->enable = (unsigned char) isFamilyEnabled(VLAN_INTERFACES_FILE, FullEth_Network_if, AF_INET6);
+    else            
+        cfg->enable = (unsigned char) isFamilyEnabled(NETWORK_IF_FILE, FullEth_Network_if, AF_INET6);    
 
     if(cfg->enable != 1)
     {
@@ -1220,7 +1237,7 @@
 
     FILE *fpw;
     char oneline[80];
-    char Ifname[10];
+    char Ifname[16];
     int i;
 
     fpw=fopen(DHCP6CCONF,"w");
@@ -1238,7 +1255,12 @@
         if(m_NwCfgInfo_v6[i].enable != 1 || m_NwCfgInfo[i].enable == 0)
             continue;
 
-        sprintf(Ifname,"%s",Ifcnametable[i].Ifcname);
+        if(m_NwCfgInfo[Ifcnametable[i].Index].VLANID)
+        {
+        	sprintf(Ifname,"%s.%d",Ifcnametable[i].Ifcname,m_NwCfgInfo[Ifcnametable[i].Index].VLANID);
+        }
+        else
+        	sprintf(Ifname,"%s",Ifcnametable[i].Ifcname);
 
         INT8U iana = i+1;
         
@@ -4425,6 +4447,7 @@
 {
     FILE* fp;
     unsigned char strBuf[32];
+    unsigned char str1[INET6_ADDRSTRLEN];
     int count=0;
 
     /* Open VLAN Interface and VLAN ID files */
@@ -4442,6 +4465,47 @@
 
             if(Ifcnametable[count].Enabled != 1)
                 continue;
+
+            if( m_NwCfgInfo_v6[Ifcnametable[count].Index].enable == 1 )
+            {
+                printf("nwUpdateVLANInterfacesFile for IPv6 \n");
+                if(m_NwCfgInfo_v6[Ifcnametable[count].Index].CfgMethod == CFGMETHOD_DHCP)
+                {
+
+                    fprintf(fp,"iface %s.%d inet6 autoconf\n  ", Ifcnametable[count].Ifcname, m_NwCfgInfo[Ifcnametable[count].Index].VLANID);
+
+                }
+                else
+                {
+                    printf("nwUpdateVLANInterfacesFile for IPv6 - STATIC");
+                    /* To print the eth0/1/2 inet static  */
+                    fprintf(fp,"iface %s.%d inet6 static\n  ", Ifcnametable[count].Ifcname, m_NwCfgInfo[Ifcnametable[count].Index].VLANID);
+                    
+                    /* IPv6 address */
+                    ConvertIP6numToStr(m_NwCfgInfo_v6[Ifcnametable[count].Index].GlobalIPAddr[0],INET6_ADDRSTRLEN,str1);
+                    if (strcmp("::", (char *) str1) == 0)
+                    {
+                        strcpy((char*) str1, " ");
+                        m_NwCfgInfo_v6[Ifcnametable[count].Index].GlobalPrefix[0] = 0;
+                    }
+                    fprintf(fp,"%s %s\n",IF_STATIC_IP_STR,str1);
+            
+                    /* IPv6 Prefix */
+                    fprintf(fp,"%s %d\n",IF_STATIC_MASK_STR,m_NwCfgInfo_v6[Ifcnametable[count].Index].GlobalPrefix[0]);
+            
+                    /* IPv6 Gateway */
+                    ConvertIP6numToStr(m_NwCfgInfo_v6[Ifcnametable[count].Index].Gateway,46,str1);
+                    if (strncmp("fe80::", (char*) str1, strlen("fe80::")) == 0)
+                    {
+                        // Gateway address is a link local address, add device name "eth0"
+                        fprintf(fp, "%s %s dev %s\n", IF_STATIC_GW_STR, str1, Ifcnametable[count].Ifcname);
+                    }
+                    else
+                    {
+                        fprintf(fp, "%s %s\n", IF_STATIC_GW_STR, str1);
+                    }
+                }
+            }
 
             if(m_NwCfgInfo[Ifcnametable[count].Index].CfgMethod == CFGMETHOD_DHCP)
             {
