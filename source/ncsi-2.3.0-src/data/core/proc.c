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
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <net/net_namespace.h>
#include <linux/fs.h>
#include "helper.h"
#include "ncsi.h"

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
static int proc_setlink(ctl_table *ctl,int write,void *buffer,size_t *lenp,loff_t* pos);  
static int proc_enable(ctl_table *ctl,int write,void *buffer,size_t *lenp,loff_t* pos);  
void ncsi_init_detect_ncsi_fg(char *devname);
static int proc_ncsi_init(ctl_table *ctl,int write,void *buffer,size_t *lenp,loff_t* pos);  
static int proc_setvlanid(ctl_table *ctl,int write,void *buffer,size_t *lenp,loff_t* pos);
static int proc_vetobit(ctl_table *ctl,int write,void *buffer,size_t *lenp,loff_t* pos);
static int proc_usercommand(ctl_table *ctl,int write,void *buffer,size_t *lenp,loff_t* pos);
static int proc_userresponse(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos);
static int proc_detect(ctl_table *ctl,int write,void *buffer,size_t *lenp,loff_t* pos);
//Quanta+++
static int proc_detect_enable_ncsi(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos);
static int proc_ncsi_state_machine(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos);
static int proc_ncsi_info_changed_ncs_gpio(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos);
//Quanta+++
#else
static int proc_setlink(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);  
static int proc_enable(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);  
static int proc_setvlanid(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);
static int proc_vetobit(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);
static int proc_usercommand(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);
static int proc_userresponse(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
							loff_t* pos);
static int proc_detect(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp,
                            loff_t* pos);
//Quanta+++
static int proc_detect_enable_ncsi(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos);
static int proc_ncsi_state_machine(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos);
static int proc_ncsi_info_changed_ncs_gpio(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos);
//Quanta+++
#endif

#if 1 //Quanta
static ssize_t proc_read( struct file *file, char __user *buffer, size_t len, loff_t *offset);
static ssize_t proc_write( struct file *file, const char __user *buffer, size_t len, loff_t *offset);

static struct proc_dir_entry   *proc_ncsi   = NULL;
static struct proc_dir_entry   *proc_ncsi_stats = NULL;
#endif
static struct ctl_table_header *sys_ncsi  = NULL;

// Quanta +++
static DECLARE_WAIT_QUEUE_HEAD(detected_enabled_event);
static int detected_enabled_wakeup = NCSI_NIC_SLEEP;
static int ncs_event_id = 0;
static DEFINE_SPINLOCK(spin_lock_ncsi_sta);
int NCSISignalID = 0;
int NCSIStateMachine = 0;
int InformChangedGPIO = 0;
// Quanta +++

#ifdef NCSI_DEBUG
int verbose = 0xFF; //SHOW_MESSAGES|SHOW_SUCCESS_COMMANDS;		// SHOW_MESSAGES|DUMP_BUFFER
#else
int verbose = 0;
#endif
int UserPackageID = 0;
int UserChannelID = 0;
char UserInterface[8] = CONFIG_SPX_FEATURE_NCSI_DEFAULT_INTERFACE;
int UserEnable = 0;
int UserInit = 0;

int UserVlanID=0;

int UserSpeed=0;
int UserDuplex=1;
int UserSetLink=0;
int UserAutoNeg=0;
int UserDetect=0;

#ifdef CONFIG_SPX_FEATURE_NCSI_KEEP_PHY_LINK_UP
int UserVetoBit=1;
#else
int UserVetoBit=0;
#endif
int UserControlVetoBit=0;

#ifdef CONFIG_SPX_FEATURE_NCSI_AUTO_FAILOVER
int UserAuto=1;
#else
int UserAuto=0;
#endif

unsigned int UserCommand[NCSI_COMMAND_SIZE] = {0};
unsigned int UserLastResponse[NCSI_COMMAND_SIZE] = {0};

