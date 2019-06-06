/***************************************************************
****************************************************************
**                                                            **
**    (C)Copyright 2009-2015, American Megatrends Inc.        **
**                                                            **
**            All Rights Reserved.                            **
**                                                            **
**        6145-F, Northbelt Parkway, Norcross,                **
**                                                            **
**        Georgia - 30071, USA. Phone-(770)-246-8600.         **
**                                                            **
****************************************************************
 ****************************************************************/

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/aio.h>
#include <linux/bigphysarea.h>
#include <linux/cdev.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/platform_device.h>	// Quanta
#include <linux/miscdevice.h>	// Quanta

#include "helper.h"
#include "ast_videocap_data.h"
#include "ast_videocap_hw.h"
#include "ast_videocap_functions.h"
#include <linux/delay.h>	// Quanta
#include "regs-video.h"		// Quanta
#include "regs-scu.h"		// Quanta
#include "ast_video.h"		// Quanta


#define AST_VIDEOCAP_DEV_NAME	"videocap"
#define AST_VIDEOCAP_DRV_NAME	"ast_videocap"
#define AST_VIDEOCAP_DEV_MAJOR	15
#define AST_VIDEOCAP_DEV_MINOR	0
#define AST_VIDEOCAP_DEV_NUM	1
/* Quanta -----, change device from cdev(char device) to misc dev for later VNC integration.
static dev_t ast_videocap_devno = MKDEV(AST_VIDEOCAP_DEV_MAJOR, AST_VIDEOCAP_DEV_MINOR);
static struct cdev ast_videocap_cdev;
*/

void *ast_videocap_reg_virt_base;
void *ast_videocap_video_buf_virt_addr;
void *ast_videocap_video_buf_phys_addr;

static int ast_videocap_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ast_videocap_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int ast_videocap_alloc_bigphysarea(void)
{
	unsigned long addr;
	unsigned long size;

	ast_videocap_video_buf_virt_addr = bigphysarea_alloc_pages(MEM_POOL_SIZE / PAGE_SIZE, 0, GFP_KERNEL);
	if (ast_videocap_video_buf_virt_addr == NULL) {
		return -1;
	}

	addr = (unsigned long) ast_videocap_video_buf_virt_addr;
	size = MEM_POOL_SIZE;
	while (size > 0) {
		SetPageReserved(virt_to_page(addr));
		addr += PAGE_SIZE;
		size -= PAGE_SIZE;
	}
	memset(ast_videocap_video_buf_virt_addr, 0x00, MEM_POOL_SIZE);

	ast_videocap_video_buf_phys_addr = (void *) virt_to_phys(ast_videocap_video_buf_virt_addr);

	//printk(KERN_INFO "got physical memory pool for size (%d on %p virtual/%08lx bus)\n", MEM_POOL_SIZE, (unsigned long*)ast_videocap_video_buf_virt_addr, ast_videocap_video_buf_phys_addr);
	return 0;
}

static void ast_videocap_free_bigphysarea(void)
{
	unsigned long addr;
	unsigned long size;

	//printk (KERN_INFO "Releasing Big Physical memory starting :0x%lx\n", ast_videocap_video_buf_virt_addr);

	if (ast_videocap_video_buf_virt_addr != NULL) {
		size = MEM_POOL_SIZE;
		addr = (unsigned long) ast_videocap_video_buf_virt_addr;
		while (size > 0) {
			ClearPageReserved(virt_to_page(addr));
			addr += PAGE_SIZE;
			size -= PAGE_SIZE;
		}
		bigphysarea_free_pages(ast_videocap_video_buf_virt_addr);
	}
}

static struct file_operations ast_videocap_fops = {
	.owner = THIS_MODULE,
	.open = ast_videocap_open,
	.release = ast_videocap_release,
#ifdef USE_UNLOCKED_IOCTL 
	.unlocked_ioctl = ast_videocap_ioctl,
#else
	.ioctl = ast_videocap_ioctl,
#endif
	.mmap = ast_videocap_mmap,
};

// Quanta -----, change driver model in order to generate probe() callback for later VNC use
struct miscdevice ast_video_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "videocap",
	.fops = &ast_videocap_fops,
};

