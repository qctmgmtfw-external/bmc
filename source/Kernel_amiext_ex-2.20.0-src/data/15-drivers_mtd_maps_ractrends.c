--- linux.old/drivers/mtd/maps/ractrends.c	2015-11-04 12:59:10.408336574 +0530
+++ linux/drivers/mtd/maps/ractrends.c	2015-11-04 13:01:55.648336209 +0530
@@ -0,0 +1,279 @@
+/*
+ * ractrends.c - MTD mapper for FMH Compatible images used in ractrends
+ * 			     series of boards.
+ * 			     Creates MTD Devices for each of the JFFS or JFFS2 modules
+ * 			     present in the FMH Image
+ * 			     
+ * Copyright 2003 American Megatrends Inc.
+ *
+ */
+
+
+#include <linux/module.h>
+#include <linux/types.h>
+#include <linux/kernel.h>
+#include <linux/mtd/mtd.h>
+#include <linux/mtd/map.h>
+#include <linux/mtd/concat.h>
+#include <linux/mtd/partitions.h>
+#include <asm/io.h>
+#include "fmh.h"
+#include "../mtdcore.h"
+#include <linux/init.h>
+
+#if defined (CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT)
+#define IMAGE_1  "imagebooted=1"
+#define IMAGE_2  "imagebooted=2"
+#endif
+
+static struct mtd_partition ractrends_flash_partitions[16];
+static char partition_name [16][9];
+unsigned long PartNameCnt=0;
+
+#define FULL_FLASH_PARTITION_NAME	"fullpart"
+
+unsigned long
+GetFMHPartitions(unsigned long FlashStart, uint64_t FlashSize, uint32_t SectorSize,struct mtd_info *ractrends_mtd,unsigned long *PartCount)
+{
+	unsigned long SectorCount = (unsigned long)div_u64(FlashSize,SectorSize);
+	unsigned long i,StartingSector =0,start_address=0;
+	FMH *fmh;
+	MODULE_INFO *mod;
+	unsigned short ModuleType;
+
+#if defined (CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT)
+	if(strstr(boot_command_line,IMAGE_2))
+	{
+	    StartingSector = (unsigned long)div_u64(CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE,SectorSize);//Image 2 Starting Sector
+	    start_address = CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;//valid offset for modules of image 2 filled in ractrends_flash_partitions
+	}
+	else if(strstr(boot_command_line,IMAGE_1))
+	{
+	    SectorCount = (unsigned long)div_u64(CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE,SectorSize);// Image 1 End Sector
+	}
+#endif
+
+	printk("*PartCount is %ld\n",*PartCount);
+	
+	/* Create a MTD for full Flash */
+	strncpy(partition_name[PartNameCnt],FULL_FLASH_PARTITION_NAME,8);
+	partition_name[PartNameCnt][8] = 0;
+	ractrends_flash_partitions[*PartCount].name   = partition_name[PartNameCnt];
+	ractrends_flash_partitions[*PartCount].offset = 0;
+	ractrends_flash_partitions[*PartCount].size 	 = FlashSize;
+	ractrends_flash_partitions[*PartCount].mask_flags = 0;
+	printk("MTD Partition %ld : %s @ 0x%lx of Size 0x%lx\n",*PartCount,
+						ractrends_flash_partitions[*PartCount].name,
+						(unsigned long)ractrends_flash_partitions[*PartCount].offset,
+						(unsigned long)ractrends_flash_partitions[*PartCount].size);
+	(*PartCount)++;
+	PartNameCnt++;
+	for (i=StartingSector;i<SectorCount;)
+	{
+		fmh = ScanforFMH((unsigned char *)(FlashStart+(i*SectorSize)),(unsigned long)SectorSize,ractrends_mtd);
+		if (fmh == NULL)
+		{
+			i++;
+			continue;	
+		}
+		mod = &(fmh->Module_Info);
+
+		/* Check if module type is JFFS or JFFS2 */
+		ModuleType = le16_to_host(mod->Module_Type);
+		if ((ModuleType != MODULE_JFFS) && (ModuleType != MODULE_JFFS2) && (ModuleType != MODULE_INITRD_CRAMFS) &&
+		    (ModuleType != MODULE_CRAMFS)  && (ModuleType != MODULE_SQUASHFS) && (ModuleType != MODULE_INITRD_SQUASHFS))
+		{
+			if ((i*SectorSize) > le32_to_host(mod->Module_Location))		/* AltFMH */
+				i++;
+			else
+				i+=(unsigned long)div_u64(le32_to_host(fmh->FMH_AllocatedSize),SectorSize);
+			continue;
+		}
+
+		/* Copy the Name String */
+		strncpy(partition_name[PartNameCnt],mod->Module_Name,8);
+		partition_name[PartNameCnt][8] = 0;
+
+		/* Fill the parition information */
+		ractrends_flash_partitions[*PartCount].name   = partition_name[PartNameCnt];
+
+		if ((fmh->FMH_Ver_Major == 1) && (fmh->FMH_Ver_Minor >= 8))
+			ractrends_flash_partitions[*PartCount].offset = start_address + le32_to_host(mod->Module_Location);
+		else
+			ractrends_flash_partitions[*PartCount].offset = (i*SectorSize)+ le32_to_host(mod->Module_Location);
+
+//		ractrends_flash_partitions[*PartCount].size = le32_to_host(fmh->FMH_AllocatedSize) - le32_to_host(mod->Module_Location);
+		ractrends_flash_partitions[*PartCount].size = le32_to_host(mod->Module_Size);
+
+		ractrends_flash_partitions[*PartCount].mask_flags = 0;
+		printk("MTD Partition %ld : %s @ 0x%lx of Size 0x%lx\n",*PartCount,
+						ractrends_flash_partitions[*PartCount].name,
+						(unsigned long)ractrends_flash_partitions[*PartCount].offset,
+						(unsigned long)ractrends_flash_partitions[*PartCount].size);
+
+		if ((i*SectorSize) > le32_to_host(mod->Module_Location))   /* AltFMH */
+			i++;
+		else
+			i+=(unsigned long)div_u64(le32_to_host(fmh->FMH_AllocatedSize),SectorSize);
+		(*PartCount)++;
+		PartNameCnt++;
+	}
+
+	return *PartCount;
+}
+
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_BKUP_FLASH_BANKS
+#define MAX_BANKS (CONFIG_SPX_FEATURE_GLOBAL_FLASH_BANKS + CONFIG_SPX_FEATURE_GLOBAL_BKUP_FLASH_BANKS)
+#else
+#define MAX_BANKS CONFIG_SPX_FEATURE_GLOBAL_FLASH_BANKS
+#endif
+struct map_info ractrends_flash_map[MAX_BANKS];
+static struct mtd_info *ractrends_mtd[MAX_BANKS];
+static struct mtd_info *concat_mtd = NULL;
+static unsigned long bankcount= 0;
+
+int __init 
+init_ractrends_flash(void)
+{
+	unsigned long FlashAddr = CONFIG_SPX_FEATURE_GLOBAL_FLASH_START;
+	unsigned long FlashSize = CONFIG_SPX_FEATURE_GLOBAL_FLASH_SIZE;
+	unsigned long PartitionCount;
+	unsigned long bank;
+	unsigned long PartCount=0;
+	int status = 0;
+	printk(KERN_NOTICE "Ractrends Flash mapping: 0x%08lx at 0x%08lx\n",
+					FlashSize, FlashAddr);
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_BKUP_FLASH_BANKS
+	printk(KERN_NOTICE "Flash total banks (%d) - BOOT(%d) BACKUP(%d)\n",
+		MAX_BANKS, CONFIG_SPX_FEATURE_GLOBAL_FLASH_BANKS, CONFIG_SPX_FEATURE_GLOBAL_BKUP_FLASH_BANKS);
+#else
+	printk(KERN_NOTICE "Flash total banks (%d)\n", MAX_BANKS);
+#endif
+
+//	FlashSize = (unsigned long)div_u64(FlashSize,MAX_BANKS);
+	for (bank = 0; bank < MAX_BANKS; bank++)
+		ractrends_mtd[bank] = 0;
+
+	for (bank = 0; bank < MAX_BANKS; bank++)
+	{
+		printk("Probing for Flash at Bank # %ld\n",bank);
+		ractrends_flash_map[bank].name = "Ractrends";
+		ractrends_flash_map[bank].bankwidth = 2;
+		ractrends_flash_map[bank].size = FlashSize;
+		ractrends_flash_map[bank].phys = FlashAddr + (bank * FlashSize);
+		ractrends_flash_map[bank].map_priv_1 = bank;
+		ractrends_flash_map[bank].virt = (void __iomem *)ioremap(ractrends_flash_map[bank].phys, ractrends_flash_map[bank].size);
+		if (!ractrends_flash_map[bank].virt)
+		{
+			if (bank == 0)
+			{
+				printk("ERROR: init_ractrends_flash: failed to ioremap\n");
+				return -EIO;
+			}
+			break;
+		}
+		ractrends_mtd[bank] = NULL;
+		simple_map_init(&ractrends_flash_map[bank]);	
+
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FLASH_TYPE_SPI
+		ractrends_mtd[bank] = do_map_probe("spi_probe",&ractrends_flash_map[bank]);
+#else
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FLASH_TYPE_NOR
+		ractrends_mtd[bank] = do_map_probe("cfi_probe",&ractrends_flash_map[bank]);
+#else
+#error	CONFIG_SPX_FEATURE_GLOBAL_FLASH_TYPE is not defined or defined with unknown flash type
+#endif
+#endif
+		if (!ractrends_mtd[bank]) 
+		{
+			if (bank == 0)
+			{
+				printk("ERROR: init_ractrends_flash: flash probe failed\n");
+				return -ENXIO;
+			}
+			break;
+		}
+		ractrends_mtd[bank]->owner = THIS_MODULE;
+	}
+	bankcount = bank;
+
+	if (bank > 1)	
+		concat_mtd = mtd_concat_create(ractrends_mtd,bank,"Concat Ractrends");
+	else	
+		concat_mtd = ractrends_mtd[0];
+	if (!concat_mtd){
+		printk("ERROR: init_ractrends_flash: flash concat failed\n");
+		return -ENXIO;}
+	PartitionCount = GetFMHPartitions(0,concat_mtd->size,
+				concat_mtd->erasesize,concat_mtd,&PartCount);
+
+	status = add_mtd_partitions(concat_mtd,ractrends_flash_partitions,PartitionCount);
+	if(status !=0) 
+	{
+		printk("Error in adding partitions \n");
+		return status;
+	}
+
+#if defined (CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT) && defined (CONFIG_SPX_FEATURE_COMMON_CONF_SECTION)
+	printk("Creating Partition for conf \n");
+	ractrends_flash_partitions[PartCount].name   		= "conf";
+	ractrends_flash_partitions[PartCount].offset 		= (CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE * 2);//concat_mtd->size - CONFIG_SPX_FEATURE_GLOBAL_FLASH_SIZE; //starting of SPI-3
+	ractrends_flash_partitions[PartCount].size   		= CONFIG_SPX_FEATURE_GLOBAL_CONF_SIZE;
+	ractrends_flash_partitions[PartCount].mask_flags 	= 0;
+	printk("MTD Partition %ld : %s @ 0x%lx of Size 0x%lx\n",PartCount,
+                                                ractrends_flash_partitions[PartCount].name,
+                                                (unsigned long)ractrends_flash_partitions[PartCount].offset,
+                                                (unsigned long)ractrends_flash_partitions[PartCount].size);
+	status = add_mtd_partitions(concat_mtd,&ractrends_flash_partitions[PartCount],1);
+	if(status !=0) 
+		return status;
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
+	ractrends_flash_partitions[PartCount].name   		= "conf";
+	ractrends_flash_partitions[PartCount].offset 		= (CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE * 2) + CONFIG_SPX_FEATURE_GLOBAL_CONF_SIZE;//concat_mtd->size - (CONFIG_SPX_FEATURE_GLOBAL_FLASH_SIZE- CONFIG_SPX_FEATURE_GLOBAL_CONF_SIZE); //starting of SPI-3
+	ractrends_flash_partitions[PartCount].size   		= CONFIG_SPX_FEATURE_GLOBAL_CONF_SIZE;
+	ractrends_flash_partitions[PartCount].mask_flags 	= 0;
+	printk("MTD Partition %ld : %s @ 0x%lx of Size 0x%lx\n",PartCount,
+                                                ractrends_flash_partitions[PartCount].name,
+                                                (unsigned long)ractrends_flash_partitions[PartCount].offset,
+                                                (unsigned long)ractrends_flash_partitions[PartCount].size);
+	status = add_mtd_partitions(concat_mtd,&ractrends_flash_partitions[PartCount],1);
+	if(status!=0) 
+		return status;
+#endif
+#endif
+
+	return status;
+}
+
+static void __exit 
+cleanup_ractrends_flash(void)
+{
+	unsigned long bank;
+	if (bankcount > 1)
+	{
+		del_mtd_partitions(concat_mtd);
+		map_destroy(concat_mtd);
+		concat_mtd = NULL;
+
+	}
+	for (bank = 0; bank < bankcount; bank++)
+	{
+		if (ractrends_mtd[bank])
+		{
+			del_mtd_partitions(ractrends_mtd[bank]);
+			map_destroy(ractrends_mtd[bank]);
+			ractrends_mtd[bank] = NULL;
+			iounmap((void *)ractrends_flash_map[bank].virt);
+			ractrends_flash_map[bank].virt = 0;
+		}
+	}
+
+}
+
+module_init(init_ractrends_flash);
+module_exit(cleanup_ractrends_flash);
+
+MODULE_AUTHOR("Samvinesh Christopher. American Megatrends Inc.");
+MODULE_DESCRIPTION("MTD map driver for the Ractrends Series of Boards");
