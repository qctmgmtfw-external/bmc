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

#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/kernel.h>	
#include <linux/module.h>
#include <linux/version.h>
#include "helper.h"

/* Define the symbols to be exported to outside world */
//EXPORT_SYMBOL(AddProcEntry);
//EXPORT_SYMBOL(RemoveProcEntry);
//EXPORT_SYMBOL(AddSysctlTable);
//EXPORT_SYMBOL(RemoveSysctlTable);

struct 
proc_dir_entry *
AddProcEntry(struct proc_dir_entry *moduledir ,char *KeyName,read_proc_t *read_proc, 
		write_proc_t *write_proc, void *PrivateData)
{
	struct proc_dir_entry *res;
	mode_t mode = S_IFREG;

	/* Check Module Dir  is present and atleast one proc function is present*/ 
	if (!moduledir)
		return NULL;
	if ((!read_proc) && (!write_proc))
		return NULL;
	
	/* Set Mode depending upon the presence of read and write proc functions */	
	if (read_proc)
		mode|= S_IRUGO;
	if (write_proc)
		mode|= S_IWUGO;

	res = create_proc_entry(KeyName,mode,moduledir);
	if (!res)
		return NULL;
	
	res->data 	= PrivateData;
	res->read_proc 	= read_proc;
	res->write_proc	= write_proc;

	return res;	
}

void
RemoveProcEntry(struct proc_dir_entry *moduledir, char *KeyName)
{
	if (KeyName)
		remove_proc_entry(KeyName,moduledir);
	return;
}

struct ctl_table_header *
AddSysctlTable(char *ModuleName,struct ctl_table* ModuleTable)
{
	struct ctl_table *root_table;
	struct ctl_table *module_root_table;
	struct ctl_table_header *table_header;

	/* Create the root directory under /proc/sys*/
	root_table = kmalloc(sizeof(ctl_table)*2,GFP_KERNEL);
	if (!root_table)
		return NULL;

	/* Create the module directory under /proc/sys/ractrends*/
	module_root_table = kmalloc(sizeof(ctl_table)*2,GFP_KERNEL);
	if (!module_root_table)
	{
		kfree(root_table);
		return NULL;
	}
	
	/* Fill up root table */	
	memset(root_table,0,sizeof(ctl_table)*2);
	root_table[1].ctl_name  = 0;			/* Terminate Structure */
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24))
	root_table[0].ctl_name	= CTL_UNNUMBERED;
#else
	root_table[0].ctl_name  = 5000;
#endif
	root_table[0].procname  = PROC_DIR;
	root_table[0].data		= NULL;
	root_table[0].maxlen 	= 0;
	root_table[0].mode		= 0555;		/* _r_xr_xr_x */
	root_table[0].child 	= module_root_table;

	/* Fill up the module root table */
	memset(module_root_table,0,sizeof(ctl_table)*2);
	module_root_table[1].ctl_name = 0;	  /* Terminate Structure */	
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24))
	module_root_table[0].ctl_name = CTL_UNNUMBERED;		/* What happens when another module registers */
#else
	module_root_table[0].ctl_name = 1;
#endif
	module_root_table[0].procname = ModuleName;
	module_root_table[0].data		= NULL;
	module_root_table[0].maxlen 	= 0;
	module_root_table[0].mode		= 0555;		/* _r_xr_xr_x */
	module_root_table[0].child 	= ModuleTable;

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24))
	table_header = register_sysctl_table(root_table);
#else
	table_header = register_sysctl_table(root_table, 1);
#endif
	
	return table_header;

}

void
RemoveSysctlTable(struct ctl_table_header *table_header)
{
	struct ctl_table *root_table;

	if (!table_header)
		return;
			
	/* Hack: Get the root_table from table_header : Refer sysctl.c */
	root_table = table_header->ctl_table;
		
	/* unregister the sysctl table from kernel */		
	unregister_sysctl_table(table_header);

	if (!root_table)		
		return;
	
	/* free module root table */
	if (root_table->child)
			kfree(root_table->child);

	/* free the root table */
	kfree(root_table);
	return;
}



