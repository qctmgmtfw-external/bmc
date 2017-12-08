--- .pristine/libipmilan-2.29.0-src/data/LANIfc.c	Thu Aug 14 14:33:33 2014
+++ source/libipmilan-2.29.0-src/data/LANIfc.c	Thu Aug 14 15:00:07 2014
@@ -246,6 +246,12 @@
                         pBMCInfo->LANConfig.VLANUDPSocket[j] = -1;
                         pBMCInfo->LANConfig.VLANIFcheckFlag[j] = 0;
                     }
+                    if(pBMCInfo->LANConfig.VLANTCPSocket[j] != -1)
+                    {
+                        shutdown(pBMCInfo->LANConfig.VLANTCPSocket[j],SHUT_RDWR);
+                        close(pBMCInfo->LANConfig.VLANTCPSocket[j]);
+                        pBMCInfo->LANConfig.VLANTCPSocket[j] = -1;
+                    }
                 }
             }
         }
@@ -341,6 +347,8 @@
     BMC = BMCInst;
     /* Initialize the elements of VLAN socket array to -1 */ 
     memset(pBMCInfo->LANConfig.VLANUDPSocket, -1, sizeof(SOCKET) * MAX_LAN_CHANNELS); 
+    
+    memset(pBMCInfo->LANConfig.VLANTCPSocket, -1, sizeof(SOCKET) * MAX_LAN_CHANNELS); 
 
     memset(pBMCInfo->LANConfig.UDPSocket, -1,sizeof(SOCKET) * MAX_LAN_CHANNELS);
 
@@ -890,6 +898,8 @@
         {
             if(pBMCInfo->LANConfig.TCPSocket[x] != -1)
                 FD_SET (pBMCInfo->LANConfig.TCPSocket[x], fd);
+            if(pBMCInfo->LANConfig.VLANTCPSocket[x] != -1)
+            	FD_SET(pBMCInfo->LANConfig.VLANTCPSocket[x],fd);
         }
     } 
 
@@ -1175,6 +1185,10 @@
             {
                 *n = (UTIL_MAX (*n, pBMCInfo->LANConfig.TCPSocket[x])) ;
             }
+             if(pBMCInfo->LANConfig.VLANTCPSocket[x] != -1)
+             {
+            	 *n = (UTIL_MAX(*n,pBMCInfo->LANConfig.VLANTCPSocket[x]));
+             }
         }
     }
 
@@ -1317,6 +1331,32 @@
                             }
                         }
                     }
+                    if(pBMCInfo->LANConfig.VLANTCPSocket[i] != -1)
+                    {
+                    	if (FD_ISSET (pBMCInfo->LANConfig.VLANTCPSocket[i], &fdRead))
+                    	{
+                            SourceLen = sizeof(Source);
+                            /*Accept new TCP connections */
+                            NewTCPSocket = accept(pBMCInfo->LANConfig.VLANTCPSocket[i],(struct sockaddr *)&Source, &SourceLen );
+                            if (-1 == NewTCPSocket)
+                            {
+                            	IPMI_WARNING ("LANIfc.c : Error accepting connections for BMCInst %d\n", BMCInst);
+                            	continue; //addednow
+                            }
+                            /* Add the socket to the table */
+                            if(0 == AddSocket (NewTCPSocket, IsloopBackStatus, FALSE, BMCInst))
+                            {
+                            	 /* Add the new TCP client to set */
+                            	 FD_SET (NewTCPSocket, &fdRead);
+                            	 n = (NewTCPSocket >= n) ? NewTCPSocket + 1 : n;
+                            }
+                            else
+                            {
+                                IPMI_WARNING ("LANIfc.c : Closing socket\n");
+                                close (NewTCPSocket);
+                            }
+                    	}
+                    }
             }
         }
 
@@ -1630,6 +1670,12 @@
             close(pBMCInfo->LANConfig.VLANUDPSocket[Index]);
             pBMCInfo->LANConfig.VLANUDPSocket[Index] = -1;
         }
+        if(pBMCInfo->LANConfig.VLANTCPSocket[Index] != -1)
+        {
+        	shutdown(pBMCInfo->LANConfig.VLANTCPSocket[Index],SHUT_RDWR);
+        	close(pBMCInfo->LANConfig.VLANTCPSocket[Index]);
+            pBMCInfo->LANConfig.VLANTCPSocket[Index] = -1;
+        }
 
 
         if(pBMCInfo->LANConfig.VLANID[Index] != 0)
