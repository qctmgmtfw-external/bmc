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
#define GPIO_ENABLE_DEBOUNCE       1
#define GPIO_DISABLE_DEBOUNCE      0

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
	GPIO_BOTH_EDGES,
	NOT_LEVEL_TRIGGER
}  gpio_trigger_type_info;

typedef struct gpio_interrupt_sensor
{
	int 	       int_num;
	int 	       ext_irq;
	unsigned short gpio_number;
	unsigned short sensor_number;
	unsigned short OwnerLUN;
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

// Quanta++
typedef struct gpio_debounce_setting
{
	unsigned char setting;
	unsigned long time;
}  __attribute__((packed)) gpio_debounce_setting;
// Quanta--
typedef struct gpio_list_data_info
{
	unsigned short PinNum;
	unsigned char  data;
}  __attribute__((packed)) gpio_list_data_info;

typedef struct gpio_list_data
{
	unsigned int count;
	gpio_list_data_info *info;
}  __attribute__((packed)) gpio_list_data;

typedef struct sgpio_config_data
{
	unsigned int 	       bus;
	unsigned int 	       speed_hz;
	unsigned int 	       n_inputs;
	unsigned int 	       n_outputs;
}  __attribute__((packed)) sgpio_config_data;

typedef struct gpio_reading_on_interrupt
{
	unsigned short gpio_number;
	unsigned long va_address;
} __attribute__((packed)) gpio_reading_on_interrupt;


typedef struct
{
	unsigned char id;
}__attribute__((packed)) gpio_property_t;

/****This is the structure that is passed back and forth between userspace and driver as an ioctl arg*****/
typedef struct 
{
	unsigned short PinNum; /* Not used in case of interrupt sensor data */
	union
	{
		unsigned char  data; /* Direction or Value or Polarity */
		gpio_interrupt_data gpio_int_sensor_data;
		sgpio_config_data sgpio_config;
		gpio_list_data gpio_list;
		gpio_debounce_setting gpio_debounce_data; // Quanta

	};
	gpio_property_t property;
}  __attribute__((packed)) Gpio_data_t;	


/******This is an internal structure in the driver which is used for maintaining qs******/
typedef struct _pending_interrupt_info
{
	unsigned short gpio;
	unsigned char  state; //use to determine what transition caused the interrupt.State is reading of the pin after the interrupt.
							//THIS HAS TO BE DETERMINED IN THE IRQ ITSELF
	gpio_trigger_type_info trigger_type; //cause of the interrupt, as determined by the IRQ itself
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


#define GET_MODULE_GPIO_PROPERTIES      _IOC(_IOC_WRITE,'K',0x00,0x3FFF)
#define GET_GPIO_DIRECTION              _IOC(_IOC_WRITE,'K',0x01,0x3FFF)
#define SET_GPIO_DIRECTION              _IOC(_IOC_WRITE,'K',0x02,0x3FFF)
#define GET_GPIO_POLARITY               _IOC(_IOC_WRITE,'K',0x03,0x3FFF)
#define SET_GPIO_POLARITY               _IOC(_IOC_WRITE,'K',0x04,0x3FFF)
#define GET_GPIO_PULL_UP_DOWN           _IOC(_IOC_WRITE,'K',0x05,0x3FFF)
#define SET_GPIO_PULL_UP_DOWN           _IOC(_IOC_WRITE,'K',0x06,0x3FFF)
#define READ_GPIO                       _IOC(_IOC_WRITE,'K',0x07,0x3FFF)
#define WRITE_GPIO                      _IOC(_IOC_WRITE,'K',0x08,0x3FFF)
#define SET_GPIO_OD_OUT                 _IOC(_IOC_WRITE,'K',0x09,0x3FFF)
#define REGISTER_SENSOR_INTS            _IOC(_IOC_WRITE,'K',0x0A,0x3FFF)
#define WAIT_FOR_SENSOR_INT             _IOC(_IOC_WRITE,'K',0x0B,0x3FFF)
#define UNREGISTER_SENSOR_INTS          _IOC(_IOC_WRITE,'K',0x0C,0x3FFF)
#define REGISTER_CHASSIS_INTS           _IOC(_IOC_WRITE,'K',0x0D,0x3FFF)
#define WAIT_FOR_CHASSIS_INT            _IOC(_IOC_WRITE,'K',0x0E,0x3FFF)
#define UNREGISTER_CHASSIS_INTS         _IOC(_IOC_WRITE,'K',0x0F,0x3FFF)
#define GET_SGPIO_BUS_CONFIG            _IOC(_IOC_WRITE,'K',0x10,0x3FFF)
#define SET_SGPIO_BUS_CONFIG            _IOC(_IOC_WRITE,'K',0x11,0x3FFF)
#define SET_CONT_MODE                   _IOC(_IOC_WRITE,'K',0x12,0x3FFF)
#define READ_GPIOS                      _IOC(_IOC_WRITE,'K',0x13,0x3FFF)
#define WRITE_GPIOS                     _IOC(_IOC_WRITE,'K',0x14,0x3FFF)
#define GET_GPIOS_DIR                   _IOC(_IOC_WRITE,'K',0x15,0x3FFF)
#define SET_GPIOS_DIR                   _IOC(_IOC_WRITE,'K',0x16,0x3FFF)
#define ENABLE_GPIO_TOGGLE              _IOC(_IOC_WRITE,'K',0x17,0x3FFF)
#define SET_GPIO_PROPERTY               _IOC(_IOC_WRITE,'K',0x18,0x3FFF)
#define GET_GPIO_PROPERTY               _IOC(_IOC_WRITE,'K',0x19,0x3FFF)
#define GET_GPIO_DEBOUNCE               _IOC(_IOC_WRITE,'K',0x1A,0x3FFF)
#define SET_GPIO_DEBOUNCE               _IOC(_IOC_WRITE,'K',0x1B,0x3FFF)
#define GET_GPIO_DEBOUNCE_CLOCK         _IOC(_IOC_WRITE,'K',0x1C,0x3FFF)
#define SET_GPIO_DEBOUNCE_CLOCK         _IOC(_IOC_WRITE,'K',0x1D,0x3FFF)
#define REGISTER_READING_ON_GPIO_INT    _IOC(_IOC_WRITE,'K',0x1E,0x3FFF)
#define GET_READING_ON_GPIO_INT         _IOC(_IOC_WRITE,'K',0x1F,0x3FFF)


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
    int (*setcontmode) ( void* gpin_data );
    int (*get_sgpio_bus_config) ( void* sgbus_data );
    int (*set_sgpio_bus_config) ( void* sgbus_data );
    int (*cleanup)   ( void *gpiodata);
	int (*EnableGpioToggle) (void *ToggleData); 
	int (*set_gpio_property)  ( unsigned short gpionum, unsigned char property, unsigned char value);
	int (*get_gpio_property)  ( unsigned short gpionum, unsigned char property, unsigned char *value);
	int (*get_debounce_event)    ( void* gpin_data );
	int (*set_debounce_event)    ( void* gpin_data );
	int (*reg_reading_on_ints) (unsigned short gpionum, unsigned long va_address);
	int (*get_reading_on_ints) (unsigned short gpionum, unsigned char *value);
	int (*readgpios) ( void* gpio_list_data_info, unsigned int count);
	int (*writegpios) ( void* gpio_list_data_info, unsigned int count);
	int (*setgpiosdir) ( void* gpio_list_data_info, unsigned int count);
	int (*getgpiosdir) ( void* gpio_list_data_info, unsigned int count);
	int (*get_debounce_clock)    ( void* gpin_data );
	int (*set_debounce_clock)    ( void* gpin_data );
}gpio_hal_operations_t;

typedef struct
{
        int (*process_gpio_intr) (int gpio_num, int gpio_port_pin_offset, unsigned char state, unsigned char trigger_type);
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

#define MAX_NUMBER_OF_TOGGLE_INSTANCE 4
typedef struct 
{
	unsigned short int ToggleCycle; //< multiple with 10ms(1 kernel tick)
	struct
	{
		unsigned int CyclePeriod	: 20;	//< bit[0:19] mark for end of period
		unsigned int Reserved 		: 12;	//< bit[20:31]  	

	}Conf;
	struct
	{
		unsigned long long int  timeout;	//< timeout in 10ms. LED will switch off, 0xFF means no time out 
		unsigned int Pattern 		: 20; 	//< bit[0:19];
		unsigned int defaultOff		: 1;	//< bit[20)
		unsigned int enableTimeOut  : 1;	//< bit[21)
		unsigned int Reserved		: 1;	//< bit[22]	
		unsigned int Valid			: 1;	//< bit[23]
		unsigned int Port			: 5;	//< bit[24:28] Port Number
		unsigned int Number			: 3;	//< bit[29:31] GPIO Number	
	}Gpio[MAX_NUMBER_OF_TOGGLE_INSTANCE];
}__attribute__ ((packed)) ToggleData;


typedef Gpio_data_t gpio_ioctl_data;

#endif // __GPIO__
