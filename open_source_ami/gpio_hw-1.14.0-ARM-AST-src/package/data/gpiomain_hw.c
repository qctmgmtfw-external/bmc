/*****************************************************
 **                                                            				**
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            				**
 **            All Rights Reserved.                            			**
 **                                                            				**
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             	**
 **                                                            				**
 **        Georgia - 30093, USA. Phone-(770)-246-8600.          **
 **                                                            				**
 *****************************************************/

 /*
 * 	GPIO Hardware Driver
 *
 * 	Hardware layer driver of GPIO for ast2050
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <mach/platform.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cacheflush.h>
#include "helper.h"
#include "driver_hal.h"
#include "gpio_hw.h"
#include "gpio_pins.h"
#include "gpio.h"

static int m_dev_id = 0;

static uint8_t GPIO_IRQ_ENABLED = 0;

/* ------------------------------------------------------------------------- *
 *  Function prototypes
 * ------------------------------------------------------------------------- */

static int __init gpio_hw_init (void);
static void __exit gpio_hw_exit (void);

static irqreturn_t gpio_intr_handler( int this_irq, void *dev_id);
static u32    gpio_read_reg  (uint32_t offset) ;
static void  gpio_write_reg (uint32_t value, uint32_t offset);



/* ------------------------------------------------------------------------- *
 * private data
 * ------------------------------------------------------------------------- */


static gpio_dev_t gpio_dev;

static gpio_core_funcs_t *pgpio_core_funcs;

static gpio_hal_operations_t gpio_hw_ops = {
	ast2050_readgpio,	
	ast2050_writegpio,
	ast2050_getdir,
	ast2050_setdir,
    ast2050_getpol,
    ast2050_setpol,
    ast2050_getpull_up_down,
    ast2050_setpull_up_down,
	ast2050_regsensorints,
	ast2050_unregsensorints,
	ast2050_regchassisints,
	ast2050_unregchassisints
};

static hw_hal_t hw_hal = {
	.dev_type = EDEV_TYPE_GPIO,
	.owner = THIS_MODULE,
	.devname = GPIO_HW_DEV_NAME,
	.num_instances = MAX_GPIO_HW_PORT,
	.phal_ops = (void *)&gpio_hw_ops
};


static int __init gpio_hw_init (void)
{
	int ret,err;
	gpio_dev_t	*dev = &gpio_dev;

	//pgpio_core_funcs = (gpio_core_funcs_t*) kmalloc (sizeof(gpio_core_funcs_t), GFP_KERNEL);
	//if (!pgpio_core_funcs)
		//return -ENOMEM;

	if ((ret = register_hw_hal_module (&hw_hal, (void **)&pgpio_core_funcs)) < 0)
	{
		printk (KERN_ERR "%s: failed to register gpio hal module\n", GPIO_HW_DEV_NAME);
		return ret;
	}
	m_dev_id = ret;

	//gpio = kmalloc(sizeof(gpio_dev_t), GFP_KERNEL); 

	//dev->ast2050_gpio_addr = ioremap_nocache(GPIO_BASE_OFFS, GPIO_SIZE);
	if((dev->ast2050_gpio_addr = ioremap_nocache(GPIO_BASE_OFFS, GPIO_SIZE))<0)
	{
		printk("failed to map GPIO  IO space %08x-%08x to memory",GPIO_BASE_OFFS, GPIO_BASE_OFFS + GPIO_SIZE - 1);
        ret = -EIO;
        goto fail;
	}
	
	 /* check gpio memory region */
    if ((err = check_mem_region(GPIO_BASE_OFFS, GPIO_SIZE)) < 0)  
	{
		printk("GPIO IO space %08x-%08x already in use (err %d)",GPIO_BASE_OFFS, GPIO_BASE_OFFS + GPIO_SIZE - 1, err);
		ret = -EBUSY;
		goto fail;
    }
    /* request for the memory region */	
    if (request_mem_region(GPIO_BASE_OFFS, GPIO_SIZE, "gpio")  == NULL) 
	{ 
		printk("IO space %08x-%08x gpio request mem region failed (err %d)",GPIO_BASE_OFFS, GPIO_BASE_OFFS + GPIO_SIZE - 1, err);
 	    ret = -ENODEV;
		goto fail;
	}

	//printk ("gpio_hw ptr: %p\n", pgpio_core_funcs);	

	printk("GPIO HW Driver, (c) 2009 American Megatrends Inc.\n");

	return 0;
fail:
	gpio_hw_exit();
	return -1;
}


