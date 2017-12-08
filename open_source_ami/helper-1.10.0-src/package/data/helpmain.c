/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

/* Helper Module Version */
#define HELPER_MAJOR	1
#define HELPER_MINOR 	2

#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>	
#include <linux/version.h>
#if defined(__arm__)
#include <mach/platform.h>
#endif
#include "helper.h"

/* Debug Level for the driver - Controlled by sysctl */
static int DebugLevel = 0;		

/* Proc entry for /proc/ractrends . This is created in this 
 * module and  is exported to other modules */
struct proc_dir_entry *rootdir = NULL;
static struct proc_dir_entry *fwinfo_proc = NULL;

/* Define the symbols to be exported to outside world */
EXPORT_SYMBOL(rootdir);
EXPORT_SYMBOL(AddProcEntry);
EXPORT_SYMBOL(RemoveProcEntry);

EXPORT_SYMBOL(AddSysctlTable);
EXPORT_SYMBOL(RemoveSysctlTable);
/* Local Variables */
static struct ctl_table_header *helper_sys 	= NULL;
static struct proc_dir_entry *moduledir = NULL;

unsigned long FlashStart;
unsigned long FlashSize;
unsigned long EnvStart;
unsigned long EnvSize;
unsigned long FlashSectorSize;
unsigned long UsedFlashStart;
unsigned long UsedFlashSize;
unsigned long jiffiesread=0;

extern int fwinfo_read(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int proc_jiffies(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,loff_t* pos);
extern void free_info_cache(void);

/* Sysctl related tables */
static struct ctl_table HelperTable[] =
{
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24))
	{CTL_UNNUMBERED,"DebugLevel",&DebugLevel,sizeof(int),0644,NULL,NULL,&proc_dointvec},
	{CTL_UNNUMBERED,"FlashStart",&(FlashStart),sizeof(unsigned long),
                0444,NULL,NULL,&proc_doulongvec_minmax},
	{CTL_UNNUMBERED,"FlashSize",&(FlashSize),sizeof(unsigned long),
                0444,NULL,NULL,&proc_doulongvec_minmax},
	{CTL_UNNUMBERED,"EnvStart",&(EnvStart),sizeof(unsigned long),
                0444,NULL,NULL,&proc_doulongvec_minmax},
	{CTL_UNNUMBERED,"EnvSize",&(EnvSize),sizeof(unsigned long),
                0444,NULL,NULL,&proc_doulongvec_minmax},
	{CTL_UNNUMBERED,"FlashSectorSize",&(FlashSectorSize),sizeof(unsigned long),
                0444,NULL,NULL,&proc_doulongvec_minmax},
	{CTL_UNNUMBERED,"UsedFlashStart",&(UsedFlashStart),sizeof(unsigned long),
                0444,NULL,NULL,&proc_doulongvec_minmax},
	{CTL_UNNUMBERED,"UsedFlashSize",&(UsedFlashSize),sizeof(unsigned long),
                0444,NULL,NULL,&proc_doulongvec_minmax},
	{CTL_UNNUMBERED,"Jiffies",&(jiffiesread),sizeof(unsigned long),
                0644,NULL,NULL,&proc_jiffies},
#else
        {1,"DebugLevel",&DebugLevel,sizeof(int),0644,NULL,&proc_dointvec},
        {2,"FlashStart",&(FlashStart),sizeof(unsigned long),
                0444,NULL,&proc_doulongvec_minmax},
        {3,"FlashSize",&(FlashSize),sizeof(unsigned long),
                0444,NULL,&proc_doulongvec_minmax},
        {4,"EnvStart",&(EnvStart),sizeof(unsigned long),
                0444,NULL,&proc_doulongvec_minmax},
        {5,"EnvSize",&(EnvSize),sizeof(unsigned long),
                0444,NULL,&proc_doulongvec_minmax},
        {6,"FlashSectorSize",&(FlashSectorSize),sizeof(unsigned long),
                0444,NULL,&proc_doulongvec_minmax},
        {7,"UsedFlashStart",&(UsedFlashStart),sizeof(unsigned long),
                0444,NULL,&proc_doulongvec_minmax},
        {8,"UsedFlashSize",&(UsedFlashSize),sizeof(unsigned long),
                0444,NULL,&proc_doulongvec_minmax},
        {9,"Jiffies",&(jiffiesread),sizeof(unsigned long),
                0644,NULL,&proc_jiffies},
#endif
	{0}
};

static int proc_jiffies(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,loff_t* pos)
{
	int retval=0;

	jiffiesread = jiffies;
	retval = proc_doulongvec_minmax(ctl,write,filep,buffer,lenp,pos);
	return retval;
}


int
init_helper(void)
{
	printk(KERN_ALERT "Helper Module Driver Version %d.%d\n",HELPER_MAJOR,HELPER_MINOR);
	printk(KERN_ALERT "Copyright (c) 2006 American Megatrends Inc.\n");

	/* Create a new entry under /proc */
	rootdir = proc_mkdir(PROC_DIR,NULL);
	FlashStart    	= CONFIG_SPX_FEATURE_GLOBAL_FLASH_START;
	FlashSize     	= CONFIG_SPX_FEATURE_GLOBAL_FLASH_SIZE;
	EnvStart      	= CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START;
	EnvSize  	= CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_SIZE;
	FlashSectorSize = CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
	UsedFlashStart 	= CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_START;
	UsedFlashSize	= CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;

	/* Create this module's directory entry in proc and add a file "HwInfo" */
	moduledir = proc_mkdir("Helper",rootdir);
	fwinfo_proc = AddProcEntry(moduledir,"FwInfo",fwinfo_read,NULL,NULL);
	
	/* Add sysctl to access the DebugLevel */
	helper_sys  = AddSysctlTable("Helper",&HelperTable[0]);
	return 0;
}


void
exit_helper(void)
{
	printk(KERN_ALERT "Unloading Helper Module ..............\n");
	/* Remove driver related sysctl entries */
	RemoveSysctlTable(helper_sys);
	
	/* Remove the driver's proc entries */
	RemoveProcEntry(moduledir,"FwInfo");

	/* Remove the driver's proc entries */
	remove_proc_entry("Helper",rootdir);
	
	/* Remove the rootdir under /proc */
	remove_proc_entry(PROC_DIR,NULL);

	free_info_cache();
	return;
}

module_init(init_helper);
module_exit(exit_helper);

/*--------------------------- Module Information Follows --------------------------*/
//#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24))
MODULE_LICENSE("GPL");        // Need to be in GPL to use get_mtd_device function, which is exported as GPL symbol in 2.6.24
//#endif
MODULE_AUTHOR("Samvinesh Christopher - American Megatrends Inc");
MODULE_DESCRIPTION("Helper Functions for all other modules");


