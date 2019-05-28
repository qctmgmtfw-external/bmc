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
 * PwmTach Common Driver Header File
 *
 * Copyright (C) 2009-2015 American Megatrends Inc.
 *
 * This file provides common layer structures and function declarations for PWMTACH driver.
 */


#ifndef __PWMTACH_H__
#define __PWMTACH_H__

#define MAX_PWMTACH_DEVICES     255

typedef struct 
{
	int (*enable_pwm_control) (unsigned char pwm_num);
	unsigned char (*disable_pwm_control) (unsigned char pwm_num);
	void (*enable_all_pwm_control) (void);
	void (*disable_all_pwm_control) (void);
	int (*enable_fantach_control) (unsigned char ft_num);
	unsigned char (*disable_fantach_control) (unsigned char ft_num);
	void (*enable_all_fantach_control) (void);
	void (*disable_all_fantach_control) (void);
	void (*enable_counterresolution) (unsigned char pwm_num);
	unsigned char (*disable_counterresolution) (unsigned char pwm_num);
	unsigned int  (*get_current_speed) (unsigned char tachnum);
	void (*set_prescale) (unsigned char pwm_num, unsigned char prescale_value);
	unsigned int  (*get_prescale) (unsigned char pwm_num);
	void (*set_dutycycle) (unsigned char pwm_num, int dutycycle_value);
	void (*set_counterresolution) (unsigned char pwm_num, unsigned int counterres_value);
	unsigned int  (*get_counterresolution) (unsigned char pwm_num);
	int (*trigger_read_fanspeed) (unsigned char tachnum);
	int  (*can_read_fanspeed) (unsigned char tachnum);
	unsigned int  (*get_num_of_pwms) (void);
	unsigned int  (*get_num_of_tachs) (void);
	unsigned int  (*get_pwm_clk) (void);
	unsigned int  (*get_dutycycle) (unsigned char pwm_num);
	unsigned char (*get_pwm_control_status) (unsigned char pwm_num);
	unsigned char (*get_fantach_control_status) (unsigned char ft_num);
	int (*set_tach_property) (unsigned char property, unsigned char pwm_tach_num, unsigned int value);
	unsigned int (*get_tach_property) (unsigned char property, unsigned char pwm_tach_num);
	int (*set_pwm_property) (unsigned char property, unsigned char pwm_tach_num, unsigned int value);
	unsigned int (*get_pwm_property) (unsigned char property, unsigned char pwm_tach_num);
	int (*clear_tach_error) (unsigned char ft_num);
	int (*clear_pwm_errors) (void);
} pwmtach_hal_operations_t;

typedef struct
{
} pwmtach_core_funcs_t;


struct pwmtach_hal
{
        char device_name[16];
        unsigned int 	dev_id;
        unsigned int    num_fans;
        unsigned int    num_pwms;
        unsigned int    num_tachs;
        struct fan_map_entry_t* fan_map_table;                  // Mapping Table for Fan number, PWM number and Tach number
        struct fan_property_t* fan_property_table;              // Fan Properties
        pwmtach_hal_operations_t *ppwmtach_hal_ops;
};

int pwmtach_read_device_list ( char *buf, char **start, off_t offset, int count, int *eof, void *data );

#endif
