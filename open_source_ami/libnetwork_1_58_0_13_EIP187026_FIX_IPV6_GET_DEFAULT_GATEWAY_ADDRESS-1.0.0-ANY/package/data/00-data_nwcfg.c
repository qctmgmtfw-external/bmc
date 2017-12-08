--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c	Tue Oct  7 13:59:19 2014
+++ source/libnetwork-1.58.0-src/data/nwcfg.c	Tue Oct  7 14:14:54 2014
@@ -883,7 +883,7 @@
 	/* We assume that the interface is eth0. It however can be
 	changed to any interface type, we can take that interface type
 	as an input parameter */
-	sprintf(FullEth_Network_if,"%s",Ifcnametable[*Interface].Ifcname);
+	snprintf(FullEth_Network_if,MAX_STR_LENGTH,"%s",Ifcnametable[*Interface].Ifcname); 
 
 	while(!feof(ipv6_routeFp))
 	{
@@ -895,7 +895,7 @@
 			continue;
 		}
 
-		if(strcmp(tempstr,"\0")) // Skip empty lines
+		if(strncmp(tempstr,"\0",STR1_LEN)) // Skip empty lines 
 		{
 			sscanf(tempstr,"%32s %02x %32s %02x %32s %08x %08x %08x %08x %s\n",
 				otherstr,	// Destination network
@@ -909,8 +909,9 @@
 				&flags,		// Flags
 				iface);		// Interface
 
-			/* Default gateway entry has the flag field set to 0x0003 */
-			if((flags == DEFAULT_GW_FLAGS) && (strcmp(iface,(char *) FullEth_Network_if) == 0))
+			/* Default gateway entry has the flag field set to 0x0002 i.e(RTF_GATEWAY)
+			RTF_GATEWAY-0x0002 - destination is a gateway*/ 
+			if(((flags & RTF_GATEWAY) == RTF_GATEWAY) && (strncmp(iface,(char *) FullEth_Network_if,strlen(FullEth_Network_if)) == 0)) 
 			{
 				i = 0; j = 0;
 				while (i < 32)
@@ -928,6 +929,7 @@
 				if (inet_pton(AF_INET6, (char*) addrstr, gw) == 0)
 				{
 					TCRIT("Error converting v6 gateway IP address\n");
+					fclose(ipv6_routeFp); 
                                    return ENODATA; /* No default entry */
 				}
 
