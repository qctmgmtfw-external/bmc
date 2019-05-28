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
 * 	GPIO Hardware Driver
 *
 * 	Hardware layer driver of GPIO for ASPEED AST SoC
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
void *scu_virt_base = NULL;

static uint8_t GPIO_IRQ_ENABLED = 0;
#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
static uint8_t GPIO_SGPIO_ENABLED = 0;
#endif

gpio_interrupt_sensor* hw_intr_sensors = NULL;
gpio_interrupt_sensor* hw_intr_chassis = NULL;
uint32_t hw_total_interrupt_sensors = 0;
uint32_t hw_total_interrupt_chassis = 0;

#ifdef CONFIG_SPX_FEATURE_ENABLE_GPIO_INTERRUPTIBLE
DECLARE_WAIT_QUEUE_HEAD(gpio_interrupt_lock);
EXPORT_SYMBOL(gpio_interrupt_lock);
int gpio_interrupt_hit = 0;
EXPORT_SYMBOL(gpio_interrupt_hit);
#endif

#define SCU_BASE_OFFS  0x1E6E2000
#define SCU_REG_SIZE  SZ_4K
#define SCU_PULL_UP_DOWN_OFFSET 0x8C
#define UN_KNOWN 0xFF
#define GPIO_PULL_DOWN_ENABLE 0
#define GPIO_PULL_DOWN_DISABLE 3

/* ------------------------------------------------------------------------- *
 *  Function prototypes
 * ------------------------------------------------------------------------- */

static int __init gpio_hw_init (void);
static void __exit gpio_hw_exit (void);

static irqreturn_t gpio_intr_handler( int this_irq, void *dev_id);
static u32   gpio_read_reg  (uint32_t offset);
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
	ast2050_unregchassisints,
	NULL,	/* setcontmode */
	NULL,	/* get_sgpio_bus_config */ 
	NULL,	/* set_sgpio_bus_config */
	NULL,	/* cleanup */
	NULL, 	// EnableGpioToggle 
	NULL,	// set_gpio_property
	NULL,	// get_gpio_property
	ast2050_read_debounce/*NULL Quanta*/,	// get_debounce_event
	ast2050_write_debounce/*NULL Quanta*/,	// set_debounce_event
	NULL,	// reg_reading_on_ints
	NULL,	// get_reading_on_ints
	ast2050_readgpios,
	ast2050_writegpios,
    ast2050_setgpiosdir,
    ast2050_getgpiosdir,
	NULL,   // get_debounce_clock
	NULL    // set_debounce_clock
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
	gpio_dev_t *dev = &gpio_dev;

	extern int gpio_core_loaded;
	if (!gpio_core_loaded)
			return -1;

	//pgpio_core_funcs = (gpio_core_funcs_t*) kmalloc (sizeof(gpio_core_funcs_t), GFP_KERNEL);
	//if (!pgpio_core_funcs)
	//	return -ENOMEM;

	if ((ret = register_hw_hal_module (&hw_hal, (void **)&pgpio_core_funcs)) < 0) {
		printk (KERN_ERR "%s: failed to register gpio hal module\n", GPIO_HW_DEV_NAME);
		return ret;
	}
	m_dev_id = ret;

	//gpio = kmalloc(sizeof(gpio_dev_t), GFP_KERNEL); 

	if ((dev->ast2050_gpio_addr = ioremap_nocache(GPIO_BASE_OFFS, GPIO_SIZE)) < 0) {
		printk("failed to map GPIO  IO space %08x-%08x to memory",GPIO_BASE_OFFS, GPIO_BASE_OFFS + GPIO_SIZE - 1);
		ret = -EIO;
		goto fail;
	}

	 /* check gpio memory region */
	if ((err = check_mem_region(GPIO_BASE_OFFS, GPIO_SIZE)) < 0) {
		printk("GPIO IO space %08x-%08x already in use (err %d)",GPIO_BASE_OFFS, GPIO_BASE_OFFS + GPIO_SIZE - 1, err);
		ret = -EBUSY;
		goto fail;
	}

	/* request for the memory region */	
	if (request_mem_region(GPIO_BASE_OFFS, GPIO_SIZE, "gpio") == NULL) { 
		printk("IO space %08x-%08x gpio request mem region failed (err %d)",GPIO_BASE_OFFS, GPIO_BASE_OFFS + GPIO_SIZE - 1, err);
		ret = -ENODEV;
		goto fail;
	}

	if ( (scu_virt_base = ioremap(SCU_BASE_OFFS, SCU_REG_SIZE)) < 0)
	{
		printk("SCU IO space %08x-%08x already in use (err %d)", SCU_BASE_OFFS, SCU_BASE_OFFS + SCU_REG_SIZE - 1, err);
		ret = -EBUSY;
		goto fail;
	}
	//printk ("gpio_hw ptr: %p\n", pgpio_core_funcs);	

	printk("APSEED AST SoC GPIO HW Driver, (c) 2009-2015 American Megatrends Inc.\n");

	return 0;
fail:
	gpio_hw_exit();
	return -1;
}


/* Gpio  Device Functions */

static void __exit gpio_hw_exit (void)
{
	volatile uint32_t Addr;
	volatile uint32_t Value = 0x00000000;
	gpio_dev_t *dev = &gpio_dev;

	// disable interrupt
	if(GPIO_IRQ_ENABLED != 0)
	{
		free_irq(IRQ_GPIO,dev->ast2050_gpio_addr);
		if(hw_intr_sensors != NULL)
			kfree(hw_intr_sensors);
		if(hw_intr_chassis != NULL)
			kfree(hw_intr_chassis);
		#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
		free_irq(IRQ_SGPIO_MASTER,dev->ast2050_gpio_addr);
		#endif
		GPIO_IRQ_ENABLED = 0;
		hw_total_interrupt_sensors = 0;
		hw_total_interrupt_chassis = 0;
	}

	// clear all interrupt
	Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
	Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_0_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
	Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_1_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
	Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_2_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
	Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_3_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
	Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_4_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
#endif
#if defined(GROUP_AST2500)
	Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_5_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
#endif
	Addr = (uint32_t) dev->ast2050_gpio_addr + SGPIO_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
	Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
    Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_1_INT_EN_OFFSET;
	gpio_write_reg ( Value, Addr );
#endif
#endif

	// free all memory
	unregister_hw_hal_module (EDEV_TYPE_GPIO, m_dev_id);
	release_mem_region(GPIO_BASE_OFFS, GPIO_SIZE);

	if (scu_virt_base) iounmap(scu_virt_base);
	scu_virt_base = NULL;

	if (dev->ast2050_gpio_addr) iounmap(dev->ast2050_gpio_addr);
	dev->ast2050_gpio_addr = NULL;

	return;
}

//Quanta++
/*
 * ast2050_read_debounce
 * Reads a gpio debounce setting
 */
