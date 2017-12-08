--- uboot.org/vendors/ami/fwupdate/fwupdate.c	2011-06-15 16:20:25.371794873 +0800
+++ uboot/vendors/ami/fwupdate/fwupdate.c	2011-06-15 16:15:29.889843486 +0800
@@ -0,0 +1,425 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2007, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+ *****************************************************************
+ * $Header: $
+ *
+ * $Revision: $
+ *
+ * $Date: $
+ *
+ ******************************************************************
+ ******************************************************************
+ * 
+ * fwupdate.c
+ * fwupdate functionality
+ *
+ *  Author: Rama Rao Bisa <ramab@ami.com>
+ *			Modified by Anurag Bhatia for SP porting		
+ ******************************************************************/
+# include <common.h>
+# include <config.h>
+
+#ifdef CFG_YAFU_SUPPORT
+
+# include <command.h>
+# include <net.h>
+# include "externs.h"
+# include "kcs.h"
+# include "cmdhandler.h"
+
+
+#define 	UBOOT_TIMEOUT_VALUE			0xE00000
+#define 	STR_WRITING					"writing"
+#define 	STR_DONE					"completed"
+#define 	FLSTATE_DOING     			0
+#define 	FLSTATE_TOBEDONE  			1
+#define 	FLSTATE_COMPLETE  			2
+
+static unsigned char 	m_KCSCmd [MAX_KCS_PKT_LEN];
+static KCSCmd_T*	 	pKCSCmd;
+static unsigned char 	m_KCSRes [MAX_KCS_PKT_LEN];
+static KCSCmd_T*	 	pKCSRes;
+
+static IPMICmdData_T    m_IPMICmd;
+static IPMICmdData_T*   pIPMICmd;
+
+volatile unsigned long  		*cpld_base = (unsigned long*)CPLD_BASE_ADDR;
+volatile unsigned short			*fwupdateflagreg = (unsigned short*)CPLD_FW_UPDATE_REG_ADDR;
+volatile static unsigned long 	gUbootTimeout;
+
+extern int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+extern int do_flerase (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+extern flash_info_t flash_info[];
+extern int flash_erase (flash_info_t *info, int s_first, int s_last);
+extern FLASH_PARAMS	gFlashParams;
+extern unsigned char *pNewFirmwareImage;
+extern unsigned long gImageconfigStart, gImageconfigSize;
+extern unsigned long Responselen;
+
+//#define DEBUG 1
+/*
+ * ProcessKCSRequests
+ */
+static void
+ProcessKCSRequests (void)
+{
+	int retlen;
+
+	pKCSCmd  = (KCSCmd_T*) m_KCSCmd;
+	pKCSRes  = (KCSCmd_T*) m_KCSRes;
+	pIPMICmd = (IPMICmdData_T*) &m_IPMICmd;
+
+	/* Read KCS Request */
+	retlen = RecvKCSRequest (m_KCSCmd);
+	if (0 != retlen)
+	{
+#ifdef DEBUG
+		int i;
+		printf ("KCS Packet Received\n");
+		for (i = 0; i < retlen; i++)
+			printf ("0x%x\n", m_KCSCmd [i]);
+#endif					
+		pIPMICmd->NetFnLUN = pKCSCmd->NetFnLUN;
+		pIPMICmd->Cmd	   = pKCSCmd->Cmd;
+		pIPMICmd->ReqLen   = (retlen - 2);
+		if (0 != pIPMICmd->ReqLen) 
+		{
+			memcpy ((unsigned char*)pIPMICmd->ReqData, (unsigned char*)pKCSCmd->Data, pIPMICmd->ReqLen);
+		}
+
+		ProcessIPMICmd (pIPMICmd);
+		gUbootTimeout = UBOOT_TIMEOUT_VALUE;
+		pKCSRes->NetFnLUN = pKCSCmd->NetFnLUN + 4;
+		pKCSRes->Cmd      = pKCSCmd->Cmd;
+		if (Responselen)
+		{
+			memcpy (pKCSRes->Data, pIPMICmd->ResData, Responselen);
+		}
+#ifdef DEBUG
+		printf ("pIPMPCmd->ResLen = 0x%x\n", pIPMICmd->ResLen);
+#endif
+		SendKCSResponse (m_KCSRes, (sizeof (pKCSCmd->NetFnLUN) + sizeof (pKCSCmd->Cmd) + Responselen));
+		
+	}
+	return;
+}
+
+/*
+ * Perform Firmware Reset
+ */
+static int
+PerformFirmwareReset (void)
+{
+	int Counter;
+
+	argv[0] = "reset";
+	argv[1] = NULL;
+	
+	for (Counter = 0; Counter < 10000; Counter++)
+	{
+		ProcessKCSRequests ();
+	}
+
+	if (0 != do_reset(NULL, 0, 1, argv))
+	{
+		printf ("Error: reset firmware failed\n");
+		return -1;
+	}
+
+	return 0;
+}
+
+/*
+ * Set Flash Progress
+ */
+static void
+SetFlashProgress(char* SubAction,char* Progress, int State)
+{
+    
+
+    if(SubAction == NULL)
+        strcpy(gAMIFlashStatus.SubAction,"");
+    else
+        strncpy(gAMIFlashStatus.SubAction,SubAction,60);
+	
+    if(Progress == NULL)
+        strcpy(gAMIFlashStatus.Progress,"");
+    else
+        strncpy(gAMIFlashStatus.Progress,Progress,30);
+
+    gAMIFlashStatus.State = State;
+
+}
+
+/*
+ * fwupdate
+ */
+int  
+fwupdate(void)
+{	
+	int 			retries=0;
+	volatile int 			i;
+	int  			Counter;
+	unsigned long 	addr;
+	unsigned long 	cnt,BlkToUpgrade;
+	unsigned char	*TempBuf = (unsigned char*)YAFU_TEMP_SECTOR_BUFFER;
+	unsigned char	*src,*pRamAddress=0;
+	flash_info_t 	*pFlashInfo;
+	char 			Msg[100];
+    int bank = 0;
+
+ 	pFlashInfo = &flash_info[bank];
+	
+	printf ("Initing KCS...");
+	InitKCS ();
+	printf ("done\n");
+	gUbootTimeout = UBOOT_TIMEOUT_VALUE;
+	memset(&gAMIFlashStatus, 0, sizeof (gAMIFlashStatus));
+
+	printf("Uboot waiting for firmware update to start...\n");
+
+	while (1)
+	{
+		if (0 == gUbootTimeout)
+		{
+			printf ("Uboot waiting for fwupdate to start timed out\n");
+			return 0;
+		}			
+		gUbootTimeout--;
+
+		ProcessKCSRequests ();
+	
+		if (gReset)
+		{
+			gReset = 0;
+            setenv("boot_fwupd", "0");
+            saveenv();
+			PerformFirmwareReset ();
+			continue;			
+		}
+		if(gStartFirmwareUpdation == 1) 
+		{
+					BlkToUpgrade = gEraseBlock;
+					
+					pRamAddress =(unsigned char *)YAFU_IMAGE_UPLOAD_LOCATION;
+									   
+						if((BlkToUpgrade /CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE) < (CONFIG_SPX_FEATURE_GLOBAL_FLASH_SIZE/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE) )
+						{
+		
+												  
+								i = (BlkToUpgrade /CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);	
+						
+								/* Calculate the correct bank and get the flash_info of that bank*/
+                                bank = (i  /  (pFlashInfo->size  / CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE));
+                                i = i % (pFlashInfo->size  / CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);
+                                pFlashInfo = &flash_info[bank];
+										
+								if (0 != flash_read (pFlashInfo->start[i], CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE, (char *)TempBuf))
+								{
+								printf ("Flash Read Failed at sector %d and retrying...\n", i);
+								continue;
+							}
+		
+					
+							if (0 == memcmp ((unsigned char*)TempBuf, pRamAddress, CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE))	
+							{
+								 printf ("sector %d did not change\n", i);
+								 gStartFirmwareUpdation = 0;	 
+								 continue;
+									
+							}	
+							retries = 3;
+							while (retries)
+							{
+								if (0 != flash_erase (pFlashInfo, i, i))
+								{
+									printf ("Flash Erase Failed at sector %d and retrying...\n", i);
+									retries--;
+									continue;
+								}
+									break;
+								}
+							if (0 == retries)
+							{
+								PerformFirmwareReset ();
+							}
+							
+							src   = pRamAddress;
+							addr = pFlashInfo->start[i];
+							cnt  = CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+							retries = 3;
+							while (retries)
+							{
+								if (0 != write_buff (pFlashInfo, src, addr, cnt))
+								{
+									printf ("\nFlash write failed at address =0x%x of size= 0x%x\n", addr, cnt);
+									retries--;
+									flash_erase (pFlashInfo, i, i);
+									continue;
+								}	
+								break;
+							}
+						}
+						if (0 == retries)
+						{
+							PerformFirmwareReset ();
+						}
+				
+			gStartFirmwareUpdation = 0;
+				
+		}
+		if ((1 == gStartFirmwareUpdate ) && (1 == gfirmwareUpgradeProcessStarted))
+		{
+			argv[0] = "protect";
+			argv[1] = "off";
+			argv[2] = "all";
+			argv[3] = NULL;
+
+			retries = 3;
+			while (retries)
+			{
+				if (0 != do_protect (NULL,0,3,argv))
+				{
+					printf ("Protect off all Failed\n");
+					retries--;
+					continue;
+				}
+				break;
+			}
+			if (0 == retries)
+			{
+				PerformFirmwareReset ();
+			}
+			
+//			printf("gFlashParams.UsedFlashSize = %08x,gFlashParams.EraseBlockSize = %08X\n",gFlashParams.UsedFlashSize,gFlashParams.EraseBlockSize);
+			for (i = 0; i < (gFlashParams.UsedFlashSize)/(gFlashParams.EraseBlockSize); i++)
+			{
+
+				sprintf (Msg, "%d%% done         ", (int) (long)(100*i)/((gFlashParams.UsedFlashSize)/ (gFlashParams.EraseBlockSize)));
+
+				SetFlashProgress("Writing...",Msg,FLSTATE_DOING);
+				
+				for (Counter = 0; Counter < 10000; Counter++)
+				{
+					ProcessKCSRequests ();
+				}
+//				printf("gFlashParams.UpdateUboot = %d,gFlashParams.RetainConfig = %d\n",gFlashParams.UpdateUboot,gFlashParams.RetainConfig);
+
+				if ((i < ((CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START)/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE)) && (0 == gFlashParams.UpdateUboot)) 
+				{
+					printf ("Skipping U-boot sector: %d\n",i);
+						continue;
+				}
+
+				if (  (i >= ((CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START)/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE)) && 
+						(i < ((CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START + CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_SIZE)/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE)))
+				{
+					printf ("Skipping U-boot Environment sector: %d\n",i);
+						continue;
+				}
+#if 1
+				if (gFlashParams.RetainConfig)
+				{
+					if ((pFlashInfo->start[i] >= pFlashInfo->start[0]+gImageconfigStart) && 
+						(pFlashInfo->start[i] < pFlashInfo->start[0]+gImageconfigStart+gImageconfigSize))
+						{
+								printf ("Skipping config sector %d\n", i);
+								continue;
+						}
+						
+				}
+#endif
+				retries = 3;
+				while (retries)
+				{
+					if (0 != flash_read (pFlashInfo->start[i], gFlashParams.EraseBlockSize, (char *)TempBuf))
+					{
+						printf ("Flash Read Failed at sector %d and retrying...\n", i);
+						retries--;
+						continue;
+					}
+					break;
+				}
+				if (0 == retries)
+				{
+					PerformFirmwareReset ();
+				}
+
+				if (0 == memcmp ((unsigned char*)TempBuf, (unsigned char*)(pNewFirmwareImage + i * gFlashParams.EraseBlockSize), gFlashParams.EraseBlockSize))
+				{
+					printf ("sector %d did not change\n", i);
+					continue;
+				}
+				else
+				{
+					printf ("Sector %d changed\n", i);
+				}
+
+				retries = 3;
+				while (retries)
+				{
+					if (0 != flash_erase (pFlashInfo, i, i))
+					{
+						printf ("Flash Erase Failed at sector %d and retrying...\n", i);
+						retries--;
+						continue;
+					}
+					break;
+				}
+				if (0 == retries)
+				{
+					PerformFirmwareReset ();
+				}
+				for (Counter = 0; Counter < 10000; Counter++)
+				{
+					ProcessKCSRequests ();
+				}
+
+				src  = (unsigned char*)(YAFU_IMAGE_UPLOAD_LOCATION + (i * gFlashParams.EraseBlockSize));
+				addr = pFlashInfo->start[i];
+				cnt  = gFlashParams.EraseBlockSize;
+				retries = 3;
+				while (retries)
+				{
+					if (0 != write_buff (pFlashInfo, src, addr, cnt))
+					{
+						printf ("\nFlash write failed at address =0x%x of size= 0x%x\n", addr, cnt);
+						retries--;
+						flash_erase (pFlashInfo, i, i);
+						continue;
+					}	
+					break;
+				}
+				if (0 == retries)
+				{
+					PerformFirmwareReset ();
+				}
+			}
+			SetFlashProgress("Writing..", STR_DONE, FLSTATE_COMPLETE);
+			gStartFirmwareUpdate = 0;
+			gfirmwareUpgradeProcessStarted = 0;
+			for (Counter = 0; Counter < 0x100000; Counter++)
+			{
+				ProcessKCSRequests ();
+			}
+			PerformFirmwareReset ();
+		}
+	}
+
+	return 0;
+}	
+#endif
+
+
+
