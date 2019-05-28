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
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#include "helper.h"

#include "ast_videocap_data.h"
#include "ast_videocap_functions.h"

#define AST_VIDECAP_PRCO_DIR			"videocap"
#define AST_VIDECAP_PRCO_STATUS_ENTRY	"status"
#define AST_VIDECAP_PRCO_REG_ENTRY		"regs"

#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
struct proc_info *statusproc;
struct proc_info *regproc;
#endif

static struct ctl_table_header *ast_videocap_sysctl;
static struct proc_dir_entry *ast_videocap_proc_dir = NULL;

/* /proc/sys entries list */
static struct ctl_table ast_videocap_sysctl_table[] = {
	{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 1,
#endif
		.procname = "CompressionMode",
		.data = &(ast_videocap_engine_info.FrameHeader.CompressionMode),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 2,
#endif
		.procname = "JPEGTableSelector",
		.data = &(ast_videocap_engine_info.FrameHeader.JPEGTableSelector),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 3,
#endif
		.procname = "JPEGScaleFactor",
		.data = &(ast_videocap_engine_info.FrameHeader.JPEGScaleFactor),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 4,
#endif
		.procname = "JPEGYUVTableMapping",
		.data = &(ast_videocap_engine_info.FrameHeader.JPEGYUVTableMapping),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 5,
#endif
		.procname = "DifferentialSetting",
		.data = &(ast_videocap_engine_info.INFData.DifferentialSetting),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,

#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 6,
#endif
		.procname = "DigitalDifferentialThreshold",
		.data = &(ast_videocap_engine_info.INFData.DigitalDifferentialThreshold),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 7,
#endif
		.procname = "AdvanceTableSelector",
		.data = &(ast_videocap_engine_info.FrameHeader.AdvanceTableSelector),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 8,
#endif
		.procname = "AdvanceScaleFactor",
		.data = &(ast_videocap_engine_info.FrameHeader.AdvanceScaleFactor),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 9,
#endif
		.procname = "SharpModeSelection",
		.data = &(ast_videocap_engine_info.FrameHeader.SharpModeSelection),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = 10,
#endif
		.procname = "direct_mode",
		.data = &(ast_videocap_engine_info.INFData.DirectMode),
		.maxlen = sizeof(unsigned long),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_doulongvec_minmax
	}, {
		0
	}
};

#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
int ast_videocap_print_status(struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int ast_videocap_print_status(char *buf, char **start, off_t offset, int count, int *eof, void *data)
#endif
{
	int len;

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	*start = buf;
	*eof = 1;
#endif

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	if (offset != 0) 
		return 0;
#else
	if (*offset != 0) 
		return 0;
#endif

	len = 0;

	len += sprintf(buf + len, "video buf phys addr: 0x%p\n", ast_videocap_video_buf_phys_addr);
	len += sprintf(buf + len, "video buf virt addr: 0x%p\n", ast_videocap_video_buf_virt_addr);
	len += sprintf (buf + len, "video source mode: (%d x %d) @ %d Hz\n",
					ast_videocap_engine_info.src_mode.x,
					ast_videocap_engine_info.src_mode.y,
					ast_videocap_engine_info.src_mode.RefreshRate);
	len += sprintf (buf + len, "compression mode: %ld\n", ast_videocap_engine_info.FrameHeader.CompressionMode);
	len += sprintf(buf + len, "driver waiting location: ");
	if (WaitingForModeDetection)
		len += sprintf(buf + len, "mode detection\n");
	if (WaitingForCapture)
		len += sprintf(buf + len, "capture\n");
	if (WaitingForCompression)
		len += sprintf(buf + len, "compression\n");
	if (!WaitingForCompression && !WaitingForCapture && !WaitingForModeDetection)
		len += sprintf(buf + len, "idle\n");


#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
		*offset+=len;
#endif

	return len;
}

#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
int ast_videocap_print_regs(struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int ast_videocap_print_regs(char *buf, char **start, off_t offset, int count, int *eof, void *data)
#endif
{
	int len;
	int i;

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	*start = buf;
	*eof = 1;
#endif

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	if (offset != 0) 
		return 0;
#else
	if (*offset != 0) 
		return 0;
#endif

	len = 0;

	len += sprintf(buf + len, "----------------------------------------\n");

	for (i = 0; i < 0x98; i += 4) {
		len += sprintf(buf + len, "%03X: %08X\n", i, ast_videocap_read_reg(i));
	}

	for (i = 0x300; i < 0x340; i += 4) {
		len += sprintf(buf + len, "%03X: %08X\n", i, ast_videocap_read_reg(i));
	}

	len += sprintf(buf + len, "----------------------------------------\n");

#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
	*offset+=len;
#endif

	return len;
}

void ast_videocap_add_proc_entries(void)
{
	/* create directory entry of this module in /proc and add two files "info" and "regs" */
	ast_videocap_proc_dir = proc_mkdir(AST_VIDECAP_PRCO_DIR, rootdir);

#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
	statusproc=
#endif
	AddProcEntry(ast_videocap_proc_dir, AST_VIDECAP_PRCO_STATUS_ENTRY, ast_videocap_print_status, NULL, NULL);

#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
	regproc=
#endif
	AddProcEntry(ast_videocap_proc_dir, AST_VIDECAP_PRCO_REG_ENTRY, ast_videocap_print_regs, NULL, NULL);

	/* add sysctl to access the DebugLevel */
	ast_videocap_sysctl = AddSysctlTable(AST_VIDECAP_PRCO_DIR, ast_videocap_sysctl_table);
}

void ast_videocap_del_proc_entries(void)
{
	/* Remove this driver's proc entries */
#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,4,11))
	RemoveProcEntry(statusproc);
	RemoveProcEntry(regproc);
#else
	RemoveProcEntry(ast_videocap_proc_dir, AST_VIDECAP_PRCO_STATUS_ENTRY);
	RemoveProcEntry(ast_videocap_proc_dir, AST_VIDECAP_PRCO_REG_ENTRY);
#endif
	remove_proc_entry(AST_VIDECAP_PRCO_DIR, rootdir);

	/* Remove driver related sysctl entries */
	RemoveSysctlTable(ast_videocap_sysctl);
}