@@ -1706,6 +1752,68 @@
                         close(pBMCInfo->LANConfig.UDPSocket[Index]);
                         pBMCInfo->LANConfig.UDPSocket[Index] = -1;
                         TCRIT("LANIfc.c: LAN UDP sockets closed successfully %d\n",Index);
+                    }
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_IPMI_IPV6
+                    VLANSockaddr.sin6_family        = AF_INET6;
+                    inet_pton(AF_INET6, "0::0", &VLANSockaddr.sin6_addr);
+                    VLANSockaddr.sin6_port          = htons( LAN_RMCP_PORT);
+                    VLANSockaddr.sin6_flowinfo=0;
+                    VLANSockaddr.sin6_scope_id=0;
+
+                    /* Initialize The VLAN Socket */
+                    pBMCInfo->LANConfig.VLANTCPSocket[Index] = socket (AF_INET6, SOCK_STREAM, 0);
+#else
+                    VLANSockaddr.sin_family        = AF_INET;
+                    VLANSockaddr.sin_addr.s_addr   = INADDR_ANY;
+                    VLANSockaddr.sin_port          = htons( LAN_RMCP_PORT);
+
+                    /* Initialize The VLAN Socket */
+                    pBMCInfo->LANConfig.VLANTCPSocket[Index] = socket (AF_INET, SOCK_STREAM, 0);
+#endif
+                    if ( pBMCInfo->LANConfig.VLANTCPSocket[Index] == -1)
+                    {
+                        IPMI_ERROR ("LANIfc.c : Unable to create VLAN UDP socket\n");
+                        return 1;
+                    }
+
+                    IPMI_DBG_PRINT_1 ("VLAN UDP Socket = 0x%x\n", pBMCInfo->LANConfig.VLANTCPSocket[Index]);
+
+                    if ( 0 != setsockopt (pBMCInfo->LANConfig.VLANTCPSocket[Index], SOL_SOCKET, SO_BINDTODEVICE, VLANUDPInterface, strlen(VLANUDPInterface)+1) )
+                    {
+                        IPMI_ERROR ("LANIfc.c: SetSockOpt Failed for VLAN UDP Socket");
+                        return -1;
+                    }
+
+                    if (setsockopt(pBMCInfo->LANConfig.VLANTCPSocket[Index], SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1)
+                    {
+                        IPMI_ERROR("LANIfc.c: Setsockopt(SO_REUSEADDR) Failed for VLAN socket\n");
+                    }
+
+                    if (setsockopt(pBMCInfo->LANConfig.VLANTCPSocket[Index], SOL_SOCKET, SO_PRIORITY, &VLANpriority, sizeof(VLANpriority)) == -1)
+                    {
+                        IPMI_ERROR("LANIfc.c: Setsockopt(SO_PRIORITY) Failed for VLAN socket\n");
+                    }
+
+                    /* Bind VLAN TCPSocket to Port, INET address */
+                    if (bind( pBMCInfo->LANConfig.VLANTCPSocket[Index], (struct sockaddr *)&VLANSockaddr, sizeof(VLANSockaddr)) == -1)
+                    {
+                        IPMI_ERROR  ("LANIfc.c : Error binding VLAN UDP socket, %d, %s\n", errno, strerror(errno));
+                        return -1;
+                    }
+
+                    IPMI_DBG_PRINT_1("VLAN TCP socket is created %d\n",pBMCInfo->LANConfig.VLANTCPSocket[Index]);
+                    if (listen(pBMCInfo->LANConfig.VLANTCPSocket[Index], pBMCInfo->IpmiConfig.MaxSession) == -1)
+                    {
+                        IPMI_ERROR ("LANIfc.c : Error listen\n");
+                        return -1;
+                    }
+
+                    if(pBMCInfo->LANConfig.TCPSocket[Index] != -1)
+                    {
+                        shutdown(pBMCInfo->LANConfig.TCPSocket[Index],SHUT_RDWR);
+                        close(pBMCInfo->LANConfig.TCPSocket[Index]);
+                        pBMCInfo->LANConfig.TCPSocket[Index] = -1;
+                        TCRIT("LANIfc.c: LAN TCP sockets closed successfully %d\n",Index);
                     }
 
                     break;
