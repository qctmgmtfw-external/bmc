/*****************************************************************
 *****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 *****************************************************************
 *****************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#include "driver_hal.h"
#include "host_spi_flash.h"
#include "../drivers/mtd/mtdcore.h"
#include "helper.h"  // Quanta
#include <linux/proc_fs.h> // Quanta

/* What all erase sizes are supported by this SPI */
#define FL_ERASE_4K     0x00000001  /* Supports 4k erase */
#define FL_ERASE_32K    0x00000002  /* Supports 32k erase */
#define FL_ERASE_64K    0x00000004  /* Supports 64k erase */
#define FL_ERASE_ALL    (FL_ERASE_4K | FL_ERASE_32K | FL_ERASE_64K)

#ifdef HAVE_UNLOCKED_IOCTL
  #if HAVE_UNLOCKED_IOCTL
        #define USE_UNLOCKED_IOCTL
  #endif
#endif

/* commands of SPI flash */
#define SPI_FLASH_CMD_WRITE_ENABLE          0x06
#define SPI_FLASH_CMD_WRITE_DISABLE         0x04
#define SPI_FLASH_CMD_READ_STATUS_REG       0x05
#define SPI_FLASH_CMD_WRITE_STATUS_REG      0x01
#define SPI_FLASH_CMD_READ_ID               0x9F
#define SPI_FLASH_CMD_READ_SIGN             0xAB /* 0x90 for some flash chips */
#define SPI_FLASH_CMD_READ_DATA             0x03
#define SPI_FLASH_CMD_READ_DATA_FAST        0x0B
#define SPI_FLASH_CMD_READ_DATA_FAST_4B_MODE	0x0C	/* Fast Read 4-byte address mode */
#define SPI_FLASH_CMD_SECTOR_ERASE          0x20
#define SPI_FLASH_CMD_SECTOR_ERASE_4B_MODE  0x21
#define SPI_FLASH_CMD_BLK_ERASE_32K         0x52
#define SPI_FLASH_CMD_BLK_ERASE_32K_4B_MODE 0x5C
#define SPI_FLASH_CMD_BLK_ERASE_64K         0xD8
#define SPI_FLASH_CMD_BLK_ERASE_64K_4B_MODE 0xDC
#define SPI_FLASH_CMD_SECTOR_ERASE_ATMEL    0x39
#define SPI_FLASH_CMD_CHIP_ERASE            0xC7
#define SPI_FLASH_CMD_PAGE_PROGRAM          0x02
#define SPI_FLASH_CMD_PAGE_PROGRAM_4B_MODE  0x12
#define CMD_MX25XX_EN4B                     0xB7    /* Enter 4-byte address mode */
#define CMD_MX25XX_EX4B                     0xE9    /* Exit 4-byte address mode */

/* bits of SPI flash status register */
#define SPI_FLASH_SR_WIP                    0x01
#define SPI_FLASH_SR_WEL                    0x02
#define SPI_FLASH_SR_SRWD                   0x07

#define SPI_FLASH_PAGE_SZ                   256

/* Define max times to check status register before we give up. */
#define MAX_READY_WAIT_COUNT                0x3000000
#define HOST_SPI_FLASH_MAJOR_NUM            153
#define HOST_SPI_FLASH_MINOR_NUM            0
#define HOST_SPI_FLASH_MAX_DEVICES          1
static dev_t host_spi_flash_devno = MKDEV(HOST_SPI_FLASH_MAJOR_NUM, HOST_SPI_FLASH_MINOR_NUM);
static struct cdev *host_spi_flash_cdev;

#ifdef USE_UNLOCKED_IOCTL
static long host_spi_flash_module_ioctlUnlocked(struct file *file, uint cmd, ulong arg);
#else
static int host_spi_flash_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg);
#endif
static int host_spi_flash_open(struct inode *inode, struct file *file);
static int host_spi_flash_release(struct inode *inode, struct file *file);

#define HOST_SPI_FLASH_DRIVER_NAME          "host_spi_flash"
#define ADDR_16MB                           0x1000000

#define BYTE_3_ADDR_MODE                    0
#define BYTE_4_ADDR_MODE                    1

