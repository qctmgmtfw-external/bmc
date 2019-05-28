/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include "helper.h"
#include "dbgout.h"
#include "proc.h"
#include "usb_driver.h"

TDBG_DECLARE_DBGVAR(usbe);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
struct proc_info *usbproc;
#endif

static int 	usbe_notify_sys(struct notifier_block *this, unsigned long code, void *unused);

static struct notifier_block usbe_notifier =
{
       .notifier_call = usbe_notify_sys,
};
static struct proc_dir_entry *moduledir = NULL;
static struct ctl_table_header *my_sys 	= NULL;
static struct ctl_table usbectlTable [] =
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	{"DebugLevel",&(TDBG_FORM_VAR_NAME(usbe)),sizeof(int),0644,NULL,&proc_dointvec}, 
#else	
	{CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(usbe)),sizeof(int),0644,NULL,NULL,&proc_dointvec}, 
#endif
	{0} 
};


int
init_module(void)
{
	int rc;
	printk("Initializing USB Devices\n");
	rc = UsbDriverInit();
	if (!rc)
	{
		register_reboot_notifier(&usbe_notifier);
        moduledir = proc_mkdir("usbe",rootdir);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
		usbproc =
#endif
		AddProcEntry(moduledir,"status",ReadUsbeStatus,NULL,NULL);
 		my_sys = AddSysctlTable("usbe",&usbectlTable[0]);
	}
	return rc;
}

void
cleanup_module(void)
{
	printk("Unloading USB Devices\n");
	UsbDriverExit();
	unregister_reboot_notifier(&usbe_notifier);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
	if (usbproc)
		RemoveProcEntry(usbproc);
#else
	if (moduledir)
		RemoveProcEntry(moduledir,"status");
#endif
	remove_proc_entry("usbe",rootdir);
	if (my_sys) RemoveSysctlTable(my_sys);
	return;
}

static int
usbe_notify_sys(struct notifier_block *this, unsigned long code, void *unused)
{
	
	if (code == SYS_DOWN || code == SYS_HALT)
	{
	}
	return NOTIFY_DONE;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Samvinesh Christopher - American Megatrends Inc");
MODULE_DESCRIPTION("USB Devices driver for Linux 2.6");



