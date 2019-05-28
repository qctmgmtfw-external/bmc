--- u-boot-2013.07/oem/ami/fmh/cmd_fmh.c	2015-09-20 22:48:00.233968069 -0400
+++ uboot/oem/ami/fmh/cmd_fmh.c	2015-09-21 01:20:15.038033068 -0400
@@ -0,0 +1,1385 @@
+# include <common.h>
+# include <config.h>
+# include <command.h>
+# include <flash.h>
+# include <i2c.h>
+# include "cmd_fmh.h"
+# include <oem/ami/fmh/fmh.h>
+# include "cmd.h"
+
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+# include <mmc.h>
+# include <fat.h>
+# include <net.h>
+extern int Firmware_AutoRecovery(u8 force_recovery, u8 imgheadererr);
+#endif
+
+# define  KERNEL_LOADADDR	0xC00000
+# define  KERNEL_FILE		"/boot/uImage"
+
+
+# define AUTO_HIGH_VER_FW          	0x0
+# define IMAGE_1                   	0x1
+# define IMAGE_2                   	0x2
+# define AUTO_LOW_VER_FW           	0x3
+# define MOST_RECENTLY_PROG_FW     	0x4
+# define LEAST_RECENTLY_PROG_FW    	0x5
+
+#define IMG1_index_val  0
+#define IMG2_index_val  1
+
+typedef struct
+{
+    u8 major_ver;
+    u8 minor_ver;
+#if (FMH_MAJOR==1 && FMH_MINOR==6)
+    u8 aux_ver[2];
+#elif (FMH_MAJOR==1 && FMH_MINOR>=7)
+    u8 aux_ver[6];
+#endif
+}VersionInfo;
+
+#ifdef CONFIG_BOOTLOGO_SUPPORT
+extern int ShowBootLogoByAddress(unsigned int logo_bin_addr, int idx);
+#endif
+
+extern int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
+extern int Get_bootargs(char *bootargs,int rootisinitrd,int RootMtdNo, int mmc);
+
+/* Note: Currently FMH support is done only for the first bank */
+extern flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];
+
+#if defined(CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING) || defined(CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY)
+extern int Enable_watchdog(unsigned long *wdt_timeout_status);
+extern int Disable_watchdog(void);
+#endif
+
+#ifdef CONFIG_YAFU_SUPPORT
+extern int fwupdate(void);
+#endif
+
+#ifdef CONFIG_R2C_SUPPORT
+extern int rconsole(void);
+#else
+int  
+UpdateFirmware(void)
+{
+#ifdef CONFIG_YAFU_SUPPORT
+	setenv("mode", "1"); 
+	fwupdate();
+#endif
+return 0;
+}
+int
+do_fwupdate(cmd_tbl_t *cmdtp, int flag , int argc, char * const argv[])
+{
+	return UpdateFirmware();
+}
+
+U_BOOT_CMD(
+	fwupdate,	1,	1,	do_fwupdate,
+	"Start Firmware Recovery Update",
+	""
+);
+
+#endif
+
+unsigned long g_wdt_reset_status = 0 ;
+
+
+int __ReadJumperConfig(void)
+{
+	return -1;
+}
+int ReadJumperConfig(void) 
+	__attribute__((weak, alias("__ReadJumperConfig")));
+
+/* U-boot's cmd function to list FMH */
+int
+do_fmh (cmd_tbl_t *cmdtp, int flag , int argc, char * const argv[])
+{
+	return ListFMH();
+}
+
+U_BOOT_CMD(
+	fmh,	1,	1,	do_fmh,
+	"List the Flash Module Headers",
+	""
+);
+
+/* U-boot's cmd function to boot using FMH  Has one argument with
+ * the values from 0 to 2.  It is  either  0 (OS), 1 (DIAG), 2 (RECOVERY)
+ */
+int
+do_bootfmh(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
+{
+	short PathID;
+
+	if (argc == 1)
+	{
+		/* Get the BootPath ID from the Hardware configuration Jumper */
+		PathID = ReadJumperConfig();
+
+		/* If Read Jumper Config not supported, boot to OS */
+		if (PathID == -1)
+			PathID = 0;
+	}
+	else
+	{
+		/* Get the BootPath ID from the Argument */
+		PathID = (unsigned short)simple_strtoul(argv[1],NULL,0);
+	}
+
+	if (PathID > 4)
+	{
+		printf("Usage : bootfmh <Arg> . Arg = 0 to 4\n");
+		return 1;
+	}
+
+	return BootFMH((unsigned short)1 << PathID);
+}
+
+U_BOOT_CMD(bootfmh,	2,	0,	do_bootfmh,
+	"Boot using FMH Modules",
+	"<Arg>  (Arg = 0 to 2)"
+);
+
+int
+do_bootmenu (cmd_tbl_t *cmdtp, int flag , int argc, char * const argv[])
+{
+	unsigned short PathID = 5;
+	unsigned char *mac;
+	
+	while (PathID >= 5)
+	{
+    	mac = (unsigned char *)getenv((char *)"ethaddr");
+		if (strncmp((char*)mac,"00:00:00:00:00:00",17) == 0)
+			printf("WARNING: Ethernet MAC Address is not set\n");
+		printf("------ Boot Options-------\n");
+		printf("\t0. Normal Boot\n");
+		printf("\t1. Diagnostics\n");
+		printf("\t2. Remote Recovery\n");
+		printf("\t3. Management Console\n");
+		printf("\t4. Raw Console\n");
+		printf("Select Boot Option:\n");
+		PathID = getc() - '0';
+		
+	}
+	return BootFMH((unsigned short)1 << PathID);
+}
+
+U_BOOT_CMD(bootmenu,	1,	1,	do_bootmenu,
+	"Show Boot Path menu",
+	""
+);
+
+
+/* Actual function implementing FMH Listing*/
+/* Returns 0 on success, else 1 on failure */
+int
+ListFMH(void)
+{
+	flash_info_t *flinfo;
+	unsigned long SectorSize=CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+	unsigned long SectorCount=0,flashsize=0;
+	long SkipSize;
+	int i,bank;
+	FMH	*fmh;
+	MODULE_INFO *mod;
+	unsigned char Name[9];
+
+	printf("Listing FMH Modules\n");
+
+	for (bank=0;bank<CONFIG_SYS_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		flashsize += flinfo->size;		
+		if (flinfo->size == 0)
+			break;
+	}
+	SectorCount = flashsize/SectorSize;
+	for (i=0;i<SectorCount;)
+	{	
+            #ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
+            if( (i*SectorSize) >= (2 * CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE) ) { break; }
+            #else
+            if( (i*SectorSize) >= CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE ) { break; }
+            #endif
+
+			fmh = ScanforFMH((unsigned char *)(CONFIG_SYS_FLASH_BASE+(i*SectorSize)),SectorSize);
+			if (fmh == NULL)
+			{
+				i++;
+				continue;
+			}
+
+			/* Print Information about the FMH and its module */
+			printf("\nFMH Located at 0x%08lx of Size 0x%08x\n",
+					(unsigned long)(CONFIG_SYS_FLASH_BASE+(i*SectorSize)),le32_to_host(fmh->FMH_AllocatedSize));
+
+			mod = &(fmh->Module_Info);
+			strncpy((char *)Name,(char *)mod->Module_Name,8);
+			Name[8]=0;
+			printf("Name    : %s\n",Name);
+			
+			if( FMH_MAJOR==1 && FMH_MINOR>=7 )
+				printf("Ver     : %d.%02d.%.6s\n",mod->Module_Ver_Major,mod->Module_Ver_Minor,mod->Module_Ver_Aux);
+			else if( FMH_MAJOR==1 && FMH_MINOR==6 )
+				printf("Ver     : %d.%02d.%.2s\n",mod->Module_Ver_Major,mod->Module_Ver_Minor,mod->Module_Ver_Aux);
+			else
+				printf("Ver     : %d.%02d\n",mod->Module_Ver_Major,mod->Module_Ver_Minor);
+
+			printf("Type 	: 0x%04x\n",le16_to_host(mod->Module_Type));
+			printf("Flags	: 0x%04x\n",le16_to_host(mod->Module_Flags));
+			printf("Size 	: 0x%08x\n",le32_to_host(mod->Module_Size));
+
+			if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+				printf("Location: 0x%08x\n", CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location));
+			else
+				printf("Location: 0x%08lx\n",(CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location));
+
+			printf("LoadAddr: 0x%08x\n",le32_to_host(mod->Module_Load_Address));
+
+			/* Validate CheckSum */
+			if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_VALID_CHECKSUM)
+			{
+				UINT32 ChkSum;
+				if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+					ChkSum=  CalculateCRC32((unsigned char *)CONFIG_SYS_FLASH_BASE+
+								le32_to_host(mod->Module_Location),
+								le32_to_host(mod->Module_Size));
+				else
+					ChkSum  = CalculateCRC32((unsigned char *)(CONFIG_SYS_FLASH_BASE+(i*SectorSize))+
+								le32_to_host(mod->Module_Location),
+								le32_to_host(mod->Module_Size));
+				if (ChkSum == le32_to_host(mod->Module_Checksum))
+					printf("CheckSum: Valid\n");
+				else
+					printf("CheckSum: Invalid\n");
+			}
+			else
+					printf("CheckSum: Not Computed\n");
+
+
+
+			printf("--------------------------------------------------\n");
+
+
+			if (i*SectorSize > le32_to_host(mod->Module_Location)) /* Special case of AltFMH (FMH location > Module_Location */
+				SkipSize=SectorSize;
+			else
+				/* Skip the Sectors occupied by the Module */
+				SkipSize = le32_to_host(fmh->FMH_AllocatedSize);
+
+			while ((SkipSize > 0) && (i < SectorCount))
+			{
+				SkipSize-=SectorSize;
+				i++;
+			}
+			
+		} /* sector */
+
+	return 0;
+}
+
+int
+GetFMHSectorLocationSize(char * Name, unsigned long * Location, unsigned long * Size)
+{
+        flash_info_t *flinfo;
+        unsigned long SectorSize=CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+        unsigned long SectorCount=0,flashsize=0;
+        long SkipSize;
+        int i,bank;
+        FMH     *fmh;
+        MODULE_INFO *mod;
+
+	if ((NULL == Name) || (NULL == Location) || (NULL == Size))
+	{
+		printf ("Invalid pointers to get FMH information\n");
+		return -1;
+	}
+
+        for (bank=0;bank<CONFIG_SYS_MAX_FLASH_BANKS;bank++)
+        {
+                flinfo = &flash_info[bank];
+                flashsize += flinfo->size;
+                if (flinfo->size == 0)
+                        break;
+        }
+
+        SectorCount = flashsize/SectorSize;
+        for (i=0;i<SectorCount;)
+        {
+                #ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
+                if( (i*SectorSize) >= (2 * CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE) ) { break; }
+                #else
+                if( (i*SectorSize) >= CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE ) { break; }
+                #endif
+
+                fmh = ScanforFMH((unsigned char *)(CONFIG_SYS_FLASH_BASE+(i*SectorSize)),SectorSize);
+                if (fmh == NULL)
+                {
+                        i++;
+                        continue;
+                }
+
+                mod = &(fmh->Module_Info);
+                if (0 == strncmp((char *)Name,(char *)mod->Module_Name,8))
+                {
+                        *Size = le32_to_host(mod->Module_Size);
+						if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+                        	*Location = CONFIG_SYS_FLASH_BASE  + le32_to_host(mod->Module_Location);
+						else
+                        	*Location = CONFIG_SYS_FLASH_BASE + (i*SectorSize) + le32_to_host(mod->Module_Location);
+			break;
+                }
+
+				if (i*SectorSize > le32_to_host(mod->Module_Location)) /* Special case of AltFMH (FMH location > Module_Location */
+					SkipSize=SectorSize;
+				else
+					/* Skip the Sectors occupied by the Module */
+					SkipSize = le32_to_host(fmh->FMH_AllocatedSize);
+                while ((SkipSize > 0) && (i < SectorCount))
+                {
+                        SkipSize-=SectorSize;
+                        i++;
+                }
+
+        } /* sector */
+
+        return (i >= SectorCount) ? -1 : 0;
+}
+
+/****************************************************************************/
+extern int gDefaultEnvironment;
+
+
+/**
+*@fn ValidateBootImage
+*@brief checks the boot image according to boot option.
+*@param imagetoboot[in]  -  boot option, firmware image versions.
+*@param imagetoboot[out] -  boot image.
+**/
+u8 ValidateBootImage(u8 bootopt,  VersionInfo fwimgver[])
+{
+    u8 bootimg = 0;
+    VersionInfo *fwinfo1 = NULL;
+    VersionInfo *fwinfo2 = NULL;
+	
+    if(bootopt == AUTO_HIGH_VER_FW)
+    {
+	    fwinfo1 = &fwimgver[IMG1_index_val];
+	    fwinfo2 = &fwimgver[IMG2_index_val];
+    }
+    if(bootopt == AUTO_LOW_VER_FW)
+    {
+	    fwinfo1 = &fwimgver[IMG2_index_val];
+	    fwinfo2 = &fwimgver[IMG1_index_val];
+    }
+
+    if ( fwinfo1->major_ver > fwinfo2->major_ver )
+        bootimg = IMAGE_1;
+    else if ( fwinfo1->major_ver < fwinfo2->major_ver )
+        bootimg = IMAGE_2;
+    else if ( fwinfo1->minor_ver > fwinfo2->minor_ver )
+        bootimg = IMAGE_1;
+    else if ( fwinfo1->minor_ver < fwinfo2->minor_ver )
+        bootimg = IMAGE_2;
+    else
+    {
+	if( FMH_MAJOR==1 && FMH_MINOR>=6 ) // AUX available with FMH version 1.6 and above
+	{
+	    if(strncmp((char *)fwinfo1->aux_ver,(char *)fwinfo2->aux_ver,sizeof(fwinfo1->aux_ver)) > 0)
+	        bootimg = IMAGE_1;
+	    else if(strncmp((char *)fwinfo1->aux_ver,(char *)fwinfo2->aux_ver,sizeof(fwinfo1->aux_ver)) < 0)
+	        bootimg = IMAGE_2;
+	    else
+            	bootimg = 0;
+	}
+    }
+
+return bootimg;
+}
+
+
+/**
+*@fn ReadEnvVarAndGetBootOption
+*@brief Reads the Env variable bootselector, based on the value set it env variable, the image to be booted is assigned
+*@param imagetoboot[in] - the image to be booted (image1 or image2).
+*@param failsafeboot[in] - is it fail safe boot.
+*@param firmware_ver[in] - firmware info versions.
+*@param [out]  0 - on success
+*             -1 - on error
+**/
+int  ReadEnvVarAndGetBootOption(u8 *imagetoboot,u8 failsafeboot,  VersionInfo  firmware_ver[], VersionInfo  platform_ver[])
+{
+      char *selector = NULL;
+      char *update   = NULL;
+      u8 boot_opt, recent_img, boot_img;
+
+      selector = getenv("bootselector");
+      if (selector == NULL) {
+           printf("uboot env variable bootselector not present \n");
+           return -1;
+      }
+      boot_opt = *selector - 48;
+
+      update = getenv("recentlyprogfw");
+      if (update == NULL) {
+          printf(" uboot env variable recentlyprogfw not present");
+          return -1;
+      }
+      recent_img = *update - 48;
+
+
+      switch(boot_opt)
+      {
+	case IMAGE_1:
+		*imagetoboot = IMAGE_1;
+	        break;
+	case IMAGE_2:
+		*imagetoboot = IMAGE_2;
+		break;
+	case AUTO_HIGH_VER_FW:
+                if( (boot_img = ValidateBootImage(AUTO_HIGH_VER_FW, firmware_ver) ))
+                {
+                    *imagetoboot = boot_img;
+                }// ValidateBootImage will return zero if both firmware image versions matches, then need to check platform versions
+                else if( (boot_img = ValidateBootImage(AUTO_HIGH_VER_FW, platform_ver) ) )
+                {
+                    *imagetoboot = boot_img;
+                }// ValidateBootImage will return zero if both platform versions matches, then booting image1 as defautly.
+                else
+                {
+                    *imagetoboot = IMAGE_1;
+                }
+                break;
+	case AUTO_LOW_VER_FW:
+                if( (boot_img = ValidateBootImage(AUTO_LOW_VER_FW, firmware_ver) ))
+                {
+                    *imagetoboot = boot_img;
+                }// ValidateBootImage will return zero if both firmware image versions  matches, then need to check platform versions
+                else if( (boot_img = ValidateBootImage(AUTO_LOW_VER_FW, platform_ver) ))
+                {
+                    *imagetoboot = boot_img;
+                }// ValidateBootImage will return zero if both platform versions matches, then booting image1 as defautly.
+                else
+                {
+                    *imagetoboot = IMAGE_1;
+                }
+                break;
+	case MOST_RECENTLY_PROG_FW:
+		if ( recent_img == IMAGE_1 )
+			*imagetoboot = IMAGE_1;
+		else
+			*imagetoboot = IMAGE_2;
+
+		break;
+	case  LEAST_RECENTLY_PROG_FW:
+                recent_img = *update - 48;
+		if ( recent_img == IMAGE_1 )
+			*imagetoboot = IMAGE_2;
+		else
+			*imagetoboot = IMAGE_1;
+		break;
+	default:
+	      printf("Invalid boot option \n");
+              return -1;
+	      break;
+      }
+
+      if(failsafeboot)
+      {
+	 if(*imagetoboot == IMAGE_1)
+	 {
+		setenv("fwimage1corrupted","yes");
+		setenv("bootselector","2");
+		saveenv();
+		*imagetoboot = IMAGE_2;
+	 }
+	 else if(*imagetoboot == IMAGE_2)
+	 {
+		setenv("fwimage2corrupted","yes");
+		setenv("bootselector","1");
+		saveenv();
+		*imagetoboot = IMAGE_1;
+	 }
+         else
+       	 {
+             printf("Invalid Boot option...\n");
+             return -1;
+         }
+      }
+
+return 0;
+}
+
+
+/**
+*@fn GetBootImage
+*@brief provides image to boot. 
+*@param imagetoboot[in] - the image to be booted (image1 or image2).
+*@param [out]  0 - on success
+*             -1 - on error
+**/
+int GetBootImage(u8 *imagetoboot)
+{
+	flash_info_t *flinfo;
+	unsigned long i=0,SectorSize=CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+	unsigned long SectorCount=0,flashsize=0;
+	unsigned long watchdog_reset_status = 0;
+	u8 imgIndex=0,bank=0,img1_presence=0,img2_presence=0;
+	FMH	*fmh = NULL;
+	MODULE_INFO *mod = NULL;
+	VersionInfo firmware_ver[2], platform_ver[2];
+
+
+	#ifdef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+		Enable_watchdog(&watchdog_reset_status);
+	#endif
+
+
+	for (bank=0;bank<CONFIG_SYS_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		flashsize += flinfo->size;
+		if (flinfo->size == 0)
+			break;
+	}
+
+	SectorCount=flashsize/SectorSize;
+
+        memset(&firmware_ver,0,sizeof(firmware_ver));
+        memset(&platform_ver,0,sizeof(platform_ver));
+ 
+		/* Scan through the flash for FMH */
+		for (i=0;i<SectorCount;)
+		{
+	            #ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
+        	        if( (i*SectorSize) >= (2 * CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE) ) { break; }
+	            #else
+        	        if( (i*SectorSize) >= CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE ) { break; }
+	            #endif
+			
+			/* Check if FMH found at this sector */
+			fmh = ScanforFMH((unsigned char *)(CONFIG_SYS_FLASH_BASE+(i*SectorSize)),SectorSize);
+			if (fmh == NULL)
+			{
+				i++;
+				continue;
+			}
+
+			/* Extract Module information and module name */
+			mod = &(fmh->Module_Info);
+						
+			/* if firmware contains platform module need to consider*/
+			if( (mod->Module_Type) == MODULE_JFFS2)
+			{
+				if( strncmp((char *)mod->Module_Name,"platform",8)==0 )
+				{
+					platform_ver[imgIndex].major_ver = mod->Module_Ver_Major;
+					platform_ver[imgIndex].minor_ver = mod->Module_Ver_Minor;
+					if( FMH_MAJOR == 1 && FMH_MINOR == 6 )
+		        		{			
+						strncpy((char *)platform_ver[imgIndex].aux_ver,(char *)mod->Module_Ver_Aux, 2);
+					}					
+					else if(FMH_MAJOR == 1 && FMH_MINOR >= 7)
+					{
+						strncpy((char *)platform_ver[imgIndex].aux_ver,(char *)mod->Module_Ver_Aux,sizeof(mod->Module_Ver_Aux));
+					}
+				}
+			}
+
+	
+			if( (mod->Module_Type) == MODULE_FMH_FIRMWARE)
+			{
+				firmware_ver[imgIndex].major_ver = mod->Module_Ver_Major;
+				firmware_ver[imgIndex].minor_ver = mod->Module_Ver_Minor;
+				if( FMH_MAJOR == 1 && FMH_MINOR == 6 )
+		                {
+					strncpy((char *)firmware_ver[imgIndex].aux_ver,(char *)mod->Module_Ver_Aux, 2);
+				}
+				else if(FMH_MAJOR == 1 && FMH_MINOR >= 7)
+				{
+					strncpy((char *)firmware_ver[imgIndex].aux_ver,(char *)mod->Module_Ver_Aux,sizeof(mod->Module_Ver_Aux));
+				}                   
+				if( imgIndex == IMG1_index_val)
+					img1_presence = 1;
+				if( imgIndex == IMG2_index_val)
+					img2_presence = 1;
+		        }
+			i++;
+			
+			if ((i*SectorSize) >= CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE)//Image Size
+			{
+				if(mod->Module_Type == FIRMWARE_INFO_TYPE)
+				{
+					imgIndex++;
+				}
+			}
+		}
+
+                /* check whether reboot caused by watchdog modules reset */
+	        if(watchdog_reset_status)
+		{
+                        /*if both images corrupts uboot has to launch Recovery Console */
+                        if( getenv("fwimage1corrupted") && getenv("fwimage2corrupted") )
+                        {
+                                printf("Both Images corrupted so moving Remote Recovery Mode...\n");
+                                /*Need to disable watchdog reset before firmware Recovery */
+                                #ifdef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+                                    Disable_watchdog();
+                                #endif
+
+				#ifdef CONFIG_R2C_SUPPORT
+					return rconsole();      /* Remote Recovery Mode */
+				#else
+					return UpdateFirmware();
+				#endif
+                        } 
+                        else
+                        {
+	        	   // Check uboot env variable bootselector and get boot option for failsafeboot 
+			   if( ReadEnvVarAndGetBootOption(imagetoboot,1,firmware_ver, platform_ver) != 0)
+			      return -1;
+                        }
+		}
+		else
+		{
+			 if( !img1_presence && img2_presence)
+			 {
+       			     // if any of the firmware image corrupted or not flashed, going to boot valid firmware image 
+	        	     *imagetoboot = IMAGE_2;
+			 }
+			 else if( !img2_presence && img1_presence)
+			 {
+	        	     // if any of the firmware image corrupted or not flashed, going to boot valid firmware image
+        		     *imagetoboot = IMAGE_1;
+			 }
+			 else if( img2_presence && img1_presence)
+			 {
+	        	     // Check uboot env variable bootselector and get boot option
+			     if( ReadEnvVarAndGetBootOption(imagetoboot,0,firmware_ver, platform_ver) != 0)
+	                     return -1; 
+			 }
+			 else
+			 {
+				printf("Both Images corrupted/Not Flashed so moving to Remote Recovery Mode...\n");
+                                #ifdef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+                                    Disable_watchdog();
+                                #endif
+		                /*Need to disable watchdog reset before firmware Recovery */
+				#ifdef CONFIG_R2C_SUPPORT
+					return rconsole();      /* Remote Recovery Mode */
+				#else
+					return UpdateFirmware();
+				#endif
+		 	 }
+		}
+
+return 0;
+}
+
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+void BMCAutoRecovery (void)
+{
+    char *pStr;
+    char value[2] = {0};
+    u8 force_recovery = 0, imgheadererr = 0, bootretry = 1, currentbootretry = 0;
+
+    /*Check whether force recovery image boot is triggered.*/
+    pStr= getenv("forcerecovery");
+    if ((pStr!= NULL ) && (strcmp(pStr,"yes") == 0))
+    {
+        force_recovery = 1;
+    }
+
+    pStr = NULL;
+    pStr = getenv("imgheadererr");
+    if ((pStr != NULL ) && (strcmp(pStr,"yes") == 0))
+    {
+        imgheadererr = 1;
+    }
+
+    pStr = NULL;
+    pStr = getenv("bootretry");
+    if (pStr != NULL )
+    {
+        bootretry = (int)simple_strtoul(pStr,NULL,0);
+    }
+
+    pStr = NULL;
+    pStr = getenv("currentbootretry");
+    if (pStr != NULL )
+    {
+        currentbootretry = (int)simple_strtoul(pStr,NULL,0);
+    }
+
+    currentbootretry++;
+    sprintf(value,"%d",currentbootretry);
+    setenv("currentbootretry",(char *)value);
+    saveenv();
+
+    if(force_recovery || imgheadererr || (currentbootretry > bootretry) )
+    {
+        if ( Firmware_AutoRecovery(force_recovery, imgheadererr) != 0 )
+        {
+            if(force_recovery == 1)
+            {
+                setenv("forcerecovery",NULL);
+                saveenv();
+            }
+
+            /*Disable the watchdog as both method was failed*/
+            Disable_watchdog();
+
+            /*Reset the lastrecoveryboot*/
+            setenv("lastrecoveryboot",NULL);
+            setenv("recoveryinprog",NULL);
+            saveenv();
+            /*Wait On KCS for Fimware update when Auto-Recovery failed */ 
+            fwupdate();
+            return;
+        }
+    }
+    else
+    {
+        Enable_watchdog(&g_wdt_reset_status);
+        if (g_wdt_reset_status & 0x00000002)
+        {
+            setenv("goldenside", "1");
+        }
+        else
+        {
+            setenv("goldenside", "0");
+        }
+
+        saveenv();
+
+    }
+
+}
+#endif
+
+	
+int
+BootFMH(unsigned short PathID)
+{
+	char Command[128] = "FMHEXECUTE";
+	char AddrStr[128] = "0xFFFFFFFF";
+	char bootargs[256] = {0};
+	char imagebooted[16] = {0};
+    char goldenside[16];
+	char INITRDAddrStr[128] = "0xFFFFFFFF";
+	char baudrate_str[16] = {0};
+	unsigned char mod_Name[9]={0};
+	char FSName[16] = {0};  /* Hope no filesystem name is bigger then 15 char long */
+	u8 imagetoboot = IMAGE_1; /* By default will boot image-1 */
+	char *argv[6];
+	char *s = NULL;
+	flash_info_t *flinfo = NULL;
+	FMH	*fmh = NULL;
+	MODULE_INFO *mod = NULL;
+	unsigned long i=0,SectorSize=CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+	unsigned long SectorCount,StartingSector =0 ;	
+	unsigned long initrd_address = 0xFFFFFFFF;
+	long SkipSize;
+	unsigned long ExecuteAddr = 0xFFFFFFFF;
+	unsigned short ExecuteType = MODULE_BOOTLOADER;
+	unsigned short Compression;
+	int retval;
+	int  silent = 0;
+	int MtdPartition = 1;		/* mtdblock0 is used for full flash */
+	int RootMtdNo = 0;
+	int rootisinitrd = 0; /*indicates root is actually an initrd*/
+    char *side = NULL;
+    u8 golden_side;
+
+#ifdef CONFIG_YAFU_SUPPORT
+	int  boot_fwupd = 0;
+#endif
+
+#if defined (CONFIG_CMD_EXT4) && defined (CONFIG_CMD_MMC) && defined (CONFIG_SPX_FEATURE_MMC_BOOT)
+	char bootpart[256] = {0};
+	int str_pos =0;
+	int devicenumber =0;
+	char partition[128] = {0};
+	char *bootselect = NULL;
+#endif
+
+	sprintf(baudrate_str,"%d",CONFIG_BAUDRATE);
+
+#ifdef CONFIG_YAFU_SUPPORT
+	s = getenv("boot_fwupd");
+	if (s != NULL)
+	{
+		boot_fwupd = (int)simple_strtoul(s,NULL,0);
+		printf("boot_fwupd is %d\n",boot_fwupd);
+	}
+
+	/* Check the variable to find out if we need to go into fwupdate mode or not */
+	if(boot_fwupd)
+	{
+		/* If variable is set, then clear it and call fwupdate() */
+		setenv("mode", "1");   
+		saveenv();
+		fwupdate();
+	}
+#endif
+	if (gDefaultEnvironment == 1)
+	{
+		gDefaultEnvironment = 0;
+		saveenv ();
+	}
+
+	/* silentboot = 0 (Verbose) 2 (Progress bar) 1 (Quiet) */
+	s = getenv("silent");
+	if (s != NULL)
+		silent = (int)simple_strtoul(s,NULL,0);
+
+	if (PathID == 0x04)
+	{
+		#ifdef CONFIG_R2C_SUPPORT
+			return rconsole();      /* Remote Recovery Mode */
+		#else
+			return UpdateFirmware();
+		#endif
+	}
+	if (PathID == 0x08)
+	{
+		return 0;			/* Management Console (Restricted Version of Uboot) */
+	}
+	if (PathID == 0x10)
+	{
+		return 0;			/* Full Access to Uboot Console */
+	}
+
+
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
+    if( GetBootImage(&imagetoboot) != 0 )
+	 return -1;
+#endif
+
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+        BMCAutoRecovery();
+#endif
+
+	printf("Image to be booted is %d\n",imagetoboot);
+
+        if(imagetoboot == IMAGE_1)
+		{
+			flinfo = &flash_info[IMG1_index_val];
+			if (flinfo->size == 0)
+				return -1;
+			SectorCount = (CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/SectorSize);
+			StartingSector = 0;
+		}
+        else//for image2
+		{
+			StartingSector = (CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/SectorSize);
+			SectorCount = ( (2*CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE)/SectorSize );
+		}
+
+	/* Scan through the flash for FMH */
+	for (i=StartingSector;i<SectorCount;)
+	{
+		if (imagetoboot == IMAGE_2)
+		{		
+			if ((i*SectorSize) < CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE)
+			{
+				i++;
+				continue;
+			}
+		}
+		else if (imagetoboot == IMAGE_1)
+		{
+			if ((i*SectorSize) == CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE)//16MB
+			{
+				//printf("For img0 of dualimg in singlebank, flash size is completed @ %x\n",flinfo->size);
+				break;
+			}
+		}
+		/* Check if FMH found at this sector */
+
+		fmh = ScanforFMH((unsigned char *)(CONFIG_SYS_FLASH_BASE+(i*SectorSize)),SectorSize);
+		if (fmh == NULL)
+		{
+			i++;
+			continue;
+		}
+
+		/* Extract Module information and module name */
+		mod = &(fmh->Module_Info);
+		
+		/*To avoid junk prints if module name length is equals to size */
+		strncpy((char *)mod_Name,(char *)mod->Module_Name,sizeof(mod->Module_Name));
+		mod_Name[sizeof(mod->Module_Name)]=0;
+#ifdef CONFIG_BOOTLOGO_SUPPORT
+		if ((le16_to_host(mod->Module_Type) == MODULE_BOOTLOGO))
+		{
+			//showLogo
+			if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8)){
+				flash_read( /*addr,cnt,dest */
+						(ulong)(CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location)),
+						le32_to_host(mod->Module_Size),
+						(char *)le32_to_host(mod->Module_Load_Address));
+			} else {
+				flash_read( /*addr,cnt,dest */
+						(ulong)((CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location)),
+						le32_to_host(mod->Module_Size),
+						(char *)le32_to_host(mod->Module_Load_Address));
+			}
+			ShowBootLogoByAddress( le32_to_host(mod->Module_Load_Address), 0);
+		}
+#endif
+
+		/* Check if Path ID Matches */
+		if (!(le16_to_host(mod->Module_Flags) & PathID))
+		{
+			goto Skip;
+			continue;
+		}
+
+		/* Check if module checksum to be validated */
+		if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_VALID_CHECKSUM)
+		{
+			UINT32 ChkSum;
+			if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+				ChkSum =  CalculateCRC32((unsigned char *)CONFIG_SYS_FLASH_BASE+
+							le32_to_host(mod->Module_Location),
+							le32_to_host(mod->Module_Size));
+			else
+				ChkSum = CalculateCRC32((unsigned char *)(CONFIG_SYS_FLASH_BASE+(i*SectorSize))+
+							le32_to_host(mod->Module_Location),
+							le32_to_host(mod->Module_Size));
+			if (ChkSum == le32_to_host(mod->Module_Checksum))
+			{
+				printf("CheckSum is Invalid. Ignoring module %s \n",mod_Name);
+				goto Skip;
+				continue;
+			}
+		}
+
+		if ((le16_to_host(mod->Module_Type) == MODULE_JFFS2) ||
+			(le16_to_host(mod->Module_Type) == MODULE_JFFS) ||
+			(le16_to_host(mod->Module_Type) == MODULE_CRAMFS) ||
+			(le16_to_host(mod->Module_Type) == MODULE_SQUASHFS) ||
+			(le16_to_host(mod->Module_Type) == MODULE_INITRD_CRAMFS) ||
+			(le16_to_host(mod->Module_Type) == MODULE_INITRD_SQUASHFS))
+		{
+			if (strncasecmp((char *)mod->Module_Name,"ROOT",8) == 0)
+			{
+				if(le16_to_host(mod->Module_Type) == MODULE_INITRD_CRAMFS)
+				{
+					rootisinitrd = 1; //found root and it is initrd
+				}
+				if(le16_to_host(mod->Module_Type) == MODULE_INITRD_SQUASHFS)
+				{
+					rootisinitrd = 1; //found root and it is initrd
+				}
+				RootMtdNo = MtdPartition;
+				if(rootisinitrd == 0)
+				{
+					printf("Found Root File System @ /dev/mtdblock%d\n",RootMtdNo);
+				}
+				else
+				{
+					if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+						initrd_address = CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location);
+					else
+						initrd_address = (CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location);
+					printf("Found INITRD Image for Root File System @ /dev/mtdblock%d flash address %lx\n",RootMtdNo,initrd_address);
+				}
+
+ 				switch (le16_to_host(mod->Module_Type))
+        		{
+         			case MODULE_JFFS:
+               			printf("Root File System is JFFS\n");
+                		strncpy(FSName,"jffs",15);
+                		break;
+            		case MODULE_JFFS2:
+               			printf("Root File System is JFFS2\n");
+           			    strncpy(FSName,"jffs2",15);
+              			break;
+           			case MODULE_CRAMFS:
+         			case MODULE_INITRD_CRAMFS:
+               			printf("Root File System is CRAMFS\n");
+                		strncpy(FSName,"cramfs",15);
+                		break;
+            		case MODULE_SQUASHFS:
+         			case MODULE_INITRD_SQUASHFS:
+                		printf("Root File System is SQUASHFS\n");
+                		strncpy(FSName,"squashfs",15);
+                		break;
+            		default:
+                		strncpy(FSName,"",15);
+       	 		}
+			}
+
+			if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+				printf("%s @ /dev/mtdblock%d Address %x\n",mod_Name, MtdPartition, CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location));
+			else
+				printf("%s @ /dev/mtdblock%d Address %lx\n",mod_Name, MtdPartition, (CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location));
+
+			MtdPartition++;
+		}
+
+
+		/* Calculate the compresion Type */
+		Compression = le16_to_host(mod->Module_Flags);
+	    	Compression &= MODULE_FLAG_COMPRESSION_MASK;
+		Compression >>= MODULE_FLAG_COMPRESSION_LSHIFT;
+
+
+		/* If module to be loaded to memory */
+		if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_COPY_TO_RAM)
+		{
+			/* TODO: If compression is enabled, decompress to ram */
+			if (Compression == MODULE_COMPRESSION_NONE)
+			{
+				if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+				{
+					printf("Copying Module of size 0x%x From 0x%x to 0x%x\n",
+								le32_to_host(mod->Module_Size),
+								CONFIG_SYS_FLASH_BASE+le32_to_host(mod->Module_Location),
+								le32_to_host(mod->Module_Load_Address));
+					flash_read( /*addr,cnt,dest */
+		   					(ulong)(CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location)),
+							 le32_to_host(mod->Module_Size),
+							(char *)le32_to_host(mod->Module_Load_Address));
+				}
+				else
+				{
+					printf("Copying Module of size 0x%x From 0x%lx to 0x%x\n",
+								le32_to_host(mod->Module_Size),
+								(CONFIG_SYS_FLASH_BASE+(i*SectorSize))+le32_to_host(mod->Module_Location),
+								le32_to_host(mod->Module_Load_Address));
+
+					flash_read( /*addr,cnt,dest */
+		   					(ulong)((CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location)),
+							 le32_to_host(mod->Module_Size),
+							(char *)le32_to_host(mod->Module_Load_Address));
+				}
+
+				if (strncasecmp((char *)mod->Module_Name,"ROOT",8) == 0)
+				{
+					if(rootisinitrd == 1)
+					{
+						initrd_address =  le32_to_host(mod->Module_Load_Address);
+						printf("INITRD Image moved to 0x%lx\n",initrd_address);
+					}
+				}
+			}
+			else
+			{
+				/* TODO: If compression is enabled, decompress to ram */
+				printf("ERROR: Compression support not present in U-Boot\n");
+			}
+		}
+
+		/* If no other previous module to be executed and this module to
+		 * be executed. Save the ExecuteAddr for later execution */
+		if ((le16_to_host(mod->Module_Flags) & MODULE_FLAG_EXECUTE)
+						&& 	(ExecuteAddr == 0xFFFFFFFF))
+		{
+			ExecuteType =le16_to_host(mod->Module_Type);
+			if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_COPY_TO_RAM)
+				ExecuteAddr =le32_to_host(mod->Module_Load_Address);
+			else
+			{
+				if (Compression == MODULE_COMPRESSION_NONE)
+				{
+					if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+						ExecuteAddr = CONFIG_SYS_FLASH_BASE + le32_to_host(mod->Module_Location);
+					else
+						ExecuteAddr = (CONFIG_SYS_FLASH_BASE+(i*SectorSize)) + le32_to_host(mod->Module_Location);
+				}
+				else
+					printf("ERROR: Compressed Module cannot be XIP\n");
+			}
+		}
+
+		/* Skip the Sectors occupied by the Module */
+Skip:
+		if (i*SectorSize > le32_to_host(mod->Module_Location)) /* Special case of AltFMH (FMH location > Module_Location */
+				SkipSize=SectorSize;
+		else
+			/* Skip the Sectors occupied by the Module */
+			SkipSize = le32_to_host(fmh->FMH_AllocatedSize);
+		while ((SkipSize > 0) && (i < SectorCount))
+		{
+			/*if (i == (SectorCount-1))
+				SectorSize= flinfo->size - flinfo->start[i];
+			else
+				SectorSize = flinfo->start[i+1]-flinfo->start[i];
+			*/
+			SkipSize-=SectorSize;
+			i++;
+		}
+	}	/* For Scan */
+
+
+
+	/* Create the bootarguments */
+#ifdef  CONFIG_SPX_FEATURE_MMC_BOOT 
+	if (1)
+#else
+	if (RootMtdNo > 0) 
+#endif
+	{
+		// Some flash does auto protect on powerup. Linux MTD as of now will return 
+        // error on writes to protected blocks. This will create problems in JFFS2 
+		// writes. So unprotect all blocks before booting to linux
+		argv[0] = &Command[0];
+		argv[1] = "off";
+		argv[2] = "all";
+		argv[3] = NULL;
+		do_protect (NULL,0,3,argv);
+#ifdef  CONFIG_SPX_FEATURE_MMC_BOOT 
+		Get_bootargs(bootargs,rootisinitrd,RootMtdNo,1);
+#else
+		Get_bootargs(bootargs,rootisinitrd,RootMtdNo,0);
+#endif
+/*
+		//we found root mtd.now we see if it is initrd
+		if(rootisinitrd == 1)
+		{
+			sprintf(bootargs,"root=/dev/ramdisk ro ip=none ramdisk_blocksize=4096 ");
+		}
+		else
+		{
+			sprintf(bootargs,"root=/dev/mtdblock%d ro ip=none ",RootMtdNo);
+		}
+*/
+		if (silent > 1)
+		{
+			strcat(bootargs,"console=");
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_CONSOLE_TTY
+			strcat(bootargs,CONFIG_SPX_FEATURE_GLOBAL_CONSOLE_TTY);
+#else
+			strcat(bootargs,"ttyS0");
+#endif
+			strcat(bootargs,",");
+			strcat(bootargs,baudrate_str);
+			strcat(bootargs," bootprogress");
+		}
+		else
+		{
+			if (silent == 1)
+				strcat(bootargs,"console=null");
+			else
+			{
+				strcat(bootargs,"console=");
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_CONSOLE_TTY
+				strcat(bootargs,CONFIG_SPX_FEATURE_GLOBAL_CONSOLE_TTY);
+#else
+				strcat(bootargs,"ttyS0");
+#endif
+				strcat(bootargs,",");
+				strcat(bootargs,baudrate_str);
+			}
+		}
+	
+		if (FSName[0] != 0)
+		{
+			strcat(bootargs," rootfstype=");
+			strcat(bootargs,FSName);
+		}
+
+#ifdef CONFIG_BIGPHYSAREA
+		strcat(bootargs," bigphysarea=" CONFIG_BIGPHYSAREA);
+#endif		
+#ifdef CONFIG_EXTRA_BOOTARGS 
+		strcat(bootargs," " CONFIG_EXTRA_BOOTARGS);
+#endif
+		sprintf(imagebooted," imagebooted=%d",imagetoboot);
+		strcat(bootargs,imagebooted);		
+
+        side = getenv("goldenside");
+        if (side == NULL) 
+        {
+            printf("Booting from Primary side\n");
+            golden_side = 0;
+        }
+        else
+        {
+            golden_side = *side - 48;
+        }
+
+        if (golden_side)
+        {
+            sprintf(goldenside, " goldenside=1");
+            strcat(bootargs,goldenside); 
+        }
+	}
+
+	/* Check if anything to be executed */
+	if (ExecuteAddr != 0xFFFFFFFF)
+	{
+		/* Form Arguments */
+		if(rootisinitrd != 1)
+		{
+			sprintf(AddrStr,"0x%lx",ExecuteAddr);
+			argv[0] = &Command[0];
+			argv[1] = &AddrStr[0];
+			argv[2] = NULL;
+		}
+		else
+		{
+			sprintf(AddrStr,"0x%lx",ExecuteAddr);
+			sprintf(INITRDAddrStr,"0x%lx",initrd_address);
+			argv[0] = &Command[0];
+			argv[1] = &AddrStr[0];
+			argv[2] = &INITRDAddrStr[0];
+			argv[3] = NULL;
+		}
+
+		/* If module is a ELF Executable */
+		if (ExecuteType == MODULE_ELF)
+		{
+			if (silent == 1)
+				console_assign(stdout,"nulldev");
+			retval = do_bootelf(NULL,0,2,argv);
+			console_assign(stdout,"serial");
+			return retval;
+		}
+
+		/* If module is a U-Boot Format Linux Image */
+		if (ExecuteType == MODULE_PIMAGE)
+		{
+			printf("Booting from MODULE_PIMAGE ...\n");
+			printf("Bootargs = [%s]\n",bootargs);
+			setenv("bootargs",bootargs);
+			if (silent == 1)
+				console_assign(stdout,"nulldev");
+			if(rootisinitrd != 1)
+			{
+				retval =  do_bootm(NULL,0,2,argv);
+			}
+			else
+			{
+				setenv("initrd_high","off");
+				retval = do_bootm(NULL,0,3,argv);
+			}
+			console_assign(stdout,"serial");
+			printf("Failed\n");
+			return retval;
+		}
+
+		/* Other Executable modules, jump directly */
+		if ((ExecuteType == MODULE_KERNEL) || (ExecuteType == MODULE_UNKNOWN))
+		{
+			if (silent == 1)
+				console_assign(stdout,"nulldev");
+			retval =  do_go(NULL,0,2,argv);
+			console_assign(stdout,"serial");
+			return retval;
+		}
+	}
+
+	/* If PathID is 1 (OS) and nothing was executed, try booting
+	 * kernel from JFFS2 File system if any available
+	 */
+	if ((PathID == 1) && (RootMtdNo > 0))
+	{
+#if (CONFIG_CMD_JFFS2)
+		printf("Booting from image in %s ...\n",KERNEL_FILE);
+		printf("Bootargs = [%s]\n",bootargs);
+		setenv("bootargs",bootargs);
+		if (silent == 1)
+			console_assign(stdout,"nulldev");
+		/* Change active JFFS2 partition to wherever  (Root FS ) */
+		sprintf(AddrStr,"%d",RootMtdNo-1);	/* -1 because Zero Based */
+		argv[0] = &Command[0];
+		argv[1] = &AddrStr[0];
+		argv[2] = NULL;
+		do_jffs2_chpart(NULL,0,2,argv);
+
+		/* Load linux kernel into memory */
+		sprintf(AddrStr,"0x%x",KERNEL_LOADADDR);
+		argv[0] = &Command[0];
+		argv[1] = &AddrStr[0];
+		argv[2] = KERNEL_FILE;
+		argv[3] = NULL;
+		if (do_jffs2_fsload(NULL,0,3,argv) == 0 )
+		{
+			argv[2] = NULL;
+			/* Boot the kernel from memory */
+			retval = do_bootm(NULL,0,2,argv);
+			console_assign(stdout,"serial");
+			printf("Failed\n");
+			return retval;
+		}
+		console_assign(stdout,"serial");
+#else
+        printf("JFFS2 support is not enabled - Cannot locate kernel File in Root\n");
+#endif                
+		printf("Unable to locate %s\n",KERNEL_FILE);
+	}
+
+#if defined (CONFIG_CMD_EXT4) && defined (CONFIG_CMD_MMC) && defined (CONFIG_SPX_FEATURE_MMC_BOOT)
+	printf("Booting from emmc/sd using image %s ...\n",KERNEL_FILE);
+	printf("Bootargs = [%s]\n",bootargs);
+	setenv("bootargs",bootargs);
+	if (silent == 1)
+		console_assign(stdout,"nulldev");
+
+	retval = snprintf(AddrStr,128,"0x%x",CONFIG_SYS_LOAD_ADDR);
+	if(retval < 0 || retval >= 128)
+	{
+		return retval;
+	}
+
+	strncpy(bootpart,&bootargs[10],9);//Copy boot partition from bootargs to bootpart 
+	str_pos = strspn(bootpart,"mmcblk");
+	devicenumber = simple_strtoul( (bootpart + str_pos), NULL, 10);
+	str_pos += 2;
+
+#if CONFIG_SD_BOOT_SELECTOR 
+	bootselect = getenv("sdbootselector");
+	str_pos = simple_strtoul(bootselect,NULL,10);	
+#else
+	strncpy(partition,&bootpart[str_pos],strlen(&bootpart[str_pos]));
+	str_pos = simple_strtoul(partition,NULL,10);
+	memset(partition,0,sizeof(partition));
+#endif
+	retval = snprintf(partition,128,"%d:%d", devicenumber, str_pos);
+	if(retval < 0 || retval >= 128)
+	{
+		return retval;
+	}
+
+	argv[0] = "ext4load";
+	argv[1] = "mmc";
+	argv[2] = &partition[0]; /* It is assumed that boot partition will be in slot 0 */
+	argv[3] = &AddrStr[0];
+	argv[4] = KERNEL_FILE;
+	argv[5] = NULL;
+
+	if(do_ext4_load(NULL,0,5,argv) == 0)
+	{
+		argv[0] = &Command[0];
+		argv[1] = &AddrStr[0];
+		argv[2] = NULL;
+		retval = do_bootm(NULL,0,2,argv);
+		console_assign(stdout,"serial");
+		printf("Failed\n");
+		return retval;
+	}
+	console_assign(stdout,"nulldev");
+	printf("Unable to locate %s\n",KERNEL_FILE);
+	UpdateFirmware();
+#else
+	printf("EMMC and EXT4 is not enabled - Cannot locate kernel file in Root\n");
+#endif
+
+	UpdateFirmware();
+	/* Module Type JFFS and JFFS2 are File System  - Not executable */
+	/* Module Type FIRMWARE is a information module- Not Executable */
+	/* Module Type FPGA has FPGA microcodes - Not Executable by CPU */
+	/* Module Type BOOT LOADER is loaded automatically */
+
+	return 0;
+}
+
