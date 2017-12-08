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


#ifndef GPIOIFC_H
#define GPIOIFC_H

#ifdef __cplusplus
extern "C" {
#endif


/**/
#define DEFAULT_EXPANDED_GPIO_I2C_BUS     		0
#define DEFAULT_EXPANDED_GPIO_I2C_SLAVE	  		(0xA8 >> 1)

#define EXPANDED_GPIO_I_2_0_OFFSET				0x32
#define EXPANDED_GPIO_I_J_2_0_OFFSET			0x33
#define EXPANDED_GPIO_I_7_4_OFFSET				0x34
#define EXPANDED_GPIO_I_7_4_DRI_SLEW_OFFSET		0x35
#define EXPANDED_GPIO_I_7_3_OFFSET				0x36
#define EXPANDED_GPIO_I_3_0_OFFSET				0x39
#define EXPANDED_GPIO_I_7_4_INPUT_OFFSET		0x3A
#define EXPANDED_GPIO_J_2_0_OFFSET				0x3B
#define EXPANDED_GPIO_J_2_func_OFFSET			0x3C
#define EXPANDED_GPIO_J_1_0_OFFSET				0x3E
#define EXPANDED_GPIO_J_2_OFFSET				0x3F
#define EXPANDED_GPIO_I_0_OFFSET				0xA0

#define WRITE_MODE      0
#define READ_MODE      	1

#define EXPANDED_GPIO_0		0
#define EXPANDED_GPIO_1		1
#define EXPANDED_GPIO_2		2
#define EXPANDED_GPIO_3		3
#define EXPANDED_GPIO_4		4
#define EXPANDED_GPIO_5		5
#define EXPANDED_GPIO_6		6
#define EXPANDED_GPIO_7		7
#define EXPANDED_GPIO_8		8
#define EXPANDED_GPIO_9		9
#define EXPANDED_GPIO_10	10

#define DRIVING_MODE_4MA	0
#define DRIVING_MODE_8MA	1
#define DRIVING_MODE_12MA	2
#define DRIVING_MODE_16MA	3


/** \file gpioifc.h
 *  \brief Public headers for the gpio interface library
 *  
 *  This library contains friendly function call interfaces for setting 
 *  gpio directions and data.  It hides all the details of playing with
 *  gpios through the gpio manager (opening the device file, calling ioctl,
 *  etc.)
*/
	extern int set_expanded_gpio_bus( int bus );
	extern int get_expanded_gpio_dir ( unsigned short gpio_number);
	extern int set_expanded_gpio_dir_input ( unsigned short gpio_number );
	extern int set_expanded_gpio_dir_output ( unsigned short gpio_number );
	extern int get_expanded_gpio_slew ( unsigned short gpio_number );
	extern int set_expanded_gpio_slew_slow ( unsigned short gpio_number );
	extern int set_expanded_gpio_slew_fast ( unsigned short gpio_number );
	extern int get_expanded_gpio_data ( unsigned short gpio_number );
	extern int set_expanded_gpio_data_high  ( unsigned short gpio_number );
	extern int set_expanded_gpio_data_low ( unsigned short gpio_number ); 
	extern int set_expanded_gpio_driving_mode( unsigned short gpio_number, unsigned short driving_mode);
	extern int get_expanded_gpio_driving_mode( unsigned short gpio_number);
	extern int enable_gpio_J2(void);
	extern int enable_expanded_gpio(void);
	extern int check_expanded_gpio_init(void);

#ifdef __cplusplus
}
#endif

#endif
