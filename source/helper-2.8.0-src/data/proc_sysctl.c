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

#include <linux/kernel.h>	
#include <linux/module.h>
#include <linux/version.h>
#include <linux/slab.h>
#include "helper.h"

/* Define the symbols to be exported to outside world */
EXPORT_SYMBOL(AddProcEntry);
EXPORT_SYMBOL(RemoveProcEntry);
EXPORT_SYMBOL(AddSysctlTable);
EXPORT_SYMBOL(RemoveSysctlTable);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))

struct proc_info  *
AddProcEntry(struct proc_dir_entry *moduledir ,char *KeyName,read_proc_t read_proc, 
		write_proc_t write_proc, void *PrivateData)
{
	mode_t mode = S_IFREG;
	struct proc_info *procinfo;

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

	/* Allocate memory */
	procinfo  = kmalloc(sizeof (struct proc_info), GFP_KERNEL);
	if (procinfo == NULL)
	{	
		return NULL;
	}	
	procinfo->module_fops = kmalloc(sizeof (struct file_operations),GFP_KERNEL);
	if (procinfo->module_fops == NULL)
	{
		kfree(procinfo);
		return NULL;
	}
	memset(procinfo->module_fops,0,sizeof(struct file_operations));


	procinfo->PrivateData = PrivateData;

	/* Create proc entry */
	procinfo->module_fops->read = read_proc;
	procinfo->module_fops->write = write_proc;
	procinfo->module_proc = proc_create(KeyName,mode,moduledir,procinfo->module_fops);
	if (!procinfo->module_proc)
	{
		kfree(procinfo->module_fops);
		kfree(procinfo);
		return NULL;
	}


	return procinfo;
}

void
RemoveProcEntry(struct proc_info *procinfo)
{
	if (!procinfo)
		return;

	proc_remove(procinfo->module_proc);
	kfree(procinfo->module_fops);
	kfree(procinfo);
	return;
}

#else

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
#endif


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
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	root_table[1].ctl_name  = 0;			/* Terminate Structure */
	root_table[0].ctl_name	= CTL_UNNUMBERED;
#endif
	root_table[0].procname  = PROC_DIR;
	root_table[0].data		= NULL;
	root_table[0].maxlen 	= 0;
	root_table[0].mode		= 0555;		/* _r_xr_xr_x */
	root_table[0].child 	= module_root_table;

	/* Fill up the module root table */
	memset(module_root_table,0,sizeof(ctl_table)*2);
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))	
	module_root_table[1].ctl_name = 0;	  /* Terminate Structure */	
	module_root_table[0].ctl_name = CTL_UNNUMBERED;		/* What happens when another module registers */
#endif
	module_root_table[0].procname = ModuleName;
	module_root_table[0].data		= NULL;
	module_root_table[0].maxlen 	= 0;
	module_root_table[0].mode		= 0555;		/* _r_xr_xr_x */
	module_root_table[0].child 	= ModuleTable;


	table_header = register_sysctl_table(root_table);

	return table_header;

}

void
RemoveSysctlTable(struct ctl_table_header *table_header)
{
	struct ctl_table *root_table;

	if (!table_header)
		return;
			
	/* Hack: Get the root_table from table_header : Refer sysctl.c */
	root_table = table_header->ctl_table_arg;
		
	/* unregister the sysctl table from kernel */		
	unregister_sysctl_table(table_header);

	if (!root_table)		
		return;
	
	/* free module root table */
	if (root_table->child)
			kfree(root_table->child);

	/* free the root table */
	if(root_table)
		kfree(root_table);
	return;
}



