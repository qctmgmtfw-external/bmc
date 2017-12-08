--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c	Thu Aug 14 14:33:28 2014
+++ source/libnetwork-1.58.0-src/data/nwcfg.c	Thu Aug 14 15:00:19 2014
@@ -4214,8 +4214,10 @@
     int i=0;
     struct ifaddrs *ifAddrOrig = 0;
     struct ifaddrs *ifAddrsP = 0;
+    char ifcname[IFNAMSIZ] = {0};
 
     GetNoofInterface();
+    GetNwCfgInfo();
 
     if (getifaddrs (&ifAddrOrig) == 0)
     {
@@ -4226,7 +4228,20 @@
             {
                     for(i =0;i<sizeof(Ifcnametable)/sizeof(IfcName_T);i++)
                     {
-                        if((strcmp(Ifcnametable[i].Ifcname,ifAddrsP->ifa_name) == 0)  && (memcmp(((struct sockaddr_in6*)(ifAddrsP->ifa_addr))->sin6_addr.s6_addr,IPAddr,IP6_ADDR_LEN) == 0))
+                    	if(Ifcnametable[i].Ifcname[0] == 0)
+                    	{
+                    		continue;
+                    	}
+                        
+                    	if(m_NwCfgInfo[Ifcnametable[i].Index].VLANID)
+                        {
+                            sprintf(ifcname,"%s.%d",Ifcnametable[i].Ifcname,m_NwCfgInfo[Ifcnametable[i].Index].VLANID);
+                        }
+                        else
+                        {
+                            strcpy(ifcname,Ifcnametable[i].Ifcname);
+                        }
+                        if((strcmp(ifcname,ifAddrsP->ifa_name) == 0)  && (memcmp(((struct sockaddr_in6*)(ifAddrsP->ifa_addr))->sin6_addr.s6_addr,IPAddr,IP6_ADDR_LEN) == 0))
                         {
                             freeifaddrs(ifAddrOrig);
                             return Ifcnametable[i].Index;
