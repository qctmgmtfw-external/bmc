--- u-boot-2013.07/drivers/spiflash/spiflash.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/drivers/spiflash/spiflash.c	2013-12-05 12:17:55.215503948 -0500
@@ -0,0 +1,529 @@
+/*
+ * Copyright (C) 2007-2013 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+ */
+
+#include <common.h>
+#ifdef	CONFIG_FLASH_SPI_DRIVER
+#include <asm/u-boot.h>
+#include <flash.h>
+#include <asm/sections.h>
+#include "spiflash.h"
+
+int spi_verbose = 3;
+
+flash_info_t    flash_info[CONFIG_SYS_MAX_FLASH_BANKS];
+
+/* This will be defined by the SPI Controller driver */
+extern struct spi_ctrl_driver SPI_CTRL_DRIVER; 
+extern void SPI_CTRL_DRIVER_INIT(void);
+
+/*----------------------------------------------------------------------------------*/
+/*           Low level functions which finally talk to the chip driver              */
+/*----------------------------------------------------------------------------------*/
+static void inline
+chip_unlock_sector(struct map_info *map, unsigned long sect_addr,int unlock)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->unlock_sector)
+		return;
+	(*priv->chip_drv->unlock_sector)(map,sect_addr,unlock);
+	return;
+}
+
+static int inline
+chip_is_sector_locked(struct map_info *map, unsigned long sect_addr)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->is_sector_locked)
+		return 0;
+	return (*priv->chip_drv->is_sector_locked)(map,sect_addr);
+}
+
+static int inline
+chip_read_bytes(struct map_info *map, loff_t addr, size_t bytes, unsigned char *buf)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->read_bytes)
+		return -1;
+	return (*priv->chip_drv->read_bytes)(map,addr,bytes,buf);
+}
+
+static int inline
+chip_write_bytes(struct map_info *map, loff_t addr, size_t bytes, const unsigned char *buf)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->write_bytes)
+		return -1;
+	return (*priv->chip_drv->write_bytes)(map,addr,bytes,buf);
+}
+
+static int inline
+chip_erase_sector(struct map_info *map, unsigned long sect_addr)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->erase_sector)
+		return -1;
+	return (*priv->chip_drv->erase_sector)(map,sect_addr);
+}
+
+static int
+Addr2Bank(unsigned long Addr, unsigned long *Offset)
+{
+	unsigned long start,end;
+	int bank;
+	for (bank=0; bank < CONFIG_SYS_MAX_FLASH_BANKS;bank++)
+	{
+		if (flash_info[bank].size == 0)
+		{
+			printk("ERROR: Addr2Bank Failed.No more valid banks\n"); 
+			return -1;
+		}
+
+		start = flash_info[bank].start[0];
+		end   = start + flash_info[bank].size;
+		
+		if ((Addr >= start) && (Addr < end))
+		{
+			*Offset	= Addr-start;
+			return bank;
+		}
+			 
+	}
+	printk("ERROR: Addr2Bank Failed. Bank list exhausted\n");
+	return -1;
+}
+
+/*----------------------------------------------------------------------------------*/
+/* 		Probe Function 			                        	    */
+/*----------------------------------------------------------------------------------*/
+static
+int
+probe_spi_chips(int bank,struct spi_flash_private *private,struct spi_flash_info *spi_info)
+{
+	int chip;
+	struct spi_chip_driver *chip_drv;
+	struct spi_ctrl_driver *ctrl_drv;
+	
+	memset(spi_info, 0, sizeof(struct spi_flash_info));
+
+	ctrl_drv = &SPI_CTRL_DRIVER;
+	chip = 0;
+	chip_drv = get_spi_chip_driver_by_index(chip);
+	while (chip_drv != NULL)
+	{	
+			
+		/* Probe for the chip on the controller */
+		if (chip_drv->probe(bank,ctrl_drv,spi_info) == 0)
+		{
+			private->chip_drv = chip_drv;
+			private->ctrl_drv = ctrl_drv;	
+			return 0;
+		}
+		/* Try Next chip */
+		chip++;
+		chip_drv = get_spi_chip_driver_by_index(chip);
+	}
+	return -1;
+}
+	
+static
+int
+configure_spi_clock(int bank, unsigned int clock)
+{
+	struct spi_ctrl_driver *ctrl_drv = &SPI_CTRL_DRIVER;
+
+	if (ctrl_drv->spi_configure_clock == NULL)
+		return 0;
+	return ctrl_drv->spi_configure_clock(bank, clock);
+}
+
+/*----------------------------------------------------------------------------------*/
+/* 		Flash Helper functions needed by uboot                     	    */
+/*----------------------------------------------------------------------------------*/
+static ulong	PhysicalTotalFlashSize = 0;
+
+int 
+addr_spi (unsigned long addr)
+{
+	if(addr >= CONFIG_SYS_FLASH_BASE && addr < CONFIG_SYS_FLASH_BASE + PhysicalTotalFlashSize)
+		return 1;
+	return 0;
+
+}
+
+#if defined(CONFIG_ENV_IS_IN_FLASH) || defined(CONFIG_ENV_IS_IN_SPI) || defined(CONFIG_ENV_ADDR_REDUND) || (CONFIG_SYS_MONITOR_BASE >= CONFIG_SYS_FLASH_BASE)
+static 
+flash_info_t *flash_get_info(ulong base)
+{
+	int i;
+	flash_info_t * info = 0;
+
+	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i ++) 
+	{
+		info = & flash_info[i];
+		if (info->size && info->start[0] <= base &&
+			base <= info->start[0] + info->size - 1)
+			break;
+	}
+	return (i == CONFIG_SYS_MAX_FLASH_BANKS) ? 0 : info;
+}
+#endif
+
+void 
+flash_print_info  (flash_info_t *info)
+{
+	int i;
+    	printf("  Size: %ld MB in %d Sectors\n",
+        	   info->size >> 20, info->sector_count);
+ 
+	if (info->sector_count == 0)   
+		return;
+
+	printf("  Sector Start Addresses:");
+    	for (i = 0; i < info->sector_count; i++) 
+	{
+        	if ((i % 5) == 0) 
+            		printf ("\n   ");
+        	printf (" %08lX%s", info->start[i], info->protect[i] ? " (RO)" : "     ");
+    	}
+    	printf ("\n");
+    	return;
+}
+
+
+/*----------------------------------------------------------------------------------*/
+/* 		CORE ROUTINES - Uboot Interface for SPI Flash        	    	    */
+/*----------------------------------------------------------------------------------*/
+
+static struct map_info dev_map[CONFIG_SYS_MAX_FLASH_BANKS];
+static struct spi_flash_private private[CONFIG_SYS_MAX_FLASH_BANKS];
+
+extern int atmel_init(void);
+extern int m25pxx_init(void);
+extern int spansion_init(void);
+extern int macronix_init(void);
+extern int winbond_init(void);
+extern int numonyx_init(void);
+extern int s33_init(void);
+extern int default_init(void);
+#ifdef POST_SPI_INIT
+extern void post_spi_init(int bank, struct spi_flash_info *spi_info);
+#endif
+
+/* Called by env_spi before console is opened */
+void
+spi_silent_init(void)
+{
+	spi_verbose = 0;
+	flash_init();
+	spi_verbose = 1;
+}
+
+
+unsigned char chip_drvs_are_init = 0;
+ulong 
+flash_init(void)
+{
+	struct spi_flash_info spi_info;
+	int i,j;
+	unsigned long erase_size,erase_count;
+	unsigned long start,sector;
+	ulong totalsize;
+	int bank;
+	unsigned long env_addr;
+
+	memset(&spi_info,0,sizeof(spi_info));
+	memset(&dev_map,0,sizeof(dev_map));
+	memset(&private,0,sizeof(private));
+
+	/* Initialize the SPI Driver */
+	SPI_CTRL_DRIVER_INIT();
+
+
+	/* Load the Alogrithms */
+    /* Can only do once or it corrupts internal lists, yet flash_init() gets called twice (!!) */
+    if (!chip_drvs_are_init)
+    {
+#ifdef CONFIG_DEFAULT_SPI_SIZE
+	#if CONFIG_DEFAULT_SPI_SIZE != 0
+		default_init();	/* This should be the first as the order of probing is in reverse order */	
+	#endif
+#endif	
+    	atmel_init();
+		m25pxx_init();
+		spansion_init();
+		macronix_init();
+		winbond_init();
+        numonyx_init();
+    	s33_init();
+
+        chip_drvs_are_init = 1;
+    }
+
+
+	for (bank=0; bank < CONFIG_SYS_MAX_FLASH_BANKS;bank++)
+	{
+		memset(&(flash_info[bank]), 0, sizeof(flash_info_t));
+
+		flash_info[bank].start[0] = 0;
+       		flash_info[bank].size = 0;
+		flash_info[bank].flash_id = FLASH_UNKNOWN;
+	}
+	totalsize=0;
+
+
+	for (bank=0; bank < CONFIG_SYS_MAX_FLASH_BANKS;bank++)
+	{
+		/* Probe the SPI Bus using the algorithms provided */	
+		if (probe_spi_chips(bank,&private[bank],&spi_info) == -1)
+		{
+			if (totalsize == 0)
+			{
+       			printf("ERROR: Unable to Detect SPI Flash\n");
+				return 0;	/* Size */
+			}
+			else
+				goto probe_success;
+		}
+
+		flash_info[bank].start[0] = CONFIG_SYS_FLASH_BASE+totalsize;
+	    	flash_info[bank].size = spi_info.size;
+		flash_info[bank].flash_id = ((spi_info.mfr_id << 16) | spi_info.dev_id);
+	
+		sector=0;
+		start = CONFIG_SYS_FLASH_BASE + totalsize;
+
+		for (i=0;i<spi_info.numeraseregions;i++)
+		{
+			erase_size  = spi_info.regions[i].erasesize;
+			erase_count = spi_info.regions[i].numblocks;
+			for(j=0;j<erase_count;j++)
+			{
+				flash_info[bank].start[sector] = start;
+				start += erase_size;
+				sector++;
+			}
+		}
+		flash_info[bank].sector_count = sector;
+
+		totalsize += spi_info.size;
+
+		/* Fill private info - Compatible with Linux MTD */
+		private[bank].numchips = 1;
+		private[bank].interleave = 1;
+		private[bank].device_type = DEVICE_TYPE_X8;
+		private[bank].address32 = spi_info.address32;
+		private[bank].size = spi_info.size;
+		dev_map[bank].fldrv_priv = &private[bank];
+		dev_map[bank].map_priv_1= bank;
+		configure_spi_clock(bank, spi_info.max_clock);
+#ifdef POST_SPI_INIT
+		post_spi_init(bank, &spi_info);
+#endif
+	}
+
+
+probe_success:
+  	/* Monitor protection ON by default */
+#if (CONFIG_SYS_MONITOR_BASE >= CONFIG_SYS_FLASH_BASE)
+	flash_protect (FLAG_PROTECT_SET,
+		       CONFIG_SYS_MONITOR_BASE,
+		       CONFIG_SYS_MONITOR_BASE + monitor_flash_len  - 1,
+		       flash_get_info(CONFIG_SYS_MONITOR_BASE));
+#endif
+
+	/* Environment protection ON by default */
+#if defined(CONFIG_ENV_IS_IN_FLASH) || defined(CONFIG_ENV_IS_IN_SPI)
+	env_addr = CONFIG_SYS_FLASH_BASE+
+		(((_end_ofs + CONFIG_ENV_SECT_SIZE-1)/ CONFIG_ENV_SECT_SIZE) * CONFIG_ENV_SECT_SIZE);
+	flash_protect (FLAG_PROTECT_SET,
+		       env_addr,
+		       env_addr + CONFIG_ENV_SECT_SIZE - 1,
+		       flash_get_info(env_addr));
+#endif
+
+	/* Redundant environment protection ON by default */
+#ifdef CONFIG_ENV_ADDR_REDUND
+	flash_protect (FLAG_PROTECT_SET,
+		       CONFIG_ENV_ADDR_REDUND,
+		       CONFIG_ENV_ADDR_REDUND + CONFIG_ENV_SIZE_REDUND - 1,
+		       flash_get_info(CONFIG_ENV_ADDR_REDUND));
+#endif
+
+	PhysicalTotalFlashSize = totalsize;
+	return totalsize;
+}
+
+
+
+int 
+flash_erase (flash_info_t *info, int s_first, int s_last)
+{
+	int flag, prot, sect; 
+    	int rc = ERR_OK;
+	int bank; 
+	unsigned long offset;
+
+    	if (info->flash_id == FLASH_UNKNOWN)
+       		return ERR_UNKNOWN_FLASH_TYPE;
+
+    	if ((s_first < 0) || (s_first > s_last)) 
+        	return ERR_INVAL;
+
+    	prot = 0;
+    	for (sect=s_first; sect<=s_last; ++sect) 
+	{
+        	if (info->protect[sect]) 
+		{
+	    		printf("Sector %d is Protected\n",sect);
+            		prot++;
+        	}
+    	}
+    	if (prot)
+    	{
+		printf("Protected Sectors. Erase Failed\n");
+        	return ERR_PROTECTED;
+    	}
+
+    	/* 
+     	 * Disable interrupts which might cause a timeout
+         * here. Remember that our exception vectors are
+         * at address 0 in the flash, and we don't want a
+         * (ticker) exception to happen while the flash
+         * chip is in programming mode.
+         */
+    	flag = disable_interrupts();
+		printf("\n");
+
+    	/* Start erase on unprotected sectors */
+    	for (sect = s_first; sect<=s_last && !ctrlc(); sect++) 
+	{
+        	printf("Erasing sector %2d ... ", sect);
+        	if (info->protect[sect] == 0) 
+		{ 
+			bank = Addr2Bank(info->start[sect],&offset);
+			if (bank == -1)
+				return ERR_INVAL;
+			chip_erase_sector(&dev_map[bank],offset) ;            
+        	}
+		else
+		{
+			printf("Protected!!\n");
+		}
+       		printf("ok.\n");
+   	 }
+    	if (ctrlc())
+      		printf("User Interrupt!\n");
+
+    	/* allow flash to settle - wait 10 ms */
+    	udelay_masked(10000);
+   
+    	if (flag)
+      		enable_interrupts();
+   
+    	return rc;
+}
+
+int 
+read_buff(flash_info_t *info, uchar *dest, ulong addr, ulong cnt)
+{
+	int bank; 
+	unsigned long offset;
+	ulong len = 0;
+	int rc=ERR_OK;
+
+	while (cnt)
+	{	
+		/* Convert Addr to bank:offset */
+		bank = Addr2Bank(addr,&offset);
+		if (bank == -1)
+			return ERR_INVAL;
+
+		if (flash_info[bank].size > 0x02000000)
+		{
+			if ((offset < 0x02000000) && ((offset + len) > 0x02000000))
+			{
+				len = (0x02000000 - offset);
+			}
+			else
+			{
+				len = cnt;
+			}
+		}
+		else
+		{
+			/* Get the remaiing length in the bank */
+			len = flash_info[bank].size - offset;
+
+			/* Get the transfer length for the bank */
+			len = (cnt > len)? len:cnt;
+		}
+
+		/* Issue command */
+		rc=ERR_INVAL;
+		rc= chip_read_bytes(&dev_map[bank],offset,len ,dest);
+		if (rc != 0)
+			return ERR_INVAL;
+
+		/* Adjust values for next bank */
+		cnt-=len;
+		addr+=len;
+		dest+=len;
+	}
+	return rc;
+}
+
+int 
+write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
+{
+	int bank; 
+	unsigned long offset;
+	ulong len;
+	int rc=ERR_OK;
+
+	while (cnt)
+	{	
+		/* Convert Addr to bank:offset */
+		bank = Addr2Bank(addr,&offset);
+		if (bank == -1)
+			return ERR_INVAL;
+
+		/* Get the remaiing length in the bank */
+		len = flash_info[bank].size - offset;
+
+		/* Get the transfer length for the bank */
+		len = (cnt > len)? len:cnt;
+
+		/* Issue command */
+		rc= chip_write_bytes(&dev_map[bank],offset,len,src);
+		if (rc != 0)
+			return ERR_INVAL;
+
+		/* Adjust values for next bank */
+		cnt-=len;
+		addr+=len;
+	}
+	return rc;
+}
+
+
+#endif
