/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* libi2c-private.h
*
* Private prototypes and defines for libi2c.
*
******************************************************************/
#ifndef LIBI2C_PRIVATE_H
#define LIBI2C_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "coreTypes.h"
#include "libi2c.h"

/**************************** I2C Definitions ********************************/

/* The most messages we'll send without issuing a STOP and releasing the bus */
#define MAX_CONSEC_MSGS	    20

/* Default I/O timeout values */
#define WRITE_TIMEOUT	    1
#define READ_TIMEOUT	    1
#define RDWR_TIMEOUT	    1

/************************* Function Prototypes *******************************/

static ssize_t internal_master_write_on_bus( u8 bus, u8 slave, u8 *data,
                                             size_t count, bool do_wait );
static ssize_t internal_master_read_on_bus( u8 bus, u8 slave, u8 *data,
                                            size_t count, bool do_wait );
//static ssize_t internal_slave_read_on_bus( u8 bus, /*@out@*/u8 *data,
//                                           size_t count );
static int internal_writeread_on_bus( u8 slave, u8 *write_data, u8 *read_data,
                                      size_t write_count, size_t read_count,
                                      u8 bus, bool do_wait );
static ssize_t internal_master_write( int i2cfd, u8 slave, u8 *data,
                                      size_t count, bool do_wait );
static ssize_t internal_master_read( int i2cfd, u8 slave, u8 *data,
                                     size_t count, bool do_wait );
static ssize_t internal_slave_read( int i2cfd, /*@out@*/u8 *data,
                                    size_t count );
static int internal_writeread( u8 slave, u8 *write_data, u8 *read_data,
                               size_t write_count, size_t read_count,
                               int i2cdev, bool do_wait );
static int internal_multimsg( i2c_message *messages, int msgcount,
                              int i2cdev, bool do_wait );

    
#ifdef __cplusplus
}
#endif

#endif