/* Gpio  Device Functions */

static void __exit gpio_hw_exit (void)
{
	//disable_gpio_interrupt();
	gpio_dev_t *dev = &gpio_dev;

	unregister_hw_hal_module (EDEV_TYPE_GPIO, m_dev_id);

	release_mem_region(GPIO_BASE_OFFS, GPIO_SIZE);
	
	if (dev->ast2050_gpio_addr) iounmap(dev->ast2050_gpio_addr);
		   dev->ast2050_gpio_addr = NULL;

	return;
}


/*
 * ast2050_readgpio
 * Reads a gpio pin
 */
int ast2050_readgpio (void* gpin_data  )
{
	int	bank,tpin;
	volatile uint32_t  Addr;
	volatile uint32_t  Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t	*dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = gpindata->PinNum / 8;
	tpin = gpindata->PinNum % 32;

#ifdef SOC_AST2300	
	if(bank >= BANK_EXTSGPIO)
	{
		ast2300_enable_serial_gpio();
		tpin = (gpindata->PinNum - SGPIO_START_PIN)%32;
		if(bank >= 23)
			Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_DATA_OFFSET;
		else
			Addr = (uint32_t) dev->ast2050_gpio_addr + SGPIO_DATA_OFFSET;
	}
	else if(bank > BANK_GPIO_DIR_Q_S)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_Q_S;
	else if(bank > BANK_GPIO_DIR_M_P)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_M_P;
	else if(bank > BANK_GPIO_DATA_I_L)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_I_L;
	else if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DATA_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_OFFSET;
#else
	if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DATA_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_OFFSET;

#endif

	//printk ("Addr to readdata :0x%lx Bit:0x%lx\n", addr_map.data_reg_addr, 1 << addr_map.bit_to_enable);

	/* Get the particular bit */
	Value = gpio_read_reg ( Addr );
	Value &=  ( 0x01 << tpin );
	Value >>= tpin ;
	gpindata->data = Value;

	//printk ("data is :%d\n", gpindata->data);

	return 0;
}


/*
 * ast2050_writegpio
 * Writes to a gpio pin
 */
