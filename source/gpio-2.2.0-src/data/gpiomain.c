/*********************************************************
 **                                                     **
 **    (C)Copyright 2009-2015, American Megatrends Inc. **
 **                                                     **
 **            All Rights Reserved.                     **
 **                                                     **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,      **
 **                                                     **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.  **
 **                                                     **
 ********************************************************/
 
 /*
 * File name: gpiomain.c
 * This driver provides common layer, independent of the hardware, for the GPIO driver.
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/sched.h>
//#include <mach/platform.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cacheflush.h>
#include "helper.h"
#include "driver_hal.h"
#include "gpio.h"
#include "dbgout.h"

#define DRIVER_AUTHOR "American Megatrends Inc."
#define DRIVER_DESC "GPIO Common Driver"
#define DRIVER_LICENSE "GPL"

/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE ( DRIVER_LICENSE );

#define GPIO_MAJOR           101
#define GPIO_MINOR	    	 0
#define GPIO_MAX_DEVICES     255
#define GPIO_DEV_NAME        "gpio"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
DEFINE_SPINLOCK(sensor_lock);
DEFINE_SPINLOCK(chassis_lock);
#else
spinlock_t sensor_lock = SPIN_LOCK_UNLOCKED;
spinlock_t chassis_lock = SPIN_LOCK_UNLOCKED;
#endif

#define DEBUG
#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)       
#endif

#ifdef HAVE_UNLOCKED_IOCTL
  #if HAVE_UNLOCKED_IOCTL
	#define USE_UNLOCKED_IOCTL
  #endif
#endif

/* Interrupt sensors related definitions */
uint32_t total_interrupt_sensors;
gpio_interrupt_sensor* intr_sensors;
pending_interrupt_info *front_sensor_ints = NULL;
pending_interrupt_info *rear_sensor_ints = NULL;
DECLARE_WAIT_QUEUE_HEAD(gpio_sensorint_wq);

/* Chassis Interrupts related definitions */
uint32_t total_chassis_interrupts;
gpio_interrupt_sensor* intr_chassis;
pending_interrupt_info *front_chassis_ints = NULL;
pending_interrupt_info *rear_chassis_ints = NULL;
DECLARE_WAIT_QUEUE_HEAD(gpio_chassisint_wq);


TDBG_DECLARE_DBGVAR(gpio);

/* ------------------------------------------------------------------------- *
 *  Function prototypes
 * ------------------------------------------------------------------------- */

static int  gpio_init(void);
static void gpio_exit(void);
#ifdef USE_UNLOCKED_IOCTL 
static  long gpio_ioctlUnlocked(struct file *, uint, ulong);
#else
static int  gpio_ioctl(struct inode *, struct file *, uint, ulong);
#endif
static int  gpio_open(struct inode *, struct file *);
static int  gpio_release(struct inode *inode, struct file *file);
static unsigned int gpio_poll(struct file* file, poll_table *wait);


/* ------------------------------------------------------------------------- *
 * private data
 * ------------------------------------------------------------------------- */

static struct file_operations gpio_ops = {
        owner:      THIS_MODULE,
        read:       NULL,
        write:      NULL,
#ifdef USE_UNLOCKED_IOCTL 
	unlocked_ioctl:      gpio_ioctlUnlocked,
#else
	ioctl:      gpio_ioctl,
#endif
        open:       gpio_open,
        release:    gpio_release,
        poll:       gpio_poll,
};


static struct cdev *gpio_cdev;
static dev_t gpio_devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);

int 
register_gpio_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct gpio_hal* phw_hal;

	phw_hal = (struct gpio_hal*) kmalloc (sizeof(struct gpio_hal), GFP_KERNEL);
	if (!phw_hal)
	{
		return -ENOMEM;
	}

	phw_hal->pgpio_hal_ops = ( gpio_hal_operations_t *) phal_ops;
	*phw_data = (void *) phw_hal;	

	return 0;	
}

int 
unregister_gpio_hal_module (void *phw_data)
{
	struct gpio_hal *pgpio_hal = (struct gpio_hal*) phw_data;

	kfree (pgpio_hal);

	return 0;
}