struct host_spi_flash_info_t *g_spi_info = NULL;
struct semaphore host_spi_flash_lock;
struct host_spi_flash_core_ops_t host_spi_flash_core_ops;
//Quanta---
static int spi_ctrl_4b_mode = 0;
#define SUPPORT     0x01
#define NOT_SUPPORT 0x00
//---Quanta
struct host_spi_flash_info_t
{
    char        *name;
    uint32_t    id;
    unsigned    sector_size;
    unsigned    n_sectors;
    uint32_t    supported_blk_size;
	unsigned char supported_4b_mode_cmd;
};
static struct file_operations host_spi_flash_ops = {
        owner:      THIS_MODULE,
        read:       NULL,
        write:      NULL,
#ifdef USE_UNLOCKED_IOCTL
        unlocked_ioctl: host_spi_flash_module_ioctlUnlocked,
#else
        ioctl:      host_spi_flash_module_ioctl,
#endif
        open:       host_spi_flash_open,
        release:    host_spi_flash_release,
};
static struct host_spi_flash_info_t host_spi_flash_info_tab[] =
{
    { "S25FL64P",       0x172001, 64 * 1024,    128,       FL_ERASE_ALL, NOT_SUPPORT },      // Spansion 8MB
    { "M25P64",         0x172020, 64 * 1024,    128,       FL_ERASE_ALL, NOT_SUPPORT },      // Macronix 8MB
    { "MX25L644SE",     0x1720C2, 64 * 1024,    128,       FL_ERASE_ALL, NOT_SUPPORT },      // Macronix 8MB
    { "S25FL128P",      0x182001, 64 * 1024,    256,       FL_ERASE_ALL, NOT_SUPPORT },      // Spansion 16MB
    { "M25P128",        0x182020, 256 * 1024,   64,        FL_ERASE_ALL, NOT_SUPPORT },      // Macronix 16MB
    { "MX25L12805D",    0x1820C2, 64 * 1024,    256,       FL_ERASE_ALL, NOT_SUPPORT },      // Macronix 16MB
    { "MX25L2005",      0x1220C2, 64 * 1024,    4,         FL_ERASE_ALL, NOT_SUPPORT },      // Macronix 256kB
    { "N25Q064",        0x17BA20, 64 * 1024,    128,       FL_ERASE_ALL, NOT_SUPPORT },      // Numonyx 8MB
    { "W25Q64BV",       0x1740EF, 64 * 1024,    128,       FL_ERASE_ALL, NOT_SUPPORT },      // Winbond 8MB
    { "W25Q128BV",      0x1840EF, 64 * 1024,  	256,       FL_ERASE_ALL, NOT_SUPPORT },      // Winbond 16MB Quanta
    { "AT25DF64",       0x00481F, 64 * 1024,    128,       FL_ERASE_ALL, NOT_SUPPORT },      // Atmel 8MB
    { "W25Q128FV",      0x1840EF, 64 * 1024,    256,       FL_ERASE_ALL, NOT_SUPPORT },      // Winbond 16MB
	{ "W25Q256FV",      0x1940EF, 64 * 1024,    512,       FL_ERASE_ALL, NOT_SUPPORT },      // Winbond 32MB Quanta
    { "MX25L25635F",    0x1920C2, 64 * 1024,    512,       FL_ERASE_ALL, SUPPORT },      // Macronix 32MB
    { "MX25L51245G",    0x1A20C2, 64 * 1024,    1024,      FL_ERASE_ALL, SUPPORT },      // Macronix 64MB
    { "MT25QL512AB",    0x20BA20, 64 * 1024,    1024,      FL_ERASE_ALL, SUPPORT },      // Micron 64MB
	{ "N25Q256A13ESF40G", 0x19BA20, 64 * 1024,   512,      FL_ERASE_ALL, NOT_SUPPORT }       // Micron 32MB
};

// Quanta ---
static struct host_spi_flash_hal_ops_t *host_spi_hw_ops = NULL;

static struct proc_dir_entry   *host_spi_proc_hostspi = NULL;
static struct ctl_table_header *host_spi_sys_hostspi  = NULL;

static uint32_t ReadID = 0;
static uint32_t ReadSR = 0;

static int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
host_spi_proc_read_id(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos);
#else
host_spi_proc_read_id(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos);
#endif


static int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
host_spi_proc_read_sr(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos);
#else
host_spi_proc_read_sr(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos);
#endif

static struct ctl_table host_spi_proc_tab[] = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
    {"ReadID", &ReadID,  sizeof(int), 0444, NULL, &host_spi_proc_read_id, NULL },
    {"ReadSR",  &ReadSR, sizeof(int), 0444, NULL, &host_spi_proc_read_sr, NULL },
	{ 0 }
#else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0))
	{"ReadID", &ReadID,       sizeof(int), 0444, NULL, NULL, &host_spi_proc_read_id },
	{"ReadSR",  &ReadSR, sizeof(int), 0444, NULL, NULL, &host_spi_proc_read_sr },
	{ 0 }
