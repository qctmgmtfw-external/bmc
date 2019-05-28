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

#ifndef __PWMTACH_IOCTL_H__
#define __PWMTACH_IOCTL_H__

typedef struct
{
	unsigned char id;
	unsigned int value;
}__attribute__((packed)) pwmtach_property_t;

typedef struct 
{
	char	device_name[16];
	unsigned int dev_id;
	unsigned int num_fans;
	unsigned char fannumber;
	unsigned int rpmvalue;
	unsigned int min_rpm;
	unsigned int max_rpm;
	unsigned char prevdutycycle;
	unsigned char dutycycle;
	unsigned int prescalervalue;
	unsigned int counterresvalue;
	unsigned int tachnumber;
	unsigned int pwmnumber;
	unsigned char status;
	pwmtach_property_t property;
	void* fanproperty_dataptr;
	void* fanmap_dataptr;
}  __attribute__((packed)) pwmtach_data_t;

typedef enum {
	ENABLE_PWM_CHANNEL,
	DISABLE_PWM_CHANNEL,
	ENABLE_TACH_CHANNEL,
	DISABLE_TACH_CHANNEL,
	SET_DUTY_CYCLE_BY_PWM_CHANNEL,
	SET_DUTY_CYCLE_VALUE_BY_PWM_CHANNEL,
	GET_TACH_VALUE_BY_TACH_CHANNEL,
	ENABLE_PWM,
	ENABLE_ALL_PWM,
	ENABLE_TACH,
	ENABLE_ALL_TACH,
	DISABLE_PWM,
	DISABLE_ALL_PWM,
	DISABLE_TACH,
	DISABLE_ALL_TACH,
	GET_TACH_STATUS,
	GET_PWM_STATUS,
	GET_PWM_CHANNEL_STATUS,
	GET_TACH_VALUE,
	SET_DUTY_CYCLE,
	INIT_PWMTACH,	
	CONFIGURE_FANMAP_TABLE,
	CONFIGURE_FANPROPERTY_TABLE,
	SHOW_FANMAP_TABLE,
	SHOW_FANPROPERTY_TABLE,
	GET_FAN_RPM_RANGE,
	GET_DUTY_CYCLE,
	SET_TACH_PROPERTY,
	GET_TACH_PROPERTY,
	SET_PWM_PROPERTY,
	GET_PWM_PROPERTY,
	CLEAR_TACH_ERROR,
	CLEAR_PWM_ERRORS,
	END_OF_FUNC_TABLE	
}ePwmTachFunc;

typedef pwmtach_data_t pwmtach_ioctl_data;

#endif // __PWMTACH_IOCTL_H__
