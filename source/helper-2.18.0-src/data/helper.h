/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#ifndef AMI_HELPER_H
#define AMI_HELPER_H

#include <linux/version.h>
#include <linux/sysctl.h>	
#include <linux/proc_fs.h>		

/* Name of directory under /proc and /proc/sys where all the modules 
 * put their proc entries and sysctl entries respectivly  */
#define PROC_DIR	"ractrends"	

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
struct proc_info
{
		struct file_operations *module_fops;
		struct proc_dir_entry *module_proc;
		void *PrivateData;
};

typedef int (*read_proc_t)(struct file *file,  char __user *buffer,
                           size_t count, loff_t *offset);
typedef int (*write_proc_t)(struct file *file,  const char __user *buffer,
                           size_t count, loff_t *offset);				   
						   
struct proc_info  *AddProcEntry(struct proc_dir_entry *, char *, read_proc_t, write_proc_t,void *);
void RemoveProcEntry(struct proc_info *);

#else
struct proc_dir_entry *AddProcEntry(struct proc_dir_entry *moduledir,char *KeyName,
		read_proc_t *read_proc, write_proc_t *write_proc,void *PrivateData);
void RemoveProcEntry(struct proc_dir_entry *moduledir, char *KeyName);

#endif
struct ctl_table_header *AddSysctlTable(char *ModuleName,struct ctl_table* ModuleTable);

void RemoveSysctlTable(struct ctl_table_header *table_header);



/* /proc/ractrends directory entry structure */
extern struct proc_dir_entry *rootdir;

#endif