#else	
	{ CTL_UNNUMBERED, "ReadID", &ReadID, sizeof(uint32_t), 0444, NULL, NULL, &host_spi_proc_read_id },
	{ CTL_UNNUMBERED, "ReadSR", &ReadSR, sizeof(uint32_t), 0444, NULL, NULL, &host_spi_proc_read_sr },
	{ 0 }
#endif	
#endif	
};
// --- Quanta


static uint32_t current_flash_id = 0xFFFFFFFF;
static unsigned char supported_4b_mode_cmd = 0;//Quanta
static uint32_t read_id(struct host_spi_flash_hal_ops_t *hw_ops)
{
    unsigned char cmd;
    uint8_t ids[4];
    uint32_t id;

    cmd = SPI_FLASH_CMD_READ_ID;
    hw_ops->transfer(&cmd, 1, ids, 3, 0);
    id = ids[0] | (ids[1] << 8) | (ids[2] << 16);

    return id;
}

/*
 * wait status bit to be cleared when "wait_set" is 0
 * wait status bit to be set when "wait_set" is 1
 */
static int wait_for_wip_status(struct host_spi_flash_hal_ops_t *hw_ops)
{
    unsigned char cmd;
    uint8_t status = 0;
    int count = 0;

    cmd = SPI_FLASH_CMD_READ_STATUS_REG;

    for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
    {
        if (hw_ops->transfer(&cmd, 1, &status, 1, 0) < 0)
        {
            printk ("Error reading the SPI Status Register\n");
            break;
        }
        else
        {
            if (!(status & SPI_FLASH_SR_WIP))
            {
                return 0;
            }
        }
    }

	printk("spi_generic: Waiting for Ready Failed %x\n", status);
	return 1;
}

static int is_wel_enabled(struct host_spi_flash_hal_ops_t *hw_ops)
{
    unsigned char cmd;
    uint8_t status = 0;
    int count = 0;

    cmd = SPI_FLASH_CMD_READ_STATUS_REG;

    for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
    {
        if (hw_ops->transfer(&cmd, 1, &status, 1, 0) < 0)
        {
            printk ("Error reading the SPI Status Register\n");
            break;
        }
        else
        {
            if (status & SPI_FLASH_SR_WEL)
            {
                return 0;
            }
        }
    }

	printk("ERROR: Write is not Enabled in the SPI Part...\n");
	return 1;
}


static void enable_write(struct host_spi_flash_hal_ops_t *hw_ops)
{
    unsigned char cmd;

    cmd = SPI_FLASH_CMD_WRITE_ENABLE;
    hw_ops->transfer(&cmd, 1, NULL, 0, 0);

    /* check write enable latch is set */
    if (is_wel_enabled(hw_ops))
    {
        return;
    }
}

int enter_4byte_addr_mode(struct host_spi_flash_hal_ops_t *hw_ops)
{
    //enable 32 MB Address mode
    u8 code = CMD_MX25XX_EN4B;
    int retval;

    /* wait complete by check write in progress bit is clean */
    if (wait_for_wip_status(hw_ops))
    {
        return -1;
    }

    /* Issue Controller Transfer Routine */
    retval = hw_ops->transfer(&code, 1, NULL, 0, 0);
    if (retval < 0)
    {
        printk ("Could not Enter into 4-byte address mode\n");
        return -1;
    }

    hw_ops->set_addr_mode(BYTE_4_ADDR_MODE);

    return 0;
}

int exit_4byte_addr_mode(struct host_spi_flash_hal_ops_t *hw_ops)
{
    //Disable 32 MB Address mode
    u8 code = CMD_MX25XX_EX4B;
    int retval;

    if (wait_for_wip_status(hw_ops))
    {
        return -1;
    }

    /* Issue Controller Transfer Routine */
    retval = hw_ops->transfer(&code, 1, NULL, 0, 0);
    if (retval < 0)
    {
        printk ("Could not Exit from 4-byte address mode\n");
        return -1;
    }

    hw_ops->set_addr_mode(BYTE_3_ADDR_MODE);

    return 0;
}

static int get_erase_cmd(uint32_t erasesize, uint32_t *cmd)
{
    switch (erasesize)
    {
        case 0x1000:
        {
            if (spi_ctrl_4b_mode)
                *cmd = SPI_FLASH_CMD_SECTOR_ERASE_4B_MODE;
            else
                *cmd = SPI_FLASH_CMD_SECTOR_ERASE;
        }
        break;

        case 0x8000:
        {
            if (spi_ctrl_4b_mode)
                *cmd = SPI_FLASH_CMD_BLK_ERASE_32K_4B_MODE;
            else
                *cmd = SPI_FLASH_CMD_BLK_ERASE_32K;
        }
        break;

        case 0x10000:
        {
            if (spi_ctrl_4b_mode)
			{
				// "N25Q256A13ESF40G" does not support 4 byte sector erase command.
				if (supported_4b_mode_cmd == NOT_SUPPORT)
					*cmd = SPI_FLASH_CMD_BLK_ERASE_64K;
				else
					*cmd = SPI_FLASH_CMD_BLK_ERASE_64K_4B_MODE;
			}
            else
                *cmd = SPI_FLASH_CMD_BLK_ERASE_64K; 
        }
        break;

        default:
        {
            printk ("Invalid Erase Block Size\n");
            return -1;
        }
    }

    /* ATMEL "AT25DF64" - the default status is protected all sectors, so unprotect the current sector before erase
       Overriding the command field with the command specific for Atmel */
    if (current_flash_id == 0x0000481F)
    {
        *cmd = SPI_FLASH_CMD_SECTOR_ERASE_ATMEL;
    }

    return 0;
}

/*
 * Erase one sector at offset "offset" which is any address within the sector which should be erased.
 */
static int erase_sector(struct host_spi_flash_hal_ops_t *hw_ops, uint32_t offset, uint32_t eraseblocksize)
{
    unsigned char cmd[5] = {0};
    int cmd_size = 4;
    uint32_t command = 0;

    if (get_erase_cmd(eraseblocksize, &command) < 0)
    {
        return -1;
    }   
 
    /* wait complete by check write in progress bit is clean */
    if (wait_for_wip_status(hw_ops))
    {
        return -1;
    }

    // For 4B Addressing mode, the command size would be incremented by one to accomodate 4 byte address
    // Also command is different for 4B mode
    // If 3B addressing mode, then we would have to rever to generic 3B addressing SPI command
    if (spi_ctrl_4b_mode)
    {
        cmd_size++;
        cmd[0] = command;
        cmd[1] = (unsigned char) (offset >> 24);
        cmd[2] = (unsigned char) (offset >> 16);
        cmd[3] = (unsigned char) (offset >> 8);
        cmd[4] = (unsigned char) offset;
    }
    else
    {
        cmd[0] = command;
        cmd[1] = (unsigned char) (offset >> 16);
        cmd[2] = (unsigned char) (offset >> 8);
        cmd[3] = (unsigned char) offset;
    }

    enable_write(hw_ops);
    hw_ops->transfer(cmd, cmd_size, NULL, 0, 0);

    /* wait complete by check write in progress bit is clean */
    if (wait_for_wip_status(hw_ops))
    {
        return -1;
    }

    return 0;
}

static void write_buffer(struct host_spi_flash_hal_ops_t *hw_ops, const uint8_t *src, uint32_t offset, int len)
{
    unsigned char cmd[5] = {0};
    int cmd_size = 4;
    int index = 0;

    /* wait complete by check write in progress bit is clean */
    if (wait_for_wip_status(hw_ops))
    {
        return;
    }

    // For 4B Addressing mode, the command size would be incremented by one to accomodate 4 byte address
    // Also command is different for 4B mode
    // If 3B addressing mode, then we would have to rever to generic 3B addressing SPI command
    if (spi_ctrl_4b_mode)
    {
        cmd_size++;
		// "N25Q256A13ESF40G" does not support 4 byte page program command.
		if (supported_4b_mode_cmd == NOT_SUPPORT)
			cmd[index++] = SPI_FLASH_CMD_PAGE_PROGRAM;
		else
			cmd[index++] = SPI_FLASH_CMD_PAGE_PROGRAM_4B_MODE;
        cmd[index++] = 0;
    }
    else
    {
        cmd[index++] = SPI_FLASH_CMD_PAGE_PROGRAM;
    }

    if ((offset >= ADDR_16MB) || (spi_ctrl_4b_mode))
    {
        cmd[1] = (unsigned char) (offset >> 24);
        cmd[2] = (unsigned char) (offset >> 16);
        cmd[3] = (unsigned char) (offset >> 8);
        cmd[4] = (unsigned char) offset;
    }
    else
    {
        cmd[index++] = (unsigned char) (offset >> 16);
        cmd[index++] = (unsigned char) (offset >> 8);
        cmd[index] = (unsigned char) offset;
    }

    enable_write(hw_ops);
    hw_ops->transfer(cmd, cmd_size, (unsigned char *) src, len, 1);

    /* wait complete by check write in progress bit is clean */
    if (wait_for_wip_status(hw_ops))
    {
        return;
    }
}


