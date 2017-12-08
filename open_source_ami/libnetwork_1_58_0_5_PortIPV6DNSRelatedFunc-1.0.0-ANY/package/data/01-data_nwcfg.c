--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Fri Feb  3 16:17:34 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Mon Feb 13 19:20:24 2012
@@ -139,7 +139,6 @@
 
 int CheckInterfacePresence (char * Ifc);
 int registerBMCstatus (INT8U *registerBMC);
-int registerFQDNstatus(INT8U *registerFQDN);
 int UpdateBondConf(INT8U Enable,INT8U BondIndex,INT8U BondMode,INT16U MiiInterval);
 int UpdateBondInterface(INT8U Enable,INT8U BondIndex,INT8U Slaves,INT8U Ethindex);
 int UpdateActiveSlave(INT8U Flag);
@@ -529,93 +528,58 @@
     /* Get IP address */
     memset(&ifr,0,sizeof(struct ifreq));
     memset(tmp, 0, IP_ADDR_LEN);
-
-/*    memset(FullEth_Network_if,0,MAX_STR_LENGTH);
-
-    if(m_LANIndex)
-    {
-        sprintf(FullEth_Network_if,"%s%d",Eth_NetWork_If,EthIndex);
-    }
-    else
-    {
-        sprintf(FullEth_Network_if,"%s",m_VLANInterface);
-    }
-*/
+    memset(cfg->IPAddr,0,sizeof(cfg->IPAddr));
+    memset(cfg->Broadcast,0,sizeof(cfg->Broadcast));
+    memset(cfg->Mask,0,sizeof(cfg->Mask));
+
     strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
 
     info = (struct sockaddr_in *) &(ifr.ifr_addr);
     info->sin_family = AF_INET;
 
-
     r = ioctl(skfd, SIOCGIFADDR, &ifr);
 
-    if ( r < 0 )
-    {
-        close (skfd);
-        TDBG("IOCTL to get IP failed: %d\n",r);		// This is a normal case. Don't print message
-        return -1;
-    }
-
-    memcpy(cfg->IPAddr,(char *) &(info->sin_addr.s_addr),IP_ADDR_LEN);
-
-    /* Get Broadcast address */
-    memset(&ifr,0,sizeof(struct ifreq));
-    memset(tmp, 0, IP_ADDR_LEN);
-
-/*    memset(FullEth_Network_if,0,MAX_STR_LENGTH);
-
-    if(m_LANIndex)
-    {
-        sprintf(FullEth_Network_if,"%s%d",Eth_NetWork_If,EthIndex);
-    }
-    else
-    {
-        sprintf(FullEth_Network_if,"%s",m_VLANInterface);
-*/
-    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
-    info = (struct sockaddr_in *) &(ifr.ifr_broadaddr);
-    info->sin_family = AF_INET;
-    r = ioctl(skfd, SIOCGIFBRDADDR, &ifr);
-    if ( r < 0 )
-    {
-        printf("IOCTL to get Broadcast failed: %d\n",r);
-        return -1;
-    }
-    memcpy(cfg->Broadcast,(char *) &(info->sin_addr.s_addr),IP_ADDR_LEN);
-
-    /* Get Netmask */
-    memset(&ifr,0,sizeof(struct ifreq));
-    memset(tmp, 0, IP_ADDR_LEN);
-
-/*    memset(FullEth_Network_if,0,MAX_STR_LENGTH);
-
-    if(m_LANIndex)
-    {
-        sprintf(FullEth_Network_if,"%s%d",Eth_NetWork_If,EthIndex);
-    }
-    else
-    {
-        sprintf(FullEth_Network_if,"%s",m_VLANInterface);
-    }
-*/
-    strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
-
-    info = (struct sockaddr_in *) &(ifr.ifr_netmask);
-    info->sin_family = AF_INET;
-    r = ioctl(skfd, SIOCGIFNETMASK, &ifr);
-    if ( r < 0 )
-    {
-        close (skfd);
-        TCRIT("IOCTL to get Netmask failed: %d\n",r);
-        return -1;
-    }
-
-    memcpy(cfg->Mask,(char *) &(info->sin_addr.s_addr),IP_ADDR_LEN);
-
+    if ( r == 0 )
+    {
+        memcpy(cfg->IPAddr,(char *) &(info->sin_addr.s_addr),IP_ADDR_LEN);
+
+        /* Get Broadcast address */
+        memset(&ifr,0,sizeof(struct ifreq));
+        memset(tmp, 0, IP_ADDR_LEN);
+
+        strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
+        info = (struct sockaddr_in *) &(ifr.ifr_broadaddr);
+        info->sin_family = AF_INET;
+        r = ioctl(skfd, SIOCGIFBRDADDR, &ifr);
+        if ( r < 0 )
+        {
+            printf("IOCTL to get Broadcast failed: %d\n",r);
+            return -1;
+        }
+        memcpy(cfg->Broadcast,(char *) &(info->sin_addr.s_addr),IP_ADDR_LEN);
+
+        /* Get Netmask */
+        memset(&ifr,0,sizeof(struct ifreq));
+        memset(tmp, 0, IP_ADDR_LEN);
+
+        strncpy(ifr.ifr_name, FullEth_Network_if,IFNAMSIZ - 1);
+
+        info = (struct sockaddr_in *) &(ifr.ifr_netmask);
+        info->sin_family = AF_INET;
+        r = ioctl(skfd, SIOCGIFNETMASK, &ifr);
+        if ( r < 0 )
+        {
+            close (skfd);
+            TCRIT("IOCTL to get Netmask failed: %d\n",r);
+            return -1;
+        }
+
+        memcpy(cfg->Mask,(char *) &(info->sin_addr.s_addr),IP_ADDR_LEN);
+
+    }
     (void) close(skfd);
-
     return 0;
-    }
+}
 
 /**
 *@fn nwReadNWCfg_IPv6
@@ -640,7 +604,7 @@
     {
         TDBG("nwcfg.c : nwReadNWCfg_IPv6 : In LAN mode");
         sprintf(FullEth_Network_if,"%s",Ifcnametable[EthIndex].Ifcname);
-    }    
+    }
 
     /* do memset for cfg struct to avoid junk values */
     memset (cfg,0,sizeof(NWCFG6_STRUCT));
@@ -648,7 +612,7 @@
     if(m_NwCfgInfo[EthIndex].VLANID)
         cfg->enable = (unsigned char) isFamilyEnabled(VLAN_INTERFACES_FILE, FullEth_Network_if, AF_INET6);
     else            
-        cfg->enable = (unsigned char) isFamilyEnabled(NETWORK_IF_FILE, FullEth_Network_if, AF_INET6);    
+        cfg->enable = (unsigned char) isFamilyEnabled(NETWORK_IF_FILE, FullEth_Network_if, AF_INET6);
 
     if(cfg->enable != 1)
     {
@@ -682,6 +646,12 @@
                     // Link Local
                     memcpy (cfg->LinkIPAddr, ((struct sockaddr_in6 *)(ifAddrsP->ifa_addr))->sin6_addr.s6_addr16, 16);
                     cfg->LinkPrefix = (unsigned char)ConvertNETMaskArrayTonum(((struct sockaddr_in6 *)(ifAddrsP->ifa_netmask))->sin6_addr.s6_addr,IP6_ADDR_LEN);
+                }
+                else if (temp[0] == 0xFE && temp[1]>>6 == 3)
+                {
+                    //Site Local
+                    memcpy (cfg->SiteIPAddr, ((struct sockaddr_in6 *)(ifAddrsP->ifa_addr))->sin6_addr.s6_addr16, 16);
+                    cfg->SitePrefix = (unsigned char)ConvertNETMaskArrayTonum(((struct sockaddr_in6 *)(ifAddrsP->ifa_netmask))->sin6_addr.s6_addr,IP6_ADDR_LEN);
                 }
                 else
                 {
@@ -704,11 +674,11 @@
     return -1;
     }
 
-    if (cfg->GlobalPrefix[0] == 0)
-    {
+//    if (cfg->GlobalPrefix[0] == 0)
+//    {
         /* set the default prefix length to 64, which is ipv6 default */
-        cfg->GlobalPrefix[0] = 64;
-    }
+/*        cfg->GlobalPrefix[0] = 64;
+    }*/
 
     /* Get default gateway */
     if((retValue = GetDefaultGateway_ipv6(cfg->Gateway,&EthIndex) ))
@@ -976,9 +946,22 @@
     /* while reading network configurations check for enable status. else return all zeros */
     if(m_NwCfgInfo[EthIndex].enable != 1)
     {
-        TDBG ("\n LAN Enable is not true.. i.e. disabled\n");
-        memset(cfg, 0, sizeof(NWCFG_STRUCT));
-        return 0;
+        if(m_NwCfgInfo_v6[EthIndex].enable == 1)
+        {
+            #ifdef CONFIG_SPX_FEATURE_GLOBAL_IPMI_IPV6
+                TDBG("IPV6 only enabled");
+            #else
+                TDBG ("\n LAN Enable is not true.. i.e. disabled\n");
+                memset(cfg, 0, sizeof(NWCFG_STRUCT));
+                return 0;
+            #endif
+        }
+        else
+        {
+            TDBG ("\n LAN Enable is not true.. i.e. disabled\n");
+            memset(cfg, 0, sizeof(NWCFG_STRUCT));
+            return 0;
+        }
     }
     memcpy(cfg,&m_NwCfgInfo[EthIndex],sizeof(m_NwCfgInfo[EthIndex]));
     memset(FullEth_Network_if,0,MAX_STR_LENGTH);
