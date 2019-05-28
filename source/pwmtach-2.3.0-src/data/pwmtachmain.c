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

/*
 * PwmTach Common Driver
 *
 * Copyright (C) 2009-2015 American Megatrends Inc.
 *
 * This driver provides common layer, independent of the hardware, for the PWMTACH driver.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "helper.h"
#include "dbgout.h"
#include "pwmtach.h"
#include "fan_structs.h"
#include "pwmtach_ioctl.h"
#include "driver_hal.h"

MODULE_AUTHOR("American Megatrends Inc");
MODULE_DESCRIPTION("PwmTach Common driver");
MODULE_LICENSE("GPL");


#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
	#define USE_UNLOCKED_IOCTL  
  #endif  
#endif 

#define PWMTACH_MAJOR		46
#define PWMTACH_MINOR		0
#define PWMTACH_DEV_NAME	"pwmtach"

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)       
#endif

#ifdef USE_UNLOCKED_IOCTL 
static long pwmtach_ioctlUnlocked (struct file *file, uint cmd, ulong arg);
#else
static int pwmtach_ioctl (struct inode *inode, struct file *file, uint cmd, ulong arg);
#endif

static int pwmtach_release (struct inode *inode, struct file *file);
static int pwmtach_open (struct inode *inode, struct file *file);

static struct file_operations pwmtach_fops = {
	owner:  	THIS_MODULE,
#ifdef USE_UNLOCKED_IOCTL 
	unlocked_ioctl:	pwmtach_ioctlUnlocked, 
#else 
	ioctl:      pwmtach_ioctl,
#endif
	open:		pwmtach_open,
	release:	pwmtach_release,
};

struct pwmtach_dev
{
	struct pwmtach_hal* ppwmtach_hal;
	unsigned int device_num;
};

static int get_pwm_number (struct pwmtach_dev* pdev, unsigned char fan_number);
static int get_tach_number (struct pwmtach_dev* pdev, unsigned char fan_number);
static int get_tachvalue (struct pwmtach_dev* pdev, unsigned int tachnumber, unsigned int* rpmvalue);
static int configure_fanmap_table (struct pwmtach_dev* pdev, pwmtach_data_t* in_data);
static int configure_fanproperty_table (struct pwmtach_dev* pdev, pwmtach_data_t* in_data);
static int show_fanmap_table (struct pwmtach_dev* pdev);
static int show_fanproperty_table (struct pwmtach_dev* pdev);
static int init_pwmtach (struct pwmtach_dev* pdev, pwmtach_data_t* in_data);
static int get_pwmvalue (struct pwmtach_dev* pdev, unsigned int pwm_number, unsigned char* dutycycle);

static int g_dev_id = 0;
static struct cdev *pwmtach_cdev;
static dev_t pwmtach_devno = MKDEV(PWMTACH_MAJOR, PWMTACH_MINOR);
static char banner[] __initdata = KERN_INFO "PWMTACH Common Driver, (c) 2009-2015 American Megatrends Inc.\n";


/***********************************************************************************************/

static void __exit
pwmtach_exit (void)
{
        unregister_core_hal_module (EDEV_TYPE_PWMTACH);
	unregister_chrdev_region (pwmtach_devno, MAX_PWMTACH_DEVICES);

	if (NULL != pwmtach_cdev)
	{
		dbgprint ("pwmtach char device del\n");
		cdev_del (pwmtach_cdev);
	}

	printk ( "PWMTACH Driver is unloaded sucessfully.\n");
	return;
}

int
register_pwmtach_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct pwmtach_hal *ppwmtach_hal;

	ppwmtach_hal = (struct pwmtach_hal*) kmalloc (sizeof(struct pwmtach_hal), GFP_KERNEL);
	if (!ppwmtach_hal)
        {
                return -ENOMEM;
        }

	ppwmtach_hal->fan_map_table = (struct fan_map_entry_t*) kmalloc ( num_instances * sizeof(struct fan_map_entry_t), GFP_KERNEL);
	if (!ppwmtach_hal->fan_map_table)
	{
		kfree(ppwmtach_hal);
		return -ENOMEM;
	}

	ppwmtach_hal->fan_property_table = (struct fan_property_t*) kmalloc ( num_instances * sizeof(struct fan_property_t), GFP_KERNEL); 
	if (!ppwmtach_hal->fan_property_table)
	{
		kfree(ppwmtach_hal->fan_map_table);
		kfree(ppwmtach_hal);
		return -ENOMEM;
	}

	ppwmtach_hal->dev_id = g_dev_id;
	g_dev_id++;
	ppwmtach_hal->num_fans = num_instances;
	ppwmtach_hal->num_pwms = 0;
	ppwmtach_hal->num_tachs = 0;	
	ppwmtach_hal->ppwmtach_hal_ops = (pwmtach_hal_operations_t *) phal_ops;

	*phw_data = (void *) ppwmtach_hal;
        dbgprint ("private data: %p, %p\n", ppwmtach_hal, *phw_data);
	return 0;
}

