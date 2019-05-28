--- uboot_old/oem/ami/fwupdate/fwupdate.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot/oem/ami/fwupdate/fwupdate.c	2014-12-18 10:33:16.097771370 -0500
@@ -0,0 +1,691 @@
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
+# include <command.h>
+# include <net.h>
+# include <oem/ami/fwupdate/kcs.h>
+# include <oem/ami/ipmi/cmdhandler.h>
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+# include <oem/ami/fmh/fmh.h>
+# include "fat.h"
+#endif
+
+//#define 	DEBUG 						1
+
+#define 	MAX_KCS_CHANNELS	    	2
+#define 	UBOOT_TIMEOUT_VALUE			0xE00000
+
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+extern block_dev_desc_t* mmc_get_dev(int dev);
+extern int Enable_watchdog(unsigned long *wdt_timeout_status);
+extern int Disable_watchdog(void);
+#endif
+
+extern int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+extern int flash_erase (flash_info_t *info, int s_first, int s_last);
+extern flash_info_t flash_info[];
+
+
+/* From cmdhandler.c */
+extern int 				gReset;
+extern int 				gStartFirmwareUpdation;
+extern INT32U			gEraseBlock;
+extern unsigned char	gFWUploadSelector;
+
+volatile static unsigned long 	gUbootTimeout;
+
+static unsigned char 	m_KCSCmd [MAX_KCS_PKT_LEN];
+static unsigned char 	m_KCSRes [MAX_KCS_PKT_LEN];
+static IPMICmdData_T    m_IPMICmd;
+
+static KCSCmd_T*		pKCSRes; 
+static KCSCmd_T*		pKCSCmd;
+static IPMICmdData_T*   pIPMICmd;
+
+
+/*
+ * ProcessKCSRequests
+ */
+static void
+ProcessKCSRequests (void)
+{
+
+	int retlen;
+	unsigned char ChannelNum=0;
+	unsigned long Responselen;
+
+	/* Read KCS Request */
+	for (ChannelNum=0;ChannelNum<MAX_KCS_CHANNELS;ChannelNum++)
+	{
+	retlen = RecvKCSRequest (ChannelNum,&m_KCSCmd[0]);
+		if (0 != retlen)
+		{
+#ifdef DEBUG
+		int i;
+		printf ("KCS Packet Received\n");
+		for (i = 0; i < retlen; i++)
+				printf ("C%d  0x%x\n", ChannelNum, m_KCSCmd [i]);
+#endif					
+		pIPMICmd->NetFnLUN = pKCSCmd->NetFnLUN;
+		pIPMICmd->Cmd	   = pKCSCmd->Cmd;
+		pIPMICmd->ReqLen   = (retlen - 2);
+		if (0 != pIPMICmd->ReqLen) 
+		{
+			memcpy ((unsigned char*)pIPMICmd->ReqData, (unsigned char*)pKCSCmd->Data, pIPMICmd->ReqLen);
+		}
+
+		Responselen = ProcessIPMICmd (pIPMICmd);
+
+		gUbootTimeout = UBOOT_TIMEOUT_VALUE;
+		pKCSRes->NetFnLUN = pKCSCmd->NetFnLUN + 4;
+		pKCSRes->Cmd      = pKCSCmd->Cmd;
+		if (Responselen)
+		{
+			memcpy (pKCSRes->Data, pIPMICmd->ResData, Responselen);
+		}
+#ifdef DEBUG
+		printf ("pIPMPCmd->ResLen = 0x%lx\n", pIPMICmd->ResLen);
+#endif
+		SendKCSResponse (ChannelNum, m_KCSRes, (sizeof (pKCSCmd->NetFnLUN) + sizeof (pKCSCmd->Cmd) + Responselen));
+		
+		}
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
+	char *argv[2];
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
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+int CopyRecoveryImage(void)
+{
+    block_dev_desc_t* desc = NULL;
+    long size;
+    unsigned long cnt = 0;
+    int dev = 0, part = 1;
+    char fname[20] = {0};
+    char *value;
+
+    /*TODO: dev,part and cnt values can be get from U-Boot env also.*/
+
+    /*Get the MMC device descriptor*/
+    desc = mmc_get_dev(dev);
+    if(desc == NULL){
+        return -1;
+    }
+
+    /*Register the MMC device partition with FAT file system*/
+    if(fat_register_device(desc,part) != 0){
+        return -1;
+    }
+
+    /*Get the recovery image name from U-Boot env*/
+    value = getenv("recoverybootfile");
+    if(value == NULL)
+    {
+        /*default image name rom.ima*/
+        strcpy(fname,"rom.ima");
+    }
+    else
+    {
+        strcpy(fname,value);
+    }
+
+
+    /*Read the recovery image from file system*/
+    size = file_fat_read(fname,(unsigned char *)YAFU_IMAGE_UPLOAD_LOCATION,cnt);
+    if(size == -1){
+        return -1;
+    }
+    return 0;
+}
+
+int VerifyRAMImage(unsigned long *ConfStart, unsigned long *ConfSize)
+{
+    unsigned long   offset = 0;
+    FMH     *pFMH = NULL;
+    MODULE_INFO *pmod = NULL;
+    char        sectionname[10] = {0}, conffound = 0;
+    unsigned char   *TempBuf = (unsigned char *)YAFU_TEMP_SECTOR_BUFFER;
+    int i,ret = -1;
+
+    for(i = 0; i < (CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE); i++)
+    {
+        memcpy(TempBuf, (unsigned char *)(YAFU_IMAGE_UPLOAD_LOCATION + (i * CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE)),CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);
+
+        /*Locate the FMH Section*/
+        pFMH = ScanforFMH_RAM(TempBuf,CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);
+        if(pFMH != NULL)
+        {
+            /*Get the Module Information*/
+            pmod = &pFMH->Module_Info;
+            memcpy(sectionname,pmod->Module_Name,8);
+            printf("Found Module : %s\n",sectionname);
+            /*Firmware Module found*/
+            if(pmod->Module_Type == 0x02)
+            {
+                /*return success if the firmware module found, else failure*/
+                ret = 0;
+            }
+
+            if((strncmp(sectionname,"conf",8) == 0) && (conffound == 0))
+            {
+                conffound = 1;
+                *ConfStart = offset;
+                *ConfSize = pFMH->FMH_AllocatedSize;
+            }
+        }
+        offset += CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+        
+    }
+
+    return ret;
+}
+
+int VerifyandFlash (void)
+{
+    flash_info_t    *pFlashInfo;
+    int bank = 0,sector,retries = 0;
+    unsigned char   *TempBuf = (unsigned char*)YAFU_TEMP_SECTOR_BUFFER;
+    unsigned char   *pRamAddr;
+    unsigned long   cnt,confstartaddr = 0,confsize = 0, FlashAddr= 0;
+    char        *argv[4];
+
+    /*TODO: Flash Info need to be get properly based on the bank (i.e Dual Image)*/
+    pFlashInfo = &flash_info[bank];
+
+    /*Validate the Recovery Image which is in RAM Memory*/
+    if(VerifyRAMImage(&confstartaddr,&confsize) != 0)
+    {
+        printf("Image in the RAM address is invalid !!!");
+        return -1;
+    }
+
+    /*Un-protect the flash*/
+    argv[0] = "protect";
+    argv[1] = "off";
+    argv[2] = "all";
+    argv[3] = NULL;
+
+    retries = 3;
+    while(retries)
+    {
+        if(0 != do_protect(NULL, 0, 3, argv))
+        {
+            retries--;
+            continue;
+        }
+        break;
+    }
+
+    if(0 == retries)
+    {
+        return -1;
+    }
+
+    for(sector = 0; sector < pFlashInfo->sector_count; sector++)
+    {
+        pRamAddr = (unsigned char *) (YAFU_IMAGE_UPLOAD_LOCATION + (sector * CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE));
+
+        /*Skip the U-Boot Env Sector*/
+        if((sector >= ((CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START)/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE)) &&
+            (sector < ((CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START + CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_SIZE)/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE)))
+        {
+            printf("Skipping U-Boot Environment sector: %d\n",sector);
+            continue;
+        }
+#if 0
+        /*Skip the conf section to preserve the existing configuration.*/
+        if((pFlashInfo->start[sector] >= pFlashInfo->start[0] + confstartaddr) && 
+            (pFlashInfo->start[sector] < pFlashInfo->start[0] + confstartaddr + confsize))
+        {
+            printf("Skip the sector %d\n",sector);
+            continue;
+        }
+#endif
+
+        retries = 3;
+        while(retries)
+        {
+            /*Read one sector from SPI and copy to RAM memory*/
+            if(0 != flash_read(pFlashInfo->start[sector],CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE,(char*)TempBuf))
+            {
+                retries--;
+                continue;
+            }
+            break;
+        }
+
+        if(retries == 0)
+            return -1;
+
+        /*Skip the sector if there is no change*/
+        if(0 == memcmp((unsigned char*)TempBuf,(unsigned char*)pRamAddr,CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE))
+        {
+            printf("sector %d did not change\n",sector);
+            continue;
+        }
+
+        retries = 3;
+        while(retries)
+        {
+            /*Erase the sector*/
+            if(0 != flash_erase(pFlashInfo, sector, sector))
+            {
+                retries--;
+                continue;
+            }
+            break;
+        }
+
+        if(retries == 0)
+            return -1;
+
+        FlashAddr = pFlashInfo->start[sector];
+        cnt = CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+        retries = 3;
+        while(retries)
+        {
+            /*Write the modified sector to SPI*/
+            if(0 != write_buff(pFlashInfo,pRamAddr,FlashAddr,cnt))
+            {
+                retries--;
+                flash_erase(pFlashInfo, sector, sector);
+                continue;
+            }
+            break;
+        }
+
+        if(retries == 0)
+            return -1;
+        
+    }
+    
+    return 0;
+}
+
+int TFTPRecoveryBoot(void)
+{
+    char    *s, tmp[22] = {0};
+
+    /*Set the netrety to "no", so that NetLoop will comeout after TFTP retry exceeds.*/
+    s = getenv("netretry");
+    setenv("netretry","no");
+    saveenv();
+
+    /*Run the DHCP command to get the IP Address, SubNetmask and Gateway*/
+    NetLoop(DHCP);
+
+    /*Update the Gateway U-Boot Env*/
+    if(NetOurGatewayIP)
+    {
+        ip_to_string(NetOurGatewayIP, tmp);
+        setenv("gatewayip", tmp);
+    }
+
+    /*Update the netmask U-Boot Env*/
+    if(NetOurSubnetMask)
+    {
+        ip_to_string(NetOurSubnetMask, tmp);
+        setenv("netmask", tmp);
+    }
+
+    /*Update the IPAddress U-Boot Env*/
+    if(NetOurIP)
+    {
+        ip_to_string(NetOurIP, tmp);
+        setenv("ipaddr", tmp);
+    }
+
+    /*Save the IP Address details*/
+    saveenv();
+
+    /*Call the Network loop to load the file from remote machine*/
+    NetLoop(RECOVERY);
+
+    /*Restore the net retry value*/
+    setenv("netretry",s);
+    saveenv();
+
+    /*Check the NetLoop state*/
+//TODO:      if(NetState != NETLOOP_SUCCESS)
+    if(net_state != NETLOOP_SUCCESS)
+    {
+        return -1;
+    }
+
+    /*Verify the recovery Image and flash it to SPI*/
+    if(0 != VerifyandFlash())
+    {
+        return -1;
+    }
+
+    return 0;
+}
+
+int Firmware_AutoRecovery(u8 force_recovery, u8 imgheadererr)
+{
+    unsigned long wdt_reset_status = 0 ;
+    char    *value = NULL;
+    int retries = 3;
+    u8  recoveryprogress = 0;
+
+    value = getenv("recoveryretry");
+    if(value != NULL)
+    {
+        retries = (int)simple_strtoul(value,NULL,0);
+    }
+
+    value = NULL;
+    value = getenv("recoveryinprog");
+    if(strcmp(value,"yes") == 0)
+    {
+        recoveryprogress = 1;
+    }
+
+    /*Enable watchdog timer only when the force recovery option is disabled*/
+    if((force_recovery == 0) && (imgheadererr == 0))
+    {
+        Enable_watchdog(&wdt_reset_status);
+    }
+
+    if(recoveryprogress == 0)
+    {
+        setenv("recoveryinprog","yes");     
+        saveenv();
+    }
+
+    if((recoveryprogress == 0) && (wdt_reset_status) && (force_recovery == 0)) 
+    {
+        /*watchdog is triggered for other purpose, so exit*/
+        return 0;
+    }
+
+    if((recoveryprogress) && (wdt_reset_status) && (force_recovery == 0) && (imgheadererr == 0))
+    {
+        Disable_watchdog();
+    }
+    if(wdt_reset_status || force_recovery || imgheadererr)
+    {
+        /*The image in the SPI is corrupted or force recovery is called, so
+         flash the recovery image from the MMC card to the SPI*/
+
+        if(imgheadererr)
+        {
+            setenv("imgheadererr",NULL);
+            saveenv();
+        }
+
+        /*Check the last recovery boot only when the force recovery is not set. Because in case of
+         force recovery we will restart the recovery image booting from MMC to TFTP.*/
+        if(force_recovery == 0)
+        {
+            /*Check the lastrecovery boot and cancel the recovery boot process*/
+            value = NULL;
+            value = getenv("lastrecoveryboot");
+            if(strcmp(value,"tftp") == 0)
+            {
+                printf("Both MMC and TFTP Recovery boot is failed !!!\n");
+                return -1;
+            }
+        }
+
+        printf("Firmware Recovery is started...\n");
+
+        /*Check the last recovery boot*/
+        value = NULL;
+        value = getenv("lastrecoveryboot");
+        if(strcmp(value,"mmc") == 0)
+        {
+            goto TFTP;
+        }
+
+        /*copy the recovery Image to RAM memory*/
+        while (retries)
+        {
+            if(0 != CopyRecoveryImage())
+            {
+                printf("Retring to copy the image from eMMC..\n");
+                retries--;
+                continue;
+            }
+            printf("Image Copied successfully.\n");
+            break;
+        }
+
+        if(retries == 0)
+        {
+            goto TFTP;
+        }
+
+        /*Verify the recovery Image and flash it to SPI*/
+        if(0 != VerifyandFlash())
+        {
+            printf("Error while flashing the recovery image. Try Recovery via TFTP...\n");
+            goto TFTP;
+        }
+
+        /*Set the Last recovery boot Env as MMC. If the image which is stored in the MMC is corrupted
+         and next recovery boot should not flash the image from MMC.*/
+        setenv("lastrecoveryboot","mmc");
+        saveenv();
+    }
+
+    goto SUCCESS;
+
+TFTP:
+    printf("TFTP Recovery Image Booting started...\n");
+
+    /*Start the TFTP recovery falshing*/
+    if (0 != TFTPRecoveryBoot())
+    {
+        printf("Recovery Image Flashing via TFTP Failed !!\n");
+        return -1;
+    }
+
+    /*Set the last recoveryboot as TFTP*/
+    setenv("lastrecoveryboot","tftp");
+    saveenv();
+
+SUCCESS:
+	/*After successfully flashing the image, start the watchdog timer to detect the kernel boot success.*/
+
+	if(wdt_reset_status || force_recovery || imgheadererr)
+	{
+		if(force_recovery == 1)
+		{
+			setenv("forcerecovery",NULL);
+			setenv("recoverysuccess", "3"); //force recovery 
+		}
+		else
+		{
+			setenv("recoverysuccess", "1");  
+		}
+		/*recoverysucess env bit 0 used for Recovery and bit 1 to diffrentiat a
+		force recovery*/
+		setenv("currentbootretry",NULL);
+		saveenv();
+		Enable_watchdog(&wdt_reset_status);
+		printf("Recovery Image Flashing is success!! Booting Recovery Image...\n");
+	}
+
+    return 0;
+}
+#endif
+
+/*
+ * fwupdate
+ */
+int  
+fwupdate(void)
+{	
+	int 			retries=0;
+	volatile int 	i;
+	unsigned long 	addr;
+	unsigned long 	cnt,BlkToUpgrade;
+	unsigned char	*TempBuf = (unsigned char*)YAFU_TEMP_SECTOR_BUFFER;
+	unsigned char	*src,*pRamAddress=0;
+	flash_info_t 	*pFlashInfo;
+    int bank = 0;
+	unsigned long 	BaseFlashAddrs = 0;
+
+ 	pFlashInfo = &flash_info[bank];
+	pKCSRes  = (KCSCmd_T*)(&m_KCSRes[0]);
+	pKCSCmd  = (KCSCmd_T*)(&m_KCSCmd[0]);
+	pIPMICmd = (IPMICmdData_T *)(&m_IPMICmd);
+	
+	printf ("Initing KCS...");
+	InitKCS ();
+	printf ("done\n");
+	gUbootTimeout = UBOOT_TIMEOUT_VALUE;
+
+	printf("Uboot waiting for firmware update to start...\n");
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
+
+		if(gStartFirmwareUpdation == 1) 
+		{
+
+		if (gFWUploadSelector == IMAGE_2)
+			BaseFlashAddrs =  CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;
+		BlkToUpgrade = gEraseBlock;
+
+		pRamAddress =(unsigned char *)YAFU_IMAGE_UPLOAD_LOCATION;
+									   
+		if((BlkToUpgrade /CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE) < (CONFIG_SPX_FEATURE_GLOBAL_FLASH_SIZE/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE) )
+		{
+
+			i = ( (BaseFlashAddrs + BlkToUpgrade) /CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);	
+						
+			/* Calculate the correct bank and get the flash_info of that bank*/
+            bank = (i  /  (pFlashInfo->size  / CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE));
+            i = i % (pFlashInfo->size  / CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);
+            pFlashInfo = &flash_info[bank];
+									
+			if (0 != flash_read (pFlashInfo->start[i], CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE, (char *)TempBuf))
+			{
+				printf ("Flash Read Failed at sector %d and retrying...\n", i);
+				continue;
+			}
+		
+			if (0 == memcmp ((unsigned char*)TempBuf, pRamAddress, CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE))	
+			{
+			 	printf ("sector %d did not change\n", i);
+				gStartFirmwareUpdation = 0;	 
+				continue;
+			}	
+			retries = 3;
+			while (retries)
+			{
+				if (0 != flash_erase (pFlashInfo, i, i))
+				{
+					printf ("Flash Erase Failed at sector %d and retrying...\n", i);
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
+			src   = pRamAddress;
+			addr = pFlashInfo->start[i];
+			cnt  = CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+			retries = 3;
+
+			while (retries)
+			{
+				if (0 != write_buff (pFlashInfo, src, addr, cnt))
+				{
+					printf ("\nFlash write failed at address =0x%lx of size= 0x%lx\n", addr, cnt);
+					retries--;
+					flash_erase (pFlashInfo, i, i);
+					continue;
+				}	
+				break;
+			}
+		}
+		if (0 == retries)
+		{
+			PerformFirmwareReset ();
+		}
+				
+		gStartFirmwareUpdation = 0;
+				
+	}
+
+
+	}
+	return 0;
+}	
+
