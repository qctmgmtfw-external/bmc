--- linux-3.14.17/drivers/mtd/spichips/generic.c	1970-01-01 08:00:00.000000000 +0800
+++ linux-3.14.17-ami/drivers/mtd/spichips/generic.c	2015-10-01 15:39:37.772660717 +0800
@@ -0,0 +1,1019 @@
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
+
+#ifdef __UBOOT__
+#include <common.h>
+#endif
+#include "spiflash.h"
+#ifdef	CFG_FLASH_SPI_DRIVER
+
+/* Flash opcodes. */
+#define	OPCODE_WREN		0x06	/* Write enable */
+#define	OPCODE_WRDI		0x04	/* Write disable*/
+#define	OPCODE_RDID		0x9F	/* Read JEDEC ID */
+#define	OPCODE_RDSR		0x05	/* Read status register */
+#define OPCODE_WRSR		0x01	/* Write status register */
+#define	OPCODE_READ		0x03	/* Read data bytes */
+#define	OPCODE_FAST_READ	0x0B	/* Read Fast read */
+#define	OPCODE_DREAD	0x3B	/* Dual Read Mode */
+#define	OPCODE_2READ	0xBB	/* 2 x I/O Read Mode */
+#define	OPCODE_PP		0x02	/* Page program */
+#define	OPCODE_SE		0xD8	/* Sector erase */
+#define OPCODE_DP		0xB9	/* Deep Power Down */
+#define	OPCODE_RES		0xAB	/* Read Electronic Signature */
+
+/* Status Register bits. */
+#define	SR_WIP			0x01	/* Write in progress */
+#define	SR_WEL			0x02	/* Write enable latch */
+#define	SR_BP0			0x04	/* Block protect 0 */
+#define	SR_BP1			0x08	/* Block protect 1 */
+#define	SR_BP2			0x10	/* Block protect 2 */
+#define	SR_SRWD			0x80	/* SR write protect */
+
+#define PROGRAM_PAGE_SIZE	256	/* Max Program Size */
+
+#define ADDR_16MB 		0x1000000
+#define CMD_MX25XX_EN4B		0xb7	/* Enter 4-byte address mode */
+#define CMD_MX25XX_EX4B		0xe9	/* Exit 4-byte address mode */
+
+
+#define ADDRESS_3BYTE	0x00
+#define ADDRESS_4BYTE	0x01
+#define ADDRESS_LO3_HI4_BYTE 0x02
+
+#define ADDRESS_DIE_LO3_HI4_BYTE 0x06
+#define ADDR_32MB 		0x2000000
+#define CMD_WX25XX_CS		0xc2	/* Die select */
+
+extern unsigned long ractrends_spiflash_flash_id[MAX_SPI_BANKS];
+
+static int wait_till_ready(int bank,struct spi_ctrl_driver *ctrl_drv);
+
+static
+int inline
+spi_error(int retval)
+{
+	printk("SPI Chip %s (%d) : Error (%d)\n",__FILE__,__LINE__,retval);
+	return retval;
+}
+
+static int
+spi_generic_read_flag_status(int bank, struct spi_ctrl_driver *ctrl_drv,unsigned char *status)
+{
+	int  retval;
+	u8 code = 0x70;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1);
+
+	if (retval < 0)
+		return spi_error(retval);
+
+	return 0;
+}
+
+
+
+int
+spi_generic_read_status(int bank, struct spi_ctrl_driver *ctrl_drv,unsigned char *status)
+{
+	int  retval;
+	u8 code = OPCODE_RDSR;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1);
+
+	if (retval < 0)
+		return spi_error(retval);
+
+	return 0;
+}
+
+int
+spi_generic_write_status(int bank,struct spi_ctrl_driver *ctrl_drv, unsigned char status)
+{
+	int retval;
+	u8 code = OPCODE_WRSR;
+
+	/* Send write enable */
+	spi_generic_write_enable(bank,ctrl_drv);
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_WRITE,&status, 1);
+	if (retval < 0) 
+		return spi_error(retval);
+
+	return 0;
+}
+
+
+int
+spi_generic_write_enable(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	u8 code = OPCODE_WREN;
+	int retval;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	if (retval < 0)
+		return spi_error(retval);
+	return 0;
+}
+
+int
+spi_generic_write_disable(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	u8 code = OPCODE_WRDI;
+	int retval;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	if (retval < 0)
+		return spi_error(retval);
+	return 0;
+}
+
+int spi_generic_select_die(int bank, u8 die_num, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+	u8 command[2];
+
+	command[0] = CMD_WX25XX_CS;
+	command[1] = die_num;
+
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		return -1;
+	}
+
+	retval = ctrl_drv->spi_transfer(bank, command, 2, SPI_NONE, NULL, 0);
+
+	if (retval < 0)
+	{
+		printk ("Could not select die.\n");
+		return spi_error(retval);
+	}
+	return 0;
+}
+
+int enter_4byte_addr_mode(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	//enable 32 MB Address mode
+	u8 code = CMD_MX25XX_EN4B;
+	int retval;
+
+	//printf("<ENTER> 4 BYTE\n");
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		return -1;
+	}
+
+
+	/* Issue Controller Transfer Routine */
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA))
+		spi_generic_write_enable(bank,ctrl_drv);
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA)) 
+		spi_generic_write_disable(bank,ctrl_drv);
+	if (retval < 0)
+	{
+		printk ("Could not Enter into 4-byte address mode\n");
+		return spi_error(retval);
+	}
+	return 0;
+}
+
+int exit_4byte_addr_mode(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	//Disable 32 MB Address mode
+	u8 code = CMD_MX25XX_EX4B;
+	int retval;
+
+	//printf("<EXIT> 4 BYTE\n");
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		return -1;
+	}
+
+
+	/* Issue Controller Transfer Routine */
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA)) 
+		spi_generic_write_enable(bank,ctrl_drv);
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA)) 
+		spi_generic_write_disable(bank,ctrl_drv);
+	if (retval < 0)
+	{
+		printk ("Could not Exit from 4-byte address mode\n");
+		return spi_error(retval);
+	}
+	return 0;
+}
+
+int spi_generic_extended_address(int bank, SPI_DIR dir, u8 addr, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+
+	if (dir == SPI_READ)
+	{
+		u8 code = 0xC8;
+		u8 reg_data;
+
+		ctrl_drv->spi_transfer(bank, &code, 1, SPI_READ, &reg_data, 1);
+		retval = (int) reg_data;
+	}
+	else if (dir == SPI_WRITE)
+	{
+		u8 command[2];
+
+		command[0] = 0xC5;
+		command[1] = addr;
+		spi_generic_write_enable(bank, ctrl_drv);
+		ctrl_drv->spi_transfer(bank, command, 2, SPI_NONE, NULL, 0);
+		spi_generic_write_disable(bank, ctrl_drv);
+		retval = command[1];
+	}
+	else // SPI_NONE
+	{
+		retval = 0;
+	}
+
+	return retval;
+}
+
+// the function just for WINBOND W25Q256 only, always revise the extended address to the defalut
+int w25q256_force_extended_address(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+	u8 code;
+	u8 reg_data;
+	u8 command[5];
+
+	code = 0xC8; // "Read Extended Address Register"
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_READ, &reg_data, 1);
+	if (reg_data == 0x01)
+	{
+		spi_generic_write_enable(bank,ctrl_drv);
+		command[0] = 0xC5; // "Write Extended Address Register" with the force address 0x00
+		command[1] = command[2] = command[3] = command[4] = 0x00;
+		retval = ctrl_drv->spi_transfer(bank, command, 5, SPI_NONE, NULL, 0);
+		spi_generic_write_disable(bank,ctrl_drv);
+	}
+	return 0;
+}
+
+/* Define max times to check status register before we give up. */
+#define	MAX_READY_WAIT_COUNT	4000000
+
+static int
+wait_till_ready(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	unsigned long  count;
+	unsigned char sr;
+
+	for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
+	{
+		if (spi_generic_read_status(bank,ctrl_drv,&sr) < 0)
+		{
+			printk("Error reading SPI Status Register\n");
+			break;
+		}
+		else
+		{
+			if (!(sr & SR_WIP))
+				return 0;
+		}
+	}
+
+	printk("spi_generic: Waiting for Ready Failed\n");
+	return 1;
+}
+
+static int
+require_read_flag_status(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	unsigned long count;
+	unsigned char sr;
+
+	for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
+	{
+		if (spi_generic_read_flag_status(bank,ctrl_drv,&sr) < 0)
+		{
+			printk("Error reading SPI Status Register\n");
+			break;
+		}
+		else
+		{
+			if (sr & SR_SRWD)
+				return 0;
+		}
+	}
+
+	printk("spi_generic %s() : Waiting for Ready Failed\n", __func__);
+	return 1;
+}
+
+
+
+int
+spi_generic_erase(struct map_info *map, unsigned long sect_addr)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;	
+	int retval;
+	unsigned char command[5];
+	int cmd_size;	
+	u8 address32 = priv->address32;
+	//unsigned long flash_size = priv->size;
+	u8 had_switch_die = 0;
+	
+	down(&priv->chip_drv->lock);
+	
+	
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		up(&priv->chip_drv->lock);
+		return -1;
+	}
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) {
+		if(sect_addr>=ADDR_32MB){
+			spi_generic_select_die( bank, 1,ctrl_drv);
+			had_switch_die = 1;
+			sect_addr-=ADDR_32MB;
+		}
+	}
+
+	/* Logic for 4 byte address mode Enter */
+	if ( ((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE )||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+	{
+		retval = enter_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to enter 4 byte address mode\n");
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+	if ((ractrends_spiflash_flash_id[bank]  == 0x002020BA) || (ractrends_spiflash_flash_id[bank]  == 0x002021BA) || (ractrends_spiflash_flash_id[bank]  == 0x00C21A20))
+		spi_generic_extended_address(bank, SPI_WRITE, (sect_addr & 0xFF000000) >> 24, ctrl_drv);
+
+	if (((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+	{
+		/* Set up command buffer. */
+		command[0] = OPCODE_SE;
+		if (ractrends_spiflash_flash_id[bank] == 0x00011902) command[0] = 0xDC; // ERASE command in 4byte mode [spansion only]
+		if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0xDC; // ERASE command in 4byte mode
+		command[1] = sect_addr >> 24;
+		command[2] = sect_addr >> 16;
+		command[3] = sect_addr >> 8;
+		command[4] = sect_addr;
+
+		cmd_size = 5;
+	}
+	else {
+		/* Set up command buffer. */
+		command[0] = OPCODE_SE;
+		command[1] = sect_addr >> 16;
+		command[2] = sect_addr >> 8;
+		command[3] = sect_addr;
+
+		cmd_size = 4;
+	}
+
+	/* Issue Controller Transfer Routine */
+	spi_generic_write_enable(bank,ctrl_drv); /* Send write enable */
+	retval = ctrl_drv->spi_transfer(bank,command, cmd_size ,SPI_NONE, NULL, 0);
+	spi_generic_write_disable(bank,ctrl_drv); /* Send write disable */
+
+	if (ractrends_spiflash_flash_id[bank] == 0x002020BA || ractrends_spiflash_flash_id[bank] == 0x002021BA)
+	{
+		/* requires the read flag status with at latest one byte. */
+		if (require_read_flag_status(bank,ctrl_drv))
+		{
+			up(&priv->chip_drv->lock);
+			return -1;
+		}
+	}
+
+	if (retval < 0)
+	{
+		//if 4 byte mode exit
+		if ( ((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+		{
+			retval = exit_4byte_addr_mode(bank, ctrl_drv);
+			if (retval < 0)
+			{
+				printk ("Unable to exit 4 byte address mode\n");
+			}
+		}
+
+		if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+		if ((ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA) || (ractrends_spiflash_flash_id[bank] == 0x00C21A20))
+			spi_generic_extended_address(bank, SPI_WRITE, 0x00, ctrl_drv);
+
+		if(had_switch_die == 1)
+		{
+			spi_generic_select_die( bank, 0,ctrl_drv);
+		}
+		up(&priv->chip_drv->lock);
+		return spi_error(retval);
+	}
+
+	if ( ((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+	{
+		retval = exit_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to exit 4 byte address mode\n");
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+	if ((ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA) || (ractrends_spiflash_flash_id[bank] == 0x00C21A20))
+		spi_generic_extended_address(bank, SPI_WRITE, 0x00, ctrl_drv);
+
+	if(had_switch_die == 1)
+	{
+		spi_generic_select_die( bank, 0,ctrl_drv);
+	}
+	up(&priv->chip_drv->lock);
+	return retval;
+}
+
+
+int
+spi_generic_read(struct map_info *map, loff_t addr, size_t bytes, unsigned char *buff)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;	
+	int retval = 0;
+	size_t transfer;
+	unsigned char command[6];
+	int cmd_size;
+	int  (*readfn)(int bank,unsigned char *,int , SPI_DIR, unsigned char *, unsigned long); //unsigned long);
+	int end_addr = (addr+bytes-1);	
+	u8 address32 = priv->address32;
+	//unsigned long flash_size = priv->size;
+	u8 had_switch_die = 0;
+	
+	/* Some time zero bytes length are sent */
+	if (bytes==0)
+		return 0;
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE)
+	{ 
+		if (addr < ADDR_32MB && end_addr >= ADDR_32MB)
+		{
+			int ErrorCode;
+			transfer = (ADDR_32MB - addr);
+			ErrorCode = spi_generic_read(map, addr, transfer, buff);
+			if (ErrorCode != 0) return ErrorCode;
+			 
+			 //fix address
+			bytes-=transfer;
+			addr+=transfer;
+			buff+=transfer;
+			
+			end_addr = (addr+bytes-1);
+			if (bytes==0) return 0;
+		}
+	}
+	down(&priv->chip_drv->lock);
+	
+	
+	
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		up(&priv->chip_drv->lock);
+		return -1;
+	}
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE){
+		if(addr >= ADDR_32MB){
+			spi_generic_select_die( bank, 1,ctrl_drv);
+			had_switch_die = 1;
+			addr-=ADDR_32MB;
+			end_addr = (addr+bytes-1);
+		}
+	}
+	
+	if (ctrl_drv->spi_burst_read)
+		readfn = ctrl_drv->spi_burst_read;
+	else
+		readfn = ctrl_drv->spi_transfer;
+
+	transfer=bytes;
+
+
+	/* Logic for 4 byte address mode Enter */
+	if ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE )||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+	{
+		//printk ("Trying to enter 4 byte mode\n");
+		retval = enter_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to enter 4 byte address mode\n");
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+	if ((ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA) || (ractrends_spiflash_flash_id[bank] == 0x00C21A20))
+		spi_generic_extended_address(bank, SPI_WRITE, (addr & 0xFF000000) >> 24, ctrl_drv);
+
+	while (bytes)
+	{
+		if (ctrl_drv->spi_burst_read)
+			transfer=bytes;
+		else
+		{
+			transfer=ctrl_drv->max_read;
+			if (transfer > bytes)
+				transfer = bytes;
+		}
+
+		if (!ctrl_drv->fast_read)
+		{
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{
+				/* Set up command buffer. */	/* Normal Read */
+				command[0] = OPCODE_READ;
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) command[0] = 0x13; // READ command in 4byte mode [spansion only]
+				if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0x13; // READ command in 4byte mode
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+
+				cmd_size = 5;
+			}
+			else {
+
+				/* Set up command buffer. */	/* Normal Read */
+				command[0] = OPCODE_READ;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+
+				cmd_size = 4;
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+		}
+		else if (ctrl_drv->fast_read == 1) // Need to check Fast Read in 4 byte address mode
+		{
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{ 
+				/* Set up command buffer. */   /* Fast Read */
+				command[0] = OPCODE_FAST_READ;
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) command[0] = 0x0C; // FAST_READ command in 4byte mode [spansion only]
+				if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0x0C; // FAST_READ command in 4byte mode
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				command[5] = 0;			/* dummy data */
+
+				cmd_size = 6;
+			}
+			else
+			{
+				/* Set up command buffer. */   /* Fast Read */
+				command[0] = OPCODE_FAST_READ;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				command[4] = 0;			/* dummy data */
+
+				cmd_size = 5;
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+		}
+		else if (ctrl_drv->fast_read == 2) // Need to check Dual Read in 4 byte address mode
+		{
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{ 
+				/* Set up command buffer. */   /* Dual Read */
+				command[0] = OPCODE_DREAD;
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				command[5] = 0;			/* dummy data */
+
+				cmd_size = 6;
+			}
+			else
+			{ 
+				/* Set up command buffer. */   /* Dual Read */
+				command[0] = OPCODE_DREAD;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				command[4] = 0;			/* dummy data */
+
+				cmd_size = 5;
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+		}		
+		else if (ctrl_drv->fast_read == 3) // Need to check 2xI/O Read in 4 byte address mode 
+		{
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{ 
+				/* Set up command buffer. */   /* 2xI/O Read */
+				command[0] = OPCODE_2READ;
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				command[5] = 0;			/* dummy data */
+
+				cmd_size = 6;
+			}
+			else
+			{ 
+				/* Set up command buffer. */   /* 2xI/O Read */
+				command[0] = OPCODE_2READ;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				command[4] = 0;			/* dummy data */
+
+				cmd_size = 5;
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+		}
+
+		if (retval < 0)
+		{
+			//if 4 byte mode, exit
+			if ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{
+				retval = exit_4byte_addr_mode(bank, ctrl_drv);
+				if (retval < 0)
+				{
+					printk ("Unable to exit 4 byte address mode\n");
+				}
+			}
+
+			if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+			if ((ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA) || (ractrends_spiflash_flash_id[bank] == 0x00C21A20))
+				spi_generic_extended_address(bank, SPI_WRITE, 0x00, ctrl_drv);
+
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+
+		bytes-=transfer;
+		addr+=transfer;
+		buff+=transfer;
+	}
+
+	//if 4 byte mode exit
+	if ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+	{
+		//printk ("Trying to exit 4 byte mode\n");
+		retval = exit_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to exit 4 byte address mode\n");
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+	if ((ractrends_spiflash_flash_id[bank]  == 0x002020BA) || (ractrends_spiflash_flash_id[bank]  == 0x002021BA) || (ractrends_spiflash_flash_id[bank]  == 0x00C21A20))
+		spi_generic_extended_address(bank, SPI_WRITE, 0x00, ctrl_drv);
+
+	if(had_switch_die == 1)
+	{
+		spi_generic_select_die( bank, 0,ctrl_drv);
+	}
+	up(&priv->chip_drv->lock);
+	return 0;
+}
+
+
+int
+spi_generic_write(struct map_info *map, loff_t addr, size_t bytes, const unsigned char *buff)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;
+	
+	int retval;
+	unsigned char command[5];
+	size_t transfer;
+	int cmd_size = 0;
+	int end_addr = (addr+bytes-1);
+	u8 address32 = priv->address32;
+	//unsigned long flash_size = priv->size;
+	u8 had_switch_die = 0;
+	
+	/* Some time zero bytes length are sent */
+	if (bytes==0)
+		return 0;
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE)
+	{ 
+		if (addr < ADDR_32MB && end_addr >= ADDR_32MB)
+		{
+			int ErrorCode;
+			transfer = (ADDR_32MB - addr);
+			ErrorCode = spi_generic_write(map, addr, transfer, buff);
+			if (ErrorCode != 0) return ErrorCode;
+			 
+			 //fix address
+			bytes-=transfer;
+			addr+=transfer;
+			buff+=transfer;
+			
+			end_addr = (addr+bytes-1);
+			if (bytes==0) return 0;
+		}
+	}
+
+	down(&priv->chip_drv->lock);
+	
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE){
+		if(addr >= ADDR_32MB){
+			spi_generic_select_die( bank, 1,ctrl_drv);
+			had_switch_die = 1;
+			addr-=ADDR_32MB;
+			end_addr = (addr+bytes-1);
+		}
+	}
+	
+	/* Logic for 4 byte address mode Enter */
+	if ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+	{
+		retval = enter_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to enter 4 byte address mode\n");
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+	if ((ractrends_spiflash_flash_id[bank]  == 0x002020BA) || (ractrends_spiflash_flash_id[bank]  == 0x002021BA) || (ractrends_spiflash_flash_id[bank]  == 0x00C21A20))
+		spi_generic_extended_address(bank, SPI_WRITE, (addr & 0xFF000000) >> 24, ctrl_drv);
+
+	while (bytes)
+	{
+		/* Wait until finished previous command. */
+		if (wait_till_ready(bank,ctrl_drv))
+		{
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return -1;
+		}
+
+		transfer = PROGRAM_PAGE_SIZE;
+		if (bytes <  transfer)
+			transfer = bytes;
+
+		if (!ctrl_drv->fast_write)
+		{
+			if (((end_addr >= ADDR_16MB) && (address32 == ADDRESS_LO3_HI4_BYTE)) || (address32 == ADDRESS_4BYTE))
+			{
+				/* Set up command buffer. */
+				command[0] = OPCODE_PP;
+				if (ractrends_spiflash_flash_id[bank]  == 0x00011902) command[0] = 0x12; // PROGRAM command in 4byte mode [spansion only]
+				if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0x12; // PROGRAM command in 4byte mode
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				cmd_size = 5;
+			}
+			else {
+				/* Set up command buffer. */
+				command[0] = OPCODE_PP;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				cmd_size = 4;
+			}
+		}
+
+		/* Issue Controller Transfer Routine */
+		spi_generic_write_enable(bank,ctrl_drv); /* Send write enable */
+		retval = ctrl_drv->spi_transfer(bank,command,cmd_size ,SPI_WRITE,
+						(unsigned char *)buff, transfer);
+		spi_generic_write_disable(bank,ctrl_drv); /* Send write disable */
+
+		if (ractrends_spiflash_flash_id[bank]  == 0x002020BA || ractrends_spiflash_flash_id[bank]  == 0x002021BA)
+		{
+			/* requires the read flag status with at latest one byte. */
+			if (require_read_flag_status(bank,ctrl_drv))
+			{
+				up(&priv->chip_drv->lock);
+				return -1;
+			}
+		}
+
+		if (retval < 0)
+		{
+			//if 4 byte mode exit
+			if ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{
+				retval = exit_4byte_addr_mode(bank, ctrl_drv);
+				if (retval < 0)
+				{
+					printk ("Unable to exit 4 byte address mode\n");
+				}
+			}
+
+			if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+			if ((ractrends_spiflash_flash_id[bank]  == 0x002020BA) || (ractrends_spiflash_flash_id[bank]  == 0x002021BA) || (ractrends_spiflash_flash_id[bank]  == 0x00C21A20))
+				spi_generic_extended_address(bank, SPI_WRITE, 0x00, ctrl_drv);
+
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+		addr+=(transfer-retval);
+		buff+=(transfer-retval);
+		bytes-=(transfer-retval);
+	}
+
+	//if 4 byte mode exit
+	if ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+	{
+		retval = exit_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to exit 4 byte address mode\n");
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+	if ((ractrends_spiflash_flash_id[bank]  == 0x002020BA) || (ractrends_spiflash_flash_id[bank]  == 0x002021BA) || (ractrends_spiflash_flash_id[bank]  == 0x00C21A20))
+		spi_generic_extended_address(bank, SPI_WRITE, 0x00, ctrl_drv);
+
+	if(had_switch_die == 1)
+	{
+		spi_generic_select_die( bank, 0,ctrl_drv);
+	}
+	up(&priv->chip_drv->lock);
+	return 0;
+}
+
+/***********************************************************************************/
+extern int spi_verbose;
+int
+spi_generic_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info,
+			char *spi_name,struct spi_flash_info *spi_list, int spi_list_len)
+{
+	int  retval;
+	u32 val;
+	int i;
+	u16 opread;
+	u16 opwrite;
+	u8 code = OPCODE_RDID;
+	//int address_mode = 0;
+	
+	if (spi_verbose == 2)
+		printk("SPI: probing for %s devices ...\n",spi_name);
+
+	/* Send write enable */
+	retval =spi_generic_write_enable(bank,ctrl_drv);
+	if (retval < 0)
+	 	return -1;
+	
+	/* Issue Controller Transfer Routine */
+	val = 0;
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,(unsigned char *)&val, 3);
+	val &= 0x00FFFFFF;
+
+	if (retval < 0)
+	{
+		spi_error(retval);
+		return -1;
+	}
+
+	/* Send write disable */
+	retval = spi_generic_write_disable(bank,ctrl_drv);
+	if (retval < 0)
+		return -1;
+
+	/* Match the ID against the table entries */
+	for (i = 0; i < spi_list_len; i++)
+	{
+		if ((spi_list[i].mfr_id == ((val)& 0xFF)) && (spi_list[i].dev_id == ((val >> 8)& 0xFFFF)))
+		{
+			/* Check Operation Mode */
+			//for Read Operation
+			opread = (spi_list[i].operationmode & 0xFFFF);
+			opread &= ctrl_drv->operation_mode_mask;
+			if (opread > 0x7)
+			{
+				ctrl_drv->fast_read = 3;
+			}
+			else if ((0x3 < opread) && (opread  <= 0x7))
+			{
+				ctrl_drv->fast_read = 2;
+			}
+			else if ((0x1 < opread) && (opread <= 0x3))
+			{
+				ctrl_drv->fast_read = 1;
+			}
+			else if (opread <= 0x1)
+			{
+				ctrl_drv->fast_read = 0;
+			}
+			
+			//for Write Operation
+			opwrite = (spi_list[i].operationmode >> 16);
+			opwrite &= (ctrl_drv->operation_mode_mask >> 16);
+			if (opwrite <= 0x1)
+			{
+				ctrl_drv->fast_write = 0;
+			}
+			
+		  break;
+		}
+	}
+
+	if (i == spi_list_len)
+	{
+//		if (spi_verbose == 2)
+//			printk("%s : Unrecognized ID (0x%x) got \n",spi_name,val);
+		return -1;
+	}
+	memcpy(chip_info,&spi_list[i],sizeof(struct spi_flash_info));
+
+	if (spi_verbose > 0)
+		printk(KERN_INFO"Found SPI Chip %s \n",spi_list[i].name);
+
+	return 0;
+
+}
+
+EXPORT_SYMBOL(spi_generic_probe);
+EXPORT_SYMBOL(spi_generic_erase);
+EXPORT_SYMBOL(spi_generic_read);
+EXPORT_SYMBOL(spi_generic_write);
+EXPORT_SYMBOL(spi_generic_write_disable);
+EXPORT_SYMBOL(spi_generic_write_enable);
+EXPORT_SYMBOL(spi_generic_read_status);
+EXPORT_SYMBOL(spi_generic_write_status);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Generic SPI flash chips");
+
+#endif