int ast2050_writegpio (void *gpin_data  )
{
	int	bank,tpin;
	volatile uint32_t  Addr;
	volatile uint32_t  Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t	*dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = gpindata->PinNum / 8;
	tpin = gpindata->PinNum % 32;

#ifdef SOC_AST2300	
	if(bank >= BANK_EXTSGPIO)
	{
		ast2300_enable_serial_gpio();
		tpin = (gpindata->PinNum - SGPIO_START_PIN)%32;	
		if(bank >= 23)
			Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_DATA_OFFSET;
		else
			Addr = (uint32_t) dev->ast2050_gpio_addr + SGPIO_DATA_OFFSET;
	}
	else if(bank > BANK_GPIO_DIR_Q_S)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_Q_S;
	else if(bank > BANK_GPIO_DIR_M_P)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_M_P;
	else if(bank > BANK_GPIO_DATA_I_L)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_I_L;
	else if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DATA_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_OFFSET;

	if(bank >= BANK_EXTSGPIO)
	{
		if(bank >= 23)
			Value = gpio_read_reg ((uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_DATA_READ_OFFSET );	
		else
			Value = gpio_read_reg ((uint32_t) dev->ast2050_gpio_addr + SGPIO_DATA_READ_OFFSET );	
	}
	else
		Value = gpio_read_reg ( Addr );
#else
	if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DATA_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_OFFSET;

	Value = gpio_read_reg ( Addr );
#endif
	if ( gpindata->data )
		Value = Value | ( 0x01 << tpin );
	else
		Value = Value & ~ ( 0x01 << tpin );

	gpio_write_reg ( Value, Addr );

	return 0;
}


/*
 * ast2050_getdir
 * Get direction of the pin
 */
int ast2050_getdir (void *gpin_data)
{
	int	bank,tpin;
	volatile uint32_t  Addr;
	volatile uint32_t  Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t	*dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = gpindata->PinNum / 8;
	tpin = gpindata->PinNum % 32;

#ifdef SOC_AST2300	
	if(bank > BANK_GPIO_DIR_Q_S)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DIR_Q_S;
	else if(bank > BANK_GPIO_DIR_M_P)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DIR_M_P;
	else if(bank > BANK_GPIO_DATA_I_L)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DIR_I_L;
	else if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_PIN_DIR_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_PIN_DIR_OFFSET;
#else
	if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_PIN_DIR_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_PIN_DIR_OFFSET;

#endif
	//printk ("Addr to read:0x%lx 0x%lx\n", addr_map.dir_reg_addr, 1 << addr_map.bit_to_enable);
	/* Get the Pin dir */

	Value  = gpio_read_reg ( Addr );
	Value  = Value & (1 << tpin);
	Value = Value >> tpin;
	gpindata->data = Value;
	
	//printk ("direction is :%d\n", gpindata->data);
	return 0;
}



/*
 * ast2050_setdir
 * Set direction of the pin
 */
int ast2050_setdir (void *gpin_data)
{
	int	bank,tpin;
	volatile uint32_t  Addr;
	volatile uint32_t  Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t	*dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = gpindata->PinNum / 8;
	tpin = gpindata->PinNum % 32;

#ifdef SOC_AST2300		
	if(bank > BANK_GPIO_DIR_Q_S)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DIR_Q_S;
	else if(bank > BANK_GPIO_DIR_M_P)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DIR_M_P;
	else if(bank > BANK_GPIO_DATA_I_L)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DIR_I_L;
	else if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_PIN_DIR_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_PIN_DIR_OFFSET;
#else
	if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_PIN_DIR_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_PIN_DIR_OFFSET;
#endif

	//printk ("Addr to readdata :0x%lx Bit:0x%lx\n", addr_map.data_reg_addr, 1 << addr_map.bit_to_enable);
	Value = gpio_read_reg ( Addr );

	if ( gpindata->data )
		Value = Value | ( 0x01 << tpin );
	else
		Value = Value & ~ ( 0x01 << tpin );

	gpio_write_reg ( Value, Addr );

	return 0;
}

/*
 * ast2050_getpol
 * Get polarity of the pin
 */
int ast2050_getpol (void *gpin_data)
{
    return -1;
}

/*
 * ast2050_setpol
 * Set polarity of the pin
 */
int ast2050_setpol (void *gpin_data)
{
    return -1;
}

/*
 * ast2050_getpull_up_down
 * Get pull-up/pull-down control bits
 */
int ast2050_getpull_up_down        ( void *gpin_data  )
{
    return -1;
}

/*
 * ast2050_setpull_up_down
 * Set pull-up/pull-down control bits
 */
int ast2050_setpull_up_down        ( void *gpin_data  )
{
    return -1;
}


static irqreturn_t gpio_intr_handler( int this_irq, void *dev_id)
{
	volatile uint32_t intr_state_ast2050_value = 0;
	uint32_t current_ast2050_intrs = 0;
	uint32_t value = 0;
	uint32_t i = 0;
	gpio_dev_t *gpio = &gpio_dev;
	int intr_type = 0;

	/* Determine the reason for this interrupt */
	if(this_irq == IRQ_GPIO)
	{
	    if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_EN_OFFSET))
	    {
		/* ast2050 GPIO portA to portD related interrupt */
		intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_STATUS_OFFSET);
		/* Handle the interrupts */
		for(i = 0;i <= GPIO_31;i++)
		{
			if(intr_state_ast2050_value & 0x00000001)
			{
				value = 1 << i;
				current_ast2050_intrs |= value;

				//common module's callback interrupt handler
				intr_type = pgpio_core_funcs->process_gpio_intr(i, 0);
			}
			intr_state_ast2050_value >>= 1;
		}
	    }
		/* Clear all the interrupts now */
		gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_STATUS_OFFSET);

	    if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_EN_OFFSET))
	    {
		/* ast2050 GPIO portE to portH related interrupt */
		intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_STATUS_OFFSET);
		/* Handle the interrupts */
		for(i = 0;i <= GPIO_31;i++)
		{
			if(intr_state_ast2050_value & 0x00000001)
			{
				value = 1 << i;
				current_ast2050_intrs |= value;
				
				//common module's callback interrupt handler
				intr_type = pgpio_core_funcs->process_gpio_intr(i, 32);				
			}
			intr_state_ast2050_value >>= 1;
		}
	    }
		/* Clear all the interrupts now */
		gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_STATUS_OFFSET);

#ifdef SOC_AST2300
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portI to portL related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_STATUS_OFFSET);
			/* Handle the interrupts */
			for(i = 0;i <= GPIO_31;i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					value = 1 << i;
					current_ast2050_intrs |= value;
					
					//common module's callback interrupt handler
					intr_type = pgpio_core_funcs->process_gpio_intr(i, 64); 			
				}
				intr_state_ast2050_value >>= 1;
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_STATUS_OFFSET);
	
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portM to portP related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_STATUS_OFFSET);
			/* Handle the interrupts */
			for(i = 0;i <= GPIO_31;i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					value = 1 << i;
					current_ast2050_intrs |= value;
					
					//common module's callback interrupt handler
					intr_type = pgpio_core_funcs->process_gpio_intr(i, 96); 			
				}
				intr_state_ast2050_value >>= 1;
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_STATUS_OFFSET);
	
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portQ to portS related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_STATUS_OFFSET);
			/* Handle the interrupts */
			for(i = 0;i <= GPIO_23/*GPIO_31*/;i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					value = 1 << i;
					current_ast2050_intrs |= value;
					
					//common module's callback interrupt handler
					intr_type = pgpio_core_funcs->process_gpio_intr(i, 128);				
				}
				intr_state_ast2050_value >>= 1;
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_STATUS_OFFSET);
#endif
	} /* end of IRQ_GPIO */

#ifdef SOC_AST2300
	if(this_irq == IRQ_SGPIO_MASTER)
	{
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_EN_OFFSET))
		{
			/* ast2050 Serial GPIO portA to portD related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_STATUS_OFFSET);
			/* Handle the interrupts */
			for(i = 0;i <= GPIO_31;i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					value = 1 << i;
					current_ast2050_intrs |= value;
					
					//common module's callback interrupt handler
					intr_type = pgpio_core_funcs->process_gpio_intr(i, 152); 			
				}
				intr_state_ast2050_value >>= 1;
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_STATUS_OFFSET);

		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_EN_OFFSET))
		{
			/* ast2050 Extended Serial GPIO portE to portH related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_STATUS_OFFSET);
			/* Handle the interrupts */
			for(i = 0;i <= GPIO_31;i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					value = 1 << i;
					current_ast2050_intrs |= value;
					
					//common module's callback interrupt handler
					intr_type = pgpio_core_funcs->process_gpio_intr(i, 184); 			
				}
				intr_state_ast2050_value >>= 1;
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_STATUS_OFFSET);
	} /* end of IRQ_SGPIO_MASTER */
#endif

	pgpio_core_funcs->wakeup_intr_queue(intr_type);

	return( IRQ_HANDLED );
}

int ast2050_regsensorints ( void* gpin_data, uint32_t total_interrupt_sensors, void* sensor_itr)
{
	int	bank = 0;
	int tpin = 0;
	gpio_dev_t *gpio = &gpio_dev;
	gpio_interrupt_sensor* intr_sensors = (gpio_interrupt_sensor *) sensor_itr;
	uint32_t i = 0;
	volatile uint32_t int_enable_ast2050_Addr;
	volatile uint32_t int_riseneg_ast2050_Addr;
	volatile uint32_t int_trigger_ast2050_Addr;
	volatile uint32_t int_both_ast2050_Addr;
	volatile uint32_t int_enable_ast2050_value;
	volatile uint32_t int_riseneg_ast2050_value;
	volatile uint32_t int_trigger_ast2050_value;
	volatile uint32_t int_both_ast2050_value;
	uint32_t set_value = 0;
	uint32_t clr_value = 0;

//	printk("total sensors = %d\n",total_interrupt_sensors);
	for(i = 0; i < total_interrupt_sensors; i++)
	{
		bank = intr_sensors[i].gpio_number / 8;
		tpin = intr_sensors[i].gpio_number % 32;

		#ifdef SOC_AST2300
			if(bank >= BANK_EXTSGPIO)
			{	
				ast2300_enable_serial_gpio();
				tpin = (intr_sensors[i].gpio_number - SGPIO_START_PIN)%32;
				if(bank >= 23)
					{
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE2_OFFSET;
					}
					else
					{
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE2_OFFSET;
					}
			}else if(bank > BANK_GPIO_DIR_Q_S)
			{
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE2_OFFSET;
			}else if(bank > BANK_GPIO_DIR_M_P)
			{
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE2_OFFSET;
			}else if(bank > BANK_GPIO_DATA_I_L)
			{
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE2_OFFSET;
			}else if(bank > BANK_EXTGPIO_DATA_OFFSET)
			{
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
			}
			else
			{
				int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_EN_OFFSET;
				int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE0_OFFSET;
				int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE1_OFFSET;
				int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE2_OFFSET;
			}

		#else
			if(bank > BANK_EXTGPIO_DATA_OFFSET)
			{
				int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
				int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
				int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
				int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
			}
			else
			{
				int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_EN_OFFSET;
				int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE0_OFFSET;
				int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE1_OFFSET;
				int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE2_OFFSET;
			}
		#endif


		int_enable_ast2050_value 	= gpio_read_reg(int_enable_ast2050_Addr);
		int_riseneg_ast2050_value	= gpio_read_reg(int_riseneg_ast2050_Addr);
		int_trigger_ast2050_value	= gpio_read_reg(int_trigger_ast2050_Addr);
		int_both_ast2050_value		= gpio_read_reg(int_both_ast2050_Addr);

		set_value = (1 << tpin);
		clr_value = ~(1 << tpin);

		/* Set the Interrupt Enable register bit */

		/* Set the interrupt trigger method/type */
		if(intr_sensors[i].trigger_method == GPIO_EDGE)
		{
			int_trigger_ast2050_value &= clr_value;
			if(intr_sensors[i].trigger_type == GPIO_FALLING_EDGE)
				int_riseneg_ast2050_value &= clr_value;
			else if(intr_sensors[i].trigger_type == GPIO_RISING_EDGE)
				int_riseneg_ast2050_value |= set_value;
			else if(intr_sensors[i].trigger_type == GPIO_BOTH_EDGES)
				int_both_ast2050_value |= set_value;
			else
				printk("Wrong edge trigger type for sensor int\n");
		}
		else if(intr_sensors[i].trigger_method == GPIO_LEVEL)
		{
			int_trigger_ast2050_value |= set_value;
			if(intr_sensors[i].trigger_type == GPIO_HIGH_LEVEL)
				int_riseneg_ast2050_value |= set_value;
			else if(intr_sensors[i].trigger_type == GPIO_LOW_LEVEL)
				int_riseneg_ast2050_value &= clr_value;
			else
				printk("Wrong level trigger type for sensor int\n");
		}
		else
			printk("Wrong trigger type for sensor int\n");
		int_enable_ast2050_value |= set_value;				
		
		/* Set the actual registers now */
		gpio_write_reg (int_trigger_ast2050_value, int_trigger_ast2050_Addr);
		gpio_write_reg (int_riseneg_ast2050_value, int_riseneg_ast2050_Addr);
		gpio_write_reg (int_both_ast2050_value, int_both_ast2050_Addr);
		
		/* Enable the interrupts now */
		if(int_enable_ast2050_value != 0x00)
			gpio_write_reg (int_enable_ast2050_value, int_enable_ast2050_Addr);
		
	}

	if(!GPIO_IRQ_ENABLED)
	{
		if( request_irq(IRQ_GPIO, gpio_intr_handler, 0, "gpio-ast2050", NULL) < 0 )
		{
			printk( KERN_ERR "Request for irq %d failed\n", IRQ_GPIO );
			return( -ENODEV );
		}
		else
		{
   			IRQ_SET_HIGH_LEVEL (IRQ_GPIO);
   			IRQ_SET_LEVEL_TRIGGER (IRQ_GPIO);
			GPIO_IRQ_ENABLED = 1;
		}

	#ifdef SOC_AST2300
		if(bank >= BANK_EXTSGPIO)
		{
			if( request_irq(IRQ_SGPIO_MASTER, gpio_intr_handler, 0, "gpio-ast2050", NULL) < 0 )
			{
				printk( KERN_ERR "Request for irq %d failed\n", IRQ_SGPIO_MASTER );
				return( -ENODEV );
			}else{
			EXT_IRQ_SET_HIGH_LEVEL (IRQ_SGPIO_MASTER);
			EXT_IRQ_SET_LEVEL_TRIGGER (IRQ_SGPIO_MASTER);
			GPIO_IRQ_ENABLED = 1;
			}
		}
	#endif
	}

	return 0; 
}

int ast2050_unregsensorints (  void* gpin_data)
{
	gpio_dev_t *gpio = &gpio_dev;

	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_SEN_TYPE2_OFFSET);

#ifdef SOC_AST2300
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_SEN_TYPE2_OFFSET);		

	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_SEN_TYPE2_OFFSET);
#endif

	if(GPIO_IRQ_ENABLED != 0)
	{
		disable_irq(IRQ_GPIO);
		free_irq(IRQ_GPIO,NULL);
	#ifdef SOC_AST2300
		disable_irq(IRQ_SGPIO_MASTER);
		free_irq(IRQ_SGPIO_MASTER,NULL);
	#endif
		GPIO_IRQ_ENABLED = 0;
	}

	return 0; 
}



/********************** Chassis Interrupts ***********************/
int ast2050_regchassisints  ( void* gpin_data, uint32_t total_chassis_interrupts,  void* chassis_itr )
{
	int	bank,tpin;
	gpio_dev_t *gpio = &gpio_dev;
	uint32_t i = 0;
	volatile uint32_t int_enable_ast2050_Addr;
	volatile uint32_t int_riseneg_ast2050_Addr;
	volatile uint32_t int_trigger_ast2050_Addr;
	volatile uint32_t int_both_ast2050_Addr;
	volatile uint32_t int_enable_ast2050_value;
	volatile uint32_t int_riseneg_ast2050_value;
	volatile uint32_t int_trigger_ast2050_value;
	volatile uint32_t int_both_ast2050_value;
	gpio_interrupt_sensor* intr_chassis = (gpio_interrupt_sensor *) chassis_itr;
	uint32_t set_value = 0;
	uint32_t clr_value = 0;

	//printk("total chassis interrupts = %d\n",total_chassis_interrupts);
	for(i = 0; i < total_chassis_interrupts; i++)
	{
		bank = intr_chassis[i].gpio_number / 8;
		tpin = intr_chassis[i].gpio_number % 32;

		#ifdef SOC_AST2300
			if(bank >= BANK_EXTSGPIO)
			{	
				ast2300_enable_serial_gpio();
				tpin = (intr_chassis[i].gpio_number - SGPIO_START_PIN)%32;
				if(bank >= 23)
				{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE2_OFFSET;
				}
				else
				{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE2_OFFSET;
				}
			}else if(bank > BANK_GPIO_DIR_Q_S)
			{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE2_OFFSET;
			}else if(bank > BANK_GPIO_DIR_M_P)
			{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE2_OFFSET;
			}else if(bank > BANK_GPIO_DATA_I_L)
			{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE2_OFFSET;
			}else if(bank > BANK_EXTGPIO_DATA_OFFSET)
			{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
			}
			else
			{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE2_OFFSET;
			}
		#else
				if(bank > BANK_EXTGPIO_DATA_OFFSET)
				{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
				}
				else
				{
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE2_OFFSET;
				}
		#endif

		
		int_enable_ast2050_value 	= gpio_read_reg(int_enable_ast2050_Addr);
		int_riseneg_ast2050_value	= gpio_read_reg(int_riseneg_ast2050_Addr);
		int_trigger_ast2050_value	= gpio_read_reg(int_trigger_ast2050_Addr);
		int_both_ast2050_value		= gpio_read_reg(int_both_ast2050_Addr);

		//printk("sensor num: %d, gpio num: %d\n",intr_chassis[i].sensor_number,gpio_num);
		set_value = (1 << tpin);
		clr_value = ~(1 << tpin);

		/* Set the Interrupt Enable register bit */

		/* Set the interrupt trigger method/type */
		if(intr_chassis[i].trigger_method == GPIO_EDGE)
		{
			int_trigger_ast2050_value &= clr_value;
			if(intr_chassis[i].trigger_type == GPIO_FALLING_EDGE)
				int_riseneg_ast2050_value &= clr_value;
			else if(intr_chassis[i].trigger_type == GPIO_RISING_EDGE)
				int_riseneg_ast2050_value |= set_value;
			else if(intr_chassis[i].trigger_type == GPIO_BOTH_EDGES)
				int_both_ast2050_value |= set_value;
			else
				printk("Wrong edge trigger type for chassis int\n");
		}
		else if(intr_chassis[i].trigger_method == GPIO_LEVEL)
		{
			int_trigger_ast2050_value |= set_value;
			if(intr_chassis[i].trigger_type == GPIO_HIGH_LEVEL)
				int_riseneg_ast2050_value |= set_value;
			else if(intr_chassis[i].trigger_type == GPIO_LOW_LEVEL)
				int_riseneg_ast2050_value &= clr_value;
			else
				printk("Wrong level trigger type for chassis int\n");
		}
		else
			printk("Wrong trigger type for sensor int\n");
		int_enable_ast2050_value |= set_value;

		/* Set the actual registers now */
		gpio_write_reg (int_trigger_ast2050_value, int_trigger_ast2050_Addr);
		gpio_write_reg (int_riseneg_ast2050_value, int_riseneg_ast2050_Addr);
		gpio_write_reg (int_both_ast2050_value, int_both_ast2050_Addr);
		/* Enable the interrupts now */
		if(int_enable_ast2050_value != 0x00)
			gpio_write_reg (int_enable_ast2050_value, int_enable_ast2050_Addr);
	}

	if(!GPIO_IRQ_ENABLED)
	{
		if( request_irq(IRQ_GPIO, gpio_intr_handler, 0, "gpio-ast2050", NULL) < 0 )
		{
			printk( KERN_ERR "Request for irq %d failed\n", IRQ_GPIO );
			return( -ENODEV );
		}
		else
		{
   			IRQ_SET_HIGH_LEVEL (IRQ_GPIO);
   			IRQ_SET_LEVEL_TRIGGER (IRQ_GPIO);
			GPIO_IRQ_ENABLED = 1;
		}
#ifdef SOC_AST2300

		if( request_irq(IRQ_SGPIO_MASTER, gpio_intr_handler, 0, "gpio-ast2050", NULL) < 0 )
		{
			printk( KERN_ERR "Request for irq %d failed\n", IRQ_SGPIO_MASTER );
			return( -ENODEV );
		}
		else
		{
//   			IRQ_SET_HIGH_LEVEL (IRQ_SGPIO_MASTER);
  // 			IRQ_SET_LEVEL_TRIGGER (IRQ_SGPIO_MASTER);
			GPIO_IRQ_ENABLED = 1;
		}
#endif
	}
	return 0; 
}

