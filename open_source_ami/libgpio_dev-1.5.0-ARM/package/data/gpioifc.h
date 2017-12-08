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

/*****************************-*- ********-*-********************************/
/* Filename:    gpioifc.h                                                    */
/* Description: Library interface to gpio access                             */
/*****************************************************************************/

#include "intrsensor.h"

#ifndef GPIOIFC_H
#define GPIOIFC_H

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_CTL_FILE   "/dev/gpio0"

/*
typedef struct interrupt_sensor_info
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
*/

/********************************************************************************/
/* The following APIs deal with GPIO interrupts and must be used accordingly.
	An application that wishes to register for GPIO interrupts must call these
	APIs in the following manner.

	The application should first call register_gpio_interrupts().
	The application should then call wait_for_gpio_interrupts() using the fd
	that was given by the register_gpio_interrupts() call.
	The application should call unregister_gpio_interrupts() in the end. */
/********************************************************************************/
	extern int register_sensor_interrupts(interrupt_sensor_info *sensors_data,unsigned int num_sensors,int fd);
	extern int wait_for_sensor_interrupts(int fd, interrupt_sensor_info *sensor_data);
	extern int unregister_sensor_interrupts(int fd);
	extern int register_chassis_interrupts(interrupt_sensor_info *sensors_data,unsigned int num_sensors,int fd);
	extern int wait_for_chassis_interrupts(int fd, interrupt_sensor_info *sensor_data);
	extern int unregister_chassis_interrupts(int fd);



/** \file gpioifc.h
 *  \brief Public headers for the gpio interface library
 *  
 *  This library contains friendly function call interfaces for setting 
 *  gpio directions and data.  It hides all the details of playing with
 *  gpios through the gpio manager (opening the device file, calling ioctl,
 *  etc.)
*/
	extern int get_gpio_dir ( unsigned short gpio_number );
	extern int set_gpio_dir_input ( unsigned short gpio_number );
	extern int set_gpio_dir_output ( unsigned short gpio_number );
	extern int get_gpio_pol ( unsigned short gpio_number );
	extern int set_gpio_pol_high ( unsigned short gpio_number );
	extern int set_gpio_pol_low ( unsigned short gpio_number );
	extern int get_gpio_data ( unsigned short gpio_number );
	extern int set_gpio_data_high  ( unsigned short gpio_number );
	extern int set_gpio_data_low ( unsigned short gpio_number ); 
	extern int set_gpio_od_output_high( unsigned short gpio_number );
	extern int set_gpio_od_output_low( unsigned short gpio_number );
	extern int set_gpio_pull_up( unsigned short gpio_number );
	extern int set_gpio_pull_down( unsigned short gpio_number );
	extern int set_gpio_no_pull_up_down( unsigned short gpio_number );
	extern int get_gpio_pull_up_down ( unsigned short gpio_number );
	extern int set_sgpio_cont_mode_high  ( unsigned short gpio_number );

#ifdef __cplusplus
}
#endif

#endif
