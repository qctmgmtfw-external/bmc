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


#ifndef __GPIO__
#define __GPIO__

//#include <stdint.h>

/* common  declaritions for gpio library and the driver */
#define GPIO_DIR_OD_OUT 2
#define GPIO_DIR_OUT   1
#define GPIO_DIR_IN    0
#define GPIO_POL_HIGH  1
#define GPIO_POL_LOW   0
#define GPIO_DATA_HIGH 1
#define GPIO_DATA_LOW  0
#define GPIO_ENABLE_PULL_DOWN       0
#define GPIO_ENABLE_PULL_UP         1
#define GPIO_DISABLE_PULL_UP_DOWN   2

//#include "gpio_pins.h"
//#include "intrsensor.h"

#define SENSOR_INTR		1
#define CHASSIS_INTR	2

/* GPIO Mapping Table */
/***This is an internal table used by the driver******/
typedef struct
{
        unsigned short VirtualPin;
        unsigned short PhysicalPin;
        unsigned char  Direction;
        unsigned char  Value;
}  __attribute__((packed)) GPIO_MAP_TABLE;

typedef enum _gpio_sensor_type_info {
	THRESHOLD_SENSOR_GPIO,
	NON_THRESHOLD_SENSOR_GPIO
}  gpio_sensor_type_info;

typedef enum _gpio_trigger_method_info {
	GPIO_EDGE,
	GPIO_LEVEL
}  gpio_trigger_method_info;

typedef enum _gpio_trigger_type_info {
	GPIO_RISING_EDGE,
	GPIO_FALLING_EDGE,
	GPIO_HIGH_LEVEL,
	GPIO_LOW_LEVEL,
	GPIO_BOTH_EDGES 
}  gpio_trigger_type_info;

typedef struct gpio_interrupt_sensor
{
	int 	       int_num;
	int 	       ext_irq;
	unsigned short gpio_number;
	unsigned short sensor_number;
	gpio_sensor_type_info sensor_type; 
	gpio_trigger_method_info trigger_method;
	gpio_trigger_type_info trigger_type;
	unsigned char reading_on_assertion; //indicates what the reading was after assertion. used to determine what caused the assertion
										//for e.g.: in both edge triggered if reading after assertion was 0 then high to low caused it
										//in level low if reading after assertion was 0 then level was low and hence the interrupt happened
										// This may be invalid and it is upto the user program to validate
										// for e.g.: a line may go low to high and then back to low again and if this line is cofnigured for rising edege 
										// then at the time of reading the pin may have gone back to low in which case the application can conclude that
										// the transition happened before the state was determined.
}  __attribute__((packed)) gpio_interrupt_sensor;


typedef struct gpio_interrupt_data
{
	unsigned int total_sensors;
	gpio_interrupt_sensor *int_sensor_info;
}  __attribute__((packed)) gpio_interrupt_data;


/****This is the structure that is passed back and forth between userspace and driver as an ioctl arg*****/
typedef struct 
{
	unsigned short PinNum; /* Not used in case of interrupt sensor data */
	union
	{
		unsigned char  data; /* Direction or Value or Polarity */
		gpio_interrupt_data gpio_int_sensor_data;
	};
}  __attribute__((packed)) Gpio_data_t;	


/******This is an internal structure in the driver which is used for maintaining qs******/
typedef struct _pending_interrupt_info
{
	unsigned short gpio;
	unsigned char  state; //use to determine what transition caused the interrupt.State is reading of the pin after the interrupt.
							//THIS HAS TO BE DETERMINED IN THE IRQ ITSELF
	struct _pending_interrupt_info *next;
}  __attribute__((packed)) pending_interrupt_info;

/****This is the structure that is passed during gpio interrupt callback registration between drivers*****/
typedef struct
{
	unsigned short       gpio_number;
	gpio_trigger_method_info  trigger_method;
	gpio_trigger_type_info    trigger_type;
	int (*callback_handler)(void);
}  __attribute__((packed)) gpio_int_info_t;


typedef enum {
	GET_MODULE_GPIO_PROPERTIES,
	GET_GPIO_DIRECTION,
	SET_GPIO_DIRECTION,
	GET_GPIO_POLARITY,
	SET_GPIO_POLARITY,
	GET_GPIO_PULL_UP_DOWN,
	SET_GPIO_PULL_UP_DOWN,
	READ_GPIO,
	WRITE_GPIO,
	SET_GPIO_OD_OUT,
	REGISTER_SENSOR_INTS,
	WAIT_FOR_SENSOR_INT,
	UNREGISTER_SENSOR_INTS,
	REGISTER_CHASSIS_INTS,
	WAIT_FOR_CHASSIS_INT,
	UNREGISTER_CHASSIS_INTS,
	SET_CONT_MODE
}eGpioFunc;


typedef struct
{
	int (*readgpio)  ( void* gpin_data );
    int (*writegpio) ( void* gpin_data );
    int (*getdir)    ( void* gpin_data );
    int (*setdir)    ( void* gpin_data );
    int (*getpol)    ( void* gpin_data );
    int (*setpol)    ( void* gpin_data );
    int (*getpull_up_down) ( void* gpin_data );
    int (*setpull_up_down) ( void* gpin_data );
    int (*regsensorints)   ( void* gpin_data, uint32_t total_interrupt_sensors, void* sensor_itr ); 
    int (*unregsensorints)   ( void* gpin_data );
    int (*regchassisints)   (  void* gpin_data, uint32_t total_interrupt_sensors, void* sensor_itr ); 
    int (*unregchassisints)   (  void* gpin_data );
	int (*cleanup)   ( void *gpiodata); 
	int (*setcontmode) ( void* gpin_data );
}gpio_hal_operations_t;

typedef struct
{
        int (*process_gpio_intr) (int gpio_num, int gpio_port_pin_offset);
		void (*wakeup_intr_queue) (int intr_type);
} gpio_core_funcs_t;

struct gpio_hal
{
	gpio_hal_operations_t *pgpio_hal_ops;
};

struct gpio_dev
{
	struct gpio_hal *pgpio_hal;
};

typedef Gpio_data_t gpio_ioctl_data;

#endif // __GPIO__