int ast2050_unregchassisints (  void* gpin_data)
{
	gpio_dev_t *gpio = &gpio_dev;

	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_SEN_TYPE2_OFFSET);

#ifdef SOC_AST2300
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_SEN_TYPE2_OFFSET);		

	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_SEN_TYPE2_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_EN_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_SEN_TYPE0_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_SEN_TYPE1_OFFSET);
	gpio_write_reg(0,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_SEN_TYPE2_OFFSET);
#endif

	if(GPIO_IRQ_ENABLED != 0)
	{
		disable_irq(IRQ_GPIO);
		free_irq(IRQ_GPIO,NULL);
	#ifdef SOC_AST2300
		disable_irq(IRQ_SGPIO_MASTER);
		free_irq(IRQ_SGPIO_MASTER,NULL);
	#endif
		GPIO_IRQ_ENABLED = 0;
	}

	return 0; 
}

#ifdef SOC_AST2300
void ast2300_enable_serial_gpio(void)
{
	volatile uint32_t  Addr;
	volatile uint32_t  Value;
	uint32_t reg;
	gpio_dev_t	*dev = &gpio_dev;

	iowrite32(AST_SCU_UNLOCK_MAGIC, SCU_KEY_CONTROL_REG); /* unlock SCU */
	reg = ioread32(AST_SCU_VA_BASE + AST_SCU_MULTI_FUNC_2);	
	reg |= AST_SCU_MULTI_PIN_SGPIO;
	iowrite32(reg, AST_SCU_VA_BASE + AST_SCU_MULTI_FUNC_2);
	iowrite32(0, SCU_KEY_CONTROL_REG);	/* lock SCU */
	Addr = (uint32_t) dev->ast2050_gpio_addr + SGPIO_CONF_OFFSET;
	Value = 0x80201;
	gpio_write_reg (Value, Addr);
}
#endif

/*
 * gpio_read_reg
 * reads a byte from the offset
 */
static u32 
gpio_read_reg(uint32_t offset)
{
    return (ioread32(offset));
}

/*
 * gpio_write_reg
 * writes a byte to the offset
 */
static void
gpio_write_reg(uint32_t value, uint32_t offset)
{
    iowrite32(value, offset);
}

MODULE_AUTHOR("American Megatrends");
MODULE_DESCRIPTION("ast2050 GPIO driver");
MODULE_LICENSE("GPL");

module_init(gpio_hw_init);
module_exit(gpio_hw_exit);