static int find_best_erase_params(struct mtd_info *mtd, struct erase_info *instr, uint32_t *erasesize)
{
	/* Smaller than 32k, use 4k */
    if (instr->len < 0x8000)
    {
        *erasesize = 0x1000;
        return 0;
    }

	/* Smaller than 64k and 32k is supported, use it */
    if ((g_spi_info->supported_blk_size & FL_ERASE_32K) && (instr->len < 0x10000))
    {
        *erasesize = 0x8000;
        return 0;
    }

	/* If 64K is not supported, use whatever smaller size is */
    if (!(g_spi_info->supported_blk_size & FL_ERASE_64K))
    {
        if (g_spi_info->supported_blk_size & FL_ERASE_32K)
        {
            *erasesize = 0x8000;
        }
        else
        {
            *erasesize = 0x1000;
        }

        return 0;
    }

	/* Allright, let's go for 64K */
    *erasesize = 0x10000;

    return 0;
}

static int host_spi_flash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
    struct host_spi_flash_hal_ops_t *hw_ops;
    uint32_t addr, len;
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
	uint64_t instr_addr = 0, instr_len = 0;
#endif
    uint32_t eraseblocksize = 0;

    /* sanity checks */
    if (instr->addr + instr->len > mtd->size)
	{
		return -EINVAL;
	}

    find_best_erase_params(mtd, instr, &eraseblocksize);

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
	/* Code to solve : WARNING: "__umoddi3". Need to check for alternative solution. */
	instr_addr = instr->addr;
	instr_len = instr->len;
	if (do_div(instr_addr, eraseblocksize) != 0 || do_div(instr_len, eraseblocksize) != 0)
		return -EINVAL;
#else
    if ((instr->addr % eraseblocksize) != 0 || (instr->len % eraseblocksize) != 0)
        return -EINVAL;
#endif

    hw_ops = mtd->priv;
    addr = instr->addr;
    len = instr->len;

    down(&host_spi_flash_lock);

    while (len)
    {
        if (erase_sector(hw_ops, addr, eraseblocksize))
        {
            instr->state = MTD_ERASE_FAILED;
            up(&host_spi_flash_lock);
            return -EIO;
        }

        addr += eraseblocksize;
        len -= eraseblocksize;
    }

    up(&host_spi_flash_lock);

    instr->state = MTD_ERASE_DONE;
    mtd_erase_callback(instr);

    return 0;
}

static int host_spi_flash_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf)
{
    struct host_spi_flash_hal_ops_t *hw_ops;
    unsigned char cmd[6] = {0};
    size_t transfer_len;
    int cmd_size = 5;
    int index = 0;

    if (len == 0)
        return 0;

    if (from + len > mtd->size)
        return -EINVAL;

    hw_ops = mtd->priv;

    /* Byte count starts at zero. */
    if (retlen != 0)
        *retlen = 0;

    // For 4B Addressing mode, the command size would be incremented by one to accomodate 4 byte address
    // Also command is different for 4B mode
    // If 3B addressing mode, then we would have to rever to generic 3B addressing SPI command
    if (spi_ctrl_4b_mode)
    {
        cmd_size = 6;
    }

    down(&host_spi_flash_lock);

    while (len > 0)
    {
        /* wait complete by check write in progress bit is clean */
        if (wait_for_wip_status(hw_ops))
        {
            return -EINVAL;
        }

		// For "N25Q256A13ESF40G", it is better to use 4 byte fast read command.
		if (current_flash_id == 0x19BA20)		
			cmd[index++] = SPI_FLASH_CMD_READ_DATA_FAST_4B_MODE;
		else
			cmd[index++] = SPI_FLASH_CMD_READ_DATA_FAST;

        if (hw_ops->burst_read != NULL)   /* controller supports burst read */
        {
            transfer_len = len;
        }
        else
        {
            transfer_len = hw_ops->max_read();
            if (transfer_len > len)
                transfer_len = len;
        }

        if (spi_ctrl_4b_mode)
        {
            cmd[index++] = 0;

            if (from >= ADDR_16MB)
            {
                cmd[1] = (unsigned char) (from >> 24);
                cmd[2] = (unsigned char) (from >> 16);
                cmd[3] = (unsigned char) (from >> 8);
                cmd[4] = (unsigned char) (from);
                cmd[5] = 0; // dummy
            }
            else
            {
                cmd[index++] = (unsigned char) (from >> 16);
                cmd[index++] = (unsigned char) (from >> 8);
                cmd[index++] = (unsigned char) from;
                cmd[index] = 0; // dummy
            }
        }
        else
        {
            cmd[index++] = (unsigned char) (from >> 16);
            cmd[index++] = (unsigned char) (from >> 8);
            cmd[index++] = (unsigned char) from;
            cmd[index] = 0; // dummy
        }

        hw_ops->transfer(cmd, cmd_size, buf, transfer_len, 0);

        buf += transfer_len;
        from += transfer_len;
        len -= transfer_len;
        (*retlen) += transfer_len;
        index = 0;
    }

    up(&host_spi_flash_lock);

    return 0;
}