/* Add interrupt item at the rear end of the Q */
static inline void add_interrupt(char* type, pending_interrupt_info **front, pending_interrupt_info **rear, unsigned short gpionum, unsigned char state, unsigned char trigger_type)
{
    pending_interrupt_info *newnode;      /* New node to be inserted */

    /*
     * For level-triggered interrupts, the IRQ handler will likely be called
     * multiple times before the interrupt is handled, resulting in multiple
     * entries in the queue.    If an level interrupt is stuck for a long period,
     * many thousands of entries may be queued.    To prevent this, only
     * allow one LOW_LEVEL and one HIGH_LEVEL triggered interrupt in the queue for
     * any GPIO pin.
     */
    if (trigger_type == GPIO_LOW_LEVEL || trigger_type == GPIO_HIGH_LEVEL) {
        pending_interrupt_info *node;
        node = *front;
        while (node) {
            if (node->gpio == gpionum && node->trigger_type == trigger_type) {
                //dbgprint("Level %s interrupt already queued for GPIO %d, ignoring\n", type, gpionum);
                //printk("low level\n");
                //touch_softlockup_watchdog();
                return;
            }
            node = node->next;
        }
    }

	
    newnode=(pending_interrupt_info*)kmalloc(sizeof(pending_interrupt_info),GFP_ATOMIC);
    if (newnode == NULL)
        return;
    
    newnode->gpio=gpionum;
    newnode->state=state;
    newnode->trigger_type=trigger_type;
    newnode->next=NULL;
    if(*front==NULL && *rear==NULL)
    {
        *front=newnode;
        *rear=newnode;
    }
    else
    {
        /* add the interrupt to the pending list */
        (*rear)->next=newnode;
        *rear=newnode;
    }
}

void add_sensor_interrupt(unsigned short gpionum, unsigned char state, unsigned char trigger_type)
{
    add_interrupt("sensor", &front_sensor_ints, &rear_sensor_ints, gpionum, state, trigger_type);
}

void add_chassis_interrupt(unsigned short gpionum, unsigned char state, unsigned char trigger_type)
{
    add_interrupt("chassis", &front_chassis_ints, &rear_chassis_ints, gpionum, state, trigger_type);
}


/* Removes the interrupt item in the front of the Q */
static inline void remove_interrupt(char* type, pending_interrupt_info **front, pending_interrupt_info **rear)
{
    pending_interrupt_info *delnode;      /* Node to be deleted */
    if(*front==NULL && *rear==NULL)
        dbgprint("Queue (%s) is empty to delete any element\n", type);
    else
    {
        delnode=*front;
        *front=(*front)->next;
        kfree(delnode);
		if(*front == NULL)
			*rear = NULL;
    }
}

void remove_sensor_interrupt(void)
{
    remove_interrupt("sensor", &front_sensor_ints, &rear_sensor_ints);
}

void remove_chassis_interrupt(void)
{
    remove_interrupt("chassis", &front_chassis_ints, &rear_chassis_ints);
}


void process_pending_sensor_interrupt(void* gpin_data)
{
	unsigned short gpio_num;
	int i = 0;

	if (front_sensor_ints == NULL)
		return;
	/* Read it from the pending interrupts */
	gpio_num = front_sensor_ints->gpio;

	for(i = 0; i < total_interrupt_sensors; i++)
	{
		if(gpio_num == intr_sensors[i].gpio_number)
		{
			/* We need to return info for this interrupt sensor */
			memcpy(gpin_data,(void *) &intr_sensors[i],sizeof(gpio_interrupt_sensor));
			break;
		}
	}
	/* Remove the read interrupt entry from the pending interrupts */
	remove_sensor_interrupt();
}


int gpio_waitforsensorint (void* gpin_data)
{
	unsigned long flags;
	
	if(front_sensor_ints != NULL)
	{
		/* Interrupt already pending */
		spin_lock_irqsave(&sensor_lock,flags);
		process_pending_sensor_interrupt(gpin_data);
		spin_unlock_irqrestore(&sensor_lock,flags);
	}
	else
	{
		if(wait_event_interruptible(gpio_sensorint_wq,(front_sensor_ints != NULL)))
			return -ERESTARTSYS;

		spin_lock_irqsave(&sensor_lock,flags);
		process_pending_sensor_interrupt(gpin_data);
		spin_unlock_irqrestore(&sensor_lock,flags);
	}
	return 0; 
}


void process_pending_chassis_interrupt(void* gpin_data)
{
	unsigned short gpio_num;
	int i = 0;


	/* Read it from the pending interrupts */
	gpio_num = front_chassis_ints->gpio;

	for(i = 0; i < total_chassis_interrupts; i++)
	{
		if(gpio_num == intr_chassis[i].gpio_number)
		{
			/* We need to return info for this interrupt sensor */
			memcpy(gpin_data,(void *) &intr_chassis[i],sizeof(gpio_interrupt_sensor));
			((gpio_interrupt_sensor *)gpin_data)->reading_on_assertion = front_chassis_ints->state;  // Quanta
			break;
		}
	}
	/* Remove the read interrupt entry from the pending interrupts */
	remove_chassis_interrupt();
}

int gpio_waitforchassisint (void* gpin_data)
{
	unsigned long flags;
	
	if(front_chassis_ints != NULL)
	{
		/* Interrupt already pending */
		spin_lock_irqsave(&chassis_lock,flags); 
		process_pending_chassis_interrupt(gpin_data);
		spin_unlock_irqrestore(&chassis_lock,flags);
	}
	else
	{
		if(wait_event_interruptible(gpio_chassisint_wq,(front_chassis_ints != NULL)))
			return -ERESTARTSYS;

		spin_lock_irqsave(&chassis_lock,flags); 
		process_pending_chassis_interrupt(gpin_data);
		spin_unlock_irqrestore(&chassis_lock,flags);
	}
	return 0; 
}

int
process_gpio_intr (int gpio_num, int gpio_port_pin_offset, unsigned char state, unsigned char trigger_type)
{
	int j;
	int intr_type = 0;
	unsigned long flags;
	/* Check if it's a sensor interrupt */
	for(j = 0; j < total_interrupt_sensors; j++)
	{
		if(gpio_num + gpio_port_pin_offset == intr_sensors[j].gpio_number)
		{
			intr_type = SENSOR_INTR;
			break;
		}
	}
	if(intr_type != SENSOR_INTR)
	{	/* If not sensor then check if it's a chassis interrupt */
		for(j = 0; j < total_chassis_interrupts; j++)
		{
			if(gpio_num + gpio_port_pin_offset == intr_chassis[j].gpio_number)
			{
				intr_type = CHASSIS_INTR;
				break;
			}
		}
	}
	if(intr_type == SENSOR_INTR)
	{
		/* If it's a sensor interrupt */
		/* Add it to the pending sensor interrupts structure */
		spin_lock_irqsave(&sensor_lock, flags);
		add_sensor_interrupt(gpio_num + gpio_port_pin_offset, state, trigger_type);
		spin_unlock_irqrestore (&sensor_lock, flags);
	}
	else if(intr_type == CHASSIS_INTR)
	{
		/* If it's a chassis interrupt */
		/* Add it to the pending chassis interrupts structure */
		spin_lock_irqsave(&chassis_lock, flags);
		add_chassis_interrupt(gpio_num + gpio_port_pin_offset, state, trigger_type);
		spin_unlock_irqrestore (&chassis_lock, flags);
	}
	else
		dbgprint("Spurious GPIO interrupt\n");

 	return intr_type;
}


void
wakeup_intr_queue (int intr_type)
{	
	if(intr_type == SENSOR_INTR )
		/* Wake up who all is waiting on this wait Q */
		wake_up_interruptible(&gpio_sensorint_wq);
	if(intr_type == CHASSIS_INTR )
		/* Wake up who all is waiting on this wait Q */
		wake_up_interruptible(&gpio_chassisint_wq);
}


/*
 * GPIO driver exit function
 */
static void 
gpio_exit(void)
{
	unregister_core_hal_module (EDEV_TYPE_GPIO);

	unregister_chrdev_region (gpio_devno, GPIO_MAX_DEVICES);

	if (NULL != gpio_cdev)
	{
		cdev_del (gpio_cdev);
	}

	printk ( "Unregistered the GPIO Driver Sucessfully\n");

	return;
}


static int
gpio_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct gpio_hal *pgpio_hal;
	struct gpio_dev *pdev;
	hw_info_t gpio_hw_info;
	unsigned char open_count;
	int ret;
	
	ret = hw_open (EDEV_TYPE_GPIO, minor,&open_count, &gpio_hw_info);
	if (ret)
		return -ENXIO;

	pgpio_hal = gpio_hw_info.pdrv_data;

	pdev = (struct gpio_dev*)kmalloc(sizeof(struct gpio_dev), GFP_KERNEL);
	
	if (!pdev)
	{
		hw_close (EDEV_TYPE_GPIO, minor, &open_count);
		printk (KERN_ERR "%s: failed to allocate gpio private dev structure for gpio iminor: %d\n", GPIO_DEV_NAME, minor);
		return -ENOMEM;
	}

	pdev->pgpio_hal = pgpio_hal;
	file->private_data = pdev;

	return 0;
}


static int
gpio_release(struct inode *inode, struct file *file)
{
	int ret;
	unsigned char open_count;
    struct gpio_dev *pdev = (struct gpio_dev*)file->private_data;
	pdev->pgpio_hal = NULL;
    file->private_data = NULL;
    ret = hw_close (EDEV_TYPE_GPIO, iminor(inode), &open_count);
    if(ret) { return -1; }
	kfree (pdev);
	return 0;
}

static unsigned int
gpio_poll(struct file* file, poll_table *wait)
{
	unsigned int mask = 0;

	//we have to actually wait on a wait queue here
	//the qs to wait on are the sensor int q and the chassis int q

	//with poll wait we can add both to poll_wait and then on any of these wait qs a event will cause poll to return to the applications
	//note that poll_Wait does not go to sleep immediately it is handled by the kernel

	poll_wait(file,&gpio_sensorint_wq,wait);
	//poll_wait(file,&gpio_chassisint_wq,wait);

	//we also have to put the conditions here that help us determine if there is a interrupt recd
	if(front_sensor_ints != NULL)
	{
		mask |= POLLPRI;
	}

	return mask;

}

