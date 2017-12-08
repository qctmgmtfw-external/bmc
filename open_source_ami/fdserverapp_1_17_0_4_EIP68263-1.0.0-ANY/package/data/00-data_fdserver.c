--- .pristine/fdserverapp-1.17.0-src/data/fdserver.c Mon Oct 10 16:31:54 2011
+++ source/fdserverapp-1.17.0-src/data/fdserver.c Tue Oct 11 11:02:01 2011
@@ -82,6 +82,7 @@
 static int gPipePending[MAX_FD_INSTANCES] = { 0, 0, 0, 0 };
 static int gNumFdInstances = 0;
 static SOCKET gMasterSock = -1;		   		/* socket descriptor */
+static SOCKET gLoopBackSock = -1;
 static int usbfd;							/* Usb Descriptor */
 static int gWasResponseKAR[MAX_FD_INSTANCES] = { 0, 0, 0, 0 }; 	/* detects if response was a keep alive respons from remote */
 static int gRRTmoutCt[MAX_FD_INSTANCES] = { 0, 0, 0, 0 }; /*remote response tmout count for KA or actual cmds */
@@ -450,7 +451,6 @@
 	} 
 	else 
 	{
-	
 		if(isNotEditable((unsigned char *)&(MediaConf.CurrentActiveSession),sizeof(MediaConf.CurrentActiveSession)) )
 		{
 			getNotEditableData((unsigned char *)&MediaConf.CurrentActiveSession/*IN*/,sizeof(MediaConf.CurrentActiveSession)/*IN*/, (unsigned char *)&activesession/*OUT*/);
@@ -864,6 +864,13 @@
 	struct timeval timeout;
 	uint8 InstanceRcvd;
 	char	token[MAX_TOKEN_LEN+1];
+	VMediaCfg_T cfgVMedia;
+	
+	if (GetVMediaCfg(&cfgVMedia) != 0)
+	{
+		TEMERG("Unable to Read VMedia configuration. Closing FD server\n");
+		return 1;
+	}
 
 
 	for (InstanceIx = 0; InstanceIx < gNumFdInstances; InstanceIx++)
@@ -890,6 +897,8 @@
 		n = -1;
 		FD_ZERO(&readfds);
 		FD_SET(gMasterSock,&readfds);
+		if (gLoopBackSock != -1)
+			FD_SET(gLoopBackSock,&readfds);
 		FD_SET(pipefd[0],&readfds);
 				
 		if( reset_event_pipe != -1)
@@ -900,6 +909,8 @@
 		
 		n = (n > pipefd[0])?n:pipefd[0];
 		n = (n > gMasterSock)?n:gMasterSock;
+		if (gLoopBackSock != -1)
+			n = (n > gLoopBackSock)?n:gLoopBackSock;
 		
 		for (InstanceIx = 0; InstanceIx < MAX_FD_INSTANCES; InstanceIx++)
 		{
@@ -1050,6 +1061,114 @@
 			}
 		}
 
+		if (gLoopBackSock != -1)
+		{
+			if (FD_ISSET(gLoopBackSock,&readfds))
+			{
+				SOCKET temp;
+				int Retval;
+				temp = WaitForClientConnection(gLoopBackSock);
+				if (temp != INVALID_SOCKET)
+				{
+					char ipaddress[64];
+					TWARN(" New FD connection got with sock = %d\n", temp);
+					GetIPAddress(temp, ipaddress);
+					printf ("ip address = %s\n", ipaddress);
+					Retval=RecvAuthFromRemote(temp, ipaddress, token, &InstanceRcvd);
+					printf ("InstanceRcvd = %d\n", InstanceRcvd);
+
+					if (InstanceRcvd != cfgVMedia.num_fd)
+					{
+						TWARN ("Invalid FD Instance %d for local FD Redirection !!!\n", InstanceRcvd);
+						continue;
+					}
+					if (RunFdChild[InstanceRcvd] == 1)
+					{
+						TWARN ("FD Instance %d is already active !!!\n", InstanceRcvd);
+						
+						if(SendAckToRemote(temp, ClientIPAddress[InstanceRcvd], CONNECTION_IN_USE, InstanceRcvd) != 0 )
+							printf("Unable to send Login failure ACK Packet\n");
+						continue;
+					}
+					
+					if (IsStatusAutoAttach())
+					{
+						ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_RECONNECT, NULL, InstanceRcvd);
+						
+						if (ioctl(usbfd, USB_DEVICE_RECONNECT, ModifiedData) < 0)
+						{
+							TCRIT("Unable to attach CD devices\n");
+						}
+					}
+					RunFdChild[InstanceRcvd] = 1;
+					
+					if( Retval != CONNECTION_ACCEPTED )
+					{
+						if( Retval == CONNECTION_IN_USE )
+						{
+							//ipaddress has the machine's ipaddress which is using fdserver
+							if(SendAckToRemote(temp, ipaddress, CONNECTION_IN_USE, InstanceRcvd) != 0 )
+								printf("Unable to send Login failure ACK Packet\n");
+						}
+						else
+						{
+							if(SendAckToRemote(temp, "", Retval,InstanceRcvd) != 0 )
+								printf("Unable to send Login failure ACK Packet\n");
+						}
+						sleep(3);
+					}
+					else
+					{
+						/* Send ACK to Remote Client that connection is accepted */
+						if(SendAckToRemote(temp, "", CONNECTION_ACCEPTED,InstanceRcvd) != 0 )
+						{
+							printf("Unable to send CONNECTION accepted ACK Packet\n");
+						}
+					
+					}
+					sock[InstanceRcvd]=temp;
+					InitStateMachine(InstanceRcvd);
+					memcpy (ClientIPAddress[InstanceRcvd], ipaddress, sizeof (ipaddress));
+					if (InstanceRcvd == 0)
+					{
+						/* Create the Child Thread */
+						if (pthread_create(&ChildId[0] , NULL,fd_child,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread creation failed\n");
+						}
+						
+					}	
+					if (InstanceRcvd == 1)
+					{
+						/* Create the Child Thread2 */
+						if (pthread_create(&ChildId[1] , NULL,fd_child2,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread2 creation failed\n");
+						}
+					}
+					
+					if (InstanceRcvd == 2)
+					{
+						/* Create the Child Thread3 */
+						if (pthread_create(&ChildId[2] , NULL,fd_child3,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread3 creation failed\n");
+						}
+					}
+					
+					if (InstanceRcvd == 3)
+					{
+						/* Create the Child Thread4 */
+						if (pthread_create(&ChildId[3] , NULL,fd_child4,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread4 creation failed\n");
+						}
+					}
+					
+					continue;
+				}
+			}
+		}
 		/* Case 1: ----------- Data Received From Remote -----------------*/
 		for (InstanceIx = 0; InstanceIx < MAX_FD_INSTANCES; InstanceIx++)
 		{
@@ -1171,10 +1290,6 @@
 		{
 			TWARN ("Local FD Instance connection closure detected\n");
 			LocalFDConnected = 0;
-			if (VMEDIA_E_SUCCESS != reset_floppy_redir(token, &racsession_id, &isOtherVMEDIAInUse))
-			{
-				TCRIT ("CleanUpSession(): reset_floppy_redir failed\n");
-			}
 			if ((isOtherVMEDIAInUse == 0) && IsStatusAutoAttach ())
 			{
 				ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
@@ -1310,12 +1425,26 @@
 	gMasterSock = InitializeServer(portnum, MediaConf.InterfaceName);
 	if (gMasterSock==INVALID_SOCKET)
    	{
-      	TCRIT("Server Initialization failure %d\n",errno);
+      	TCRIT("Server Initialization failure for interface %s %d\n", MediaConf.InterfaceName, errno);
 		for (InstanceIx = 0; InstanceIx < gNumFdInstances; InstanceIx++)
 		{
 			ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
 		}
 		return 1;
+	}
+	
+	if (strcasecmp(MediaConf.InterfaceName, BOTH_INTERFACES) != 0) 
+	{ 
+		gLoopBackSock = InitializeServer(portnum, "lo");
+		if (gLoopBackSock==INVALID_SOCKET)
+		{
+			TCRIT("Server Initialization failure for loopback interface %d\n",errno);
+			for (InstanceIx = 0; InstanceIx < gNumFdInstances; InstanceIx++)
+			{
+				ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
+			}
+			return 1;
+		}
 	}
 
 	/* Wait for clients forever */
@@ -1326,6 +1455,8 @@
 		ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
 	}
 	ShutDownServer(gMasterSock);
+	if (gLoopBackSock != -1)
+		ShutDownServer(gLoopBackSock);
 	close (usbfd);
 	return 0;
 }
@@ -1593,7 +1724,6 @@
 	uint8 IsOtherVMEDIAStarted = 0;
 	char user_name[TOKENCFG_MAX_USERNAME_LEN];
 	char client_ip[TOKENCFG_MAX_CLIENTIP_LEN];
-	char Temptoken[MAX_TOKEN_LEN+1] = "lmediafaketoken";
 	uint32 racsession_id=0;
 	uint8 Instance;
 	char TempBuffer [sizeof(IUSB_SCSI_PACKET)+FD_MAX_READ_SIZE];
@@ -1612,10 +1742,6 @@
 	{
 		TINFO ("Local Media FD connection received\n");
 		LocalFDConnected = 0x0100 + Instance;
-		if (VMEDIA_E_SUCCESS != set_floppy_redir(Temptoken, ipaddress,&IsOtherVMEDIAStarted))
-		{
-			TCRIT ("RecvAuthFromRemote(): set_floppy_redir call failed\n");
-		}
 		return CONNECTION_ACCEPTED;
 	}
 