static int ast_video_probe(struct platform_device *pdev)
{
	int ret=0;
	int i;
	struct ast_video_data *ast_video;

	ret = misc_register(&ast_video_misc);	// change device from cdev(char device) to misc dev
	if (ret){		
		printk(KERN_ERR "VIDEO : failed to request interrupt\n");
		return ret;
	}

	if (request_mem_region(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE, AST_VIDEOCAP_DRV_NAME) == NULL) {
		printk(KERN_WARNING "%s: request memory region failed\n", AST_VIDEOCAP_DRV_NAME);
		ret = -EBUSY;
		goto out_misc_reg;
	}

	ast_videocap_reg_virt_base = ioremap(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE);
	if (ast_videocap_reg_virt_base == NULL) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_VIDEOCAP_DRV_NAME);
		ret = -ENOMEM;
		goto out_mem_region;
	}

	ret = ast_videocap_alloc_bigphysarea();
	if (ret != 0) {
		printk(KERN_WARNING "failed to allocate physical memory pool\n");
		goto out_iomap;
	}

	ret = ast_videocap_remap_vga_mem();
	if (ret != 0) {
		printk(KERN_WARNING "remap VGA memory failed\n");
		goto out_bigphys;
	}

	ast_videocap_engine_data_init();
	ast_videocap_reset_hw();
	ast_videocap_hw_init();

	ast_videocap_add_proc_entries();

	// Integrate ASPEED provided JPEG capture engine for VNC usage.
	if(!(ast_video = kzalloc(sizeof(struct ast_video_data), GFP_KERNEL))) {
		ret = -ENOMEM;
		goto out_remap_vga;
        }

	//Phy assign
	ast_video->buff0_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_REF_BUF_ADDR);
	ast_video->buff1_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_IN_BUF_ADDR);
	ast_video->bcd_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_FLAG_BUF_ADDR);
	ast_video->hdr_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_HDR_BUF_ADDR);
	ast_video->stream_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_COMPRESS_BUF_ADDR);
	ast_video->jpeg_buf0_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_JPEG_BUF_ADDR);
	ast_video->jpeg_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_JPEG_ADDR);
	ast_video->video_jpeg_offset = (u32)ast_video->jpeg_phy - (u32)ast_video->stream_phy/*ast_video->hdr_phy*/;
	ast_video->jpeg_tbl_phy = (phys_addr_t *) virt_to_phys(AST_VIDEOCAP_JPEG_TBL_ADDR);

	ast_video->video_mem_size = MEM_POOL_SIZE - ((u32)ast_video->stream_phy/*ast_video->hdr_phy*/ - (u32)ast_video->buff0_phy);
	printk("video_mem_size %d MB\n",ast_video->video_mem_size/1024/1024);
	printk("\nvideo_jpeg_offset: %x \n", (u32)ast_video->video_jpeg_offset);

	VIDEO_DBG("\nstream_phy: %x, buff0_phy: %x, buff1_phy:%x, bcd_phy:%x \njpeg_phy:%x, jpeg_tbl_phy:%x \n",
	        (u32)ast_video->stream_phy, (u32)ast_video->buff0_phy, (u32)ast_video->buff1_phy, (u32)ast_video->bcd_phy, (u32)ast_video->jpeg_phy, (u32)ast_video->jpeg_tbl_phy);

	printk("\nstream_phy: %x, buff0_phy: %x, buff1_phy:%x, bcd_phy:%x \njpeg_phy:%x, jpeg_tbl_phy:%x, hdr_phy: %x \n",
	        (u32)ast_video->stream_phy, (u32)ast_video->buff0_phy, (u32)ast_video->buff1_phy, (u32)ast_video->bcd_phy, (u32)ast_video->jpeg_phy, (u32)ast_video->jpeg_tbl_phy, (u32)ast_video->hdr_phy);

	ast_video->buff0_virt = (u32 *)AST_VIDEOCAP_REF_BUF_ADDR;
	ast_video->buff1_virt = (u32 *)AST_VIDEOCAP_IN_BUF_ADDR;
	ast_video->bcd_virt = (u32 *)AST_VIDEOCAP_FLAG_BUF_ADDR;
	ast_video->hdr_virt = (u32 *)AST_VIDEOCAP_HDR_BUF_ADDR;
	ast_video->stream_virt = (u32 *)AST_VIDEOCAP_COMPRESS_BUF_ADDR;
	ast_video->jpeg_buf0_virt =  (u32 *)AST_VIDEOCAP_JPEG_BUF_ADDR;
	ast_video->jpeg_virt =  (u32 *)AST_VIDEOCAP_JPEG_ADDR;
	ast_video->jpeg_tbl_virt =  (u32 *)AST_VIDEOCAP_JPEG_TBL_ADDR;

	VIDEO_DBG("\nstream_virt: %x, buff0_virt: %x, buff1_virt:%x, bcd_virt:%x \njpeg_virt:%x, jpeg_tbl_virt:%x \n",
	        (u32)ast_video->stream_virt, (u32)ast_video->buff0_virt, (u32)ast_video->buff1_virt, (u32)ast_video->bcd_virt, (u32)ast_video->jpeg_virt, (u32)ast_video->jpeg_tbl_virt);

	printk("\nstream_virt: %x, buff0_virt: %x, buff1_virt:%x, bcd_virt:%x \njpeg_virt:%x, jpeg_tbl_virt:%x, hdr_virt: %x \n",
	        (u32)ast_video->stream_virt, (u32)ast_video->buff0_virt, (u32)ast_video->buff1_virt, (u32)ast_video->bcd_virt, (u32)ast_video->jpeg_virt, (u32)ast_video->jpeg_tbl_virt, (u32)ast_video->hdr_virt);

	memset(ast_video->buff0_virt, 0, MEM_POOL_SIZE);

	// default config 
	ast_video->input_source = VIDEO_SOURCE_INT_VGA;
	ast_video->rc4_enable = 0;
	strcpy(ast_video->EncodeKeys, "fedcba9876543210");
	ast_video->scaling = 0;

	ret = sysfs_create_group(&pdev->dev.kobj, &video_attribute_group);
	if (ret)
		goto out_kzalloc;

	for(i=0;i<2;i++) {
		ret = sysfs_create_group(&pdev->dev.kobj, &compress_attribute_groups[i]);
		if (ret)
			goto out_kzalloc;
	}

	platform_set_drvdata(pdev, ast_video);
	dev_set_drvdata(ast_video_misc.this_device, ast_video);

	ast_video_ctrl_init(ast_video);

	IRQ_SET_HIGH_LEVEL(AST_VIDEOCAP_IRQ);
	IRQ_SET_LEVEL_TRIGGER(AST_VIDEOCAP_IRQ);

	ret = request_irq(AST_VIDEOCAP_IRQ, ast_videocap_irq_handler, IRQF_DISABLED, AST_VIDEOCAP_DRV_NAME, ast_video);
	if (ret != 0) {
		printk(KERN_WARNING "%s: request IRQ failed\n", AST_VIDEOCAP_DRV_NAME);
		goto out_kzalloc;
	}

	return 0;
	
