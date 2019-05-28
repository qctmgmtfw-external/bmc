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

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <asm/pgtable.h>
#include <asm/io.h>

#include "ast_videocap_data.h"
#include "ast_videocap_hw.h"

#define VMA_OFFSET(vma) 	((vma)->vm_pgoff << PAGE_SHIFT)

void ast_videocap_vma_open(struct vm_area_struct *vma)
{
	//printk ("vma_open() Called\n");
//    MOD_INC_USE_COUNT;
}

void ast_videocap_vma_close(struct vm_area_struct *vma)
{
	//printk ("vma_close() Called\n");
 //  MOD_DEC_USE_COUNT;
}

struct vm_operations_struct videocap_vm_ops = {
	.open = ast_videocap_vma_open,
	.close = ast_videocap_vma_close,

	/* Don't use nopage, because no page will work correctly only if we have a single page.
	 * Since we are allocating a link of pages using __get_free_pages, we cannot use this. */
	//.nopage = videocap_vma_nopage,
};

int ast_videocap_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long off;
	unsigned long start;
	int ret;

	start = (unsigned long) (ast_videocap_video_buf_phys_addr + AST_VIDEOCAP_IN_BUF_SZ + AST_VIDEOCAP_IN_BUF_SZ + AST_VIDEOCAP_FLAG_BUF_ALLOC_SZ);
	
	/* Check if the offsets are aligned */
	if (VMA_OFFSET(vma) & (PAGE_SIZE - 1)) {
		printk("mmap() received unaligned offsets. Cannot map \n");
		return -ENXIO;
	}

	off = start;
	//vma->vm_pgoff = off >> PAGE_SHIFT;
#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
	vma->vm_flags |= VM_IO |  VM_WRITE | VM_READ;
#else
	vma->vm_flags |= VM_IO | VM_RESERVED | VM_WRITE | VM_READ;
#endif
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	/* map the FPGA base address. The Adviser application will be able to access
	   the entire fpga memory region */
	//if (io_remap_pfn_range(vma, vma->vm_start, off >> PAGE_SHIFT,
	ret = io_remap_pfn_range(vma, vma->vm_start, off >> PAGE_SHIFT, vma->vm_end-vma->vm_start, vma->vm_page_prot);
	if (ret != 0) {
		printk ("mmap(): remmap_page_range failed\n");
		return -EAGAIN;
	}

	vma->vm_ops = &videocap_vm_ops;

	ast_videocap_vma_open(vma);
	return 0;
}
