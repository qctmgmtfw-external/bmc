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


static ssize_t internal_smb_master_write( int i2cfd, u8 slave,u8 size, u8 comm, u8 *data,
                                      size_t count, bool do_wait,u8 pec);

static ssize_t internal_smb_host_notify_slave_read(  int i2cfd,  u8 *data, size_t count );

static ssize_t internal_smb_master_read( int i2cfd, u8 slave, u8 size,u8 *data,bool do_wait,u8 pec );

static int internal_smb_writeread( u8 slave, u8 *write_data, u8 *read_data,
                               size_t write_count, 
                               int i2cdev,u8 size,u8 comm, bool do_wait,u8 pec );

 
#ifdef __cplusplus
}
#endif

#endif