int ast2050_read_debounce (void* gpin_data  )
{
	int	bank,tpin;
	volatile uint32_t  Addr1, Addr2, setting;
	volatile uint32_t  Value1, Value2;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t	*dev = &gpio_dev;
	printk ("[ast2050_read_debounce] PinNum %d\n",gpindata->PinNum);
//	if ( gpindata->PinNum > 63 ) 
//	{
//		printk ("PinNum fail\n");
//		return -1;  // only GPIO port A,B,C,D,E,F,G,H support debounce feature in AST2300
//	}

	/* Get the register Addr  */
	bank = gpindata->PinNum / 8;
	tpin = gpindata->PinNum % 32;

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	if(bank > BANK_EXTGPIO_DATA_OFFSET) {
		Addr1 = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DEBOUNCE_SET1;
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DEBOUNCE_SET2;
	}
	else {
		Addr1 = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_SET1;
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_SET2;
	}
#else
	/* AST2050 did not support debounce */
	return -1;
#endif

	/* Get the particular bit */
	Value1 = gpio_read_reg ( Addr1 );
	Value1 &=  ( 0x01 << tpin );
	Value1 >>= tpin;
	//printk (KERN_ERR "[%s %s] Addr1 0x%x readdata 0x%x\n", __FILE__, __FUNCTION__, Addr1, Value1);
	
	Value2 = gpio_read_reg ( Addr2 );
	Value2 &=  ( 0x01 << tpin );
	Value2 >>= tpin;
	//printk (KERN_ERR "[%s %s] Addr2 0x%x readdata 0x%x\n", __FILE__, __FUNCTION__, Addr2, Value2);

	setting = (Value2 << 1) | Value1;
	//printk (KERN_ERR "[%s %s] setting 0x%x\n", __FILE__, __FUNCTION__, setting);
	gpindata->gpio_debounce_data.setting = setting;

	if (setting == 1) {
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_TIME2;
		gpindata->gpio_debounce_data.time = gpio_read_reg ( Addr2 );
	}
	else if (setting == 2) {
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_TIME1;
		gpindata->gpio_debounce_data.time = gpio_read_reg ( Addr2 );
	} 
	else if (setting == 3) {
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_TIME3;
		gpindata->gpio_debounce_data.time = gpio_read_reg ( Addr2 );
	}
	else {
		//printk(KERN_ERR "[%s %s] No Debounce\n", __FILE__, __FUNCTION__);
	}

	//printk (KERN_ERR "[%s %s] Addr2 0x%x readdata 0x%x\n", __FILE__, __FUNCTION__, Addr2, (uint32_t)gpindata->gpio_debounce_data.time);

	return 0;
}

 /*
 * ast2050_write_debounce
 * Writes a gpio debounce setting
 */
int ast2050_write_debounce (void* gpin_data  )
{
	int	bank,tpin;
	volatile uint32_t  Addr, Addr1, Addr2, setting;
	volatile uint32_t  Value, Value1, Value2;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t	*dev = &gpio_dev;
	printk ("[gpiomain_hw.c] Set PinNum %d debounce.\n", gpindata->PinNum);

	/* Get the register Addr  */
	bank = gpindata->PinNum / 8;//27
	tpin = gpindata->PinNum % 32;//26

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	if(bank >= BANK_GPIO_DATA_OFFSET && bank <= BANK_GPIO_DATA_OFFSET+3 ) {// A,B,C,D   bank(00~03)
		//printk ("[ast2050_write_debounce] A,B,C,D   bank(00~03) bank =%d\n",bank);
		Addr1 = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_SET1;//Offset 40h
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_SET2;//Offset 44h
	}else if(bank > BANK_EXTGPIO_DATA_OFFSET && bank <= BANK_EXTGPIO_DATA_OFFSET+4 ){// E,F,G,H   bank(04~07)
		//printk ("[ast2050_write_debounce] E,F,G,H   bank(04~07) bank =%d\n",bank);
		Addr1 = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DEBOUNCE_SET1;//Offset 48h
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DEBOUNCE_SET2;//Offset 4Ch
	}else if(bank > BANK_SIMGPIO_1_DATA_OFFSET && bank <= BANK_SIMGPIO_1_DATA_OFFSET+4 ){//elmo+ M,N,O,P bank(12~15)
		//printk ("[ast2050_write_debounce] M,N,O,P bank(12~15) bank =%d\n",bank);
		Addr1 = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_1_DEBOUNCE_SET1;//Offset 100h
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_1_DEBOUNCE_SET2;//Offset 104h
	}else if(bank > BANK_SIMGPIO_2_DATA_OFFSET && bank <= BANK_SIMGPIO_2_DATA_OFFSET+4 ){//elmo+ Q,R,S,T bank(16~19)
		//printk ("[ast2050_write_debounce] Q,R,S,T bank(16~19) bank =%d\n",bank);
		Addr1 = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_2_DEBOUNCE_SET1;//Offset 130h
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_2_DEBOUNCE_SET2;//Offset 134h
	}else if(bank > BANK_SIMGPIO_4_DATA_OFFSET && bank <= BANK_SIMGPIO_4_DATA_OFFSET+4 ){//raincer+ Y,Z,AA,AB bank(24~27)
		//printk ("[ast2050_write_debounce] Y,Z,AA,AB bank(24~27) bank =%d\n",bank);
		Addr1 = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_4_DEBOUNCE_SET1;//Offset 190h
		Addr2 = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_4_DEBOUNCE_SET2;//Offset 194h
	}
	else {
		printk ("[ast2050_write_debounce] Undefined bank =%d\n",bank);
	}
#else
	/* AST2050 did not support debounce */
	printk("AST2050 did not support debounce in write\n");
	return -1;
#endif
	setting = ( gpindata->gpio_debounce_data.setting & 0x3 );

	Value1 = gpio_read_reg ( Addr1 );
	Value1 &=  ~( 0x01 << tpin );
	Value1 |=  ( (0x01 & setting) << tpin );
	gpio_write_reg ( Value1, Addr1 );
	//printk ("[ast2050_write_debounce] Addr1 0x%x writedata 0x%x and bank %d tpin %d setting %d\n", Addr1, Value1,bank,tpin,setting);

	Value2 = gpio_read_reg ( Addr2 );
	Value2 &=  ~( 0x01 << tpin );
	if (tpin ==0)
	{
		Value2 |=  ( (0x02 & setting) >> 1 );
	}
	else
		Value2 |=  ( (0x02 & setting) << (tpin - 1) );	
	//	Value2 |=  (0x01);
	gpio_write_reg ( Value2, Addr2 );
	//printk ("[ast2050_write_debounce] Addr2 0x%x writedata 0x%x and bank %d tpin %d setting %d\n", Addr2, Value2,bank,tpin,setting);
	//	printk (KERN_ERR "[%s %s] Addr2 0x%x writedata 0x%x\n", __FILE__, __FUNCTION__, Addr2, Value2);
	//setting definition ast2500v11 page 574
	//[DebounceSetting#2] [DebounceSetting#1]
	// 0 0 is setting 0 , no debounce
	// 1 0 is setting 2 , GPIO050 as Debounce Timer Setting Register #1
	// 0 1 is setting 1 , GPIO054 as Debounce Timer Setting Register #2
	// 1 1 is setting 3 , GPIO080 as Debounce Timer Setting Register #3
			
	if (setting == 0) {
		printk (KERN_INFO "[ast2050_write_debounce] No setting Debounce time!\n");
		//printk(KERN_ERR "[%s] No Debounce\n", __FUNCTION__);
		return 0;
	}	
	else if (setting == 1)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_TIME2;//Offset 54h
	else if (setting == 2)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_TIME1;//Offset 50h
	else if (setting == 3)
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DEBOUNCE_TIME3;//Offset 58h

	Value = gpindata->gpio_debounce_data.time;
	gpio_write_reg ( Value, Addr );
	//printk (KERN_ERR "[%s %s] Addr 0x%x writedata 0x%x\n", __FILE__, __FUNCTION__, Addr, Value);
	//printk ("set debounce in driver\n");

	return 0;
}
//Quanta--

/*
 * ast2050_readgpio
 * Reads a gpio pin
 */
int ast2050_readgpio (void* gpin_data  )
{
	int bank,tpin;
	volatile uint32_t Addr;
	volatile uint32_t Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t *dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = (gpindata->PinNum / 8);
	tpin = (gpindata->PinNum % 32);

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	if(bank >= BANK_EXTSGPIO) {
        if(GPIO_SGPIO_ENABLED == 0) {
            GPIO_SGPIO_ENABLED = 1;
            ast2300_enable_serial_gpio();
        }
		tpin = ((gpindata->PinNum - SGPIO_START_PIN) % 32);
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
		if(bank >= BANK_EXTSGPIO_1_DATA_OFFSET)
			Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_1_DATA_OFFSET;
		else if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
#if defined(GROUP_AST2300)
        if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
			Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_DATA_OFFSET;
		else
			Addr = (uint32_t) dev->ast2050_gpio_addr + SGPIO_DATA_OFFSET;
	}
#if defined(GROUP_AST2500)
	else if(bank > BANK_SIMGPIO_5_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_5_DATA_OFFEST;
#endif
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
	else if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_4_DATA_OFFEST;
	else if(bank > BANK_SIMGPIO_3_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_3_DATA_OFFEST;
#endif
	else if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_2_DATA_OFFEST;
	else if(bank > BANK_SIMGPIO_1_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_1_DATA_OFFEST;
	else if(bank > BANK_SIMGPIO_0_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_0_DATA_OFFEST;
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
EXPORT_SYMBOL(ast2050_readgpio);

/*
 * ast2050_readgpios
 * Reads from a set of gpio pins provided in an array
 */
int ast2050_readgpios (void* gpio_list_array, unsigned int count)
{
        gpio_list_data_info     *gpindata=(gpio_list_data_info*)gpio_list_array;
        int  cnt;
        int rStatus = 0;

        for (cnt=0; cnt < count; cnt++, gpindata++)
        {
                if (ast2050_readgpio (gpindata) < 0)
                {
                        gpindata->data = -1;
                        rStatus = -1;
                }
        }

        return rStatus;
}

/*
 * ast2050_writegpio
 * Writes to a gpio pin
 */
int ast2050_writegpio (void *gpin_data  )
{
	int bank,tpin;
	volatile uint32_t Addr;
	volatile uint32_t Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t *dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = (gpindata->PinNum / 8);
	tpin = (gpindata->PinNum % 32);

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	if(bank >= BANK_EXTSGPIO) {
        if(GPIO_SGPIO_ENABLED == 0) {
            GPIO_SGPIO_ENABLED = 1;
            ast2300_enable_serial_gpio();
        }
		tpin = ((gpindata->PinNum - SGPIO_START_PIN) % 32);
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
		if(bank >=BANK_EXTSGPIO_1_DATA_OFFSET)
            Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_1_DATA_OFFSET;
        else if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
#if defined(GROUP_AST2300)
		if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
			Addr = (uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_DATA_OFFSET;
		else
			Addr = (uint32_t) dev->ast2050_gpio_addr + SGPIO_DATA_OFFSET;
	}
#if defined(GROUP_AST2500)
	else if(bank > BANK_SIMGPIO_5_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_5_DATA_OFFEST;
#endif
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
	else if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_4_DATA_OFFEST;
	else if(bank > BANK_SIMGPIO_3_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_3_DATA_OFFEST;
#endif
	else if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_2_DATA_OFFEST;
	else if(bank > BANK_SIMGPIO_1_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_1_DATA_OFFEST;
	else if(bank > BANK_SIMGPIO_0_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_0_DATA_OFFEST;
	else if(bank > BANK_EXTGPIO_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + EXTGPIO_DATA_OFFSET;
	else
		Addr = (uint32_t) dev->ast2050_gpio_addr + GPIO_DATA_OFFSET;

	if(bank >= BANK_EXTSGPIO) {
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
		if(bank >=BANK_EXTSGPIO_1_DATA_OFFSET)
            Value = gpio_read_reg ((uint32_t) dev->ast2050_gpio_addr + EXTSGPIO_1_DATA_READ_OFFSET );
		else if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
#if defined(GROUP_AST2300)
		if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
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
EXPORT_SYMBOL(ast2050_writegpio);

/*
 * ast2050_writegpios
 * Writse to a set of gpio pins provided in an array
 */
int ast2050_writegpios (void* gpio_list_array, unsigned int count)
{
        gpio_list_data_info     *gpindata=(gpio_list_data_info*)gpio_list_array;
        int  cnt;
        int rStatus = 0;

        for (cnt=0; cnt < count; cnt++, gpindata++)
        {
                if (ast2050_writegpio (gpindata) < 0)
                {
                        gpindata->data = -1;
                        rStatus = -1;
                }
        }

        return rStatus;
}


/*
 * ast2050_getdir
 * Get direction of the pin
 */
int ast2050_getdir (void *gpin_data)
{
	int bank,tpin;
	volatile uint32_t Addr;
	volatile uint32_t Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t *dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = (gpindata->PinNum / 8);
	tpin = (gpindata->PinNum % 32);

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
  #if defined(GROUP_AST2500)
    if(bank > BANK_SIMGPIO_5_DATA_OFFSET)
        Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_5_PIN_DIR_OFFEST;
    else if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
  #elif defined(GROUP_AST2400)
    if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
  #endif
        Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_4_PIN_DIR_OFFEST;
    else if(bank > BANK_SIMGPIO_3_DATA_OFFSET)
        Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_3_PIN_DIR_OFFEST;
    else if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
#elif defined(GROUP_AST2300)
    if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
#endif
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_2_PIN_DIR_OFFEST;
	else if(bank > BANK_SIMGPIO_1_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_1_PIN_DIR_OFFEST;
	else if(bank > BANK_SIMGPIO_0_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_0_PIN_DIR_OFFEST;
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
	int bank,tpin;
	volatile uint32_t Addr;
	volatile uint32_t Value;
	Gpio_data_t *gpindata = gpin_data;
	gpio_dev_t *dev = &gpio_dev;

	if ( gpindata->PinNum > MAX_GPIO_IDX ) return -1;

	/* Get the register Addr  */
	bank = (gpindata->PinNum / 8);
	tpin = (gpindata->PinNum % 32);

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
  #if defined(GROUP_AST2500)
	// group-W, group-X always INPUT
	if ((bank == 22) || (bank == 23)) return -1; // unable to set the direction

    if(bank > BANK_SIMGPIO_5_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_5_PIN_DIR_OFFEST;
    else if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
  #endif
  #if defined(GROUP_AST2400)
	// group-W, group-X always INPUT
	// group-Z, group-AA, group-AB always OUTPUT
	if ((bank == 22) || (bank == 23) || (bank == 25) || (bank == 26) || (bank == 27)) return -1; // unable to set the direction

	if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
  #endif
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_4_PIN_DIR_OFFEST;
	else if(bank > BANK_SIMGPIO_3_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_3_PIN_DIR_OFFEST;
	else if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
#endif
#if defined(GROUP_AST2300)
	if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
#endif
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_2_PIN_DIR_OFFEST;
	else if(bank > BANK_SIMGPIO_1_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_1_PIN_DIR_OFFEST;
	else if(bank > BANK_SIMGPIO_0_DATA_OFFSET)
		Addr = (uint32_t) dev->ast2050_gpio_addr + SIMGPIO_0_PIN_DIR_OFFEST;
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
 * ast2050_getgpiosdir
 * Gets the direction of a set of gpio pins provided in an array
 */
int ast2050_getgpiosdir (void* gpio_list_array, unsigned int count)
{
        gpio_list_data_info     *gpindata=(gpio_list_data_info*)gpio_list_array;
        int  cnt;
        int rStatus = 0;

        for (cnt=0; cnt < count; cnt++, gpindata++)
        {
                if (ast2050_getdir (gpindata) < 0)
                {
                        gpindata->data = -1;
                        rStatus = -1;
                }
        }

        return rStatus;
}

/*
 * ast2050_setgpiosdir
 * Sets the direction of a set of gpio pins provided in an array
 */
int ast2050_setgpiosdir (void* gpio_list_array, unsigned int count)
{
        gpio_list_data_info     *gpindata=(gpio_list_data_info*)gpio_list_array;
        int  cnt;
        int rStatus = 0;

        for (cnt=0; cnt < count; cnt++, gpindata++)
        {
                if (ast2050_setdir (gpindata) < 0)
                {
                        gpindata->data = -1;
                        rStatus = -1;
                }
        }

        return rStatus;
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




int get_pull_up_down_Address( Gpio_data_t * gpindata, volatile uint32_t *Addr, int *tbit)
{
#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
int group,cnt;

	/* Below array represents SCU register bit postions for each gpio group, 0xFF = pull down NA */
#if defined(GROUP_AST2300) || defined(GROUP_AST2400)

        gpio_pull_up_down_Info_t PBitsinfo[] = { { 0,16},  { 1,17}, { 2,0xFF},{ 3,19},  { 4,20}, { 5,21}, { 6,22}, 
                                                 { 7,23},  { 8,24}, { 9,25},  {10,0xFF},{11,26}, {12,27}, {13,28}, 
                                                 {14,29},  {15,30}, {16,0xFF},{17,31},  {0xFF, 0xFF} };
#endif
#if defined(GROUP_AST2500)
        gpio_pull_up_down_Info_t PBitsinfo[] = { { 0,16},  { 1,17},  { 2,18},  { 3,19}, { 4,20}, { 5,21},  { 6,22},
                                                 { 7,23},  { 8,24},  { 9,25},  {10,26}, {11,27}, {12,28},  {13,29},
                                                 {14,30},  {15,31},  {16,16},  {17,17}, {18,18}, {19,0xFF},{20,0xFF},
                                                 {21,0xFF},{22,0xFF},{23,0xFF},{24,19}, {25,20}, {26,21},  {27,22},
                                                 {28,23},  {0xFF,0xFF} };
#endif


	if ( gpindata->PinNum > MAX_GPIO_IDX )
		return -1;

	/* Get the register Addr  */
	group = (gpindata->PinNum / 8);
	(*Addr) = (uint32_t) scu_virt_base + SCU_PULL_UP_DOWN_OFFSET;

	for(cnt = 0; cnt != MAX_GPIO_GRPS; cnt++)
	{
		if( (PBitsinfo[cnt].group == group) && (PBitsinfo[cnt].bit != UN_KNOWN))
		{
			(*tbit) = PBitsinfo[cnt].bit;
			break;
		}
	}

	if ( !(*tbit) )
		return -1; 

	return 0;
#else
	return -1;
#endif
}

/*
 * ast2050_getpull_up_down
 * Get pull-up/pull-down control bits
 */
int ast2050_getpull_up_down ( void *gpin_data )
{
#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	Gpio_data_t *gpindata = gpin_data;
	volatile uint32_t Value;
	volatile uint32_t Addr;
	int tbit = 0;

	/*  Map to SCU Base addrees as pullup / pull down resister information comes under SCU Controller, not GPIO Contoller  */
	if( get_pull_up_down_Address(gpindata, &Addr, &tbit) < 0)
		return -1;

	Value = gpio_read_reg ( Addr );
	Value = (Value & (1 << tbit)) >> tbit;

	if(Value)
		gpindata->data = GPIO_PULL_DOWN_DISABLE;
	else
		gpindata->data = GPIO_PULL_DOWN_ENABLE;
	return 0;
#else

	return -1;
#endif
}

/*
 * ast2050_setpull_up_down
 * Set pull-up/pull-down control bits
 */
int ast2050_setpull_up_down ( void *gpin_data  )
{
#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	Gpio_data_t *gpindata = gpin_data;
	volatile uint32_t Value;
	volatile uint32_t Addr;
	int tbit = 0;
	int ret = -1;

	/* Write protection key to unlock SCU registers */
	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)SCU_KEY_CONTROL_REG); /* unlock SCU */

	/*  Map to SCU Base addrees as pullup / pull down resister information comes under SCU Controller, not GPIO Contoller  */
	if( get_pull_up_down_Address(gpindata, &Addr, &tbit) < 0)
		goto error;

	Value = gpio_read_reg ( Addr );

	if(gpindata->data == GPIO_ENABLE_PULL_DOWN)
		Value &= ~(1 << tbit );
	else if(gpindata->data == GPIO_DISABLE_PULL_UP_DOWN)
		Value |= (1 << tbit);
	else if (gpindata->data == GPIO_ENABLE_PULL_UP)
	{
		printk("\t Pullup is not supported ");
		goto error;
	}

	gpio_write_reg (Value, Addr);
	ret = 0;

error:
	/* Clear protection key to lock SCU registers */
	iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG);
	return ret;
#else
	return -1;
#endif 
}


int ast2050_gpio_verify(int gpio_num)
{
 	int j = 0;

	for(j = 0; j < hw_total_interrupt_sensors; j++)
	{
		if(hw_intr_sensors[j].gpio_number == gpio_num)
			return 1;			
	}
	for(j = 0; j < hw_total_interrupt_chassis; j++)
	{
		if(hw_intr_chassis[j].gpio_number == gpio_num)
			return 1;		
	}
	
	return 0;
}

static irqreturn_t gpio_intr_handler( int this_irq, void *dev_id)
{
	volatile uint32_t intr_state_ast2050_value = 0;
	volatile uint32_t data_state_ast2050_value = 0;  // Quanta
	uint32_t current_ast2050_intrs = 0;
	uint32_t value = 0;
	uint32_t i = 0;
	//uint32_t j = 0;
	gpio_dev_t *gpio = &gpio_dev;
	int intr_type = 0;
	uint16_t gpio_num = 0;
	//gpio_interrupt_sensor* intr_info;
#ifdef CONFIG_SPX_FEATURE_ENABLE_GPIO_INTERRUPTIBLE
	volatile uint32_t int_enable_ast2050_value;
	volatile uint32_t int_trigger_ast2050_value;
#endif

	/* Determine the reason for this interrupt */
	if(this_irq == IRQ_GPIO)
	{
	    if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_EN_OFFSET))
		{
			/* ast2050 GPIO portA to portD related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t)(gpio->ast2050_gpio_addr + GPIO_DATA_OFFSET));  // Quanta

			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;
						//common module's callback interrupt handler
#ifdef CONFIG_SPX_FEATURE_ENABLE_GPIO_INTERRUPTIBLE
						if ((1 << i) & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_A_TO_D)
						{
							/* Read the type of trigger and if LEVEL triggered, disable the interrupts for that particular pin 
							 * else the notification will always be taking high priority not allowing the sleep process to handle the wakeup 
							 */ 
							int_trigger_ast2050_value	= gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE1_OFFSET);
							if (int_trigger_ast2050_value & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_A_TO_D)
							{
								/* Disable interrupt */
								int_enable_ast2050_value = gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_EN_OFFSET);
								int_enable_ast2050_value &= (~(CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_A_TO_D));
								gpio_write_reg(int_enable_ast2050_value,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_EN_OFFSET);
							}
							gpio_interrupt_hit = 1;
							wake_up_interruptible(&gpio_interrupt_lock);
						}
#endif
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 0, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 0, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
	    }
		/* Clear all the interrupts now */
		gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + GPIO_INT_STATUS_OFFSET);

		current_ast2050_intrs = 0;	// Quanta
	    if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_EN_OFFSET))
		{
			/* ast2050 GPIO portE to portH related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t)(gpio->ast2050_gpio_addr + EXTGPIO_DATA_OFFSET));  // Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = 32 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{					
						value = 1 << i;
						current_ast2050_intrs |= value;
						//common module's callback interrupt handler
#ifdef CONFIG_SPX_FEATURE_ENABLE_GPIO_INTERRUPTIBLE
						if ((1 << i) & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_E_TO_H)
						{
							/* Read the type of trigger and if LEVEL triggered, disable the interrupts for that particular pin 
							 * else the notification will always be taking high priority not allowing the sleep process to handle the wakeup 
							 */ 
							int_trigger_ast2050_value	= gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET);
							if (int_trigger_ast2050_value & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_E_TO_H)
							{
								/* Disable interrupt */
								int_enable_ast2050_value = gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET);
								int_enable_ast2050_value &= (~(CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_E_TO_H));
								gpio_write_reg(int_enable_ast2050_value,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_EN_OFFSET);
							}
							gpio_interrupt_hit = 1;
							wake_up_interruptible(&gpio_interrupt_lock);
						}
#endif
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 32, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 32, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
	    }
		/* Clear all the interrupts now */
		gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + EXTGPIO_INT_STATUS_OFFSET);

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
		current_ast2050_intrs = 0;	// Quanta
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portI to portL related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_DATA_OFFEST); //Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = 64 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;					
						//common module's callback interrupt handler
#ifdef CONFIG_SPX_FEATURE_ENABLE_GPIO_INTERRUPTIBLE
						if ((1 << i) & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_I_TO_L)
						{
							/* Read the type of trigger and if LEVEL triggered, disable the interrupts for that particular pin 
							 * else the notification will always be taking high priority not allowing the sleep process to handle the wakeup 
							 */ 
							int_trigger_ast2050_value	= gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE1_OFFSET);
							if (int_trigger_ast2050_value & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_I_TO_L)
							{
								/* Disable interrupt */
								int_enable_ast2050_value = gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_EN_OFFSET);
								int_enable_ast2050_value &= (~(CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_I_TO_L));
								gpio_write_reg(int_enable_ast2050_value,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_EN_OFFSET);
							}
							gpio_interrupt_hit = 1;
							wake_up_interruptible(&gpio_interrupt_lock);
						}
#endif
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 64, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 64, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_0_INT_STATUS_OFFSET);

		current_ast2050_intrs = 0;	// Quanta
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portM to portP related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_DATA_OFFEST); //Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = 96 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;				
						//common module's callback interrupt handler
#ifdef CONFIG_SPX_FEATURE_ENABLE_GPIO_INTERRUPTIBLE
						if ((1 << i) & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_M_TO_P)
						{
							/* Read the type of trigger and if LEVEL triggered, disable the interrupts for that particular pin 
							 * else the notification will always be taking high priority not allowing the sleep process to handle the wakeup 
							 */ 
							int_trigger_ast2050_value	= gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE1_OFFSET);
							if (int_trigger_ast2050_value & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_M_TO_P)
							{
								/* Disable interrupt */
								int_enable_ast2050_value = gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_EN_OFFSET);
								int_enable_ast2050_value &= (~(CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_M_TO_P));
								gpio_write_reg(int_enable_ast2050_value,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_EN_OFFSET);
							}
							gpio_interrupt_hit = 1;
							wake_up_interruptible(&gpio_interrupt_lock);
						}
#endif
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 96, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 96, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_1_INT_STATUS_OFFSET);

		current_ast2050_intrs = 0;	// Quanta
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_EN_OFFSET)) 
		{
			/* ast2300 GPIO portQ to portS related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_DATA_OFFEST); //Quanta
			/* Handle the interrupts */
	#if defined(GROUP_AST2300)
			for(i = 0; i <= GPIO_23; i++)
			{
	#endif
	#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
			for(i = 0; i <= GPIO_31; i++)
			{
	#endif
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = 128 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;					
						//common module's callback interrupt handler
#ifdef CONFIG_SPX_FEATURE_ENABLE_GPIO_INTERRUPTIBLE
						if ((1 << i) & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_Q_TO_S)
						{
							/* Read the type of trigger and if LEVEL triggered, disable the interrupts for that particular pin 
							 * else the notification will always be taking high priority not allowing the sleep process to handle the wakeup 
							 */ 
							int_trigger_ast2050_value	= gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE1_OFFSET);
							if (int_trigger_ast2050_value & CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_Q_TO_S)
							{
								/* Disable interrupt */
								int_enable_ast2050_value = gpio_read_reg((uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_EN_OFFSET);
								int_enable_ast2050_value &= (~(CONFIG_SPX_FEATURE_BITMAP_GPIO_PORT_Q_TO_S));
								gpio_write_reg(int_enable_ast2050_value,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_EN_OFFSET);
							}
							gpio_interrupt_hit = 1;
							wake_up_interruptible(&gpio_interrupt_lock);
						}
#endif
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 128, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 128, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_2_INT_STATUS_OFFSET);

	#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
		current_ast2050_intrs = 0;	// Quanta
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_3_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portQ to portS related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_3_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_3_DATA_OFFEST); // Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = 160 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;
						//common module's callback interrupt handler
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 160, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 160, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_3_INT_STATUS_OFFSET);

		current_ast2050_intrs = 0;	// Quanta
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_4_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portQ to portS related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_4_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_4_DATA_OFFEST); //Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = 192 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;
						//common module's callback interrupt handler
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 192, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 192, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_4_INT_STATUS_OFFSET);
	#endif /* GROUP_AST2400 || GROUP_AST2500 */

    #if defined(GROUP_AST2500)
		current_ast2050_intrs = 0;	// Quanta
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_5_INT_EN_OFFSET))
		{
			/* ast2300 GPIO portQ to portS related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_5_INT_STATUS_OFFSET);
            data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SGPIO_DATA_OFFSET); // Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
					gpio_num = 160 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;
						//common module's callback interrupt handler
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, 160, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 160, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SIMGPIO_5_INT_STATUS_OFFSET);
	#endif /* GROUP_AST2500 */

#endif /* GROUP_AST2300 || GROUP_AST2400 || GROUP_AST2500 */
	} /* end of IRQ_GPIO */

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
	if(this_irq == IRQ_SGPIO_MASTER)
	{
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_EN_OFFSET))
		{
			/* ast2050 Serial GPIO portA to portD related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_STATUS_OFFSET);
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
			{
				if(intr_state_ast2050_value & 0x00000001)
				{
                    /* Seril GPIO 1st pin number */
					gpio_num = SGPIO_START_PIN + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;
						//common module's callback interrupt handler
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, SGPIO_START_PIN, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, SGPIO_START_PIN, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER);
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + SGPIO_INT_STATUS_OFFSET);

		current_ast2050_intrs = 0;	// Quanta
		if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_EN_OFFSET))
		{
			/* ast2050 Extended Serial GPIO portE to portH related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_DATA_OFFSET); //Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++) 
			{
				if(intr_state_ast2050_value & 0x00000001)
				{

					gpio_num = (BANK_EXTSGPIO_DATA_OFFSET * 8) + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
						current_ast2050_intrs |= value;
						//common module's callback interrupt handler 
						//intr_type = pgpio_core_funcs->process_gpio_intr(i, (BANK_EXTSGPIO_DATA_OFFSET * 8), 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, (BANK_EXTSGPIO_DATA_OFFSET * 8), data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
			/* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_INT_STATUS_OFFSET);
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
        if (gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_1_INT_EN_OFFSET)) {
			/* ast2050 Extended Serial GPIO portI to portJ related interrupt */
			intr_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_1_INT_STATUS_OFFSET);
			data_state_ast2050_value = gpio_read_reg((uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_1_DATA_OFFSET); //Quanta
			/* Handle the interrupts */
			for(i = 0; i <= GPIO_31; i++)
            {
				if(intr_state_ast2050_value & 0x00000001)
				{
				    gpio_num = 288 + i;
					if(ast2050_gpio_verify(gpio_num)) 
					{
						value = 1 << i;
                        current_ast2050_intrs |= value;
                        //common module's callback interrupt handler
                        //intr_type = pgpio_core_funcs->process_gpio_intr(i, 288, 0, NOT_LEVEL_TRIGGER);//Quanta--
						intr_type = pgpio_core_funcs->process_gpio_intr(i, 288, data_state_ast2050_value & 0x01, NOT_LEVEL_TRIGGER); //Quanta
					}
				}
				intr_state_ast2050_value >>= 1;
				data_state_ast2050_value >>= 1;  // Quanta
			}
		}
            /* Clear all the interrupts now */
			gpio_write_reg(current_ast2050_intrs,(uint32_t) gpio->ast2050_gpio_addr + EXTSGPIO_1_INT_STATUS_OFFSET);
#endif
	} /* end of IRQ_SGPIO_MASTER */
#endif

	pgpio_core_funcs->wakeup_intr_queue(intr_type);

	return( IRQ_HANDLED );
}

int ast2050_regsensorints ( void* gpin_data, uint32_t total_interrupt_sensors, void* sensor_itr)
{
	int bank = 0;
	int tpin = 0;
	gpio_dev_t *gpio = &gpio_dev;
	//hw_intr_sensors = (gpio_interrupt_sensor *) sensor_itr;
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

	hw_intr_sensors = (gpio_interrupt_sensor *) kmalloc(sizeof(gpio_interrupt_sensor)*total_interrupt_sensors,GFP_KERNEL);
	if (!hw_intr_sensors)
		return -ENOMEM;
	
	memcpy(hw_intr_sensors,(gpio_interrupt_sensor *)sensor_itr,(sizeof(gpio_interrupt_sensor)*total_interrupt_sensors));
	hw_total_interrupt_sensors = total_interrupt_sensors;
	
	for(i = 0; i < hw_total_interrupt_sensors; i++) {
		bank = (hw_intr_sensors[i].gpio_number / 8);
		tpin = (hw_intr_sensors[i].gpio_number % 32);

		#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
			if(bank >= BANK_EXTSGPIO) {
                if(GPIO_SGPIO_ENABLED == 0) {
                    GPIO_SGPIO_ENABLED = 1;
                    ast2300_enable_serial_gpio();
                }
				tpin = ((hw_intr_sensors[i].gpio_number - SGPIO_START_PIN) % 32);
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
				if(bank >= BANK_EXTSGPIO_1_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE2_OFFSET;
				} else if(bank >= BANK_EXTSGPIO_DATA_OFFSET) {
#endif
#if defined(GROUP_AST2300)
				if(bank >= BANK_EXTSGPIO_DATA_OFFSET) {
#endif
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE2_OFFSET;
				} else {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE2_OFFSET;
				}
		#if defined(GROUP_AST2500)
			} else if(bank > BANK_SIMGPIO_5_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE2_OFFSET;
		#endif
		#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
			} else if(bank > BANK_SIMGPIO_4_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_SIMGPIO_3_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE2_OFFSET;
		#endif
			} else if(bank > BANK_SIMGPIO_2_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_SIMGPIO_1_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_SIMGPIO_0_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_EXTGPIO_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
			} else {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE2_OFFSET;
			}
		#else
			if(bank > BANK_EXTGPIO_DATA_OFFSET) {
				int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
				int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
				int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
				int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
			} else {
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
		if(hw_intr_sensors[i].trigger_method == GPIO_EDGE) {
			int_trigger_ast2050_value &= clr_value;
			if(hw_intr_sensors[i].trigger_type == GPIO_FALLING_EDGE)
				int_riseneg_ast2050_value &= clr_value;
			else if(hw_intr_sensors[i].trigger_type == GPIO_RISING_EDGE)
				int_riseneg_ast2050_value |= set_value;
			else if(hw_intr_sensors[i].trigger_type == GPIO_BOTH_EDGES)
				int_both_ast2050_value |= set_value;
			else
				printk("Wrong edge trigger type for sensor int\n");
		} else if(hw_intr_sensors[i].trigger_method == GPIO_LEVEL) {
			int_trigger_ast2050_value |= set_value;
			if(hw_intr_sensors[i].trigger_type == GPIO_HIGH_LEVEL)
				int_riseneg_ast2050_value |= set_value;
			else if(hw_intr_sensors[i].trigger_type == GPIO_LOW_LEVEL)
				int_riseneg_ast2050_value &= clr_value;
			else
				printk("Wrong level trigger type for sensor int\n");
		} else {
			printk("Wrong trigger type for sensor int\n");
		}
		int_enable_ast2050_value |= set_value;

		/* Set the actual registers now */
		gpio_write_reg (int_trigger_ast2050_value, int_trigger_ast2050_Addr);
		gpio_write_reg (int_riseneg_ast2050_value, int_riseneg_ast2050_Addr);
		gpio_write_reg (int_both_ast2050_value, int_both_ast2050_Addr);

		/* Enable the interrupts now */
		if(int_enable_ast2050_value != 0x00)
			gpio_write_reg (int_enable_ast2050_value, int_enable_ast2050_Addr);
	}

	if(!GPIO_IRQ_ENABLED) {
		if( request_irq(IRQ_GPIO, gpio_intr_handler, IRQF_SHARED, AST_GPIO_DEV_NAME, gpio->ast2050_gpio_addr) < 0 ) {
			printk( KERN_ERR "Request for irq %d failed\n", IRQ_GPIO );
			return( -ENODEV );
		} else {
   			IRQ_SET_HIGH_LEVEL (IRQ_GPIO);
   			IRQ_SET_LEVEL_TRIGGER (IRQ_GPIO);
			GPIO_IRQ_ENABLED = 1;
		}
	#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
		//if(bank >= BANK_EXTSGPIO) {
			if( request_irq(IRQ_SGPIO_MASTER, gpio_intr_handler, 0, AST_SGPIO_DEV_NAME, gpio->ast2050_gpio_addr) < 0 ) {
				printk( KERN_ERR "Request for irq %d failed\n", IRQ_SGPIO_MASTER );
				return( -ENODEV );
			} else {
				EXT_IRQ_SET_HIGH_LEVEL (IRQ_SGPIO_MASTER);
				EXT_IRQ_SET_LEVEL_TRIGGER (IRQ_SGPIO_MASTER);
				GPIO_IRQ_ENABLED = 1;
			}
		//}
	#endif
	}

	return 0; 
}

int ast2050_unregsensorints (  void* gpin_data )
{
	int bank = 0;
	int tpin = 0;
	gpio_dev_t *gpio = &gpio_dev;
	//gpio_interrupt_sensor* intr_sensors = (gpio_interrupt_sensor *) sensor_itr;
	uint32_t i = 0;
	volatile uint32_t int_enable_ast2050_Addr;
	volatile uint32_t int_riseneg_ast2050_Addr;
	volatile uint32_t int_trigger_ast2050_Addr;
	volatile uint32_t int_both_ast2050_Addr;
	volatile uint32_t int_enable_ast2050_value;
	volatile uint32_t int_riseneg_ast2050_value;
	volatile uint32_t int_trigger_ast2050_value;
	volatile uint32_t int_both_ast2050_value;
	uint32_t clr_value = 0;

	for(i = 0; i < hw_total_interrupt_sensors; i++) 
	{
		bank = (hw_intr_sensors[i].gpio_number / 8);
		tpin = (hw_intr_sensors[i].gpio_number % 32);

		#if defined(GROUP_AST2300) || defined(GROUP_AST2400)
		if(bank >= BANK_EXTSGPIO)
		{	
			if(GPIO_SGPIO_ENABLED == 0) {
				GPIO_SGPIO_ENABLED = 1;
				ast2300_enable_serial_gpio();
			}
			tpin = ((hw_intr_sensors[i].gpio_number - SGPIO_START_PIN) % 32);
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
				if(bank >= BANK_EXTSGPIO_1_DATA_OFFSET) {
						int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_EN_OFFSET;
						int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE0_OFFSET;
						int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE1_OFFSET;
						int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE2_OFFSET;
				}
				else if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
#if defined(GROUP_AST2300)
			if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
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
		#if defined(GROUP_AST2500)
		} 
		else if(bank > BANK_SIMGPIO_5_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE2_OFFSET;
		#endif
		#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
		} 
		else if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE2_OFFSET;
		} 
		else if(bank > BANK_SIMGPIO_3_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE2_OFFSET;
		#endif
		} 
		else if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE2_OFFSET;
		} 
		else if(bank > BANK_SIMGPIO_1_DATA_OFFSET) 
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE2_OFFSET;
		} else if(bank > BANK_SIMGPIO_0_DATA_OFFSET) 
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE2_OFFSET;
		} 
		else if(bank > BANK_EXTGPIO_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
		} else 
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
		} else 
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

		clr_value = ~(1 << tpin);

		int_enable_ast2050_value    &= clr_value;
		int_riseneg_ast2050_value   &= clr_value;
		int_trigger_ast2050_value 	&= clr_value;
		int_both_ast2050_value		&= clr_value;

		gpio_write_reg (int_enable_ast2050_value, int_enable_ast2050_Addr);		
		gpio_write_reg (int_riseneg_ast2050_value, int_riseneg_ast2050_Addr);
		gpio_write_reg (int_trigger_ast2050_value, int_trigger_ast2050_Addr);
		gpio_write_reg (int_both_ast2050_value, int_both_ast2050_Addr);

	}
	if(GPIO_IRQ_ENABLED != 0)
	{
		free_irq(IRQ_GPIO,gpio->ast2050_gpio_addr);
		if(hw_intr_sensors != NULL)
			kfree(hw_intr_sensors);
		hw_intr_sensors = NULL;
		hw_total_interrupt_sensors = 0;
		#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
		disable_irq(IRQ_SGPIO_MASTER);
		free_irq(IRQ_SGPIO_MASTER,gpio->ast2050_gpio_addr);
		#endif
		GPIO_IRQ_ENABLED = 0;
	}

	return 0; 
}



/********************** Chassis Interrupts ***********************/
int ast2050_regchassisints  ( void* gpin_data, uint32_t total_chassis_interrupts,  void* chassis_itr )
{
	int bank, tpin;
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
	//gpio_interrupt_sensor* intr_chassis = (gpio_interrupt_sensor *) chassis_itr;
	uint32_t set_value = 0;
	uint32_t clr_value = 0;

	hw_intr_chassis = (gpio_interrupt_sensor *) kmalloc(sizeof(gpio_interrupt_sensor)*total_chassis_interrupts,GFP_KERNEL);
	if (!hw_intr_chassis)
		return -ENOMEM;

	memcpy(hw_intr_chassis,(gpio_interrupt_sensor *)chassis_itr,(sizeof(gpio_interrupt_sensor)*total_chassis_interrupts));
	hw_total_interrupt_chassis = total_chassis_interrupts;

	for(i = 0; i < hw_total_interrupt_chassis; i++)
	{
		bank = (hw_intr_chassis[i].gpio_number / 8);
		tpin = (hw_intr_chassis[i].gpio_number % 32);

		#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
			if(bank >= BANK_EXTSGPIO) {	
                if(GPIO_SGPIO_ENABLED == 0) {
                    GPIO_SGPIO_ENABLED = 1;
                    ast2300_enable_serial_gpio();
                }
				tpin = ((hw_intr_chassis[i].gpio_number - SGPIO_START_PIN) % 32);
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
				if(bank >= BANK_EXTSGPIO_1_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE2_OFFSET;
				} else if(bank >= BANK_EXTSGPIO_DATA_OFFSET) {
#endif
#if defined(GROUP_AST2300)
                if(bank >= BANK_EXTSGPIO_DATA_OFFSET) {
#endif
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_INT_SEN_TYPE2_OFFSET;
				} else {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SGPIO_INT_SEN_TYPE2_OFFSET;
				}
		#if defined(GROUP_AST2500)
			} else if(bank > BANK_SIMGPIO_5_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE2_OFFSET;
		#endif
		#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
			} else if(bank > BANK_SIMGPIO_4_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_SIMGPIO_3_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE2_OFFSET;
		#endif
			} else if(bank > BANK_SIMGPIO_2_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_SIMGPIO_1_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_SIMGPIO_0_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE2_OFFSET;
			} else if(bank > BANK_EXTGPIO_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
			} else {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+GPIO_INT_SEN_TYPE2_OFFSET;
			}
		#else
				if(bank > BANK_EXTGPIO_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
				} else {
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
		if(hw_intr_chassis[i].trigger_method == GPIO_EDGE) {
			int_trigger_ast2050_value &= clr_value;
			if(hw_intr_chassis[i].trigger_type == GPIO_FALLING_EDGE)
				int_riseneg_ast2050_value &= clr_value;
			else if(hw_intr_chassis[i].trigger_type == GPIO_RISING_EDGE)
				int_riseneg_ast2050_value |= set_value;
			else if(hw_intr_chassis[i].trigger_type == GPIO_BOTH_EDGES)
				int_both_ast2050_value |= set_value;
			else
				printk("Wrong edge trigger type for chassis int\n");
		} else if(hw_intr_chassis[i].trigger_method == GPIO_LEVEL) {
			int_trigger_ast2050_value |= set_value;
			if(hw_intr_chassis[i].trigger_type == GPIO_HIGH_LEVEL)
				int_riseneg_ast2050_value |= set_value;
			else if(hw_intr_chassis[i].trigger_type == GPIO_LOW_LEVEL)
				int_riseneg_ast2050_value &= clr_value;
			else
				printk("Wrong level trigger type for chassis int\n");
		} else {
			printk("Wrong trigger type for sensor int\n");
		}
		int_enable_ast2050_value |= set_value;

		/* Set the actual registers now */
		gpio_write_reg (int_trigger_ast2050_value, int_trigger_ast2050_Addr);
		gpio_write_reg (int_riseneg_ast2050_value, int_riseneg_ast2050_Addr);
		gpio_write_reg (int_both_ast2050_value, int_both_ast2050_Addr);

		/* Enable the interrupts now */
		if(int_enable_ast2050_value != 0x00)
			gpio_write_reg (int_enable_ast2050_value, int_enable_ast2050_Addr);
	}

	if(!GPIO_IRQ_ENABLED) {
		if( request_irq(IRQ_GPIO, gpio_intr_handler, IRQF_SHARED, AST_GPIO_DEV_NAME, gpio->ast2050_gpio_addr) < 0 ) {
			printk( KERN_ERR "Request for irq %d failed\n", IRQ_GPIO );
			return( -ENODEV );
		} else {
   			IRQ_SET_HIGH_LEVEL (IRQ_GPIO);
   			IRQ_SET_LEVEL_TRIGGER (IRQ_GPIO);
			GPIO_IRQ_ENABLED = 1;
		}
#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
		if( request_irq(IRQ_SGPIO_MASTER, gpio_intr_handler, 0, AST_SGPIO_DEV_NAME, gpio->ast2050_gpio_addr) < 0 ) {
			printk( KERN_ERR "Request for irq %d failed\n", IRQ_SGPIO_MASTER );
			return( -ENODEV );
		} else {
//			IRQ_SET_HIGH_LEVEL (IRQ_SGPIO_MASTER);
//			IRQ_SET_LEVEL_TRIGGER (IRQ_SGPIO_MASTER);
			GPIO_IRQ_ENABLED = 1;
		}
#endif
	}

	return 0; 
}

int ast2050_unregchassisints (void* gpin_data)
{
	int bank, tpin;
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
	
	uint32_t clr_value = 0;

	//printk("total chassis interrupts = %d\n",total_chassis_interrupts);
	for(i = 0; i < hw_total_interrupt_chassis; i++) 
	{
		bank = (hw_intr_chassis[i].gpio_number / 8);
		tpin = (hw_intr_chassis[i].gpio_number % 32);

		#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
		if(bank >= BANK_EXTSGPIO)
		{	
			if(GPIO_SGPIO_ENABLED == 0) {
				GPIO_SGPIO_ENABLED = 1;
				ast2300_enable_serial_gpio();
			}
			tpin = ((hw_intr_chassis[i].gpio_number - SGPIO_START_PIN) % 32);
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
				if(bank >= BANK_EXTSGPIO_1_DATA_OFFSET) {
					int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_EN_OFFSET;
					int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE0_OFFSET;
					int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE1_OFFSET;
					int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTSGPIO_1_INT_SEN_TYPE2_OFFSET;
				} else if(bank >= BANK_EXTSGPIO_DATA_OFFSET) 
#endif
#if defined(GROUP_AST2300)
			if(bank >= BANK_EXTSGPIO_DATA_OFFSET)
#endif
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
		#if defined(GROUP_AST2500)
		} 
		else if(bank > BANK_SIMGPIO_5_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_5_INT_SEN_TYPE2_OFFSET;
		#endif
		#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
		} 
		else if(bank > BANK_SIMGPIO_4_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_4_INT_SEN_TYPE2_OFFSET;
		} 
		else if(bank > BANK_SIMGPIO_3_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_3_INT_SEN_TYPE2_OFFSET;
		#endif
		} 
		else if(bank > BANK_SIMGPIO_2_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_2_INT_SEN_TYPE2_OFFSET;
		} 
		else if(bank > BANK_SIMGPIO_1_DATA_OFFSET) 
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_1_INT_SEN_TYPE2_OFFSET;
		} else if(bank > BANK_SIMGPIO_0_DATA_OFFSET) 
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+SIMGPIO_0_INT_SEN_TYPE2_OFFSET;
		} 
		else if(bank > BANK_EXTGPIO_DATA_OFFSET)
		{
			int_enable_ast2050_Addr 	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_EN_OFFSET;
			int_riseneg_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE0_OFFSET;
			int_trigger_ast2050_Addr	= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE1_OFFSET;
			int_both_ast2050_Addr		= (uint32_t)gpio->ast2050_gpio_addr+EXTGPIO_INT_SEN_TYPE2_OFFSET;
		} else 
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
		} else 
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

		clr_value = ~(1 << tpin);

		int_enable_ast2050_value    &= clr_value;
		int_riseneg_ast2050_value   &= clr_value;
		int_trigger_ast2050_value 	&= clr_value;
		int_both_ast2050_value		&= clr_value;

		gpio_write_reg (int_enable_ast2050_value, int_enable_ast2050_Addr);		
		gpio_write_reg (int_riseneg_ast2050_value, int_riseneg_ast2050_Addr);
		gpio_write_reg (int_trigger_ast2050_value, int_trigger_ast2050_Addr);
		gpio_write_reg (int_both_ast2050_value, int_both_ast2050_Addr);

	}

	if(GPIO_IRQ_ENABLED != 0)
	{
		free_irq(IRQ_GPIO,gpio->ast2050_gpio_addr);
		if(hw_intr_chassis != NULL)
			kfree(hw_intr_chassis);
		hw_intr_chassis = NULL;
		hw_total_interrupt_chassis = 0;
		#if defined(GROUP_AST2300) || defined(GROUP_AST2400)
		disable_irq(IRQ_SGPIO_MASTER);
        free_irq(IRQ_SGPIO_MASTER,gpio->ast2050_gpio_addr);
		#endif
		GPIO_IRQ_ENABLED = 0;
	}

	return 0; 
}

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
void ast2300_enable_serial_gpio(void)
{
	volatile uint32_t Addr;
	volatile uint32_t Value;
	uint32_t reg;
	gpio_dev_t *dev = &gpio_dev;

	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)SCU_KEY_CONTROL_REG); /* unlock SCU */
	reg = ioread32((void * __iomem)AST_SCU_VA_BASE + AST_SCU_MULTI_FUNC_2);	
	reg |= AST_SCU_MULTI_PIN_SGPIO;
	iowrite32(reg, (void * __iomem)AST_SCU_VA_BASE + AST_SCU_MULTI_FUNC_2);
	iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG);	/* lock SCU */

	Addr = (uint32_t) dev->ast2050_gpio_addr + SGPIO_CONF_OFFSET;
#if defined(GROUP_AST2300)
	Value = 0x80201; // 8 bytes,
#endif
#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
	Value = 0x80281; // 10 bytes,
#endif
	gpio_write_reg (Value, Addr);
}
#endif

/*
 * gpio_read_reg
 * reads a byte from the offset
 */
static u32 gpio_read_reg(uint32_t offset)
{
    return (ioread32((void * __iomem)offset));
}

/*
 * gpio_write_reg
 * writes a byte to the offset
 */
static void gpio_write_reg(uint32_t value, uint32_t offset)
{
    iowrite32(value, (void * __iomem)offset);
}

MODULE_AUTHOR("American Megatrends");
MODULE_DESCRIPTION("ASPEED AST SoC GPIO Driver");
MODULE_LICENSE("GPL");

module_init(gpio_hw_init);
module_exit(gpio_hw_exit);

