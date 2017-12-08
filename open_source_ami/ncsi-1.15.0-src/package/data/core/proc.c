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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <net/net_namespace.h>
#include <linux/fs.h>
#include "helper.h"
#include "ncsi.h"

static int proc_setlink(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);  
static int proc_enable(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);  
static int proc_setvlanid(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);
static ssize_t proc_read( struct file *file, char __user *buffer, size_t len, loff_t *offset);
static ssize_t proc_write( struct file *file, const char __user *buffer, size_t len, loff_t *offset);

static struct proc_dir_entry   *proc_ncsi   = NULL;
static struct proc_dir_entry   *proc_ncsi_stats = NULL;
static struct ctl_table_header *sys_ncsi  = NULL;

#ifdef NCSI_DEBUG
int verbose = 0xFF; //SHOW_MESSAGES|SHOW_SUCCESS_COMMANDS;		// SHOW_MESSAGES|DUMP_BUFFER
#else
int verbose = 0;
#endif
int UserPackageID = 0;
int UserChannelID = 0;
char UserInterface[8] = "eth0";
int UserEnable = 0;
int UserVlanID=0;

int UserSpeed=0;
int UserDuplex=1;
int UserSetLink=0;

static struct ctl_table SysNcsiTable[] = 
{
	{CTL_UNNUMBERED, "DebugLevel", &verbose,       sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{CTL_UNNUMBERED, "PackageID",  &UserPackageID, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{CTL_UNNUMBERED, "ChannelID",  &UserChannelID, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{CTL_UNNUMBERED, "Interface",  &UserInterface, sizeof(UserInterface), 0644, NULL, NULL, &proc_dostring },
	{CTL_UNNUMBERED, "Enable"   ,  &UserEnable,    sizeof(int), 0644, NULL, NULL, &proc_enable },	
	{CTL_UNNUMBERED, "VlanID"	,  &UserVlanID,    sizeof(int), 0644, NULL, NULL,&proc_setvlanid },
	{CTL_UNNUMBERED, "UserSpeed"	,  &UserSpeed,    sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{CTL_UNNUMBERED, "UserDuplex"	,  &UserDuplex,   sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{CTL_UNNUMBERED, "UserSetLink"	,  &UserSetLink,   sizeof(int), 0644, NULL, NULL,&proc_setlink },
	{ 0 }
};


/* This is a function Called when something is written to sysctl "Emable " */
static
int 
proc_enable(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
{
	int retval;
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);

	if (UserEnable == 0)
		return retval;
	
	if (UserEnable != 1)
	{
		printk("NCSI: Sysctl \"Enable\" should be 1 or 0 \n");
		return retval;
	}

	/* UserEnable == 1 */
	SetUserSettings(0);
 	return retval;
} 

static
int 
proc_setlink(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
{
	int retval;
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);

	if (UserSetLink == 0)
		return retval;
	
	if (UserSetLink != 1)
	{
		printk("NCSI: Sysctl \"SetLink\" should be 1 or 0 \n");
		return retval;
	}

	if (UserSpeed != 0)			/* 0 = AutoNeg */
	{
		if ((UserDuplex != 0) && (UserDuplex != 1))
		{
			printk("NCSI: Sysctl \"Duplex\" should be 1 (Full) or 0  (Half)\n");
			return retval;
		}

		if ((UserSpeed != 10) && (UserSpeed != 100) && ( UserSpeed != 1000))
		{
			printk("NCSI: Sysctl \"Speed\" should be 10/100/1000 Mbps\n");
			return retval;
		}
	}

	SetUserLink();
 	return retval;
} 


/* This is a function Called when something is written to sysctl "VlanID " */
static
int
proc_setvlanid(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
{
	int retval;
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);

	/* Set VLAN Filter , Enable VLAN */
	SetUserSettings(UserVlanID);
 	return retval;
}

static struct file_operations proc_ops = 
{
	.read = proc_read,
	.write = proc_write
};

static ssize_t 
proc_read( struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
/*
	loff_t pos = *offset;
	struct proc_data *priv = (struct proc_data*)file->private_data;

	if (!priv->rbuffer)
		return -EINVAL;

	if (pos < 0)
		return -EINVAL;
	if (pos >= priv->readlen)
		return 0;
	if (len > priv->readlen - pos)
		len = priv->readlen - pos;
	if (copy_to_user(buffer, priv->rbuffer + pos, len))
		return -EFAULT;
	*offset = pos + len;
	return len;
*/
	printk("NCSI Statistics: Read : UnImplemented \n");
	return 0;
	
}

static ssize_t 
proc_write( struct file *file, const char __user *buffer, size_t len , loff_t *offset)
{
	/*
	loff_t pos = *offset;
	struct proc_data *priv = (struct proc_data*)file->private_data;

	if (!priv->wbuffer)
		return -EINVAL;

	if (pos < 0)
		return -EINVAL;
	if (pos >= priv->maxwritelen)
		return 0;
	if (len > priv->maxwritelen - pos)
		len = priv->maxwritelen - pos;
	if (copy_from_user(priv->wbuffer + pos, buffer, len))
		return -EFAULT;
	if ( pos + len > priv->writelen )
		priv->writelen = len + file->f_pos;
	*offset = pos + len;
	return len;
	*/
	printk("NCSI Statistics: Write : UnImplemented \n");
	return -EINVAL;
}


int 
CreateProcSys(void)
{
	proc_ncsi = proc_mkdir("ncsi",init_net.proc_net);
	if (proc_ncsi == NULL)
	{
		printk("NCSI: ERROR: Unable to /proc/net/ncsi \n");
		return 1;
	}
	proc_ncsi_stats =create_proc_entry("stats",0600,proc_ncsi);
	if (proc_ncsi_stats == NULL)
	{
		printk("NCSI: ERROR: Unable to /proc/net/ncsi/stats\n");
		remove_proc_entry("ncsi",init_net.proc_net);
		return 1;
	}
	proc_ncsi_stats->proc_fops = &proc_ops;
	
	sys_ncsi = AddSysctlTable("ncsi",&SysNcsiTable[0]);
	if (sys_ncsi == NULL)
	{
		printk("NCSI: ERROR: Unable to create /proc/sys/ractrends/ncsi \n");
		if (proc_ncsi_stats)
			remove_proc_entry("stats",proc_ncsi);
		if (proc_ncsi)
			remove_proc_entry("ncsi",init_net.proc_net);
	}
	return 0;
}


int
RemoveProcSys(void)
{
	if (sys_ncsi)
		RemoveSysctlTable(sys_ncsi);
	if (proc_ncsi_stats)
		remove_proc_entry("stats",proc_ncsi);
	if (proc_ncsi)
		remove_proc_entry("ncsi",init_net.proc_net);
	return 0;
}

