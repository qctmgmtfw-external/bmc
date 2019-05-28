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


#ifndef __FAN_STRUCTS_H__
#define __FAN_STRUCTS_H__

#define MAX_PWMTACH_DEVICES     255

struct fan_map_entry_t {
        unsigned int   fan_num;
        unsigned int   pwm_num;
        unsigned int   tach_num;
};

struct fan_property_t {
        unsigned int    fan_num;
        unsigned int    min_rpm;
        unsigned int    max_rpm;
        unsigned int    pulses_per_revolution;
};

#endif