int unregister_pwmtach_hal_module (void *phw_data)
{
	struct pwmtach_hal *ppwmtach_hal = (struct pwmtach_hal *)phw_data;
        dbgprint ("unregister hal addr : %p\n", ppwmtach_hal);
	kfree (ppwmtach_hal->fan_property_table);
	kfree (ppwmtach_hal->fan_map_table);
	kfree (ppwmtach_hal);
	return 0;
}


static int
pwmtach_open (struct inode *inode, struct file *file)
{
	struct pwmtach_hal *ppwmtach_hal;
    unsigned int minor = iminor(inode);
	struct pwmtach_dev* pdev;
    hw_info_t pwmtach_hw_info;
    unsigned char open_count;
    int ret;

    ret = hw_open (EDEV_TYPE_PWMTACH, minor, &open_count, &pwmtach_hw_info);
    if (ret)
       return -ENXIO;
	
	ppwmtach_hal = pwmtach_hw_info.pdrv_data;
	
	pdev = (struct pwmtach_dev*) kmalloc (sizeof(struct pwmtach_dev), GFP_KERNEL);
	if (!pdev)
    {
		hw_close (EDEV_TYPE_PWMTACH, minor, &open_count);
        printk (KERN_ERR "%s: failed to allocate pwmtach private dev structure for pwmtach iminor: %d\n", PWMTACH_DEV_NAME, minor);
        return -ENOMEM;
    }


	pdev->ppwmtach_hal = ppwmtach_hal;
	//pdev->device_num = pwmtach_hw_info.ch_num;
	file->private_data = pdev;

	return nonseekable_open (inode, file);
}


static int
pwmtach_release (struct inode *inode, struct file *file)
{
	int ret;
	unsigned char open_count;
	struct pwmtach_dev* pdev = (struct pwmtach_dev*) file->private_data;
	
	pdev->ppwmtach_hal = NULL;
	ret = hw_close (EDEV_TYPE_PWMTACH, iminor(inode), &open_count);
	if(ret) { return -1; }
	kfree (pdev);
	return 0;
}