@@ -1115,7 +1098,9 @@
     FILE *cfgFile = NULL;
     INT8S  FullEth_Network_if[MAX_STR_LENGTH];
     int i;
+    int j = 0;
     unsigned char str[32];
+    unsigned char NewAddr[IP6_ADDR_LEN];
     unsigned char str1[INET6_ADDRSTRLEN];
 
     if((cfgFile = fopen(NETWORK_IF_FILE,"w"))==NULL)
@@ -1134,8 +1119,11 @@
             if (m_NwCfgInfo[Ifcnametable[i].Index].enable != 1)
             {
                 // If interface is not enabled dont write the network configurations
-                TDBG("Interface eth%d is not enabled. so dont write anything");
-                continue;
+                if( m_NwCfgInfo_v6[Ifcnametable[i].Index].enable == 0 )
+                {
+                    TDBG("Interface eth%d is not enabled. so dont write anything",Ifcnametable[i].Index);
+                    continue;
+                }   
             }
             else
             {
@@ -1181,27 +1169,41 @@
 
                     /* IPv6 Gateway */
                     ConvertIP6numToStr( m_NwCfgInfo_v6[Ifcnametable[i].Index].Gateway,46,str1);
-			if (strncmp("fe80::", (char*) str1, strlen("fe80::")) == 0)
-			{
-				// Gateway address is a link local address, add device name "eth0"
-				fprintf(cfgFile, "%s %s dev %s\n", IF_STATIC_GW_STR, str1, Ifcnametable[i].Ifcname);
-			}
-			else
-			{
-				fprintf(cfgFile, "%s %s\n", IF_STATIC_GW_STR, str1);
-			}
+                    if (strncmp("fe80::", (char*) str1, strlen("fe80::")) == 0)
+                    {
+                        // Gateway address is a link local address, add device name "eth0"
+                        fprintf(cfgFile, "%s %s dev %s\n", IF_STATIC_GW_STR, str1, Ifcnametable[i].Ifcname);
+                    }
+                    else
+                    {
+                        fprintf(cfgFile, "%s %s\n", IF_STATIC_GW_STR, str1);
+                    }
+                    memset (NewAddr, 0, sizeof (NewAddr));
+                    /*Add the static ipv6 address to file*/
+                    for (j = 1; j< MAX_IPV6ADDRS; j++)
+                    {
+                        if (0 != memcmp(NewAddr, m_NwCfgInfo_v6[Ifcnametable[i].Index].GlobalIPAddr[j], IP6_ADDR_LEN))
+                        {
+                            ConvertIP6numToStr(m_NwCfgInfo_v6[Ifcnametable[i].Index].GlobalIPAddr[j],INET6_ADDRSTRLEN,str1);
+                            fprintf(cfgFile, "%s %s/%d %s %s\n", IF_STATIC_ADDR_STR, str1, m_NwCfgInfo_v6[Ifcnametable[i].Index].GlobalPrefix[j], \
+                            "dev", Ifcnametable[i].Ifcname);
+                        }
+                    }
                 }
         }
 
-        if(m_NwCfgInfo[Ifcnametable[i].Index].CfgMethod == CFGMETHOD_DHCP)
-        {
-            TDBG("nwWriteNwcfg for IPv4 - DHCP");
-            memset(FullEth_Network_if,0,MAX_STR_LENGTH);
-            sprintf(FullEth_Network_if,"iface %s inet dhcp\n  ",Ifcnametable[i].Ifcname);
-            fprintf(cfgFile,"%s",FullEth_Network_if);
-        }
-        else
-        {
+        // Dont write IPV4 configurations if it is disabled.
+        if (m_NwCfgInfo[Ifcnametable[i].Index].enable == 1)
+        {
+            if(m_NwCfgInfo[Ifcnametable[i].Index].CfgMethod == CFGMETHOD_DHCP)
+            {
+                TDBG("nwWriteNwcfg for IPv4 - DHCP");
+                memset(FullEth_Network_if,0,MAX_STR_LENGTH);
+                sprintf(FullEth_Network_if,"iface %s inet dhcp\n  ",Ifcnametable[i].Ifcname);
+                fprintf(cfgFile,"%s",FullEth_Network_if);
+            }
+            else
+            {
 
                 TDBG("nwWriteNwcfg for IPv4- STATIC");
                 /* To print the eth0/1/2 inet static  */
@@ -1222,6 +1224,7 @@
                 fprintf(cfgFile,"%s %s\n",IF_STATIC_GW_STR,str);
                 }
         }
+	}
         fclose(cfgFile);
     }
     return 0;