static int host_spi_flash_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf)
{
    struct host_spi_flash_hal_ops_t *hw_ops;
    uint32_t page_offset, page_size;
    uint32_t i;

    if (!len)
        return 0;

    if (to + len > mtd->size)
        return -EINVAL;

    hw_ops = mtd->priv;

    if (retlen)
        *retlen = 0;

    down(&host_spi_flash_lock);

    page_offset = to % SPI_FLASH_PAGE_SZ; /* judge which page do we start with */

    if (page_offset + len <= SPI_FLASH_PAGE_SZ)   /* all the bytes fit onto one page */
    {
        write_buffer(hw_ops, buf, to, len);
        *retlen += len;
    }
    else     /* multi pages */
    {
        /* the size of data remaining on the first page */
        page_size = SPI_FLASH_PAGE_SZ - page_offset;
        write_buffer(hw_ops, buf, to, page_size);
        *retlen += page_size;

        /* write everything in PAGESIZE chunks */
        for (i = page_size; i < len; i += page_size)
        {
            page_size = len - i;
            if (page_size > SPI_FLASH_PAGE_SZ)
                page_size = SPI_FLASH_PAGE_SZ;

            /* write the next page to flash */
            write_buffer(hw_ops, buf + i, to + i, page_size);
            *retlen += page_size;
        }
    }

    up(&host_spi_flash_lock);

    return 0;
}

static struct mtd_info *host_spi_flash_probe(struct host_spi_flash_hal_ops_t *hw_ops)
{
    struct host_spi_flash_info_t *info;
    struct mtd_info *new_mtd;
    uint32_t id;
    int i;

    id = read_id(hw_ops);
    for (i = 0, info = host_spi_flash_info_tab; i < ARRAY_SIZE(host_spi_flash_info_tab); i ++, info ++)
    {
        if (id == info->id)
        {
            break;
        }
    }
    if (i == ARRAY_SIZE(host_spi_flash_info_tab))   /* no matched id */
    {
        printk(KERN_WARNING "%s: not supported id %06x\n", HOST_SPI_FLASH_DRIVER_NAME, id);
        return NULL;
    }

    new_mtd = kzalloc(sizeof(struct mtd_info), GFP_KERNEL);
    if (!new_mtd)
        return NULL;

    new_mtd->name = info->name;
    new_mtd->type = MTD_NORFLASH;
    new_mtd->flags = MTD_CAP_NORFLASH;
    new_mtd->size = info->sector_size * info->n_sectors;
    new_mtd->erasesize = info->sector_size;
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
	new_mtd->_erase = host_spi_flash_erase;
	new_mtd->_read = host_spi_flash_read;
	new_mtd->_write = host_spi_flash_write;
#else
    new_mtd->erase = host_spi_flash_erase;
    new_mtd->read = host_spi_flash_read;
    new_mtd->write = host_spi_flash_write;
#endif
    new_mtd->writesize = 1;
    current_flash_id = id;
    g_spi_info = info;
    supported_4b_mode_cmd = info->supported_4b_mode_cmd;//Quanta

    return new_mtd;
}

int host_spi_flash_register(unsigned char num, void *hw_ops, void **hw_data)
{
    struct host_spi_flash_hal_t *hw_hal;

    if (hw_ops == NULL)
    {
        return -ENOMEM;
    }

    hw_hal = (struct host_spi_flash_hal_t *) kmalloc(sizeof(struct host_spi_flash_hal_t), GFP_KERNEL);
    if (hw_hal == NULL)
    {
        return -ENOMEM;
    }

    hw_hal->ops = (struct host_spi_flash_hal_ops_t *) hw_ops;
    hw_hal->mtd = host_spi_flash_probe(hw_hal->ops);
    if (hw_hal->mtd == NULL)
    {
        kfree(hw_hal);
        return -ENODEV;
    }

    hw_hal->mtd->owner = THIS_MODULE;
    hw_hal->mtd->priv = hw_ops;
	host_spi_hw_ops = hw_ops;  // Quanta

    hw_hal->partitions.name = CONFIG_SPX_FEATURE_HOST_SPI_FLASH_MTD_NAME;
    hw_hal->partitions.offset = 0;
    hw_hal->partitions.size = hw_hal->mtd->size;
    hw_hal->partitions.mask_flags = 0;
#if !(LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
    hw_hal->partitions.mtdp = NULL;
#endif

    add_mtd_partitions(hw_hal->mtd, &hw_hal->partitions, 1);

    *hw_data = (void *) hw_hal;

    hw_hal->ops->configure_clock(hw_hal->ops->max_clock);

    // If the SPI Size is more than 16MB, then setting the SPI and Controller to 4B mode
    // Also set a flag to indicate to other read/write/erase functions to identify the mode
    if (hw_hal->partitions.size > ADDR_16MB)
    {
        //spi_ctrl_4b_mode = 1;
        //enter_4byte_addr_mode(hw_ops);
#ifdef CONFIG_SPX_FEATURE_QUANTA_AMD_SUPPORT
		spi_ctrl_4b_mode = 0;
		exit_4byte_addr_mode(hw_ops);
#else
		spi_ctrl_4b_mode = 1;
		enter_4byte_addr_mode(hw_ops);
#endif
    }

    return 0;
}

int host_spi_flash_unregister(void *hw_data)
{
    struct host_spi_flash_hal_t *hw_hal = (struct host_spi_flash_hal_t *) hw_data;

    if (hw_hal->mtd != NULL)
    {
        del_mtd_partitions(hw_hal->mtd);
        kfree(hw_hal->mtd);
    }

    kfree (hw_hal);

    return 0;
}

// Quanta ---
static int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
host_spi_proc_read_id(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
host_spi_proc_read_id(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	if (host_spi_hw_ops) {
		ReadID = read_id(host_spi_hw_ops);
	}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
    proc_doulongvec_minmax(ctl, write, buffer, lenp, pos);
#else
	proc_doulongvec_minmax(ctl, write, filep, buffer, lenp, pos);
#endif
	return 0;
}

static int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
host_spi_proc_read_sr(ctl_table *ctl, int write, void *buffer, size_t *lenp, loff_t *pos)
#else
host_spi_proc_read_sr(ctl_table *ctl, int write, struct file *filep, void *buffer, size_t *lenp, loff_t *pos)
#endif
{
	uint8_t  cmd, status;

	if (host_spi_hw_ops) {
		cmd = SPI_FLASH_CMD_READ_STATUS_REG;
		host_spi_hw_ops->transfer(&cmd, 1, &status, 1, 0);
		ReadSR = status;
		if ((ReadID & 0xFFFF) == 0x40EF) {
			cmd = 0x35;  // status register 2
			host_spi_hw_ops->transfer(&cmd, 1, &status, 1, 0);
			ReadSR |= (status << 8);
		}
	}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
    proc_doulongvec_minmax(ctl, write, buffer, lenp, pos);
#else
	proc_doulongvec_minmax(ctl, write, filep, buffer, lenp, pos);
#endif

	return 0;
}

static int host_spi_create_proc(void)
{
	if ((host_spi_proc_hostspi = proc_mkdir("hostspi", rootdir)) == NULL) {
		printk("%s: proc_mkdir() hostspi failed\n", HOST_SPI_FLASH_DRIVER_NAME);
		return -1;
	}

	if ((host_spi_sys_hostspi = AddSysctlTable("hostspi", &host_spi_proc_tab[0])) == NULL) {
		printk("%s: Unable to create /proc/sys/ractrends/hostspi\n", HOST_SPI_FLASH_DRIVER_NAME);
		return -1;
	}

	return 0;
}

static int host_spi_remove_proc(void)
{
	if (host_spi_sys_hostspi)
		RemoveSysctlTable(host_spi_sys_hostspi);

	if (host_spi_proc_hostspi)
		remove_proc_entry("hostspi", rootdir);

	return 0;
}
// --- Quanta

static core_hal_t host_spi_flash_core_hal =
{
    .owner = THIS_MODULE,
    .name = "Host SPI flash core",
    .dev_type = EDEV_TYPE_HOST_SPI_FLASH,
    .register_hal_module = host_spi_flash_register,
    .unregister_hal_module = host_spi_flash_unregister,
    .pcore_funcs = (void *) &host_spi_flash_core_ops
};

int host_spi_flash_init(void)
{
    int ret;

    /* host_spi_flash device initialization */
    if ((ret = register_chrdev_region (host_spi_flash_devno, HOST_SPI_FLASH_MAX_DEVICES, HOST_SPI_FLASH_DRIVER_NAME)) < 0)
    {
        printk (KERN_ERR "failed to register host_spi_flash device <%s> (err: %d)\n", HOST_SPI_FLASH_DRIVER_NAME, ret);
        return -1;
    }

    host_spi_flash_cdev = cdev_alloc();
    if(!host_spi_flash_cdev)
    {
        unregister_chrdev_region (host_spi_flash_devno, HOST_SPI_FLASH_MAX_DEVICES);
        printk (KERN_ERR "%s: failed to allocate host_spi_flash cdev structure\n", HOST_SPI_FLASH_DRIVER_NAME);
        return -1;
    }
    cdev_init (host_spi_flash_cdev, &host_spi_flash_ops);
    host_spi_flash_cdev->owner = THIS_MODULE;

    if ((ret = cdev_add (host_spi_flash_cdev, host_spi_flash_devno, HOST_SPI_FLASH_MAX_DEVICES)) < 0)
    {
        cdev_del (host_spi_flash_cdev);
        unregister_chrdev_region (host_spi_flash_devno, HOST_SPI_FLASH_MAX_DEVICES);
        printk  (KERN_ERR "failed to add <%s> char device\n", HOST_SPI_FLASH_DRIVER_NAME);
        ret = -ENODEV;
        return ret;
    }

	host_spi_create_proc();  // Quanta

    //init_MUTEX(&host_spi_flash_lock);
    sema_init(&host_spi_flash_lock, 1);

    ret = register_core_hal_module(&host_spi_flash_core_hal);
    if (ret < 0)
    {
        printk(KERN_ERR "failed to register the host SPI flash core module\n");
        cdev_del (host_spi_flash_cdev);
        unregister_chrdev_region (host_spi_flash_devno, HOST_SPI_FLASH_MAX_DEVICES);
        ret = -EINVAL;
        return ret;
    }

    return 0;
}

void __exit host_spi_flash_exit(void)
{
	host_spi_remove_proc();  // Quanta
    unregister_core_hal_module(EDEV_TYPE_HOST_SPI_FLASH);
    unregister_chrdev_region (host_spi_flash_devno, HOST_SPI_FLASH_MAX_DEVICES);
    if (NULL != host_spi_flash_cdev)
    {
        cdev_del (host_spi_flash_cdev);
    }
}
static int 
host_spi_flash_open(struct inode *inode, struct file *file)
{
    unsigned int minor = iminor(inode);
    struct host_spi_flash_hal_t *phost_spi_flash_hal;
    struct host_spi_flash_dev *pdev;
    hw_info_t host_spi_flash_hw_info;
    int ret;
    unsigned char open_count;

    ret = hw_open (EDEV_TYPE_HOST_SPI_FLASH, minor, &open_count, &host_spi_flash_hw_info);
    if (ret)
        return -ENXIO;

    phost_spi_flash_hal = host_spi_flash_hw_info.pdrv_data;

    pdev = (struct host_spi_flash_dev*)kmalloc(sizeof(struct host_spi_flash_dev), GFP_KERNEL);

    if (!pdev)
    {
        hw_close (EDEV_TYPE_HOST_SPI_FLASH, minor, &open_count);
        printk (KERN_ERR "%s: failed to allocate private dev structure for host_spi_flash iminor: %d\n", HOST_SPI_FLASH_DRIVER_NAME, minor);
        return -ENOMEM;
    }

    pdev->phost_spi_flash_hal = phost_spi_flash_hal;
    file->private_data = pdev;

    return 0;
}


static int
host_spi_flash_release(struct inode *inode, struct file *file)
{
    int ret;
    unsigned char open_count;
    struct host_spi_flash_dev *pdev = (struct host_spi_flash_dev*)file->private_data;
    pdev->phost_spi_flash_hal = NULL;
    file->private_data = NULL;
    ret = hw_close (EDEV_TYPE_HOST_SPI_FLASH, iminor(inode), &open_count);
    if(ret) { return -1; }
        kfree (pdev);
    return 0;
}

static long
host_spi_flash_module_ioctlUnlocked(struct file *file, uint cmd, ulong arg)
{
   	struct host_spi_flash_dev *pdev = (struct host_spi_flash_dev*) file->private_data;
   	int host_spi_access = 0;

    switch (cmd)
    {
         case SET_HOST_SPI_ACCESS:
            if(copy_from_user((void *)&host_spi_access,(void *)arg, sizeof(int)))
            {
                printk("SET_HOST_SPI_ACCESS: Error copying data from user \n");
                return -EFAULT;
            }
            if (pdev->phost_spi_flash_hal->ops->host_spi_flash_change_mode)
                pdev->phost_spi_flash_hal->ops->host_spi_flash_change_mode(host_spi_access);
            break;

         default:
             printk("ERROR: host_spi_flash: Invalid IOCTL \n");
             return -EINVAL;
   	}
    return 0;
}

#ifndef USE_UNLOCKED_IOCTL
static int
host_spi_flash_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg)
{
        return (host_spi_flash_module_ioctlUnlocked(file, cmd, arg));
}
#endif

module_init(host_spi_flash_init);
module_exit(host_spi_flash_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("Common module of host SPI flash driver");
MODULE_LICENSE("GPL");
