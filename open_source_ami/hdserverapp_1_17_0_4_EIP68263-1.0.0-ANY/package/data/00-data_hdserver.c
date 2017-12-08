--- .pristine/hdserverapp-1.17.0-src/data/hdserver.c Mon Oct 10 16:31:55 2011
+++ source/hdserverapp-1.17.0-src/data/hdserver.c Tue Oct 11 11:20:17 2011
@@ -83,6 +83,7 @@
 static int gNumHdInstances = 0;
 static char *ProcName = NULL;
 static SOCKET gMasterSock = -1;
+static SOCKET gLoopBackSock = -1;
 static int usbhd;
 static int gWasResponseKAR [MAX_HD_INSTANCES] = { 0, 0, 0, 0 }; 
 static u8  gHdiskType[MAX_HD_INSTANCES] = { 0, 0, 0, 0 };
@@ -894,6 +895,13 @@
 	struct 	timeval timeout; 
 	uint8 	InstanceRcvd;
 	char 	token[MAX_TOKEN_LEN+1];
+	VMediaCfg_T cfgVMedia;
+	
+	if (GetVMediaCfg(&cfgVMedia) != 0)
+	{
+		TEMERG("Unable to Read VMedia configuration. Closing HD server\n");
+		return 1;
+	}
 
 	for (InstanceIx = 0; InstanceIx < gNumHdInstances; InstanceIx++)
 	{
@@ -919,6 +927,8 @@
 		n = -1;
 		FD_ZERO(&readhds);
 		FD_SET(gMasterSock,&readhds);
+		if (gLoopBackSock != -1)
+			FD_SET(gLoopBackSock,&readhds);
 		FD_SET(pipehd[0],&readhds);
 		if( reset_event_pipe != -1)
 		{
@@ -928,6 +938,8 @@
 		
 		n = (n > pipehd[0])?n:pipehd[0];
 		n = (n > gMasterSock)?n:gMasterSock;
+		if (gLoopBackSock != -1)
+			n = (n > gLoopBackSock)?n:gLoopBackSock;
 		
 		for (InstanceIx = 0; InstanceIx < MAX_HD_INSTANCES; InstanceIx++)
 		{
@@ -1099,6 +1111,132 @@
 			}
 		}
 
+		if (gLoopBackSock != -1)
+		{
+			if (FD_ISSET(gLoopBackSock,&readhds))
+			{
+				SOCKET temp;
+				int Retval;
+				uint8 HDType;
+				int NumSecs = 8;
+				
+				temp = WaitForClientConnection(gLoopBackSock);
+				if (temp != INVALID_SOCKET)
+				{
+					char ipaddress[64];
+					TWARN(" New HD connection got with sock = %d\n", temp);
+					while (NumSecs)
+					{
+						if (HDConfigInProgress == 0)
+							break;
+						else
+						{
+							printf ("Previous HD disconnection in progress...\n");
+							sleep (1);
+							NumSecs -= 1;
+						}
+					}
+					if (NumSecs == 0)
+					{
+						HDConfigInProgress = 0;
+						printf ("Error with Previous HD Disconnection Process\n");
+					}
+					GetIPAddress(temp, ipaddress);
+					printf ("ip address = %s\n", ipaddress);
+					Retval=RecvAuthFromRemote(temp, ipaddress, token, &InstanceRcvd, &HDType);
+					printf ("InstanceRcvd = %d\n", InstanceRcvd);
+					printf ("HDType = 0x%02x\n", HDType);
+					gHdiskType[InstanceRcvd] = HDType;
+					if (InstanceRcvd != cfgVMedia.num_hd)
+					{
+						TWARN ("Invalid HD/VF Instance %d  for HD/VF redirection !!!\n", InstanceRcvd);
+						continue;
+					}
+					if (RunHdChild[InstanceRcvd] == 1)
+					{
+						TWARN ("HD Instance %d is already active !!!\n", InstanceRcvd);
+						
+						if(SendAckToRemote(temp, ClientIPAddress[InstanceRcvd], CONNECTION_IN_USE, InstanceRcvd) != 0 )
+							printf("Unable to send Login failure ACK Packet\n");
+						continue;
+					}
+					if (IsStatusAutoAttach() && gHdiskType[InstanceRcvd])
+					{
+						ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_RECONNECT, NULL, InstanceRcvd);
+						if( ioctl(usbhd, USB_DEVICE_RECONNECT, ModifiedData) < 0 )
+						{
+							TCRIT("Unable to attach HDISK devices\n");
+						}
+					}
+					
+					RunHdChild[InstanceRcvd] = 1;
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
+						if (pthread_create(&ChildId[0] , NULL,hd_child,(void *)pipehd[1]) == 1)
+						{
+							TCRIT("  Child thread creation failed\n" );
+						}
+					}
+					if (InstanceRcvd == 1)
+					{
+						/* Create the Child Thread2 */
+						if (pthread_create(&ChildId[1] , NULL,hd_child2,(void *)pipehd[1]) == 1)
+						{
+							TCRIT(" Child thread2 creation failed\n");
+						}
+					}
+					
+					if (InstanceRcvd == 2)
+					{
+						/* Create the Child Thread3 */
+						if (pthread_create(&ChildId[2] , NULL,hd_child3,(void *)pipehd[1]) == 1)
+						{
+							TCRIT(" Child thread3 creation failed\n");
+						}
+					}
+					
+					if (InstanceRcvd == 3)
+					{
+						/* Create the Child Thread4 */
+						if (pthread_create(&ChildId[3] , NULL,hd_child4,(void *)pipehd[1]) == 1)
+						{
+							TCRIT(" Child thread4 creation failed\n");
+						}
+					}
+					continue;
+				}
+			}
+		}
+
 		/* Case 1: ----------- Data Received From Remote -----------------*/
 		for (InstanceIx = 0; InstanceIx < MAX_HD_INSTANCES; InstanceIx++)
 		{
@@ -1220,10 +1358,6 @@
 		{
 			TWARN ("Local HD Instance connection closure detected\n");
 			LocalHDConnected = 0;
-			if (VMEDIA_E_SUCCESS != reset_hdisk_redir(token, &racsession_id, &isOtherVMEDIAInUse))
-			{
-				TCRIT ("CleanUpSession(): reset_hdisk_redir failed\n");
-			}
 			if ((isOtherVMEDIAInUse == 0) && IsStatusAutoAttach ())
 			{
 				ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
@@ -1357,10 +1491,24 @@
 	gMasterSock = InitializeServer(portnum, MediaConf.InterfaceName);
 	if (gMasterSock==INVALID_SOCKET)
    	{
-      	TCRIT(" Server Initialization failure %d\n",errno);
+      	TCRIT("Server Initialization failure for interface %s %d\n", MediaConf.InterfaceName, errno);
 		ReleaseiUSBDevice (&iUSBDevInfo[0]);
 		return 1;
 	}
+	if (strcasecmp(MediaConf.InterfaceName, BOTH_INTERFACES) != 0) 
+	{ 
+		gLoopBackSock = InitializeServer(portnum, "lo");
+		if (gLoopBackSock==INVALID_SOCKET)
+	   	{
+	      	TCRIT("Server Initialization failure for loopback interface %d\n",errno);
+			for (InstanceIx = 0; InstanceIx < gNumHdInstances; InstanceIx++)
+			{
+				ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
+			}
+			return 1;
+		}
+	}
+
 
 	/* Wait for clients forever */
 	SaveMyPid(HD_PID_FILE);
@@ -1370,6 +1518,8 @@
 		ReleaseiUSBDevice (&iUSBDevInfo[InstanceIx]);
 	}
 	ShutDownServer(gMasterSock);
+	if (gLoopBackSock != -1)
+		ShutDownServer(gLoopBackSock);
 	close (usbhd);
 	return 0;
 }
@@ -1648,7 +1798,6 @@
 	char user_name[TOKENCFG_MAX_USERNAME_LEN];
 	char client_ip[TOKENCFG_MAX_CLIENTIP_LEN];
 	uint32 racsession_id=0;
-	char Temptoken[MAX_TOKEN_LEN+1] = "lmediafaketoken";
 	uint8 Instance;
 	char TempBuffer [sizeof(IUSB_SCSI_PACKET)+HD_MAX_READ_SIZE];
 	IUSB_SCSI_PACKET *TempResPkt = (IUSB_SCSI_PACKET *)&TempBuffer[0];
@@ -1667,10 +1816,6 @@
 	{
 		TINFO ("Local Media HD connection received\n");
 		LocalHDConnected = 0x0100 + Instance;
-		if (VMEDIA_E_SUCCESS != set_hdisk_redir(Temptoken, ipaddress,&IsOtherVMEDIAStarted))
-		{
-			TCRIT ("RecvAuthFromRemote(): set_hdisk_redir call failed\n");
-		}
 		return CONNECTION_ACCEPTED;
 	}
 