out_kzalloc:
	kfree (ast_video);
out_remap_vga:
	ast_videocap_unmap_vga_mem();
out_bigphys:
	ast_videocap_free_bigphysarea();
out_iomap:
	iounmap(ast_videocap_reg_virt_base);
out_mem_region:
	release_mem_region(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE);
out_misc_reg:
	misc_deregister(&ast_video_misc);

	return ret;
}

static int ast_video_remove(struct platform_device *pdev)
{
	// Quanta, Integrate ASPEED provided JPEG capture engine for VNC usage
	struct ast_video_data *ast_video = platform_get_drvdata(pdev);

	printk("[Jason] ast_video_remove\n");

	ast_videocap_del_proc_entries();

	free_irq(AST_VIDEOCAP_IRQ, ast_video);
	iounmap(ast_videocap_reg_virt_base);
	release_mem_region(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE);
	misc_deregister(&ast_video_misc);

	ast_videocap_free_bigphysarea();
	ast_videocap_unmap_vga_mem();

	kfree(ast_video);

	return 0;	
}

static const struct platform_device_id ast_video_idtable[] = {
	{
		.name = "ast-video",
//		.driver_data = ast_video_data,
		/* sentinel */
	}
	,{}
};
MODULE_DEVICE_TABLE(platform, ast_video_idtable);

static struct platform_driver ast_video_driver = {
	.probe 		= ast_video_probe, 
	.remove 		= ast_video_remove,
	.suspend        = NULL,
	.resume         = NULL,
	.driver  	       = {
	        .name   = "ast-video",
	        .owner  = THIS_MODULE,
	},
	.id_table	= ast_video_idtable,	
};


static struct platform_device *ast_video_device;
// ----- Quanta