@@ -1252,7 +1255,7 @@
 
     for(i=0;i<sizeof(Ifcnametable)/sizeof(IfcName_T);i++)
     {
-        if(m_NwCfgInfo_v6[i].enable != 1 || m_NwCfgInfo[i].enable == 0)
+        if(m_NwCfgInfo_v6[i].enable != 1)
             continue;
 
         if(m_NwCfgInfo[Ifcnametable[i].Index].VLANID)
@@ -1616,11 +1619,13 @@
                 m_NwCfgInfo[i].Slave = 0;
                 m_NwCfgInfo[i].BondIndex = 0;
                 m_NwCfgInfo[i].enable = 1;
+                m_NwCfgInfo_v6[i].enable =1;
             }
 
             if((Slaves >> i) & IFACE_ENABLED && m_NwCfgInfo[i].Slave == 0)
             {
                 m_NwCfgInfo[i].enable = 0;
+                m_NwCfgInfo_v6[i].enable =0;
                 m_NwCfgInfo[i].Slave=1;
                 m_NwCfgInfo[i].BondIndex = BondIndex;
                 Ifcnametable[i].Enabled = 0;
@@ -1665,6 +1670,7 @@
         if (m_NwCfgInfo[EthIndex].enable != 0)
         {
             m_NwCfgInfo[EthIndex].enable = 0;
+            m_NwCfgInfo_v6[EthIndex].enable = 0;
         }
         m_NwCfgInfo[EthIndex].Master = 0;
         m_NwCfgInfo[EthIndex].BondIndex = 0;
@@ -1754,29 +1760,21 @@
  */
 int nwConfigureBonding(BondIface *BondCfg,INT8U EthIndex)
 {
-    char IfupdownStr[64],EnslaveIface[64],IName[16],str[64],BondSlaves[64];
+    char IfupdownStr[64],EnslaveIface[64],IName[16],str[64],BondSlaves[64],ifcname[16] = {0};
     int i,Ethindex;
     NWCFG6_STRUCT cfg6;
     NWCFG_STRUCT cfg;
     DOMAINCONF      DomainCfg;
-    DNSIPV4CONF     DNSv4;
-    DNSIPV6CONF     DNSv6;
-    INT8U               registerBMC[MAX_CHANNEL];
-    INT8U               regBMC[MAX_CHANNEL];
-    INT8U               registerFQDN[MAX_CHANNEL];
-    INT8U               regFQDN[MAX_CHANNEL];
+    DNSCONF     DNS;
+    INT8U               regBMC_FQDN[MAX_CHANNEL];
     struct stat       Buf;
 
     memset(IfupdownStr,'\0',64);
     memset(str,'\0',64);
     memset(BondSlaves,'\0',64);
     memset(&DomainCfg,0,sizeof(DOMAINCONF));
-    memset(&DNSv4,0,sizeof(DNSIPV4CONF));
-    memset(&DNSv6,0,sizeof(DNSIPV6CONF));
-    memset(registerBMC,0,sizeof(registerBMC));
-    memset(registerFQDN,0,sizeof(registerFQDN));
-    memset(regBMC,0,sizeof(regBMC));
-    memset(regFQDN,0,sizeof(regFQDN));
+    memset(&DNS,0,sizeof(DNSCONF));
+    memset(regBMC_FQDN,0,sizeof(regBMC_FQDN));
 
     /*Remove the file if it exits*/
     if(0 == stat(ACTIVESLAVE,&Buf))
@@ -1803,7 +1801,7 @@
     nwReadNWCfg_v4_v6(&cfg,& cfg6,EthIndex);
     nwGetBondCfg();
     /*Read the DNS File*/
-    if(ReadDNSConfFile(&DomainCfg,&DNSv4,&DNSv6,registerBMC,registerFQDN) != 0)
+    if(ReadDNSConfFile(&DomainCfg,&DNS,regBMC_FQDN) != 0)
     {
         TCRIT("Error  in reading DNS Configuration File\n");
     }
@@ -1861,6 +1859,16 @@
         sprintf(IfupdownStr,"%s bond%d ",IfDown_Ifc,BondCfg->BondIndex);
         safe_system(IfupdownStr);
 
+        /*Update the interface name for service configuration*/
+        if(m_NwCfgInfo[Ifcnametable[i].Index].VLANID)
+        {
+            sprintf(ifcname,"%s.%d",Ifcnametable[i].Ifcname,m_NwCfgInfo[Ifcnametable[i].Index].VLANID);
+        }
+        else
+        {
+            sprintf(ifcname,"%s",Ifcnametable[i].Ifcname);
+        }
+
         /*Disable the VLAN ID*/
         m_NwCfgInfo[Ethindex].VLANID = 0;
     }
@@ -1875,13 +1883,8 @@
     {
         DomainCfg.EthIndex=Ethindex;
         DomainCfg.v4v6=1;
-        DNSv4.EthIndex=Ethindex;
-        DNSv6.EthIndex=Ethindex;
-        if(registerFQDN[EthIndex] ==1)
-            registerFQDN[Ethindex]=1;
-        if(registerBMC[EthIndex] == 1)
-            registerBMC[Ethindex]=1;
-
+        DNS.DNSIndex=Ethindex;
+        regBMC_FQDN[Ethindex] = regBMC_FQDN[EthIndex];
     }
     else
     {
@@ -1889,7 +1892,7 @@
     }
 
     /*Update DNS configuration for bond interface*/
-    WriteDNSConfFile(&DomainCfg, &DNSv4, &DNSv6,registerBMC,registerFQDN);
+    WriteDNSConfFile(&DomainCfg, &DNS,regBMC_FQDN);
 
     /*Update the DNS File*/
     if(BondCfg->Enable == 1)
@@ -1959,29 +1962,19 @@
 
         nwUpdateInterfaces_v4_v6();
 
-        registerBMCstatus(regBMC);
-        registerFQDNstatus(regFQDN);
+        registerBMCstatus(regBMC_FQDN);
 
         DomainCfg.EthIndex=EthIndex;
-        DNSv4.EthIndex=EthIndex;
-        DNSv6.EthIndex=EthIndex;
-
-        if(registerFQDN[Ethindex] ==1)
-            regFQDN[EthIndex]=1;
-        else
-            regFQDN[EthIndex] = 0;
-
-        if(registerBMC[Ethindex] == 1)
-            regBMC[EthIndex]=1;
-        else
-            regBMC[EthIndex] = 0;
+        DNS.DNSIndex=EthIndex;
+
+        regBMC_FQDN[EthIndex] = regBMC_FQDN[Ethindex];
 
         if(DomainCfg.dhcpEnable == 0)
         {
             for(i=0;i<MAX_CHANNEL;i++)
             {
-                if(regFQDN[i] == 1)
-                    regFQDN[i] = 0;
+                if((regBMC_FQDN[i] & 0x10) == 0x10)
+                    regBMC_FQDN[i] |= 0x00;
             }
         }
 
@@ -1989,7 +1982,7 @@
         Write_dhcp6c_conf();
 
         /*Update DNS configuration for bond interface*/
-        WriteDNSConfFile(&DomainCfg, &DNSv4, &DNSv6,regBMC,regFQDN);
+        WriteDNSConfFile(&DomainCfg, &DNS,regBMC_FQDN);
 
         /*Activate all the interface in interface file*/
         for(i=0;i<sizeof(Ifcnametable)/sizeof(IfcName_T);i++)
@@ -2482,169 +2475,190 @@
 pamam   : domain - domain name of the network.
 pamam   : domainnamelen - domain name length.
 ************************************************************************/
-int nwGetResolvConf_v4_v6(char* DNS1,char*DNS2,char *DNS1v6, char *DNS2v6, char* domain,unsigned char* domainnamelen)
-{
-	//read resolv.conf entries anyways
-	FILE* fpresolv;
-	char oneline[300];
-       char nameserverip[INET6_ADDRSTRLEN];
-	char domainval[300];
-	int domainfound = 0;
+int nwGetResolvConf_v4_v6(char* DNS1,char*DNS2,char *DNS3, INT8U DNSIPPriority,char* domain,unsigned char* domainnamelen)
+{
+    //read resolv.conf entries anyways
+    FILE* fpresolv;
+    char oneline[300];
+    char nameserverip[INET6_ADDRSTRLEN];
+    char domainval[300];
+    int domainfound = 0;
     //start with primary
-	int servernov4 = 1;
-       int servernov6 = 1;
-       char ipAddr[46];
-
-        if(domain != NULL)
-        {
+    int serverno = 1;
+    int v4count = 0;
+    int v6count = 0;
+    char ipAddr[46];
+
+    if(domain != NULL)
+    {
 #ifdef CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_DOMAINNAME
-			strcpy(domain,CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_DOMAINNAME);
+        strcpy(domain,CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_DOMAINNAME);
 #else
-			strcpy(domain,"Unknown");
+        strcpy(domain,"Unknown");
 #endif
-        }
-        if(DNS1 != NULL)
-        {
-            strcpy(DNS1,"0.0.0.0");
-        }
-        if(DNS2 != NULL)
-        {
-            strcpy(DNS2,"0.0.0.0");
-        }
-        if(DNS1v6 != NULL)
-        {
-            strcpy(DNS1v6,"::");
-        }
-        if(DNS2v6 != NULL)
-        {
-            strcpy(DNS2v6,"::");
-        }
-
-	fpresolv = fopen(RESOLV_CONF_FILE,"r");
-	if (fpresolv == NULL)
-	{
-		//no file just return NULL entires
-		return 0;
-	}
-	file_lock_write(fileno(fpresolv));
-
-	//now read resolv.conf file here
-	while (!feof(fpresolv))
-	{
-		if (fgets(oneline,299,fpresolv) == NULL)
-		{
-			TDBG("EOF reading resolv.conf file\n");
-			file_unlock(fileno(fpresolv));
-			fclose(fpresolv);
-			return 0;
-		}
-
-		//see if the line has nameserver
-		//a comment will be ignored by scanf
-		 if( oneline[0] == '#') continue;
-
-		if (sscanf(oneline," nameserver  %s",nameserverip) == 1)
-		{
-		//this is the line!!
-		//check if it is a vlaid IP!!
-                    if( inet_aton(nameserverip,(struct in_addr*)ipAddr) != 0 )
+    }
+
+    fpresolv = fopen(RESOLV_CONF_FILE,"r");
+    if (fpresolv == NULL)
+    {
+        //no file just return NULL entires
+        return 0;
+    }
+    file_lock_write(fileno(fpresolv));
+
+    //now read resolv.conf file here
+    while (!feof(fpresolv))
+    {
+        if (fgets(oneline,299,fpresolv) == NULL)
+        {
+            TDBG("EOF reading resolv.conf file\n");
+/*            file_unlock(fileno(fpresolv));
+            fclose(fpresolv);
+            return 0;*/
+            break;
+        }
+
+        //see if the line has nameserver
+        //a comment will be ignored by scanf
+        if( oneline[0] == '#') continue;
+
+        if (sscanf(oneline," nameserver  %s",nameserverip) == 1)
+        {
+            //this is the line!!
+            //check if it is a vlaid IP!!
+            if( inet_aton(nameserverip,(struct in_addr*)ipAddr) != 0 )
+            {
+                if (serverno == 1)
+                {
+                    if(DNS1 != NULL)
                     {
-                        if (servernov4 == 1)
-                        {
-                            if(DNS1 != NULL)
-                            {
-                                strncpy(DNS1,nameserverip,INET_ADDRSTRLEN);
-                            }
-                            TDBG("found server 1 and it is %s\n",DNS1);
-                            servernov4++;
-                        }
-                        else if (servernov4 == 2)
-                        {
-                            if(DNS2 != NULL)
-                            {
-                                strncpy(DNS2,nameserverip,INET_ADDRSTRLEN);
-                            }
-                            TDBG("found server 2 and it is %s\n",DNS2);
-                            servernov4++;
-                            //we are done with dns server names
-                        }
-                        else
-                        {
-                            TDBG("Strange error finding too many DNS names\n");
-                            //we just ignore extra nameservers
-                        }
+                        DNS1[10] = 0xFF;
+                        DNS1[11] = 0xFF;
+                        memcpy(&DNS1[IP6_ADDR_LEN - IP_ADDR_LEN],ipAddr,IP_ADDR_LEN);
                     }
-                    else if( inet_pton (AF_INET6, nameserverip, ipAddr) > 0 )
+                    TDBG("found server 1 and it is %d %d %d %d\n",ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
+                    serverno++;
+                    v4count++;
+                }
+                else if (serverno == 2)
+                {
+                    if(DNS2 != NULL)
                     {
-                        TDBG("IPv6 DNS server");
-                        if (servernov6 == 1)
-                        {
-                            if(DNS1v6 != NULL)
-                            {
-                                strncpy(DNS1v6,nameserverip,INET6_ADDRSTRLEN);
-                            }
-                            TDBG("found ipv6 server 1 and it is %s\n",DNS1v6);
-                            servernov6++;
-                        }
-                        else if (servernov6 == 2)
-                        {
-                            if(DNS2v6 != NULL)
-                            {
-                                strncpy(DNS2v6,nameserverip,INET6_ADDRSTRLEN);
-                            }
-                            TDBG("found ipv6 server 2 and it is %s\n",DNS2v6);
-                            servernov6++;
-                        }
-                        else
-                        {
-                            TWARN("Strange error finding too many v6 DNS names \n");
-                            //we just ignore extra nameservers
-                        }
+                        DNS2[10] = 0xFF;
+                        DNS2[11] = 0xFF;
+                        memcpy(&DNS2[IP6_ADDR_LEN - IP_ADDR_LEN],ipAddr,IP_ADDR_LEN);
                     }
-		}
-		else if (sscanf(oneline," search %s",domainval) == 1)
-		{
-			//we found domain here
-			TDBG("found domain\n");
-			domainfound = 1;
-			
-			/* It might be much reasonable to check length here by our max value instead of user's max value */
-			//if (*domainnamelen <= strlen(domainval)+1)
-			if (DNSCFG_MAX_DOMAIN_NAME_LEN <= strlen(domainval)+1)
-			{
-			        printf("\n *domainnamelen - %d \n", *domainnamelen);
-			        printf("\n  strlen(domainval)+1 - %d \n",  strlen(domainval)+1);
-				TWARN("Insufficiient buffer for holding domain name\n");
-				//Maybe it shouldn't leave here just because getting domain name error.
-				//*domainnamelen = strlen(domainval)+1;
-				//file_unlock(fileno(fpresolv));
-				//fclose(fpresolv);
-				//return -1;
-			}
-			else
-                    {         
-				//strncpy(domain,domainval,299);
+                    TDBG("found server 2 and it is %d %d %d %d\n",ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
+                    serverno++;
+                    v4count++;
+                    //we are done with dns server names
+                }
+                else if(serverno == 3)
+                {
+                    if(DNS3 != NULL)
+                    {
+                        DNS3[10] = 0xFF;
+                        DNS3[11] = 0xFF;
+                        memcpy(&DNS3[IP6_ADDR_LEN - IP_ADDR_LEN],ipAddr,IP_ADDR_LEN);
+                    }
+                    TDBG("found server 3 and it is %d %d %d %d\n",ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
+                    serverno++;
+                    v4count++;
+                }
+            }
+            else if( inet_pton (AF_INET6, nameserverip, ipAddr) > 0 )
+            {
+                TDBG("IPv6 DNS server");
+                if (serverno == 1)
+                {
+                    if(DNS1 != NULL)
+                    {
+                        memcpy(DNS1,ipAddr,IP6_ADDR_LEN);
+                    }
+                    TDBG("found ipv6 server 1 and it is %s\n",DNS1);
+                    serverno++;
+                    v6count++;
+                }
+                else if (serverno == 2)
+                {
+                    if(DNS2 != NULL)
+                    {
+                        memcpy(DNS2,ipAddr,IP6_ADDR_LEN);
+                    }
+                    TDBG("found ipv6 server 2 and it is %s\n",DNS2);
+                    serverno++;
+                    v6count++;
+                }
+                else if(serverno == 3)
+                {
+                    if(DNS3 != NULL)
+                    {
+                        memcpy(DNS3,ipAddr,IP6_ADDR_LEN);
+                    }
+                    serverno++;
+                    v6count++;
+                }
+            }
+        }
+        else if (sscanf(oneline," search %s",domainval) == 1)
+        {
+            //we found domain here
+            TDBG("found domain\n");
+            domainfound = 1;
+
+            /* It might be much reasonable to check length here by our max value instead of user's max value */
+            //if (*domainnamelen <= strlen(domainval)+1)
+            if (DNSCFG_MAX_DOMAIN_NAME_LEN <= strlen(domainval)+1)
+            {
+                printf("\n *domainnamelen - %d \n", *domainnamelen);
+                printf("\n  strlen(domainval)+1 - %zu \n",  strlen(domainval)+1);
+                TWARN("Insufficiient buffer for holding domain name\n");
+                //Maybe it shouldn't leave here just because getting domain name error.
+                //*domainnamelen = strlen(domainval)+1;
+                //file_unlock(fileno(fpresolv));
+                //fclose(fpresolv);
+                //return -1;
+            }
+            else
+            {
                 if(domain != NULL)
-                {    
-					*domainnamelen = strlen(domainval);        
+                {
+                    *domainnamelen = strlen(domainval);        
                      //strncpy(domain,domainval,79);
                      strncpy(domain,domainval,*domainnamelen);
                 }
              }
-		}
-
-		if (domainfound == 1 && servernov4 > 2 && servernov6>2)
-		{
-			//we found both domain and nameservers
-			//printf("\nwe found both domain and nameservers\n");
-			break;
-		}
-	}
-	file_unlock(fileno(fpresolv));
-	fclose(fpresolv);
-
-       
-	return 0;
+        }
+
+        if (domainfound == 1 && serverno > 3)
+        {
+            //we found both domain and nameservers
+            //printf("\nwe found both domain and nameservers\n");
+            break;
+        }
+    }
+
+    if(DNSIPPriority != 0)
+    {
+        TDBG("DNSIPPriority %d v4count %d v6count %d\n",DNSIPPriority,v4count,v6count);
+        if((DNSIPPriority == 1 && v4count == 1) || (DNSIPPriority == 2 && v6count == 1))
+        {
+            memcpy(DNS3,DNS2,IP6_ADDR_LEN);
+            memset(DNS2,0,IP6_ADDR_LEN);
+        }
+
+        if((DNSIPPriority == 1 && v4count == 0) || (DNSIPPriority == 2 && v6count == 0))
+        {
+            memcpy(DNS3,DNS1,IP6_ADDR_LEN);
+            memset(DNS1,0,IP6_ADDR_LEN);
+            memset(DNS2,0,IP6_ADDR_LEN);
+        }
+    }
+
+    file_unlock(fileno(fpresolv));
+    fclose(fpresolv);
+
+    return 0;
 }
 
 
@@ -2661,22 +2675,19 @@
 *@param registerBMC - the array of register BMC flags with MAX LAN channel length 
 *@return Returns 0 on success and -1 on fails
 */
-int nwGetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, 
-					 DNSIPV4CONF *Dnsv4IPConfig, DNSIPV6CONF *Dnsv6IPConfig, 
-					 INT8U *registerBMC,INT8U *registerFQDN)
+int nwGetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig, INT8U *regBMC_FQDN)
 {
     int retVal=0;
     int hs;
     char *Domain;
-    char *DNSv4_1;
-    char *DNSv4_2;
-    char *DNSv6_1;
-    char *DNSv6_2;
+    char *DNS1;
+    char *DNS2;
+    char *DNS3;
     char hostname[MAX_HOSTNAME_LEN];
 
 
     /* Read the dns related configuration from dns.conf file */
-    retVal =ReadDNSConfFile(DomainConfig, Dnsv4IPConfig, Dnsv6IPConfig, registerBMC,registerFQDN);
+    retVal =ReadDNSConfFile(DomainConfig, DnsIPConfig, regBMC_FQDN);
     if( retVal != 0)
     {
         TCRIT(" Failed to Read DNS Configuration File..!!");
@@ -2684,6 +2695,7 @@
 
     if (HostnameConfig != NULL)
     {
+        memset(hostname,0,sizeof(MAX_HOSTNAME_LEN));
         //GetHostNameConf( hostname, (int *)&HostnameConfig->HostSetting);
         GetHostNameConf( hostname, &hs);
         HostnameConfig->HostSetting = hs;
@@ -2710,30 +2722,21 @@
         Domain=NULL;
     }
 
-    if( Dnsv4IPConfig != NULL )     //check for NULL
-    {
-        DNSv4_1= (char *) &Dnsv4IPConfig->v4DNSIP1;
-        DNSv4_2= (char *) &Dnsv4IPConfig->v4DNSIP2;
+    if( DnsIPConfig != NULL )     //check for NULL
+    {
+        DNS1 = (char *) &DnsIPConfig->DNSIP1;
+        DNS2 = (char *) &DnsIPConfig->DNSIP2;
+        DNS3 = (char *) &DnsIPConfig->DNSIP3;
     }
     else
     {
-        DNSv4_1=NULL;
-        DNSv4_2=NULL;    
-    }
-
-    if( Dnsv6IPConfig != NULL )     //check for NULL
-    {
-        DNSv6_1= (char *) &Dnsv6IPConfig->v6DNSIP1;
-        DNSv6_2= (char *) &Dnsv6IPConfig->v6DNSIP2;
-    }
-    else
-    {
-        DNSv6_1=NULL;
-        DNSv6_2=NULL;
+        DNS1 = NULL;
+        DNS2 = NULL;
+        DNS3 = NULL;
     }
 
     /* read the resolv.conf file for domain name and DNS server IP's */
-    retVal= nwGetResolvConf_v4_v6(DNSv4_1, DNSv4_2, DNSv6_1, DNSv6_2, Domain, &DomainConfig->domainnamelen);
+    retVal= nwGetResolvConf_v4_v6(DNS1, DNS2, DNS3, DnsIPConfig->IPPriority,Domain, &DomainConfig->domainnamelen);
 
     if(retVal != 0)
     {
@@ -2753,99 +2756,23 @@
 *@param registerBMC - the array of register BMC flags with MAX LAN channel length 
 *@return Returns 0 on success and -1 on fails
 */
-int nwSetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, 
-				     DNSIPV4CONF *Dnsv4IPConfig, DNSIPV6CONF *Dnsv6IPConfig,
-				     INT8U *registerBMC,INT8U *registerFQDN)
+int nwSetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig, INT8U *regBMC_FQDN)
 {
     int retVal=0;
- 
     char *Domain = NULL;
-    char *DNSv41;
-    char *DNSv42;
-    char *DNSv61;
-    char *DNSv62;
-    //variable used to check for whether DNS changes are relevant to only one interface    
-    char singleNIC=0;
-    char EthIndex=0xff;
-    // variables to get host name 
-    char hname[256];
-    int hsettings;
-    // variable to check status of registerBMC flag
-    INT8U reg_status[MAX_CHANNEL];
     INT8U i=0;
 
-    if( (DomainConfig != NULL) && (Dnsv4IPConfig != NULL) )
-    {
-        if( DomainConfig->EthIndex == Dnsv4IPConfig->EthIndex)
-        {
-        	if(Dnsv6IPConfig != NULL)
-        	{
-                if(Dnsv4IPConfig->EthIndex == Dnsv6IPConfig->EthIndex)
-                {
-                    singleNIC = 1;
-                    EthIndex = Dnsv4IPConfig->EthIndex;
-                }
-        	}
-        	else
-        	{
-        		singleNIC = 1;
-        		EthIndex = Dnsv4IPConfig->EthIndex;
-        	}
-        }
-    }
-   
-   /* This part of code below is used to check whether host name or any registerBMC flags modified 
-      and this results used to decide whether only single NIC has to be restarted or all the NIC's
-   */
-   if(singleNIC != 0)	// below check is not needed if singleNIC is already 0
-   {
-        //check whether there is a change in host name 
-        retVal = GetHostNameConf( hname, &hsettings);
-        if( (strcmp(hname,(char*) &HostnameConfig->HostName)!=0) || (hsettings != HostnameConfig->HostNameLen ) )
-        {
-            singleNIC=0;
-        }
-
-        // check for change in register BMC flag status for each interface
-        registerBMCstatus(reg_status);
-        GetNoofInterface();
-
-        for (i=0;i< sizeof(Ifcnametable)/sizeof(IfcName_T); i++)
-        {
-            if(Ifcnametable[i].Enabled != 1)
-                continue;
-
-            if( (Ifcnametable[i].Index != EthIndex) && (reg_status[Ifcnametable[i].Index] != registerBMC[Ifcnametable[i].Index]) ) //ignore the EthIndex if singleNIC is already selected as 1 
-            {
-                singleNIC=0;  
-            }
-        }
-        TDBG(" singleNIC - %d \n", singleNIC);
-    }	//end of if(singleNIC != 0)
-   
-   /* End of host name or register BMC flag change status check*/
-   
-    if( singleNIC == 1 )
-    {
-        TDBG(" ETH%d interface restart starts........ ", EthIndex);
-        nwMakeIFDown(EthIndex);
-        TDBG(" ETH%d DOWN is done........ ", EthIndex);    
-    }
-    else
-    {
-
-        /* Do network restart for All the available Interfaces */
-        TDBG(" ETH interface restart starts........");
-        
-        for(i=0; i<sizeof(Ifcnametable)/sizeof(IfcName_T); i++)
-        {
-            if(Ifcnametable[i].Enabled != 1)
-                continue;
-            nwMakeIFDown(Ifcnametable[i].Index);
-            TDBG(" eth%d DOWN is done........ ",i);    
-        }
-    }
-    
+    GetNoofInterface();
+
+    for(i=0; i<sizeof(Ifcnametable)/sizeof(IfcName_T); i++)
+    {
+        if(Ifcnametable[i].Enabled != 1)
+            continue;
+        nwMakeIFDown(Ifcnametable[i].Index);
+        TDBG(" eth%d DOWN is done........ ",i);    
+    }
+
+
     /* Check for NULL data in each parameter that is passed to this function */
     /* If Null then Fill NULL values to the respective pointers thats going to be passed to 
     the nwSetResolvConf_v4_v6 which writes resolv.conf file. if a particular 
@@ -2884,7 +2811,7 @@
         }
         else
         {
-            Domain = (char *) DomainConfig->domainname;
+            Domain = (char *) &DomainConfig->domainname;
         }
     }
     else
@@ -2892,95 +2819,48 @@
         Domain=NULL;
     }
 
-    /* Dnsv4IPConfig */
-    if( Dnsv4IPConfig != NULL )     //check for NULL
-    {
-        if(Dnsv4IPConfig->dhcpEnable ==1)     //check for dhcp option
-        {
-            memset(Dnsv4IPConfig->v4DNSIP1,'\0',INET_ADDRSTRLEN);
-            memset(Dnsv4IPConfig->v4DNSIP2,'\0',INET_ADDRSTRLEN);
-            DNSv41=NULL;
-            DNSv42=NULL;
-        }
-        else
-        {
-            DNSv41 = (char *) Dnsv4IPConfig->v4DNSIP1;
-            DNSv42 = (char *) Dnsv4IPConfig->v4DNSIP2;
-        }
-    }
-    else
-    {
-            DNSv41=NULL;
-            DNSv42=NULL;    
-    }
-
-    /* Dnsv6IPConfig */
-    if( Dnsv6IPConfig != NULL )     //check for NULL
-    {
-        if(Dnsv6IPConfig->dhcpEnable == 1)     //check for dhcp option
-        {
-            memset(Dnsv6IPConfig->v6DNSIP1,'\0',INET6_ADDRSTRLEN);
-            memset(Dnsv6IPConfig->v6DNSIP2,'\0',INET6_ADDRSTRLEN);
-            DNSv61=NULL;
-            DNSv62=NULL;
-        }
-        else
-        {
-            DNSv61 = (char *) Dnsv6IPConfig->v6DNSIP1;
-            DNSv62 = (char *) Dnsv6IPConfig->v6DNSIP2;
-        }
-    }
-    else
-    {
-            DNSv61=NULL;
-            DNSv62=NULL;
-    }
-
-    retVal = WriteDNSConfFile( DomainConfig, Dnsv4IPConfig, Dnsv6IPConfig, registerBMC, registerFQDN);
-    
-    retVal= nwSetResolvConf_v4_v6(DNSv41,DNSv42,DNSv61,DNSv62,Domain);
+    /* DnsIPConfig */
+    if( DnsIPConfig != NULL )     //check for NULL
+    {
+        if(DnsIPConfig->DNSDHCP==1)     //check for dhcp option
+        {
+            memset(DnsIPConfig->DNSIP1,'\0',IP6_ADDR_LEN);
+            memset(DnsIPConfig->DNSIP2,'\0',IP6_ADDR_LEN);
+            memset(DnsIPConfig->DNSIP3,'\0',IP6_ADDR_LEN);
+        }
+    }
+
+    retVal = WriteDNSConfFile( DomainConfig, DnsIPConfig, regBMC_FQDN);
     if(retVal != 0)
     {
+        TCRIT("Error in update dns.conf file\n");
+    }
+
+    retVal= nwSetResolvConf_v4_v6((char *)&DnsIPConfig->DNSIP1,(char *)&DnsIPConfig->DNSIP2,(char *)&DnsIPConfig->DNSIP3,Domain);
+    if(retVal != 0)
+    {
         TCRIT("Error writing resolv.conf File..");
     }
 
-    /* Check whether DNS changes are belong to the single Interface.
-         If changes are belongs to single interface just do ifdown/ ifup for 
-         that single Interface. else just do ifdown/ifup for All the interfaces.
-    */
-  
-    if( singleNIC == 1 )
-    {
-        nwMakeIFUp(EthIndex);
-        TDBG(" ETH%d up is done........ ", EthIndex);        
-    }
-    else
-    {
-
-        GetNoofInterface();
-
-        /* Do network restart for All the available Interfaces */
-        TDBG(" ETH interface restart starts........");
-        
-        for(i=0; i<sizeof(Ifcnametable)/sizeof(IfcName_T); i++)
-        {
-            if(Ifcnametable[i].Enabled != 1)
-                continue;
-             nwMakeIFUp(Ifcnametable[i].Index);
-            TDBG(" eth%d up is done........ ",i);    
-        }
-
-        /* While restarting both the interfaces, the first interface (eth0) 
-        will not be registered properly to DNS, because of any one of the 
-        parameter (Domain name or DNS server IP) is missing at that time.
-        so just restart the first interface again to register with the DNS.
-        */
-        nwMakeIFDown(0);
-        TDBG(" eth0 DOWN is done........ ");    
-        nwMakeIFUp(0);
-        TDBG(" eth0 restart is done........ ");    
-
-    }
+    GetNoofInterface();
+
+    for(i=0; i<sizeof(Ifcnametable)/sizeof(IfcName_T); i++)
+    {
+        if(Ifcnametable[i].Enabled != 1)
+            continue;
+         nwMakeIFUp(Ifcnametable[i].Index);
+        TDBG(" eth%d up is done........ ",i);    
+    }
+
+    /* While restarting both the interfaces, the first interface (eth0) 
+    will not be registered properly to DNS, because of any one of the 
+    parameter (Domain name or DNS server IP) is missing at that time.
+    so just restart the first interface again to register with the DNS.
+
+    nwMakeIFDown(0);
+    TDBG(" eth0 DOWN is done........ ");
+    nwMakeIFUp(0);
+    TDBG(" eth0 restart is done........ ");*/
 
     return retVal;
 }
@@ -3008,7 +2888,7 @@
 *@param registerBMC - the array of register BMC flags with MAX LAN channel length 
 *@return Returns 0 on success and -1 on fails
 */
-int registerBMCstatus ( INT8U *registerBMC )
+int registerBMCstatus ( INT8U *regBMC_FQDN )
 {
     FILE *fp;
     char oneline[80];
@@ -3068,15 +2948,34 @@
                             //scan for the flag value
                             sscanf(ptr,"DO_DDNS=%s",ch);
 
-                            if((registerBMC != NULL) && (i<MAX_CHANNEL))
+                            if((regBMC_FQDN!= NULL) && (i<MAX_CHANNEL))
                             {
                                 if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0) )
                                 {
-                                    registerBMC[Ifcnametable[i].Index]=1;
+                                    regBMC_FQDN[Ifcnametable[i].Index]=1;
                                 }
                                 else
                                 {
-                                    registerBMC[Ifcnametable[i].Index]=0;
+                                    regBMC_FQDN[Ifcnametable[i].Index]=0;
+                                }
+                            }
+                        }
+                        /* Search for SET_FQDN flag */
+                        if((strstr(oneline,"SET_FQDN"))!= NULL)
+                        {
+                            ptr=strskip(oneline);
+                            //scan for the flag value
+                            sscanf(ptr,"SET_FQDN=%s",ch);
+
+                            if((regBMC_FQDN!= NULL) && (i<MAX_CHANNEL))
+                            {
+                                if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0) )
+                                {
+                                    regBMC_FQDN[Ifcnametable[i].Index] |=0x10;
+                                }
+                                else
+                                {
+                                    regBMC_FQDN[Ifcnametable[i].Index] |=0x00;
                                 }
                             }
                         }
@@ -3090,105 +2989,16 @@
     return 0;
 }
 
-/**
-*@fn registerFQDNstatus
-*@brief This function is invoked to Get the FQDN status with the DNS server.
-*@param registerBMC - the array of register FQDN flags with MAX LAN channel length 
-*@return Returns 0 on success and -1 on fails
-*/
-int registerFQDNstatus(INT8U *registerFQDN)
-{
-    FILE *fp;
-    char oneline[80];
-    char ch[10];
-    int i=0;
-    char iface[10];
-        
-    GetNoofInterface();
-
-    fp = fopen (CONFDNSCONF,"r");
-    if(fp ==NULL)
-    {
-        printf("\n cannot open %s File ..!! \n",CONFDNSCONF);
-        return -1;
-    }
-    while (!feof(fp))
-    {
-        if (fgets(oneline,79,fp) == NULL)
-        {
-            TDBG("EOF reading %s file\n",CONFDNSCONF);
-            fclose(fp);
-            return 0;
-        }
-
-        //a comment will be ignored by scanf
-        if( oneline[0] == '#') continue;
-
-        /* Read the DNS configurations by Interface wise */
-        for(i=0;i<sizeof(Ifcnametable)/sizeof(IfcName_T);i++)
-        {
-
-            if(Ifcnametable[i].Enabled != 1)
-                continue;
-
-            sprintf(iface,"%s",Ifcnametable[i].Ifcname);
-
-            // After finding the entry for one interface, try reading the configutaions for that iface
-            if(strstr(oneline,iface)!= NULL)
-            {
-                //printf("\n %s Entry Found..!! \n",iface);
-                while (!feof(fp))
-                {
-                    char *ptr;
-                    if (fgets(oneline,79,fp) != NULL)
-                    {
-                        if(strstr(oneline,";;")!= NULL)
-                        {
-                            //printf("\n Reading all entries done..!! \n");
-                            //End of the iface configuration is reached.. just braek
-                            break;
-                        }
-                        
-                        /* Search for DO_DDNS flag */
-                        if((strstr(oneline,"SET_FQDN"))!= NULL)
-                        {
-                            ptr=strskip(oneline);
-                            //scan for the flag value
-                            sscanf(ptr,"SET_FQDN=%s",ch);
-
-                            if((registerFQDN!= NULL) && (i<MAX_CHANNEL))
-                            {
-                                if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0) )
-                                {
-                                    registerFQDN[Ifcnametable[i].Index]=1;
-                                }
-                                else
-                                {
-                                    registerFQDN[Ifcnametable[i].Index]=0;
-                                }
-                            }
-                        }
-                     }
-                }
-            }
-        }
-    }
-
-    fclose(fp);
-    return 0;
-}
 
 /**
 *@fn ReadDNSConfFile
 *@brief This function is invoked to Get all the DNS related Configurations from dns.conf file
 *@param DomainConfig - Pointer to Structure used to read Domain configurations
-*@param Dnsv4IPConfigr - pointer to structure used to read IPv4 DNS server IP Configurations
-*@param Dnsv6IPConfigr - pointer to structure used to read IPv6 DNS server IP Configurations
-*@param registerBMC - the array of register BMC flags with MAX LAN channel length 
+*@param DnsIPConfig - pointer to structure used to read DNS server IP Configurations
+*@param regBMC_FQDN - the array of register BMC and FQDN method flags with MAX LAN channel length 
 *@return Returns 0 on success and -1 on fails
 */
