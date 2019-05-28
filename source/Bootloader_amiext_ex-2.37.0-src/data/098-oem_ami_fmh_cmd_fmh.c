--- uboot_old/oem/ami/fmh/cmd_fmh.c	2016-03-07 15:32:58.309297311 +0530
+++ uboot/oem/ami/fmh/cmd_fmh.c	2016-03-07 15:38:36.341296564 +0530
@@ -781,7 +781,7 @@
 	FMH	*fmh = NULL;
 	MODULE_INFO *mod = NULL;
 	unsigned long i=0,SectorSize=CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
-	unsigned long SectorCount,StartingSector =0 ;	
+	unsigned long SectorCount,StartingSector =0, startaddress;	
 	unsigned long initrd_address = 0xFFFFFFFF;
 	long SkipSize;
 	unsigned long ExecuteAddr = 0xFFFFFFFF;
@@ -874,32 +874,18 @@
 				return -1;
 			SectorCount = (CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/SectorSize);
 			StartingSector = 0;
+			startaddress=CONFIG_SYS_FLASH_BASE;
 		}
         else//for image2
 		{
 			StartingSector = (CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/SectorSize);
 			SectorCount = ( (2*CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE)/SectorSize );
+			startaddress=CONFIG_SYS_FLASH_BASE+CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;
 		}
 
 	/* Scan through the flash for FMH */
 	for (i=StartingSector;i<SectorCount;)
 	{
-		if (imagetoboot == IMAGE_2)
-		{		
-			if ((i*SectorSize) < CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE)
-			{
-				i++;
-				continue;
-			}
-		}
-		else if (imagetoboot == IMAGE_1)
-		{
-			if ((i*SectorSize) == CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE)//16MB
-			{
-				//printf("For img0 of dualimg in singlebank, flash size is completed @ %x\n",flinfo->size);
-				break;
-			}
-		}
 		/* Check if FMH found at this sector */
 
 		fmh = ScanforFMH((unsigned char *)(CONFIG_SYS_FLASH_BASE+(i*SectorSize)),SectorSize);
@@ -915,13 +901,14 @@
 		/*To avoid junk prints if module name length is equals to size */
 		strncpy((char *)mod_Name,(char *)mod->Module_Name,sizeof(mod->Module_Name));
 		mod_Name[sizeof(mod->Module_Name)]=0;
+
 #ifdef CONFIG_BOOTLOGO_SUPPORT
 		if ((le16_to_host(mod->Module_Type) == MODULE_BOOTLOGO))
 		{
 			//showLogo
 			if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8)){
 				flash_read( /*addr,cnt,dest */
-						(ulong)(CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location)),
+						(ulong)(startaddress + le32_to_host(mod->Module_Location)),
 						le32_to_host(mod->Module_Size),
 						(char *)le32_to_host(mod->Module_Load_Address));
 			} else {
@@ -946,7 +933,7 @@
 		{
 			UINT32 ChkSum;
 			if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
-				ChkSum =  CalculateCRC32((unsigned char *)CONFIG_SYS_FLASH_BASE+
+				ChkSum =  CalculateCRC32((unsigned char *)startaddress+
 							le32_to_host(mod->Module_Location),
 							le32_to_host(mod->Module_Size));
 			else
@@ -986,7 +973,7 @@
 				else
 				{
 					if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
-						initrd_address = CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location);
+						initrd_address = startaddress + le32_to_host(mod->Module_Location);
 					else
 						initrd_address = (CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location);
 					printf("Found INITRD Image for Root File System @ /dev/mtdblock%d flash address %lx\n",RootMtdNo,initrd_address);
@@ -1018,7 +1005,7 @@
 			}
 
 			if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
-				printf("%s @ /dev/mtdblock%d Address %x\n",mod_Name, MtdPartition, CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location));
+				printf("%s @ /dev/mtdblock%d Address %lx\n",mod_Name, MtdPartition, startaddress + le32_to_host(mod->Module_Location));
 			else
 				printf("%s @ /dev/mtdblock%d Address %lx\n",mod_Name, MtdPartition, (CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location));
 
@@ -1040,12 +1027,12 @@
 			{
 				if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
 				{
-					printf("Copying Module of size 0x%x From 0x%x to 0x%x\n",
+					printf("Copying Module of size 0x%x From 0x%lx to 0x%x\n",
 								le32_to_host(mod->Module_Size),
-								CONFIG_SYS_FLASH_BASE+le32_to_host(mod->Module_Location),
+								startaddress+le32_to_host(mod->Module_Location),
 								le32_to_host(mod->Module_Load_Address));
 					flash_read( /*addr,cnt,dest */
-		   					(ulong)(CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location)),
+		   					(ulong)(startaddress + le32_to_host(mod->Module_Location)),
 							 le32_to_host(mod->Module_Size),
 							(char *)le32_to_host(mod->Module_Load_Address));
 				}
@@ -1091,7 +1078,7 @@
 				if (Compression == MODULE_COMPRESSION_NONE)
 				{
 					if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
-						ExecuteAddr = CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location);
+						ExecuteAddr = startaddress + le32_to_host(mod->Module_Location);
 					else
 						ExecuteAddr = (CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location);
 				}