static long 
gpio_ioctlUnlocked(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct gpio_dev *pdev = (struct gpio_dev*) file->private_data;
	gpio_reading_on_interrupt read_data;

    Gpio_data_t gpin_data;
	gpio_interrupt_sensor ret_int_sensor;
    if( cmd != ENABLE_GPIO_TOGGLE)
    {
    	if (cmd != REGISTER_READING_ON_GPIO_INT)
    		if ( copy_from_user(&gpin_data, (void*)arg, sizeof(Gpio_data_t)) )
    			return -EFAULT;
    }
    
	switch (cmd)
	{

	case GET_GPIO_DIRECTION :
		if (pdev->pgpio_hal->pgpio_hal_ops->getdir) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->getdir((void*)&gpin_data ))
				return -1;
		}
		if (copy_to_user((void*)arg, &gpin_data , sizeof(Gpio_data_t))) return -EFAULT;
		break;

	case SET_GPIO_DIRECTION:
		if (pdev->pgpio_hal->pgpio_hal_ops->setdir) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->setdir((void*)&gpin_data ))
			return -1;
		}
		break;
		
	case GET_GPIO_PULL_UP_DOWN:
	    if (pdev->pgpio_hal->pgpio_hal_ops->getpull_up_down) {
	        if (0 != pdev->pgpio_hal->pgpio_hal_ops->getpull_up_down((void*)&gpin_data ))
	            return -1;
	    }
	    if (copy_to_user((void*)arg, &gpin_data , sizeof(Gpio_data_t))) return -EFAULT;
	    break;
 
	case SET_GPIO_PULL_UP_DOWN:
	    if (pdev->pgpio_hal->pgpio_hal_ops->setpull_up_down) {
	        if (0 != pdev->pgpio_hal->pgpio_hal_ops->setpull_up_down((void*)&gpin_data ))
	            return -1;
	    }
	    break;
	    
    case GET_GPIO_POLARITY:
        if (pdev->pgpio_hal->pgpio_hal_ops->getpol) {
            if (0 != pdev->pgpio_hal->pgpio_hal_ops->getpol((void*)&gpin_data ))
                return -1;
        }
        if (copy_to_user((void*)arg, &gpin_data , sizeof(Gpio_data_t))) return -EFAULT;
        break;
 
    case SET_GPIO_POLARITY:
        if (pdev->pgpio_hal->pgpio_hal_ops->setpol) {
            if (0 != pdev->pgpio_hal->pgpio_hal_ops->setpol((void*)&gpin_data ))
                return -1;
        }
        break;

	case READ_GPIO:
		if (pdev->pgpio_hal->pgpio_hal_ops->readgpio) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->readgpio((void*)&gpin_data ))
				return -1;
			}
		if (copy_to_user((void*)arg, &gpin_data , sizeof(Gpio_data_t))) return -EFAULT;
 		break;

	case WRITE_GPIO:
		if (pdev->pgpio_hal->pgpio_hal_ops->writegpio) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->writegpio((void*)&gpin_data ))
				return -1;
			}
		break;

	case REGISTER_SENSOR_INTS:
		total_interrupt_sensors = gpin_data.gpio_int_sensor_data.total_sensors;
		intr_sensors = (gpio_interrupt_sensor *) kmalloc(sizeof(gpio_interrupt_sensor)*total_interrupt_sensors,GFP_KERNEL);
		if (intr_sensors == NULL)
			return -ENOMEM;
		
		if (copy_from_user((void *)intr_sensors, (const void __user *)gpin_data.gpio_int_sensor_data.int_sensor_info, sizeof(gpio_interrupt_sensor)*total_interrupt_sensors))
			return -EFAULT;

		if (pdev->pgpio_hal->pgpio_hal_ops->regsensorints) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->regsensorints((void*)&gpin_data, total_interrupt_sensors, (void *)intr_sensors))
				return -1;
			}
		break;

	case WAIT_FOR_SENSOR_INT:
		if (0 != gpio_waitforsensorint((void*)&ret_int_sensor ))
			return -1;
			
		if(copy_to_user((void __user *)arg, (void*) &ret_int_sensor,sizeof(gpio_interrupt_sensor)))
			return -EFAULT;

		break;

	case UNREGISTER_SENSOR_INTS:
		if (pdev->pgpio_hal->pgpio_hal_ops->unregsensorints) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->unregsensorints((void*)&gpin_data ))
				return -1;
		}
		/* Remove all the pending interrupts and free the memory */
		while(front_sensor_ints != NULL)
		{
			remove_sensor_interrupt();
		}
		kfree(intr_sensors);
		total_interrupt_sensors = 0;
		break;

	case REGISTER_READING_ON_GPIO_INT:
		
		if (copy_from_user((void *)(&read_data), (void *)arg, sizeof (gpio_reading_on_interrupt)))
					return -EFAULT;
		
		if (pdev->pgpio_hal->pgpio_hal_ops->reg_reading_on_ints) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->reg_reading_on_ints(read_data.gpio_number, read_data.va_address))
				return -1;
		}
		break;
		
	case GET_READING_ON_GPIO_INT:
		if ((pdev->pgpio_hal->pgpio_hal_ops->get_reading_on_ints) && 
			(0 != pdev->pgpio_hal->pgpio_hal_ops->get_reading_on_ints(gpin_data.PinNum, &gpin_data.data)) )
		{
			printk("GPIO get reading failed\n");
			return -1;
		}

		if(copy_to_user((void __user *)arg, (void*) &gpin_data, sizeof(Gpio_data_t)))
			return -EFAULT;
		break;
		
	case REGISTER_CHASSIS_INTS:
		total_chassis_interrupts = gpin_data.gpio_int_sensor_data.total_sensors;
		intr_chassis = (gpio_interrupt_sensor *) kmalloc(sizeof(gpio_interrupt_sensor)*total_chassis_interrupts,GFP_KERNEL);
		if (intr_chassis == NULL)
			return -ENOMEM;
		
		if (copy_from_user((void *)intr_chassis, (const void __user *)gpin_data.gpio_int_sensor_data.int_sensor_info, sizeof(gpio_interrupt_sensor)*total_chassis_interrupts))
			return -EFAULT;

		if (pdev->pgpio_hal->pgpio_hal_ops->regchassisints) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->regchassisints((void*)&gpin_data, total_chassis_interrupts, (void *)intr_chassis))
				return -1;
		}
		break;

	case WAIT_FOR_CHASSIS_INT:
			if (0 != gpio_waitforchassisint((void*)&ret_int_sensor ))
				return -1;

		if(copy_to_user((void __user *)arg, (void*) &ret_int_sensor,sizeof(gpio_interrupt_sensor)))
			return -EFAULT;

		break;

	case UNREGISTER_CHASSIS_INTS:
		if (pdev->pgpio_hal->pgpio_hal_ops->unregchassisints) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->unregchassisints((void*)&gpin_data ))
				return -1;
			}
		/* Remove all the pending interrupts and free the memory */
		while(front_chassis_ints != NULL)
		{
			remove_chassis_interrupt();
		}
		kfree(intr_chassis);
		total_chassis_interrupts = 0;
		break;

	case GET_SGPIO_BUS_CONFIG:
		if (pdev->pgpio_hal->pgpio_hal_ops->get_sgpio_bus_config) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->get_sgpio_bus_config((void*)&gpin_data ))
				return -1;
		}
		if (copy_to_user((void*)arg, &gpin_data , sizeof(Gpio_data_t))) return -EFAULT;
		break;

	case SET_SGPIO_BUS_CONFIG:
		if (pdev->pgpio_hal->pgpio_hal_ops->set_sgpio_bus_config) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->set_sgpio_bus_config((void*)&gpin_data ))
				return -1;
		}
		break;
	case SET_CONT_MODE:
		if (pdev->pgpio_hal->pgpio_hal_ops->setcontmode) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->setcontmode((void*)&gpin_data ))
				return -1;
			}
		break;
	case GET_GPIO_DEBOUNCE:
		if (pdev->pgpio_hal->pgpio_hal_ops->get_debounce_event) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->get_debounce_event((void*)&gpin_data ))
				return -1;
		}
		if (copy_to_user((void*)arg, &gpin_data , sizeof(Gpio_data_t))) return -EFAULT;
		
		break;
	case SET_GPIO_DEBOUNCE:
		if (pdev->pgpio_hal->pgpio_hal_ops->set_debounce_event) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->set_debounce_event((void*)&gpin_data ))
			{
				printk("SET_GPIO_DEBOUNCE for event error return -1\n");
				return -1;
			}
		}	
		break;
		
	case GET_GPIO_DEBOUNCE_CLOCK:
		if (pdev->pgpio_hal->pgpio_hal_ops->get_debounce_clock) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->get_debounce_clock((void*)&gpin_data ))
				return -1;
		}
		if (copy_to_user((void*)arg, &gpin_data , sizeof(Gpio_data_t))) return -EFAULT;
		
		break;
	case SET_GPIO_DEBOUNCE_CLOCK:
		if (pdev->pgpio_hal->pgpio_hal_ops->set_debounce_clock) {
			if (0 != pdev->pgpio_hal->pgpio_hal_ops->set_debounce_clock((void*)&gpin_data ))
				return -1;
		}
		break;

	case READ_GPIOS:
		if (pdev->pgpio_hal->pgpio_hal_ops->readgpios) {

			int			rVal;
			size_t			bytes;
			gpio_list_data_info	*info;

			bytes = sizeof (gpio_list_data_info) * gpin_data.gpio_list.count;

			info = (gpio_list_data_info *) kmalloc (bytes, GFP_KERNEL);
			if (!info) {
				return -ENOMEM;
			}

			if (copy_from_user ((void *)info, (const void __user *)gpin_data.gpio_list.info, bytes)) 
			{
				kfree (info);
				return -EFAULT;
			}

			rVal = pdev->pgpio_hal->pgpio_hal_ops->readgpios ((void*)info, gpin_data.gpio_list.count);
			/* 
			 * Upon Success/Fail the value/status is written into the pin's info.data field
			 * Always copy this information back to the user
			 */
			if (copy_to_user ((void*)gpin_data.gpio_list.info, info, bytes)) 
			{
				kfree (info);
				return -EFAULT;
			}

			kfree (info);
			if (rVal != 0) return -1;
		}
 		break;

	case GET_GPIOS_DIR:
		if (pdev->pgpio_hal->pgpio_hal_ops->getgpiosdir) {

			int			rVal;
			size_t			bytes;
			gpio_list_data_info	*info;

			bytes = sizeof (gpio_list_data_info) * gpin_data.gpio_list.count;

			info = (gpio_list_data_info *) kmalloc (bytes, GFP_KERNEL);
			if (!info) {
				return -ENOMEM;
			}

			if (copy_from_user ((void *)info, (const void __user *)gpin_data.gpio_list.info, bytes)) 
			{
				kfree (info);
				return -EFAULT;
			}

			rVal = pdev->pgpio_hal->pgpio_hal_ops->getgpiosdir ((void*)info, gpin_data.gpio_list.count);
			/* 
			 * Upon Success/Fail the value/status is written into the pin's info.data field
			 * Always copy this information back to the user
			 */
			if (copy_to_user ((void*)gpin_data.gpio_list.info, info, bytes)) 
			{
				kfree (info);
				return -EFAULT;
			}

			kfree (info);
			if (rVal != 0) return -1;
		}
 		break;

	case WRITE_GPIOS:
		if (pdev->pgpio_hal->pgpio_hal_ops->writegpios) {

			size_t			bytes;
			gpio_list_data_info	*info;

			bytes = sizeof (gpio_list_data_info) * gpin_data.gpio_list.count;

			info = (gpio_list_data_info *) kmalloc (bytes, GFP_KERNEL);
			if (!info) {
				return -ENOMEM;
			}

			if (copy_from_user ((void *)info, (const void __user *)gpin_data.gpio_list.info, bytes)) 
			{
				kfree (info);
				return -EFAULT;
			}

			if (0 != pdev->pgpio_hal->pgpio_hal_ops->writegpios ((void*)info, gpin_data.gpio_list.count)) {
				/* when a write fails a -1 is written into the pin's info.data field - copy this info to user */
				if (copy_to_user ((void*)gpin_data.gpio_list.info, info, bytes)) 
				{
					kfree (info);
					return -EFAULT;
				}
		
				kfree (info);
				return -1;
			}
			kfree (info);
		}
		break;

    case SET_GPIOS_DIR:
		if (pdev->pgpio_hal->pgpio_hal_ops->setgpiosdir) {

			size_t			bytes;
			gpio_list_data_info	*info;

			bytes = sizeof (gpio_list_data_info) * gpin_data.gpio_list.count;

			info = (gpio_list_data_info *) kmalloc (bytes, GFP_KERNEL);
			if (!info) {
				return -ENOMEM;
			}

			if (copy_from_user ((void *)info, (const void __user *)gpin_data.gpio_list.info, bytes)) 
			{
				kfree (info);
				return -EFAULT;
			}

			if (0 != pdev->pgpio_hal->pgpio_hal_ops->setgpiosdir ((void*)info, gpin_data.gpio_list.count)) {
				/* when a write fails a -1 is written into the pin's info.data field - copy this info to user */
				if (copy_to_user ((void*)gpin_data.gpio_list.info, info, bytes)) 
				{
					kfree (info);
					return -EFAULT;
				}
		
				kfree (info);
				return -1;
			}
			kfree (info);
		}
		break;

	case ENABLE_GPIO_TOGGLE: 
		{
			ToggleData Data;
			if ( copy_from_user(&Data,(void*)arg,sizeof(ToggleData)) )
			{
			   	return -EFAULT;
			}
			if (pdev->pgpio_hal->pgpio_hal_ops->EnableGpioToggle) 
			{
				if (0 != pdev->pgpio_hal->pgpio_hal_ops->EnableGpioToggle(&Data ))
					return -1;
			}
			else
				return -1;

		}
		break;

	case SET_GPIO_PROPERTY:
		if (0 != pdev->pgpio_hal->pgpio_hal_ops->set_gpio_property(gpin_data.PinNum, gpin_data.property.id, gpin_data.data))
		{
			printk("GPIO set property failed\n");
			return -1;
		}
		break;
	case GET_GPIO_PROPERTY:
		if(0 != pdev->pgpio_hal->pgpio_hal_ops->get_gpio_property(gpin_data.PinNum, gpin_data.property.id, &gpin_data.data))
		{
			printk("GPIO get property failed\n");
			return -1;
		}

		if(copy_to_user((void __user *)arg, (void*) &gpin_data, sizeof(Gpio_data_t)))
			return -EFAULT;
		break;
	default :
		printk ( "Invalid Gpio Function\n");
		return -EINVAL;
	}
       return 0;

}