int __init ast_videocap_module_init(void)
{
	int ret;

	printk("AST Video Capture Driver, (c) 2009-2015 American Megatrends Inc.\n");

#if 1	//Quanta, change driver model in order to generate probe() callback for later VNC use

	ret = platform_driver_register(&ast_video_driver);

	if (!ret) {
		ast_video_device = platform_device_register_simple("ast-video", 0, 0, 0);
		if (IS_ERR(ast_video_device)) {
			platform_driver_unregister(&ast_video_driver);
			ret = PTR_ERR(ast_video_device);
		}
	}

#else	// Quanta

	ret = register_chrdev_region(ast_videocap_devno, AST_VIDEOCAP_DEV_NUM, AST_VIDEOCAP_DEV_NAME);
	if (ret < 0) {
		printk(KERN_ERR "%s: failed to register char device\n", AST_VIDEOCAP_DEV_NAME);
		return ret;
	}

	cdev_init(&ast_videocap_cdev, &ast_videocap_fops);

	ret = cdev_add(&ast_videocap_cdev, ast_videocap_devno, AST_VIDEOCAP_DEV_NUM);
	if (ret < 0) {
		printk(KERN_ERR "%s: failed to add char device\n", AST_VIDEOCAP_DEV_NAME);
		goto out_cdev_register;
	}

	if (request_mem_region(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE, AST_VIDEOCAP_DRV_NAME) == NULL) {
		printk(KERN_WARNING "%s: request memory region failed\n", AST_VIDEOCAP_DRV_NAME);
		ret = -EBUSY;
		goto out_cdev_add;
	}

	ast_videocap_reg_virt_base = ioremap(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE);
	if (ast_videocap_reg_virt_base == NULL) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_VIDEOCAP_DRV_NAME);
		ret = -ENOMEM;
		goto out_mem_region;
	}

	IRQ_SET_HIGH_LEVEL(AST_VIDEOCAP_IRQ);
	IRQ_SET_LEVEL_TRIGGER(AST_VIDEOCAP_IRQ);

	ret = request_irq(AST_VIDEOCAP_IRQ, ast_videocap_irq_handler, IRQF_DISABLED, AST_VIDEOCAP_DRV_NAME, NULL);
	if (ret != 0) {
		printk(KERN_WARNING "%s: request IRQ failed\n", AST_VIDEOCAP_DRV_NAME);
		goto out_iomap;
	}

	ret = ast_videocap_alloc_bigphysarea();
	if (ret != 0) {
		printk(KERN_WARNING "failed to allocate physical memory pool\n");
		goto out_irq;
	}

	ret = ast_videocap_remap_vga_mem();
	if (ret != 0) {
		printk(KERN_WARNING "remap VGA memory failed\n");
		goto out_bigphys;
	}

	ast_videocap_engine_data_init();
	ast_videocap_reset_hw();
	ast_videocap_hw_init();

	ast_videocap_add_proc_entries();

	return 0;

out_bigphys:
	ast_videocap_free_bigphysarea();
out_irq:
	free_irq(AST_VIDEOCAP_IRQ, NULL);
out_iomap:
	iounmap(ast_videocap_reg_virt_base);
out_mem_region:
	release_mem_region(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE);
out_cdev_add:
	cdev_del(&ast_videocap_cdev);
out_cdev_register:
	unregister_chrdev_region(ast_videocap_devno, AST_VIDEOCAP_DEV_NUM);

#endif	// Quanta

	return ret;
}

void __exit ast_videocap_module_exit(void)
{
#if 1	//Quanta, change driver model in order to generate probe() callback for later VNC use

	platform_device_unregister(ast_video_device);
	platform_driver_unregister(&ast_video_driver);

#else	// Quanta

	ast_videocap_del_proc_entries();

	free_irq(AST_VIDEOCAP_IRQ, NULL);
	iounmap(ast_videocap_reg_virt_base);
	release_mem_region(AST_VIDEOCAP_REG_BASE, AST_VIDEOCAP_REG_SIZE);
	cdev_del(&ast_videocap_cdev);
	unregister_chrdev_region(ast_videocap_devno, AST_VIDEOCAP_DEV_NUM);

	ast_videocap_free_bigphysarea();
	ast_videocap_unmap_vga_mem();

#endif	// Quanta
}

module_init(ast_videocap_module_init);
module_exit(ast_videocap_module_exit);

MODULE_AUTHOR("American Megatrends Inc");
MODULE_DESCRIPTION("AST video capture engine driver");
MODULE_LICENSE("GPL");
