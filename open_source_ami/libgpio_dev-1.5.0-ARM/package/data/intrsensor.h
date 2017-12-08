/***********************************************************
 **                                                       **
 **    (C)Copyright 2006-2009, American Megatrends Inc.   **
 **                                                       **
 **            All Rights Reserved.                       **
 **                                                       **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,        **
 **                                                       **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.    **
 **                                                       **
 ***********************************************************/

/*****************************-*- ********-*-**************/
/* Filename:    intrsensor.h                              */
/* Description: Library interface to gpio access          */
/**********************************************************/
#ifndef __INTRSENSOR_H__
#define __INTRSENSOR_H__


/*** Global Type definitions ***/

/* */
typedef enum
{

 /* GPIO Interrupts */

   IPMI_INT_GPIO0 = 0,
   IPMI_INT_GPIO1,   	
   IPMI_INT_GPIO2,   	
   IPMI_INT_GPIO3,   	
   IPMI_INT_GPIO4,   	
   IPMI_INT_GPIO5,   	
   IPMI_INT_GPIO6,   	
   IPMI_INT_GPIO7,   	
   IPMI_INT_GPIO8,   	
   IPMI_INT_GPIO9,   	
   IPMI_INT_GPIO10,   	
   IPMI_INT_GPIO11,   	
   IPMI_INT_GPIO13,   	
   IPMI_INT_GPIO14,   	
   IPMI_INT_GPIO15,   	
   IPMI_INT_GPIO16,   	
   IPMI_INT_GPIO17,   	
   IPMI_INT_GPIO18,   	
   IPMI_INT_GPIO19,   	
   IPMI_INT_GPIO20,   	
   IPMI_INT_GPIO21,   	
   IPMI_INT_GPIO22,   	
   IPMI_INT_GPIO23,   	
   IPMI_INT_GPIO24,   	
   IPMI_INT_GPIO25,   	
   IPMI_INT_GPIO26,   	
   IPMI_INT_GPIO27,   	
   IPMI_INT_GPIO28,   	
   IPMI_INT_GPIO29,   	
   IPMI_INT_GPIO30,   	
   IPMI_INT_GPIO31,   	

   /* External Interrupts */	
   IPMI_INT_EXT0 = 100,
   IPMI_INT_EXT1,

   /* LPC Reset Interrupt */	
   IPMI_INT_LPCRESET = 150,
 	

} IPMI_INT_E;



typedef enum _sensor_type_info {
	THRESHOLD_SENSOR,
	NON_THRESHOLD_SENSOR
}  sensor_type_info;

typedef enum _trigger_method_info {
	EDGE,
	LEVEL
}  trigger_method_info;

typedef enum _trigger_type_info {
	RISING_EDGE,
	FALLING_EDGE,
	HIGH_LEVEL,
	LOW_LEVEL,
	BOTH_EDGES 
}  trigger_type_info;

typedef struct _interrupt_sensor_info
{
	int 	       int_num;
	int 	       ext_irq;
	unsigned short gpio_number;
	unsigned short sensor_number;
	sensor_type_info sensor_type; 
	trigger_method_info trigger_method;
	trigger_type_info trigger_type;
	unsigned char reading_on_assertion; //indicates what the reading was after assertion. used to determine what caused the assertion
										//for e.g.: in both edge triggered if reading after assertion was 0 then high to low caused it
										//in level low if reading after assertion was 0 then level was low and hence the interrupt happened
										// This may be invalid and it is upto the user program to validate
										// for e.g.: a line may go low to high and then back to low again and if this line is cofnigured for rising edege 
										// then at the time of reading the pin may have gone back to low in which case the application can conclude that
										// the transition happened before the state was determined.
}  __attribute__((packed)) interrupt_sensor_info;

typedef struct _interrupt_sensor_data
{
	unsigned int total_sensors;
	interrupt_sensor_info *int_sensor_info;
}  __attribute__((packed)) interrupt_sensor_data;

#endif // __INTRSENSOR_H__
