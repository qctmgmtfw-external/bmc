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
#ifndef AMI_HELPER_H
#define AMI_HELPER_H

#include <linux/sysctl.h>	
#include <linux/proc_fs.h>		

/* Name of directory under /proc and /proc/sys where all the modules 
 * put their proc entries and sysctl entries respectivly  */
#define PROC_DIR	"ractrends"	

/* Function prototypes for proc and sysctl entry addition and removal */
struct proc_dir_entry *AddProcEntry(struct proc_dir_entry *moduledir,char *KeyName,
		read_proc_t *read_proc, write_proc_t *write_proc,void *PrivateData);
struct ctl_table_header *AddSysctlTable(char *ModuleName,struct ctl_table* ModuleTable);

void RemoveProcEntry(struct proc_dir_entry *moduledir, char *KeyName);
void RemoveSysctlTable(struct ctl_table_header *table_header);

/* /proc/ractrends directory entry structure */
extern struct proc_dir_entry *rootdir;

#endif
