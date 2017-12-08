--- .pristine/cdserverapp-1.16.0-src/data/cdserver.c Mon Oct 10 16:31:53 2011
+++ source/cdserverapp-1.16.0-src/data/cdserver.c Tue Oct 11 10:49:43 2011
@@ -83,6 +83,7 @@
 static int gPipePending[MAX_CD_INSTANCES] = { 0, 0, 0, 0 };
 static int gNumCdInstances = 0;
 static SOCKET gMasterSock = -1;		   		/* socket descriptor */
+static SOCKET gLoopBackSock = -1;
 static int usbfd;							/* Usb Descriptor */
 static int gWasResponseKAR[MAX_CD_INSTANCES] = { 0, 0, 0, 0 }; 	/* detects if response was a keep alive respons from remote */
 static int gRRTmoutCt[MAX_CD_INSTANCES] = { 0, 0, 0, 0 }; /*remote response tmout count for KA or actual cmds */
@@ -887,6 +888,13 @@
 	struct timeval timeout;
 	uint8 InstanceRcvd;
 	char	token[MAX_TOKEN_LEN+1];
+	VMediaCfg_T cfgVMedia;
+	
+	if (GetVMediaCfg(&cfgVMedia) != 0)
+	{
+		TEMERG("Unable to Read VMedia configuration. Closing CD server\n");
+		return 1;
+	}
 	
 	for (InstanceIx = 0; InstanceIx < gNumCdInstances; InstanceIx++)
 	{
@@ -910,6 +918,8 @@
 		n = -1;
 		FD_ZERO(&readfds);
 		FD_SET(gMasterSock,&readfds);
+		if (gLoopBackSock != -1)
+			FD_SET(gLoopBackSock,&readfds);
 		FD_SET(pipefd[0],&readfds);
 				
 		if( reset_event_pipe != -1)
@@ -920,6 +930,8 @@
 		
 		n = (n > pipefd[0])?n:pipefd[0];
 		n = (n > gMasterSock)?n:gMasterSock;
+		if (gLoopBackSock != -1)
+			n = (n > gLoopBackSock)?n:gLoopBackSock;
 
 		for (InstanceIx = 0; InstanceIx < MAX_CD_INSTANCES; InstanceIx++)
 		{
@@ -1071,6 +1083,115 @@
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
+					TWARN(" New CD connection got with sock = %d\n", temp);
+					GetIPAddress(temp, ipaddress);
+					printf ("ip address = %s\n", ipaddress);
+					Retval=RecvAuthFromRemote(temp, ipaddress, token, &InstanceRcvd);
+					printf ("InstanceRcvd = %d\n", InstanceRcvd);
+
+					if (InstanceRcvd != cfgVMedia.num_cd)
+					{
+						TWARN ("Invalid CD Instance %d for local CD Redirection !!!\n", InstanceRcvd);
+						continue;
+					}
+					if (RunCdChild[InstanceRcvd] == 1)
+					{
+						TWARN ("CD Instance %d is already active !!!\n", InstanceRcvd);
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
+					RunCdChild[InstanceRcvd] = 1;
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
+						if (pthread_create(&ChildId[0] , NULL,cd_child,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread creation failed\n");
+						}
+						
+					}	
+					if (InstanceRcvd == 1)
+					{
+						/* Create the Child Thread2 */
+						if (pthread_create(&ChildId[1] , NULL,cd_child2,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread2 creation failed\n");
+						}
+					}
+					
+					if (InstanceRcvd == 2)
+					{
+						/* Create the Child Thread3 */
+						if (pthread_create(&ChildId[2] , NULL,cd_child3,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread3 creation failed\n");
+						}
+					}
+					
+					if (InstanceRcvd == 3)
+					{
+						/* Create the Child Thread4 */
+						if (pthread_create(&ChildId[3] , NULL,cd_child4,(void *)pipefd[1]) == 1)
+						{
+							TCRIT(" Child thread4 creation failed\n");
+						}
+					}
+					
+					continue;
+					
+				}
+			}
+		}
 
 		/* Case 1: ----------- Data Received From Remote -----------------*/
 		for (InstanceIx = 0; InstanceIx < MAX_CD_INSTANCES; InstanceIx++)
@@ -1192,10 +1313,6 @@
 		{
 			TWARN ("Local CD Instance connection closure detected\n");
 			LocalCDConnected = 0;
-			if (VMEDIA_E_SUCCESS != reset_cdrom_redir(token, &racsession_id, &isOtherVMEDIAInUse))
-			{
-				TCRIT ("CleanUpSession(): reset_cdrom_redir failed\n");
-			}
 			if ((isOtherVMEDIAInUse == 0) && IsStatusAutoAttach ())
 			{
 				ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
@@ -1330,14 +1447,26 @@
 	gMasterSock = InitializeServer(portnum, MediaConf.InterfaceName);
 	if (gMasterSock==INVALID_SOCKET)
    	{
-      	TCRIT("Server Initialization failure %d\n",errno);
+      	TCRIT("Server Initialization failure for interface %s %d\n", MediaConf.InterfaceName, errno);
 		for (InstanceIx = 0; InstanceIx < gNumCdInstances; InstanceIx++)
 		{
 			ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
 		}
 		return 1;
 	}
-
+	if (strcasecmp(MediaConf.InterfaceName, BOTH_INTERFACES) != 0) 
+	{ 
+		gLoopBackSock = InitializeServer(portnum, "lo");
+		if (gLoopBackSock==INVALID_SOCKET)
+	   	{
+	      	TCRIT("Server Initialization failure for loopback interface %d\n",errno);
+			for (InstanceIx = 0; InstanceIx < gNumCdInstances; InstanceIx++)
+			{
+				ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
+			}
+			return 1;
+		}
+	}
 	/* Wait for clients forever */
 	SaveMyPid(CD_PID_FILE);
 	iUsbTransfer ();
@@ -1346,6 +1475,8 @@
 		ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
 	}
 	ShutDownServer(gMasterSock);
+	if (gLoopBackSock != -1)
+		ShutDownServer(gLoopBackSock);
 	close (usbfd);
 	return 0;
 }
@@ -1615,7 +1746,6 @@
 	uint8 IsFloppyStarted;
 	char user_name[TOKENCFG_MAX_USERNAME_LEN];
 	char client_ip[TOKENCFG_MAX_CLIENTIP_LEN];
-	char Temptoken[MAX_TOKEN_LEN+1] = "lmediafaketoken";
 
 	uint8 Instance;
 	char TempBuffer [sizeof(IUSB_SCSI_PACKET)+CD_MAX_READ_SIZE];
@@ -1634,10 +1764,6 @@
 	{
 		TINFO ("Local Media CD connection received\n");
 		LocalCDConnected = 0x0100 + Instance;
-		if (VMEDIA_E_SUCCESS != set_cdrom_redir(Temptoken, ipaddress,&IsFloppyStarted))
-		{
-			TCRIT ("RecvAuthFromRemote(): set_cdrom_redir call failed\n");
-		}
 		return CONNECTION_ACCEPTED;
 	}
 	if( ResPkt[Instance]->CommandPkt.OpCode == AUTH_CMD )