-int ReadDNSConfFile ( DOMAINCONF *DomainConfig, DNSIPV4CONF *Dnsv4IPConfig, DNSIPV6CONF *Dnsv6IPConfig, 
-					  INT8U *registerBMC , INT8U *registerFQDN)
+int ReadDNSConfFile ( DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig, INT8U *regBMC_FQDN )
 {
     FILE *fp;
     char oneline[80];
@@ -3251,15 +3061,15 @@
                             //scan for the flag value
                             sscanf(ptr,"DO_DDNS=%s",ch);
 
-                            if((registerBMC != NULL) && (i<MAX_CHANNEL))
+                            if((regBMC_FQDN != NULL) && (i<MAX_CHANNEL))
                             {
                                 if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0) )
                                 {
-                                    registerBMC[Ifcnametable[i].Index]=1;
+                                    regBMC_FQDN[Ifcnametable[i].Index]=1;
                                 }
                                 else
                                 {
-                                    registerBMC[Ifcnametable[i].Index]=0;
+                                    regBMC_FQDN[Ifcnametable[i].Index]=0;
                                 }
                             }
                         }
@@ -3270,15 +3080,15 @@
                             //scan for the flag value
                             sscanf(ptr,"SET_FQDN=%s",ch);
 
-                            if((registerFQDN != NULL) && (i<MAX_CHANNEL))
+                            if((regBMC_FQDN != NULL) && (i<MAX_CHANNEL))
                             {
                                 if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0) )
                                 {
-                                    registerFQDN[Ifcnametable[i].Index]=1;
+                                    regBMC_FQDN[Ifcnametable[i].Index] |= 0x10;
                                 }
                                 else
                                 {
-                                    registerFQDN[Ifcnametable[i].Index]=0;
+                                    regBMC_FQDN[Ifcnametable[i].Index] |= 0x00;
                                 }
                             }
                         }
@@ -3290,20 +3100,20 @@
                             //scan for the flag value
                             sscanf(ptr,"SET_DNS=%s",ch);
 
-                            if( Dnsv4IPConfig != NULL )
+                            if( DnsIPConfig != NULL )
                             {
                                 if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0) )
                                 {
-                                    // DNS v4 is dhcp
-                                    Dnsv4IPConfig->dhcpEnable=1;
-                                    Dnsv4IPConfig->EthIndex=Ifcnametable[i].Index;
+                                    // DNS is dhcp
+                                    DnsIPConfig->DNSDHCP=1;
+                                    DnsIPConfig->DNSIndex=Ifcnametable[i].Index;
                                 }
                                 else
                                 {
-                                    if(Dnsv4IPConfig->dhcpEnable != 1)  // If Its already marked as dhcp dont change it.
+                                    if(DnsIPConfig->DNSDHCP != 1)  // If Its already marked as dhcp dont change it.
                                     {
-                                        Dnsv4IPConfig->dhcpEnable=0;
-                                        Dnsv4IPConfig->EthIndex=Ifcnametable[i].Index;
+                                        DnsIPConfig->DNSDHCP=0;
+                                        DnsIPConfig->DNSIndex=0;
                                     }
                                 }
                             }
@@ -3331,7 +3141,7 @@
                                     if( DomainFound != 1)    // If Its already marked as dhcp dont change it.
                                     {
                                         DomainConfig->dhcpEnable=0;
-                                        DomainConfig->EthIndex=Ifcnametable[i].Index;
+                                        DomainConfig->EthIndex=0;
                                         DomainConfig->v4v6=0;
                                     }
                                 }
@@ -3339,26 +3149,31 @@
                         }
 
                         /* Search for SET_v6DNS flag */
-                        else if(strstr(oneline,"SET_v6DNS")!=NULL)
+                        else if(strstr(oneline,"SET_IPV6_PRIORITY")!=NULL)
                         {
                             ptr=strskip(oneline);
                             //scan for the flag value
-                            sscanf(ptr,"SET_v6DNS=%s",ch);
-
-                            if( Dnsv6IPConfig != NULL )
+                            sscanf(ptr,"SET_IPV6_PRIORITY=%s",ch);
+
+                            if( DnsIPConfig != NULL )
                             {
-                                if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0) )
+                                if(DnsIPConfig->DNSIndex == Ifcnametable[i].Index)
                                 {
-                                    // DNS v6 is dhcp
-                                    Dnsv6IPConfig->dhcpEnable=1;
-                                    Dnsv6IPConfig->EthIndex=Ifcnametable[i].Index;
-                                }
-                                else
-                                {
-                                    if(Dnsv6IPConfig->dhcpEnable != 1)    // If Its already marked as dhcp dont change it.
+                                    if( (strcmp(ch,"y")==0) || (strcmp(ch,"yes")==0))
                                     {
-                                        Dnsv6IPConfig->dhcpEnable=0;
-                                        Dnsv6IPConfig->EthIndex=Ifcnametable[i].Index;
+                                        // DNS v6 is dhcp
+                                        DnsIPConfig->IPPriority = 2;
+                                    }
+                                    else
+                                    {
+                                        if(DnsIPConfig->DNSDHCP == 1)
+                                        {
+                                            DnsIPConfig->IPPriority = 1;
+                                        }
+                                        else
+                                        {
+                                            DnsIPConfig->IPPriority = 0;
+                                        }
                                     }
                                 }
                             }