#ifndef USE_UNLOCKED_IOCTL 
static int     
gpio_ioctl(struct inode * inode, struct file * file, unsigned int cmd,unsigned long arg)
{
	return gpio_ioctlUnlocked(file,cmd,arg);
}
#endif

/* ----- Driver registration ---------------------------------------------- */

static gpio_core_funcs_t gpio_core_funcs = {
	.process_gpio_intr = process_gpio_intr,
	.wakeup_intr_queue = wakeup_intr_queue,
};

static core_hal_t gpio_core_hal = {
	.owner		           = THIS_MODULE,
	.name		           = "GPIO CORE",
	.dev_type              = EDEV_TYPE_GPIO,
	.register_hal_module   = register_gpio_hal_module,
	.unregister_hal_module = unregister_gpio_hal_module,
	.pcore_funcs           = (void *)&gpio_core_funcs
};


/*
 * GPIO driver init function
 */
int __init
gpio_init(void)
{
    int ret =0 ;
   
   	/* gpio device initialization */ 
	if ((ret = register_chrdev_region (gpio_devno, GPIO_MAX_DEVICES, GPIO_DEV_NAME)) < 0)
	{
	   printk (KERN_ERR "failed to register gpio device <%s> (err: %d)\n", GPIO_DEV_NAME, ret);
	   return ret;
	}
   
	gpio_cdev = cdev_alloc ();
	if (!gpio_cdev)
	{
	   unregister_chrdev_region (gpio_devno, GPIO_MAX_DEVICES);
	   printk (KERN_ERR "%s: failed to allocate gpio cdev structure\n", GPIO_DEV_NAME);
	   return -1;
	}
   
	cdev_init (gpio_cdev, &gpio_ops);
	
	gpio_cdev->owner = THIS_MODULE;
	
	if ((ret = cdev_add (gpio_cdev, gpio_devno, GPIO_MAX_DEVICES)) < 0)
	{
		cdev_del (gpio_cdev);
		unregister_chrdev_region (gpio_devno, GPIO_MAX_DEVICES);
		printk	(KERN_ERR "failed to add <%s> char device\n", GPIO_DEV_NAME);
		ret = -ENODEV;
		return ret;
	}

	if ((ret = register_core_hal_module (&gpio_core_hal)) < 0)
	{
		printk(KERN_ERR "failed to register the Core GPIO module\n");
		cdev_del (gpio_cdev);
		unregister_chrdev_region (gpio_devno, GPIO_MAX_DEVICES);
		ret = -EINVAL;
		return ret;
	}

	printk("The GPIO Driver is loaded successfully.\n" );
    return 0;
 
}


module_init(gpio_init);
module_exit(gpio_exit);


int gpio_core_loaded =1;
EXPORT_SYMBOL(gpio_core_loaded);