static struct ctl_table SysNcsiTable[] = 
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
    {"DebugLevel", &verbose,       sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"PackageID",  &UserPackageID, sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"ChannelID",  &UserChannelID, sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"Interface",  &UserInterface, sizeof(UserInterface), 0644, NULL, &proc_dostring, NULL },
    {"Enable"   ,  &UserEnable,    sizeof(int), 0644, NULL, &proc_enable, NULL },
    {"Init"   ,  &UserInit,    sizeof(int), 0644, NULL, &proc_ncsi_init, NULL },
    {"VlanID"   ,  &UserVlanID,    sizeof(int), 0644, NULL, &proc_setvlanid, NULL },
    {"UserSpeed"    ,  &UserSpeed,    sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"UserDuplex"   ,  &UserDuplex,   sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"UserAutoNeg"   ,  &UserAutoNeg,   sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"UserSetLink"  ,  &UserSetLink,   sizeof(int), 0644, NULL, &proc_setlink, NULL },
    {"FailOver",  &UserAuto, sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"VetoBit",  &UserVetoBit, sizeof(int), 0644, NULL, &proc_dointvec, NULL },
    {"ControlVetoBit"   ,  &UserControlVetoBit,    sizeof(int), 0644, NULL, &proc_vetobit, NULL },
    {"UserCommand"   ,  UserCommand,    NCSI_COMMAND_SIZE, 0644, NULL, &proc_usercommand, NULL },
    {"UserLastResponse"   ,  UserLastResponse,    NCSI_COMMAND_SIZE, 0644, NULL, &proc_userresponse, NULL },
	{"Detect"   ,  &UserDetect,    sizeof(int), 0644, NULL, &proc_detect, NULL },
	{"DetectEnableCh"	,  &NCSISignalID, sizeof(int), 0644, NULL, &proc_detect_enable_ncsi, NULL },              // Quanta
	{"NCSIStateMachine"	,  &NCSIStateMachine, sizeof(int), 0644, NULL, &proc_ncsi_state_machine, NULL },          // Quanta
	{"InformChangedGPIO",  &InformChangedGPIO, sizeof(int), 0644, NULL, &proc_ncsi_info_changed_ncs_gpio, NULL }, // Quanta
    { 0 }