@@ -3386,7 +3201,7 @@
                                     if( DomainFound != 1) // If Its already marked as dhcp dont change it.
                                     {
                                         DomainConfig->dhcpEnable=0;
-                                        DomainConfig->EthIndex=Ifcnametable[i].Index;
+                                        DomainConfig->EthIndex=0;
                                         DomainConfig->v4v6=0;
                                     }
                                 }
@@ -3413,8 +3228,7 @@
 *@param registerBMC - the array of register BMC flags with MAX LAN channel length 
 *@return Returns 0 on success and -1 on fails
 */
-int WriteDNSConfFile ( DOMAINCONF *DomainConfig, DNSIPV4CONF *Dnsv4IPConfig,
-					   DNSIPV6CONF *Dnsv6IPConfig, INT8U *registerBMC,INT8U *registerFQDN)
+int WriteDNSConfFile ( DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig, INT8U *regBMC_FQDN)
 {
     FILE *fp;
     int i=0,ret=0;
@@ -3437,10 +3251,11 @@
     fprintf(fp,"\n#!/bin/sh\n\n");
 
     fprintf(fp," DO_DDNS=%s\n",no);
+    fprintf(fp," SET_FQDN=%s\n",no);
     fprintf(fp," SET_DOMAIN=%s\n",no);
     fprintf(fp," SET_v6DOMAIN=%s\n",no);
     fprintf(fp," SET_DNS=%s\n",no);
-    fprintf(fp," SET_v6DNS=%s\n",no);
+    fprintf(fp," SET_IPV6_PRIORITY=%s\n",no);
 
 	fprintf(fp,"\ncase ${IFACE} in\n\n");
  
@@ -3461,13 +3276,13 @@
          */
 
         /* HOST NAME register start*/
-        if((registerBMC != NULL) && (i<MAX_CHANNEL))
-        {
-            if(registerBMC[Ifcnametable[i].Index] == 0)
+        if((regBMC_FQDN!= NULL) && (i<MAX_CHANNEL))
+        {
+            if((regBMC_FQDN[Ifcnametable[i].Index] & 0x01) == 0x00)
             {
                 fprintf(fp,"\t\tDO_DDNS=%s\n",no);
             }
-            else if (registerBMC[Ifcnametable[i].Index] == 1)
+            else if ((regBMC_FQDN[Ifcnametable[i].Index] & 0x01) == 0x01)
             {
                 fprintf(fp,"\t\tDO_DDNS=%s\n",yes);
             }
@@ -3479,13 +3294,13 @@
         /* HOST NAME register done */
 
         /* FQDN register start*/
-        if((registerFQDN != NULL) && (i<MAX_CHANNEL))
-        {
-            if(registerFQDN[Ifcnametable[i].Index] == 0)
+        if((regBMC_FQDN != NULL) && (i<MAX_CHANNEL))
+        {
+            if((regBMC_FQDN[Ifcnametable[i].Index] & 0x10) == 0x00)
             {
                 fprintf(fp,"\t\tSET_FQDN=%s\n",no);
             }
-            else if (registerFQDN[Ifcnametable[i].Index] == 1)
+            else if ((regBMC_FQDN[Ifcnametable[i].Index] & 0x10) == 0x10)
             {
                 fprintf(fp,"\t\tSET_FQDN=%s\n",yes);
             }
@@ -3501,6 +3316,7 @@
             if(DomainConfig->dhcpEnable == 0)
             {
                 fprintf(fp,"\t\tSET_DOMAIN=%s\n",no);
+                fprintf(fp,"\t\tSET_v6DOMAIN=%s\n",no);
             }
             else if (DomainConfig->dhcpEnable == 1)
             {
@@ -3536,16 +3352,16 @@
         }
         /* DOMAIN NAME done */
         
-        /* DNS v4 Server IP */
-        if( Dnsv4IPConfig != NULL )
-        {
-            if(Dnsv4IPConfig->dhcpEnable == 0)
+        /* DNS Server IP */
+        if( DnsIPConfig != NULL )
+        {
+            if(DnsIPConfig->DNSDHCP== 0)
             {
                 fprintf(fp,"\t\tSET_DNS=%s\n",no);
             }
-            else if(Dnsv4IPConfig->dhcpEnable == 1)
-            {
-                if(Dnsv4IPConfig->EthIndex == Ifcnametable[i].Index)
+            else if(DnsIPConfig->DNSDHCP== 1)
+            {
+                if(DnsIPConfig->DNSIndex == Ifcnametable[i].Index)
                 {
                     fprintf(fp,"\t\tSET_DNS=%s\n",yes); 
                 }
@@ -3562,27 +3378,27 @@
         /* DNS v4 Server IP setting done here */
         
         /* DNS v6 Server IP */
-        if( Dnsv6IPConfig != NULL)
-        {
-            if(Dnsv6IPConfig->dhcpEnable == 0)
-            {
-                fprintf(fp,"\t\tSET_v6DNS=%s\n",no);
-            }
-            else if(Dnsv6IPConfig->dhcpEnable == 1)
-            {
-                if(Dnsv6IPConfig->EthIndex == Ifcnametable[i].Index)
+        if( DnsIPConfig != NULL)
+        {
+            if(DnsIPConfig->DNSDHCP == 0)
+            {
+                fprintf(fp,"\t\tSET_IPV6_PRIORITY=%s\n",no);
+            }
+            else if(DnsIPConfig->DNSDHCP == 1)
+            {
+                if(DnsIPConfig->IPPriority == 2 && DnsIPConfig->DNSIndex == Ifcnametable[i].Index)
                 {
-                    fprintf(fp,"\t\tSET_v6DNS=%s\n",yes); 
+                    fprintf(fp,"\t\tSET_IPV6_PRIORITY=%s\n",yes); 
                 }
                 else
                 {
-                    fprintf(fp,"\t\tSET_v6DNS=%s\n",no); 
+                    fprintf(fp,"\t\tSET_IPV6_PRIORITY=%s\n",no);
                 }
             }
         }
         else
         {
-            fprintf(fp,"\t\tSET_v6DNS=%s\n",no); 
+            fprintf(fp,"\t\tSET_IPV6_PRIORITY=%s\n",no); 
         }
         /* DNS v4 Server IP setting done here */
         
@@ -3598,8 +3414,8 @@
     ret = moveFile(CONFDNSCONF_TMP,CONFDNSCONF);
     if(ret != 0)	//Replaced system calls
     {
-    	TCRIT("Error moving %s config to %sconfig\n", CONFDNSCONF_TMP, CONFDNSCONF );
-    	return -1;
+        TCRIT("Error moving %s config to %sconfig\n", CONFDNSCONF_TMP, CONFDNSCONF );
+        return -1;
     }
 
 return 0;
@@ -3614,121 +3430,161 @@
 *@brief This function is used to Write Domain name and DNS server IP's for Both IPv4 and IPv6 to resolv.conf file
 *@param dns1 - Pointer to char that holds first IPv4 DNS server IP
 *@param dns2 - Pointer to char that holds second IPv4 DNS server IP
-*@param dnsv6_1 - Pointer to char that holds first IPv6 DNS server IP
-*@param dnsv6_2 - Pointer to char that holds second IPv6 DNS server IP
+*@param dns3 - Pointer to char that holds first IPv6 DNS server IP
 *@return Returns 0 on success and -1 on fails
 */
-int nwSetResolvConf_v4_v6(char* dns1,char* dns2,char* dnsv6_1,char* dnsv6_2,char* domain)
-{
-	FILE* fpout;
-	int namect = 0;
-	char oneline[ONELINE_LEN];
-	int ret = 0;
-    
-	//check validity of IP Addresses
-	//only if the dns addresses are not null.
-	// /if they are null just ignore them
-	if(dns1 != NULL)
-	{
-		if(!isValidIp(dns1))
-		{
-			TCRIT("invalid IP address given for dns1\n");
-			return -1;
-		}
-	}
-
-	if(dns2 != NULL)
-	{
-		if(!isValidIp(dns2))
-		{
-			TCRIT("invalid IP address given for dns2\n");
-			return -1;
-		}
-	}
-
-       /* Remove all the old entries from resolv.conf and write new entries */
-
-	fpout = fopen(RESOLV_CONF_FILE_TEMP,"w");
-	if(fpout == NULL)
-	{
-		TCRIT("Errore opening temp resolv conf for writing\n");
-		return -1;
-	}
-
-        /* Write the Domain name first */
-        /* if domain name is NULL or not present just ignore */
-        if ((domain != NULL) && (strcmp(domain,"") != 0))
-        {
-            TDBG("domain name is given as input");
-            sprintf(oneline,"search %s\n",domain);
-            fputs(oneline,fpout);
+int nwSetResolvConf_v4_v6(char* dns1,char* dns2,char* dns3,char* domain)
+{
+    FILE* fpout;
+    int namect = 0;
+    char oneline[ONELINE_LEN];
+    int ret = 0;
+    INT8U   IPv4Addr[IP_ADDR_LEN];
+    char   DNSAddr[INET6_ADDRSTRLEN];
+
+    /* Remove all the old entries from resolv.conf and write new entries */
+    fpout = fopen(RESOLV_CONF_FILE_TEMP,"w");
+    if(fpout == NULL)
+    {
+        TCRIT("Errore opening temp resolv conf for writing\n");
+        return -1;
+    }
+
+    /* Write the Domain name first */
+    /* if domain name is NULL or not present just ignore */
+    if ((domain != NULL) && (strcmp(domain,"") != 0))
+    {
+        TDBG("domain name is given as input");
+        sprintf(oneline,"search %s\n",domain);
+        fputs(oneline,fpout);
+    }
+    else
+    {
+        TDBG("No domain information present.. error in setting domain name");
+        strncpy(oneline,"",1);
+    }
+
+    /* write All the DNS server IP's. If the server IP is NULL just ignore it */
+    if(namect == 0)
+    {
+        if(dns1 != NULL)
+        {
+            if(IN6_IS_ADDR_V4MAPPED(dns1))
+            {
+                memset(IPv4Addr,0,IP_ADDR_LEN);
+                memcpy(IPv4Addr,&dns1[IP6_ADDR_LEN - IP_ADDR_LEN],IP_ADDR_LEN);
+                memset(DNSAddr,0,INET6_ADDRSTRLEN);
+                if(inet_ntop(AF_INET,IPv4Addr,(char*)DNSAddr,INET_ADDRSTRLEN) == NULL)
+                {
+                    printf("Invalid DNS Address\n");
+                }
+                else if(strcmp(DNSAddr,"0.0.0.0") != 0)
+                {
+                    sprintf(oneline,"nameserver %s\n",DNSAddr);
+                    fputs(oneline,fpout);
+                }
+            }
+            else
+            {
+                if(inet_ntop(AF_INET6,(char*)dns1,(char*)DNSAddr,INET6_ADDRSTRLEN) == NULL)
+                {
+                    printf("Invalid DNS IPv6 Address");
+                }
+                else if(strcmp(DNSAddr,"::") != 0)
+                {
+                    sprintf(oneline,"nameserver %s\n",DNSAddr);
+                    fputs(oneline,fpout);
+                }
+            }
         }
         else
         {
-            TDBG("No domain information present.. error in setting domain name");
-            strncpy(oneline,"",1);
-        }
-
-        /* write All the DNS server IP's. If the server IP is NULL just ignore it */
-        if(namect == 0)
-        {
-            if(dns1 != NULL)
-            {
-               sprintf(oneline,"nameserver %s\n",dns1);
-               fputs(oneline,fpout);
+            strncpy(oneline,"",1); //clear if dns1 name is NULL
+        }
+        namect++;
+    }
+
+    if(namect == 1)
+    {
+        //only if dns 2 is not null change it
+        if(dns2 != NULL)
+        {
+            if(IN6_IS_ADDR_V4MAPPED(dns2))
+            {
+                memset(IPv4Addr,0,IP_ADDR_LEN);
+                memcpy(IPv4Addr,&dns2[IP6_ADDR_LEN - IP_ADDR_LEN],IP_ADDR_LEN);
+                memset(DNSAddr,0,INET6_ADDRSTRLEN);
+                if(inet_ntop(AF_INET,IPv4Addr,(char*)DNSAddr,INET_ADDRSTRLEN) == NULL)
+                {
+                    printf("Invalid DNS Address\n");
+                }
+                else if(strcmp(DNSAddr,"0.0.0.0") != 0)
+                {
+                    sprintf(oneline,"nameserver %s\n",DNSAddr);
+                    fputs(oneline,fpout);
+                }
             }
             else
             {
-                strncpy(oneline,"",1); //clear if dns1 name is NULL
-            }
-            namect++;
-        }
-        
-        if(namect == 1)
-        {
-            //only if dns 2 is not null change it
-            if(dns2 != NULL)
-            {
-                sprintf(oneline,"nameserver %s\n",dns2);
-                fputs(oneline,fpout);
+                if(inet_ntop(AF_INET6,(char*)dns2,(char*)DNSAddr,INET6_ADDRSTRLEN) == NULL)
+                {
+                    printf("Invalid DNS IPv6 Address");
+                }
+                else if(strcmp(DNSAddr,"::") != 0)
+                {
+                    sprintf(oneline,"nameserver %s\n",DNSAddr);
+                    fputs(oneline,fpout);
+                }
+            }
+
+        }
+        else
+        {
+            strncpy(oneline,"",1); //clear if dns2 name is NULL
+        }
+        namect++;
+    }
+
+    if(namect == 2)
+    {
+        if(dns3 != NULL)
+        {
+            if(IN6_IS_ADDR_V4MAPPED(dns3))
+            {
+                memset(IPv4Addr,0,IP_ADDR_LEN);
+                memcpy(IPv4Addr,&dns3[IP6_ADDR_LEN - IP_ADDR_LEN],IP_ADDR_LEN);
+                memset(DNSAddr,0,INET6_ADDRSTRLEN);
+                if(inet_ntop(AF_INET,IPv4Addr,(char*)DNSAddr,INET_ADDRSTRLEN) == NULL)
+                {
+                    printf("Invalid DNS Address\n");
+                }
+                else if(strcmp(DNSAddr,"0.0.0.0") != 0)
+                {
+                    sprintf(oneline,"nameserver %s\n",DNSAddr);
+                    fputs(oneline,fpout);
+                }
             }
             else
             {
-                strncpy(oneline,"",1); //clear if dns2 name is NULL
-            }
-            namect++;
-        }
-        
-        if(namect == 2)
-        {
-            //only if dns 2 is not null change it
-            if(dnsv6_1 != NULL && (strcmp(dnsv6_1,"::") != 0))
-            {
-                sprintf(oneline,"nameserver %s\n",dnsv6_1);
-                fputs(oneline,fpout);                    
-            }
-            else
-            {
-                strncpy(oneline,"",1); //clear if dnsv6_1 name is NULL
-            }
-            namect++;
-        }
-        
-        if(namect == 3)
-        {
-            if(dnsv6_2 != NULL && (strcmp(dnsv6_2,"::") != 0))
-            {
-                sprintf(oneline,"nameserver %s\n",dnsv6_2);
-                fputs(oneline,fpout);                    
-            }
-            else
-            {
-                strncpy(oneline,"",1); //clear if dnsv6_2 name is NULL
-            }
-            namect++;
-        }           
-            
-       fclose(fpout);
+                if(inet_ntop(AF_INET6,dns3,(char*)DNSAddr,INET6_ADDRSTRLEN) == NULL)
+                {
+                    printf("Invalid DNS IPv6 Address");
+                }
+                else if(strcmp(DNSAddr,"::") != 0)
+                {
+                    sprintf(oneline,"nameserver %s\n",DNSAddr);
+                    fputs(oneline,fpout);
+                }
+            }
+        }
+        else
+        {
+            strncpy(oneline,"",1); //clear if dnsv6_1 name is NULL
+        }
+        namect++;
+    }
+
+   fclose(fpout);
 
     ret = moveFile(RESOLV_CONF_FILE_TEMP,RESOLV_CONF_FILE);
     if(ret != 0)	//Replaced system calls
@@ -4224,8 +4080,8 @@
 		}
         if(unlink(oldFile) != 0)
         {
-                 TCRIT("%s not removed",oldFile);
-                return -1;
+			TCRIT("%s not removed",oldFile);
+			return -1;
         }
         return 0;
 }