static long
pwmtach_ioctlUnlocked (struct file *file, uint cmd, ulong arg)
{
	pwmtach_data_t in_data;
	//struct pwmtach_dev * pdev = (struct pwmtach_dev*) file->private_data;
	unsigned int pwmnumber = 0;
	unsigned int tachnumber = 0;
	struct pwmtach_hal * ppwmtach_hal;
	struct pwmtach_dev dev;
	struct pwmtach_dev * pdev = &dev;
	
	if ( copy_from_user((void*)&in_data, (void*)arg, sizeof(pwmtach_data_t)) )
		return -EFAULT;

	ppwmtach_hal  = (struct pwmtach_hal *) hw_intr (EDEV_TYPE_PWMTACH, in_data.dev_id);
	pdev->ppwmtach_hal = ppwmtach_hal;

	if (ppwmtach_hal == NULL)	printk("returned HAL pointer is NULL\n");
	if ((cmd != CONFIGURE_FANMAP_TABLE) && (ppwmtach_hal->fan_map_table == NULL))
	{
		printk("Fan Map Table not configured for device.\n");
		return -1;
	}
	
	switch (cmd)
	{
/***Set/Read pwm/tach by channel basis as well as set pwm dutycycle instead of RPM****/
		case ENABLE_PWM_CHANNEL:
			if (in_data.pwmnumber < 0)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_pwm_control (in_data.pwmnumber);
			//Default: set 50% dutycycle
			//Usually one has to setup the duty cycle before enabling the PWM..hence commenting below line.
			//pdev->ppwmtach_hal->ppwmtach_hal_ops->set_dutycycle(in_data.pwmnumber, 128);
			break;
		case DISABLE_PWM_CHANNEL:
			if (in_data.pwmnumber < 0)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_pwm_control (in_data.pwmnumber);
			break;
		case ENABLE_TACH_CHANNEL:
			if (in_data.tachnumber< 0)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_fantach_control (in_data.tachnumber);
			break;
		case DISABLE_TACH_CHANNEL:
			if (in_data.tachnumber < 0)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_fantach_control (in_data.tachnumber);
			break;
		case SET_DUTY_CYCLE_BY_PWM_CHANNEL:
			if (in_data.pwmnumber < 0)
				return -1;
			if( in_data.dutycycle > 100 || in_data.dutycycle < 1 )
			{
				printk("Dutycycle value in %% should be between 1 to 100.\n");
				return -1;
			}
			in_data.dutycycle = (in_data.dutycycle*255)/100;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->set_dutycycle(in_data.pwmnumber, in_data.dutycycle);
			break;
		case SET_DUTY_CYCLE_VALUE_BY_PWM_CHANNEL:
			if (in_data.pwmnumber < 0)
				return -1;
			if( in_data.dutycycle > 255 || in_data.dutycycle < 0 )
			{
				printk("Dutycycle value should be between 0 to 255.\n");
				return -1;
			}
			pdev->ppwmtach_hal->ppwmtach_hal_ops->set_dutycycle(in_data.pwmnumber, in_data.dutycycle);
			break;
		case GET_TACH_VALUE_BY_TACH_CHANNEL:
			if (in_data.tachnumber < 0)
				return -1;
			if (0 != get_tachvalue( pdev, in_data.tachnumber, &(in_data.rpmvalue) ))
				return -1;
			break;
/***************************************************************************/
		case ENABLE_PWM:
			pwmnumber = get_pwm_number (pdev, in_data.fannumber);
			if (pwmnumber == -1)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_pwm_control (pwmnumber);
			break;
		case ENABLE_ALL_PWM:
			pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_all_pwm_control();
			break;
		case ENABLE_TACH:
			tachnumber = get_tach_number (pdev, in_data.fannumber);
			if (tachnumber == -1)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_fantach_control (tachnumber);
			break;
		case ENABLE_ALL_TACH:
			pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_all_fantach_control();
			break;
		case DISABLE_PWM:
			pwmnumber = get_pwm_number (pdev, in_data.fannumber);
			if (pwmnumber == -1)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_pwm_control (pwmnumber);
			break;
		case DISABLE_ALL_PWM:
			pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_all_pwm_control();
			break;
		case DISABLE_TACH:
			tachnumber = get_tach_number (pdev, in_data.fannumber);
			if (tachnumber == -1)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_fantach_control (tachnumber);
			break;
		case DISABLE_ALL_TACH:
			pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_all_fantach_control();
			break;
		case GET_TACH_STATUS:
			in_data.status = pdev->ppwmtach_hal->ppwmtach_hal_ops->get_fantach_control_status (in_data.tachnumber);
			break;
		case GET_PWM_STATUS:
			pwmnumber = get_pwm_number (pdev, in_data.fannumber);
			if (pwmnumber == -1)
				return -1;
			in_data.status = pdev->ppwmtach_hal->ppwmtach_hal_ops->get_pwm_control_status (pwmnumber);
			break;
		case GET_PWM_CHANNEL_STATUS:
			if (in_data.pwmnumber < 0)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->get_pwm_control_status (in_data.pwmnumber);
			break;
		case GET_TACH_VALUE:
			tachnumber = get_tach_number (pdev, in_data.fannumber);
			if (tachnumber == -1)
				return -1;
			if (0 != get_tachvalue( pdev, tachnumber, &(in_data.rpmvalue) ))
				return -1;
			break;
		case SET_DUTY_CYCLE:
			pwmnumber = get_pwm_number (pdev, in_data.fannumber);
			if (pwmnumber == -1)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->set_dutycycle(pwmnumber, in_data.dutycycle);			
			break;
		case INIT_PWMTACH:
			if (0 != init_pwmtach (pdev, &in_data))
				return -1;		
			break;
		case CONFIGURE_FANMAP_TABLE:
			if ( copy_from_user((void*)(in_data.fanmap_dataptr), ((pwmtach_data_t*)arg)->fanmap_dataptr, 
						sizeof(struct fan_map_entry_t)*(in_data.num_fans)) )
				return -EFAULT;
			if (0 != configure_fanmap_table (pdev, &in_data))
				return -1;
			break;
		case CONFIGURE_FANPROPERTY_TABLE:
			if ( copy_from_user((void*)(in_data.fanproperty_dataptr), ((pwmtach_data_t*)arg)->fanproperty_dataptr, 
						sizeof(struct fan_property_t)*(in_data.num_fans)) )
				return -EFAULT;
			dbgprint ("fan_property_table_data: %d, %d, %d\n", 
				((struct fan_property_t*) (in_data.fanproperty_dataptr))[0].min_rpm,
				((struct fan_property_t*) (in_data.fanproperty_dataptr))[0].max_rpm,
				((struct fan_property_t*) (in_data.fanproperty_dataptr))[0].pulses_per_revolution);
			if (0 != configure_fanproperty_table (pdev, &in_data))
				return -1;
			break;
		case SHOW_FANMAP_TABLE:
			if (0 != show_fanmap_table (pdev))
				return -1;
			break;
		case SHOW_FANPROPERTY_TABLE:
			if (0 != show_fanproperty_table (pdev))
				return -1;
			break;
		case GET_FAN_RPM_RANGE:
			in_data.min_rpm = pdev->ppwmtach_hal->fan_property_table[in_data.fannumber].min_rpm;
			in_data.max_rpm = pdev->ppwmtach_hal->fan_property_table[in_data.fannumber].max_rpm;
			break;
		case GET_DUTY_CYCLE:
             if (in_data.pwmnumber < 0)
				return -1;
			get_pwmvalue( pdev, in_data.pwmnumber, &(in_data.dutycycle));
			break;
		case SET_TACH_PROPERTY:
			if ((pdev->ppwmtach_hal->ppwmtach_hal_ops->set_tach_property(in_data.property.id, in_data.tachnumber, in_data.property.value)) < 0)
				return -1;
			break;
		case GET_TACH_PROPERTY:
			in_data.property.value = pdev->ppwmtach_hal->ppwmtach_hal_ops->get_tach_property(in_data.property.id, in_data.tachnumber);
			if (in_data.property.value == 0xFFFFFFFF)
				return -1;
			break;
		case SET_PWM_PROPERTY:
			if ((pdev->ppwmtach_hal->ppwmtach_hal_ops->set_pwm_property(in_data.property.id, in_data.pwmnumber, in_data.property.value)) < 0)
				return -1;
			break;
		case GET_PWM_PROPERTY:
			in_data.property.value = pdev->ppwmtach_hal->ppwmtach_hal_ops->get_pwm_property(in_data.property.id, in_data.pwmnumber);
			if (in_data.property.value == 0xFFFFFFFF)
				return -1;
			break;
		case CLEAR_TACH_ERROR:
			if (in_data.tachnumber < 0)
				return -1;
			pdev->ppwmtach_hal->ppwmtach_hal_ops->clear_tach_error(in_data.tachnumber);			
			break;
		case CLEAR_PWM_ERRORS:
			pdev->ppwmtach_hal->ppwmtach_hal_ops->clear_pwm_errors();			
			break;
			
		default :
			printk ("Invalid pwmtach Function \n");
			return -EINVAL;
	}

	if ( copy_to_user((void*)arg, &in_data, sizeof(pwmtach_data_t)) )
		return -EFAULT;

	return 0;
}
#ifndef USE_UNLOCKED_IOCTL  
static int
pwmtach_ioctl (struct inode *inode, struct file *file, uint cmd, ulong arg)
{
	return(pwmtach_ioctlUnlocked (file, cmd, arg));
}
#endif	
static int
init_pwmtach (struct pwmtach_dev* pdev, pwmtach_data_t* in_data)
{
#if 1
	in_data->pwmnumber = get_pwm_number(pdev, in_data->fannumber);
	if (in_data->pwmnumber < 0)
		return -1;

	return 0;
#else
	int clock_val =  pdev->ppwmtach_hal->ppwmtach_hal_ops->get_pwm_clk();
	int pulses_rev = pdev->ppwmtach_hal->fan_property_table[in_data->fannumber].pulses_per_revolution;
	unsigned int all_done = 0;

	in_data->pwmnumber = get_pwm_number (pdev, in_data->fannumber);
	if (in_data->pwmnumber < 0)
		return -1;
	pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_pwm_control (in_data->pwmnumber);	
	pdev->ppwmtach_hal->ppwmtach_hal_ops->disable_counterresolution (in_data->pwmnumber);

	//initialize prescale, dutycycle, counter resolution
        in_data->prescalervalue = pdev->ppwmtach_hal->ppwmtach_hal_ops->get_prescale(in_data->pwmnumber);
        while (!all_done)
        {
                in_data->counterresvalue = (clock_val * 60) / (in_data->rpmvalue * pulses_rev * in_data->prescalervalue);
                if (in_data->counterresvalue > 0xFF)
                {
                        in_data->prescalervalue *= 2;
                }
                else
                {
                        in_data->dutycycle = in_data->counterresvalue / pulses_rev;
                        all_done = 1;
                }
        }
        in_data->prevdutycycle = in_data->dutycycle;
	
	pdev->ppwmtach_hal->ppwmtach_hal_ops->set_counterresolution (in_data->pwmnumber, in_data->counterresvalue);
	pdev->ppwmtach_hal->ppwmtach_hal_ops->set_prescale (in_data->pwmnumber, in_data->prescalervalue);
	pdev->ppwmtach_hal->ppwmtach_hal_ops->set_dutycycle (in_data->pwmnumber, in_data->dutycycle);		
	pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_counterresolution (in_data->pwmnumber);
	pdev->ppwmtach_hal->ppwmtach_hal_ops->enable_pwm_control (in_data->pwmnumber);
	return 0;
#endif
}


static int 
get_tachvalue (struct pwmtach_dev* pdev, unsigned int tachnumber, unsigned int* rpmvalue)
{
	unsigned int retries = 1; //avoid spinning 2.5 seconds within kernel waiting for timeout 

	if (pdev->ppwmtach_hal->ppwmtach_hal_ops->trigger_read_fanspeed(tachnumber) < 0) {
		printk("trigger read fan speed failed\n");
		return -1;
	}

	while ((retries!=0) && (pdev->ppwmtach_hal->ppwmtach_hal_ops->can_read_fanspeed(tachnumber) != 1))
	{
		schedule_timeout( 0.1 * HZ);
		retries--;
	}
	if (retries == 0)
	{
		//printk("ran out of retries in gettachvalue(tach %d)...returning -1\n",tachnumber & 0x7F);
		return -1;
	}

	*rpmvalue = pdev->ppwmtach_hal->ppwmtach_hal_ops->get_current_speed (tachnumber);
	return 0;
}


static int 
configure_fanmap_table (struct pwmtach_dev* pdev, pwmtach_data_t* in_data)
{
	int i=0;
	struct fan_map_entry_t* fanmap_data = (struct fan_map_entry_t*) (in_data->fanmap_dataptr);
	pdev->ppwmtach_hal->num_fans = in_data->num_fans;
	for (i=0; i<in_data->num_fans; i++)
	{
		pdev->ppwmtach_hal->fan_map_table[i].fan_num = fanmap_data[i].fan_num;
		pdev->ppwmtach_hal->fan_map_table[i].pwm_num = fanmap_data[i].pwm_num;
		pdev->ppwmtach_hal->fan_map_table[i].tach_num = fanmap_data[i].tach_num;
	}
	return 0;	
}


static int
configure_fanproperty_table (struct pwmtach_dev* pdev, pwmtach_data_t* in_data)
{
	int i=0;
	struct fan_property_t* fanproperty_data = (struct fan_property_t*) (in_data->fanproperty_dataptr);
	pdev->ppwmtach_hal->num_fans = in_data->num_fans;
	for (i=0; i<in_data->num_fans; i++)
	{
		pdev->ppwmtach_hal->fan_property_table[i].fan_num = fanproperty_data[i].fan_num;
		pdev->ppwmtach_hal->fan_property_table[i].min_rpm = fanproperty_data[i].min_rpm;
		pdev->ppwmtach_hal->fan_property_table[i].max_rpm = fanproperty_data[i].max_rpm;
		pdev->ppwmtach_hal->fan_property_table[i].pulses_per_revolution = fanproperty_data[i].pulses_per_revolution;
	}
	return 0;
}


static int 
show_fanmap_table (struct pwmtach_dev* pdev)
{
	int i = 0;
	printk("\tFan Map Table for %d : (FANNUM, PWMNUM, TACHNUM)\n\n", pdev->ppwmtach_hal->dev_id);
	for (i=0; i<pdev->ppwmtach_hal->num_fans; i++)
		printk("\tENTRY[%d] = (%d, %d, %d)\n", i, pdev->ppwmtach_hal->fan_map_table[i].fan_num,
			pdev->ppwmtach_hal->fan_map_table[i].pwm_num, pdev->ppwmtach_hal->fan_map_table[i].tach_num);
	printk("\n");
	return 0;
}


static int 
show_fanproperty_table (struct pwmtach_dev* pdev)
{
	int i = 0;
        printk("\tFan Properties Table for %d : (FANNUM, MINRPM, MAXRPM, PULSES/REV)\n\n", pdev->ppwmtach_hal->dev_id);
        for (i=0; i<pdev->ppwmtach_hal->num_fans; i++)
                printk("\tENTRY[%d] = (%d, %d, %d, %d)\n", i, pdev->ppwmtach_hal->fan_property_table[i].fan_num,
			pdev->ppwmtach_hal->fan_property_table[i].min_rpm, pdev->ppwmtach_hal->fan_property_table[i].max_rpm,
			pdev->ppwmtach_hal->fan_property_table[i].pulses_per_revolution);
        printk("\n");
        return 0;
}

static int
get_pwm_number (struct pwmtach_dev* pdev, unsigned char fan_number)
{
	int i=0;
	for (i=0; i<pdev->ppwmtach_hal->num_fans; i++)
	{
		if (pdev->ppwmtach_hal->fan_map_table[i].fan_num == fan_number)
			return pdev->ppwmtach_hal->fan_map_table[i].pwm_num;
	}
	printk("get_pwm_number: Incorrect fan_number passed\n");
	return -1;
}

static int
get_tach_number (struct pwmtach_dev* pdev, unsigned char fan_number)
{
	int i=0;
	unsigned char tnum;

	for (i=0; i<pdev->ppwmtach_hal->num_fans; i++)
	{
		if (pdev->ppwmtach_hal->fan_map_table[i].fan_num == fan_number)
		{
			tnum = pdev->ppwmtach_hal->fan_map_table[i].tach_num;
			if (fan_number & 0x80)
				tnum = tnum | 0x80;
			return tnum;
		}
	}
	printk("get_tach_number: Incorrect fan_number passed\n");
	return -1;
}

static int 
get_pwmvalue (struct pwmtach_dev* pdev, unsigned int pwm_number, unsigned char* dutycycle)
{
	
	*dutycycle = pdev->ppwmtach_hal->ppwmtach_hal_ops->get_dutycycle (pwm_number);
//	printk("pwm_number %d dutycycle %d\n",pwm_number,(int)*dutycycle);
	return 0;
}



/* ----- Driver registration ---------------------------------------------- */

static pwmtach_core_funcs_t pwmtach_core_funcs = {
};

static core_hal_t pwmtach_core_hal = {
        .owner                  =  THIS_MODULE,
        .name                   =  "PWMTACH CORE",
        .dev_type               =  EDEV_TYPE_PWMTACH,
        .register_hal_module    =  register_pwmtach_hal_module,
        .unregister_hal_module  =  unregister_pwmtach_hal_module,
        .pcore_funcs            =  (void *)&pwmtach_core_funcs
};

int __init
pwmtach_init (void)
{
	int ret = 0; 
	printk(banner);
	
	if ((ret = register_chrdev_region (pwmtach_devno, MAX_PWMTACH_DEVICES, PWMTACH_DEV_NAME)) < 0)
	{
		printk ("failed to register device pwmtach device <%s> (err: %d)\n", PWMTACH_DEV_NAME, ret);
		return ret;
	}
	
	pwmtach_cdev = cdev_alloc();
	if (!pwmtach_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate pwmtach cdev structure\n", PWMTACH_DEV_NAME);
		unregister_chrdev_region (pwmtach_devno, MAX_PWMTACH_DEVICES);
		return -1;
	}

	cdev_init (pwmtach_cdev, &pwmtach_fops);

	if ((ret = cdev_add (pwmtach_cdev, pwmtach_devno, MAX_PWMTACH_DEVICES)) < 0)
	{
		printk  (KERN_ERR "failed to add <%s> char device\n", PWMTACH_DEV_NAME);
		cdev_del (pwmtach_cdev);
		unregister_chrdev_region (pwmtach_devno, MAX_PWMTACH_DEVICES);
		ret=-ENODEV;
                return ret;
	}

	if ((ret = register_core_hal_module (&pwmtach_core_hal)) < 0)
	{
		cdev_del (pwmtach_cdev);
		unregister_chrdev_region (pwmtach_devno, MAX_PWMTACH_DEVICES);
		printk("error in registering the core pwmtach module\n");
		ret=-EINVAL;
                return ret;
	}		
	return ret ;
}

module_init(pwmtach_init);
module_exit(pwmtach_exit);

int pwmtach_core_loaded =1;
EXPORT_SYMBOL(pwmtach_core_loaded);