#else	
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0))
	{"DebugLevel", &verbose,       sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{"PackageID",  &UserPackageID, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{"ChannelID",  &UserChannelID, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{"Interface",  &UserInterface, sizeof(UserInterface), 0644, NULL, NULL, &proc_dostring },
	{"Enable"   ,  &UserEnable,    sizeof(int), 0644, NULL, NULL, &proc_enable },	
	{"VlanID"	,  &UserVlanID,    sizeof(int), 0644, NULL, NULL,&proc_setvlanid },
	{"UserSpeed"	,  &UserSpeed,    sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{"UserDuplex"	,  &UserDuplex,   sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{"UserAutoNeg"   ,  &UserAutoNeg,   sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{"UserSetLink"	,  &UserSetLink,   sizeof(int), 0644, NULL, NULL,&proc_setlink },
	{"FailOver",  &UserAuto, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{"VetoBit",  &UserVetoBit, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{"ControlVetoBit"   ,  &UserControlVetoBit,    sizeof(int), 0644, NULL, NULL, &proc_usercontrolvetobit },
	{"UserCommand"   ,  UserCommand,    NCSI_COMMAND_SIZE, 0644, NULL, NULL, &proc_usercommand },
	{"UserLastResponse"   ,  UserLastResponse,    NCSI_COMMAND_SIZE, 0644, NULL, NULL, &proc_userresponse },
	{"Detect"   ,  &UserDetect,    sizeof(int), 0644, NULL, NULL, &proc_detect },
	{"DetectEnableCh"	,  &NCSISignalID, sizeof(int), 0644, NULL, NULL, &proc_detect_enable_ncsi },              // Quanta
	{"NCSIStateMachine"	,  &NCSIStateMachine, sizeof(int), 0644, NULL, NULL, &proc_ncsi_state_machine },          // Quanta
	{"InformChangedGPIO",  &InformChangedGPIO, sizeof(int), 0644, NULL, NULL, &proc_ncsi_info_changed_ncs_gpio }, // Quanta
	{ 0 }
#else
	{CTL_UNNUMBERED, "DebugLevel", &verbose,       sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{CTL_UNNUMBERED, "PackageID",  &UserPackageID, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{CTL_UNNUMBERED, "ChannelID",  &UserChannelID, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{CTL_UNNUMBERED, "Interface",  &UserInterface, sizeof(UserInterface), 0644, NULL, NULL, &proc_dostring },
	{CTL_UNNUMBERED, "Enable"   ,  &UserEnable,    sizeof(int), 0644, NULL, NULL, &proc_enable },	
	{CTL_UNNUMBERED, "VlanID"	,  &UserVlanID,    sizeof(int), 0644, NULL, NULL,&proc_setvlanid },
	{CTL_UNNUMBERED, "UserSpeed"	,  &UserSpeed,    sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{CTL_UNNUMBERED, "UserDuplex"	,  &UserDuplex,   sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{CTL_UNNUMBERED, "UserAutoNeg"   ,  &UserAutoNeg,   sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{CTL_UNNUMBERED, "UserSetLink"	,  &UserSetLink,   sizeof(int), 0644, NULL, NULL,&proc_setlink },
	{CTL_UNNUMBERED, "FailOver",  &UserAuto, sizeof(int), 0644, NULL, NULL, &proc_dointvec },
	{CTL_UNNUMBERED, "VetoBit"	,  &UserVetoBit,   sizeof(int), 0644, NULL, NULL,&proc_dointvec },
	{CTL_UNNUMBERED, "ControlVetoBit"   ,  &UserControlVetoBit,    sizeof(int), 0644, NULL, NULL, &proc_vetobit },
	{CTL_UNNUMBERED, "UserCommand"   ,  UserCommand,    NCSI_COMMAND_SIZE, 0644, NULL, NULL, &proc_usercommand },
	{CTL_UNNUMBERED, "UserLastResponse"   ,  UserLastResponse,    NCSI_COMMAND_SIZE, 0644, NULL, NULL, &proc_userresponse },
	{CTL_UNNUMBERED, "Detect"   ,  &UserDetect,    sizeof(int), 0644, NULL, NULL, &proc_detect },
	{CTL_UNNUMBERED, "DetectEnableCh"	,  &NCSISignalID, sizeof(int), 0644, NULL, NULL, &proc_detect_enable_ncsi },              // Quanta
	{CTL_UNNUMBERED, "NCSIStateMachine"	,  &NCSIStateMachine, sizeof(int), 0644, NULL, NULL, &proc_ncsi_state_machine },          // Quanta
	{CTL_UNNUMBERED, "InformChangedGPIO",  &InformChangedGPIO, sizeof(int), 0644, NULL, NULL, &proc_ncsi_info_changed_ncs_gpio }, // Quanta
	{ 0 }
#endif
#endif
};

/* 
 * This is a function Called when something is written to sysctl "UserCommand"
 * Note : First byte is the command number and followed by number of data bytes(n) and n bytes of data.
 * */
static
int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_usercommand(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_usercommand(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int retval = 0;
	
	if (UserCommand[1] != 0)		// Already given command is in progress.
		return retval;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))	
	retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif
	
	UserCommand[1]++;	// Increment for command.
	SendUserCommand();
 	return retval;
} 


/* 
 * This is a function Called when something is written to sysctl "UserCommand"
 * Note : First byte is the length(n) and followed by n bytes for command.
 * */
static
int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_userresponse(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_userresponse(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	static char read_flag = 0;
	int retval = 0, length = *lenp, i;
	
	*lenp = 0;
	if (write)
		return retval;

	if (read_flag)
	{
		read_flag = 0;
		memset (UserLastResponse, 0, NCSI_COMMAND_SIZE);
		return retval;
	}

	if (UserLastResponse[1])
	{
		(*lenp) = snprintf (buffer, length, "Command : %02X, Response Length : %02X\n", UserLastResponse[0], UserLastResponse[1]);
		read_flag = 1;
	}
	for (i = 2; i < UserLastResponse[1] + 2; i++)
	{
		if (((i - 1) % 4) == 0)
			(*lenp) += snprintf (&((char *)buffer)[(*lenp)], length, "%02X\n", UserLastResponse[i]);
		else
			(*lenp) += snprintf (&((char *)buffer)[(*lenp)], length, "%02X ", UserLastResponse[i]);
	}
	if (((i - 2) % 4) != 0)
		(*lenp) += snprintf (&((char *)buffer)[(*lenp)], length, "\n");
	
 	return retval;
} 



static
int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_ncsi_init(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_ncsi_init(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int retval;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))	
	retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif

	
	if (UserInit != 1)
	{
		printk( "NCSI: Sysctl \"UserInit\" should be 1 or 0 \n");
		return retval;
	}
	
	
    ncsi_init_detect_ncsi_fg("eth0");

 	return retval;
} 

/* This is a function Called when something is written to sysctl "Enable " */
static
int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_enable(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_enable(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int retval;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))	
	retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif

	if (UserEnable == 0)
		return retval;
	
	if (UserEnable != 1)
	{
		printk( "NCSI: Sysctl \"Enable\" should be 1 or 0 \n");
		return retval;
	}
	
	if( UserAuto != 0 && UserAuto != 1 )
	{
		printk( "NCSI: Sysctl \"Auto\" should be 1 or 0 \n");
		return retval;
	}
	
	/* UserEnable == 1 */
	SetUserSettings(0);
 	return retval;
} 

static
int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_setlink(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_setlink(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int retval;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))	
	retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif	

#ifdef CONFIG_SPX_FEATURE_NCSI_FORCE_LAN_SPEED_10G
	printk( "NCSI: Sysctl \"SetLink\" is not supported on 10G mode\n");
	return retval;
#else

	if (UserSetLink == 0)
		return retval;
	
	if (UserSetLink != 1)
	{
		printk( "NCSI: Sysctl \"SetLink\" should be 1 or 0 \n");
		return retval;
	}

	if (UserSpeed != 0)			/* 0 = Enable AutoNeg */
	{
		if ((UserDuplex != 0) && (UserDuplex != 1))
		{
			printk( "NCSI: Sysctl \"Duplex\" should be 1 (Full) or 0  (Half)\n");
			return retval;
		}

		if ((UserSpeed != 10) && (UserSpeed != 100) && ( UserSpeed != 1000))
		{
			printk( "NCSI: Sysctl \"Speed\" should be 10/100/1000 Mbps\n");
			return retval;
		}
		
		if ((UserAutoNeg != 0) && (UserAutoNeg != 1))
		{
			printk( "NCSI: Sysctl \"AutoNeg\" should be 1 (Enable) or 0  (Disable)\n");
			return retval;
		}
	}

	SetUserLink();
 	return retval;
#endif	
} 

/* This is a function Called when something is written to sysctl "Detect " */
static
int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_detect(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_detect(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
    int retval;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))  
    retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
    retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif

    if (UserDetect == 0)
        return retval;
    
    if (UserDetect != 1)
    {
        printk("NCSI: Sysctl \"Detect\" should be 1 or 0 \n");
        return retval;
    }

#ifdef CONFIG_SPX_FEATURE_NCSI_MANUAL_DETECT
    /* UserDetect == 1 */
    SetUserDetect();
#endif
    return retval;
} 


/* This is a function Called when something is written to sysctl "VlanID " */
static
int
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_setvlanid(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_setvlanid(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int retval;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))	
	retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif	

	/* Set VLAN Filter , Enable VLAN */
	SetUserSettings(UserVlanID);
 	return retval;
}

#if 1 //Quanta
static struct file_operations proc_ops = 
{
	.read = proc_read,
	.write = proc_write
};
#endif


/* This is a function Called when something is written to sysctl "Detect " */
static
int 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_vetobit(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_vetobit(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int retval;
#if (LINUX_VERSION_CODE >KERNEL_VERSION(3,4,11))	
	retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif

	if (UserControlVetoBit== 0)
		return retval;
	
	if (UserControlVetoBit!= 1)
	{
		printk( "NCSI: Sysctl \"ControlVetoBit\" should be 1 or 0 \n");
		return retval;
	}
	
/*	if( UserAuto != 0 && UserAuto != 1 )
	{
		printk("NCSI: Sysctl \"Auto\" should be 1 or 0 \n");
		return retval;
	}
*/	
	/* UserControlVetoBit== 1 */
	SetUserVetoBit();
 	return retval;
} 

// Quanta+++
static
int
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_detect_enable_ncsi(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_detect_enable_ncsi(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int retval;
#if (LINUX_VERSION_CODE >KERNEL_VERSION(3,4,11))	
	retval = proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	retval = proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif
	
	if (NCSISignalID != EVENT_ID_DO_ALL)
	{
		printk("NCSI: Sysctl \"DetectEnableCh\" should be 2\n");
		return retval;
	}
	
	ClearNCSIstaMachine();
	SendWakeEvent(NCSISignalID);
	
	return retval;
}

static
int
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_ncsi_state_machine(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_ncsi_state_machine(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int  retval;
	retval = GetNCSIstaMachine();
#if (LINUX_VERSION_CODE >KERNEL_VERSION(3,4,11))	
	proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif
	return retval;
}

static
int
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
proc_ncsi_info_changed_ncs_gpio(ctl_table *ctl,int write,void *buffer,size_t *lenp, loff_t *pos)
#else
proc_ncsi_info_changed_ncs_gpio(ctl_table *ctl,int write,struct file *filep,void *buffer,size_t *lenp, loff_t *pos)
#endif
{
	int  retval ;
	retval = UpdateNICSelectionSta(InformChangedGPIO);
#if (LINUX_VERSION_CODE >KERNEL_VERSION(3,4,11))	
	proc_dointvec(ctl,write,buffer,lenp,pos);
#else
	proc_dointvec(ctl,write,filep,buffer,lenp,pos);
#endif
	return retval;
}
// Quanta+++

#if 1 //Quanta
static ssize_t 
proc_read( struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
	unsigned long ncsi_irq_flag;  // Quanta
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

	unsigned char ncs_state = 0;
	
	spin_lock_irqsave(&spin_lock_ncsi_sta, ncsi_irq_flag);
	
	ncs_state = ncs_event_id;
	
	spin_unlock_irqrestore(&spin_lock_ncsi_sta, ncsi_irq_flag);
	
	if (copy_to_user(buffer, &ncs_state, 1))
		return -EFAULT;
	
	return 0;	
}

static ssize_t 
proc_write( struct file *file, const char __user *buffer, size_t len , loff_t *offset)
{
	unsigned long ncsi_irq_flag;  // Quanta
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
	//printk("NCSI Statistics: Write : UnImplemented \n");
	//return -EINVAL;
	
	// Quanta:+++
	wait_event_interruptible(detected_enabled_event, (detected_enabled_wakeup == NCSI_NIC_WAKE_UP));

	detected_enabled_wakeup = NCSI_NIC_SLEEP;
	
	if ((ncs_event_id & EVENT_ID_DO_ALL) == EVENT_ID_DO_ALL) {
		DetectedChannel();
		EnabledChannel();
		
		spin_lock_irqsave(&spin_lock_ncsi_sta, ncsi_irq_flag);
	 	
		ncs_event_id &= ~EVENT_ID_DO_ALL;
		
		spin_unlock_irqrestore(&spin_lock_ncsi_sta, ncsi_irq_flag);
		
		InformChangedGPIO = 0;
		
		return EVENT_ID_DO_ALL;
	}
	else if (((ncs_event_id & EVENT_ID_DO_ENABLE) == EVENT_ID_DO_ENABLE) && (InformChangedGPIO == 0)) {
		EnabledChannel();
		
		spin_lock_irqsave(&spin_lock_ncsi_sta, ncsi_irq_flag);
	 	
		ncs_event_id &= ~EVENT_ID_DO_ENABLE;
		
		spin_unlock_irqrestore(&spin_lock_ncsi_sta, ncsi_irq_flag);
		
		return EVENT_ID_DO_ENABLE;
	}
	else if (((ncs_event_id & EVENT_ID_DO_AEN) == EVENT_ID_DO_AEN) && (InformChangedGPIO == 0)) {
#ifdef CONFIG_SPX_FEATURE_NCSI_AUTO_FAILOVER
		ReceiveAEN();
		CheckNcsiLinkStatus();
#endif
		spin_lock_irqsave(&spin_lock_ncsi_sta, ncsi_irq_flag);
	 	
		ncs_event_id &= ~EVENT_ID_DO_AEN;
		
		spin_unlock_irqrestore(&spin_lock_ncsi_sta, ncsi_irq_flag);
		
		return EVENT_ID_DO_AEN;
	}
	
	return 0;
	// Quanta:
}
#endif

int 
CreateProcSys(void)
{
#if 1 //Quanta
	proc_ncsi = proc_mkdir("ncsi",init_net.proc_net);
	if (proc_ncsi == NULL)
	{
		printk("NCSI: ERROR: Unable to /proc/net/ncsi \n");
		return 1;
	}
	
	//Quanta:+++
	//proc_ncsi_stats =create_proc_entry("stats",0600,proc_ncsi);
	proc_ncsi_stats = proc_create("stats", 0, proc_ncsi, &proc_ops);
	//Quanta:+++
	if (proc_ncsi_stats == NULL)
	{
		printk("NCSI: ERROR: Unable to /proc/net/ncsi/stats\n");
		remove_proc_entry("ncsi",init_net.proc_net);
		return 1;
	}
	
	//proc_ncsi_stats->proc_fops = &proc_ops; 	//Quanta:+++
#endif
	
	sys_ncsi = AddSysctlTable("ncsi",&SysNcsiTable[0]);
	if (sys_ncsi == NULL)
	{
		printk("NCSI: ERROR: Unable to create /proc/sys/ractrends/ncsi \n");
#if 1 //Quanta
		if (proc_ncsi_stats)
			remove_proc_entry("stats",proc_ncsi);
		if (proc_ncsi)
			remove_proc_entry("ncsi",init_net.proc_net);
#endif
	}
	return 0;
}


int
RemoveProcSys(void)
{
	if (sys_ncsi)
		RemoveSysctlTable(sys_ncsi);
#if 1 //Quanta
	if (proc_ncsi_stats)
		remove_proc_entry("stats",proc_ncsi);
	if (proc_ncsi)
		remove_proc_entry("ncsi",init_net.proc_net);
#endif
	return 0;
}

// Quanta:+++
void SendWakeEvent(int eventid)
{
	unsigned long ncsi_irq_flag;  // Quanta
	
	if (((eventid == EVENT_ID_DO_ENABLE) || (eventid == EVENT_ID_DO_AEN)) && (InformChangedGPIO != 0)) {
		return;
	}
	
	spin_lock_irqsave(&spin_lock_ncsi_sta, ncsi_irq_flag);
 	
	ncs_event_id |= eventid;
	
	spin_unlock_irqrestore(&spin_lock_ncsi_sta, ncsi_irq_flag);
	
	detected_enabled_wakeup = NCSI_NIC_WAKE_UP;
	wake_up_interruptible(&detected_enabled_event);
}
// Quanta:
