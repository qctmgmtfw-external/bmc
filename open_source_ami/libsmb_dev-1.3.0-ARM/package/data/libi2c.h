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
******************************************************************/

/*****************************-*- andrew-c -*-********************************/
/* Filename:    libi2c.h                                                     */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Created:     10/03/2003                                                   */
/* Modified:    05/15/2006                                                   */
/* Description: Public prototypes for the I2C access library                 */
/*****************************************************************************/

#ifndef LIBI2C_H
#define LIBI2C_H

/** \file libi2c.h
 *  \brief Public interface for all i2c functions
 *  
 *  This library is used to access the various I2C busses connected to the card.
 *  We do all the interface to the driver here - all you need to do to send or
 *  receive I2C data is to pass it in a buffer to the appropriate function.
 *  All the details are handled internally.  At present, the following data
 *  transfer modes are supported: Master Transmit, Master Receive, Slave Receive.
 *  Both the I2C bus connected to the host machine, as well as the onboard I2C
 *  bus are supported in this library.
 */

/** \mainpage I2C Access Library
 *
 *  \section Overview
 *  This library is used to access the various I2C busses connected to the card.
 *  We do all the interface to the driver here - all you need to do to send or
 *  receive I2C data is to pass it in a buffer to the appropriate function.
 *  All the details are handled internally.  At present, the following data
 *  transfer modes are supported: Master Transmit, Master Receive, Slave Receive.
 *  Both the I2C bus connected to the host machine, as well as the onboard I2C
 *  bus are supported in this library.
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "coreTypes.h"
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/* Health monitoring status codes */
/** \brief Health monitoring is not running */
#define HEALTH_MON_INACTIVE         ( 0 )

/** \brief Health monitoring is running */
#define HEALTH_MON_ACTIVE           ( 1 )

/** \brief Filepath of file used to store whether or not we are suspended */
#define I2C_SUSPEND_STATE           "/var/state/i2c_suspend"

/* Return values */
/** \brief General failure message */
#define HHM_FAILURE                 ( -1 ) 

/** \brief Command failed because health monitoring is suspended */
#define HHM_SUSPENDED               ( -2 ) 

/** \brief Command failed because of invalid input */
#define HHM_INVAL                   ( -3 ) 

/** \brief Perform an I2C master write for this operation */
#define I2C_WRITE                   ( 0 )

/** \brief Perform an I2C master read for this operation */
#define I2C_READ                    ( 1 )


/** \brief I2C device file for multiple bus connctions /dev/i2cN */
#define I2CSTUB_DEV                 "/dev/i2c%d"
/** \brief I2C device file for the pcf8584 connected to the host */
#define I2C_DEV		                "/dev/i2c0"

/*
 * i2c_recovery is the structure used to specify the recovery parameters for the bus
 */
typedef struct
{
        int enable_recovery;
        int is_smbus_reset_enabled;
        int is_generate_clock_pulses_enabled;
        int is_force_stop_enabled;
        int num_clock_pulses;
        int frequency;
} bus_recovery_info_T;


/** \brief Encapsulation of data used to send/receive an I2C message */
typedef struct _i2c_message
{
    /** Slave address for this message */
    u8 slave;

    /** Valid options are I2C_WRITE and I2C_READ */
    int operation;

    /** The number of bytes to read or write */
    size_t size;
    
    /** Pointer to the data buffer (write or read) for this operation */
    /*@temp@*/u8 *data;
} i2c_message;


/** \name I2C Functions */
/** \{ */

/** \brief Write data to the I2C bus in master mode
 *  
 *  Write to the I2C bus using master transfer mode.  Write count bytes from
 *  the buffer \a data.  This command will send the start and stop conditions
 *  at the appropriate time, as well as handling a small number of retries in
 *  the event of sporadic i2c problems (bus collisions, bus busy, etc).
 *  Suitable for either raw I2C communication or IPMI using I2C.  This function
 *  is the normal way to write raw data to the I2C bus when you don't need to
 *  do a read immediately after the write.
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param slave Slave address for write (8-bit format)
 *  \param data Pointer to a buffer containing the data to write
 *  \param count The number of u8 bytes to write
 *  \retval "Number of bytes written" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@-exportlocal@*/
/*@external@*/extern ssize_t 
i2c_master_write( char *i2c_dev, u8 slave, u8 *data, size_t count )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;
/*@=exportlocal@*/


/** \brief Write data in master mode to an I2C bus through a specified file descriptor
 *  
 *  As \ref i2c_master_write, except it uses the specified filedescriptor instead of
 *  opening the I2C bus on its own.  Useful for some IPMI commands.
 *
 *  \param i2cfd File descriptor to which to write the data
 *  \param slave Slave address for write (8-bit format)
 *  \param data Pointer to a buffer containing the data to write
 *  \param count The number of u8 bytes to write
 *  \retval "Number of bytes written" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
extern ssize_t
i2c_master_write_on_fd( int i2cfd, u8 slave, u8 *data, size_t count );


/** \brief Write data in master mode to a specified I2C bus 
 *  
 *  As \ref i2c_master_write, except it uses a specified I2C bus.
 *  It opens and closes the I2C bus.  Useful for some IPMI commands.
 *
 *  \param bus I2C Bus number 
 *  \param slave Slave address for write (8-bit format)
 *  \param data Pointer to a buffer containing the data to write
 *  \param count The number of u8 bytes to write
 *  \retval "Number of bytes written" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
extern ssize_t
i2c_master_write_on_bus( u8 bus, u8 slave, u8 *data, size_t count );

/** \brief Read from the I2C bus in master mode
 *
 *  Read from the I2C bus using master receiver mode.  Read count bytes into
 *  the buffer \a data.  This command will send the start and stop conditions
 *  at the appropriate time, as well as handling a small number of retries in
 *  the event of sporadic i2c problems (bus collisions, bus busy, etc).
 *  Suitable for raw I2C communication with "dumb" devices such as LM7x
 *  sensors, but not IPMI devices.
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param slave Slave address for read (8-bit format)
 *  \param data Pointer to a buffer for incoming data
 *  \param count The number of u8 bytes to read
 *
 *  \retval "Number of bytes read" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern ssize_t
i2c_master_read( char *i2c_dev, u8 slave, /*@out@*/u8 *data, size_t count )
/*@modifies *data@*/;


/** \brief Read in master mode from an I2C bus through a specified file descriptor
 *
 *  As \ref i2c_master_read, except uses the passed file descriptor to do the read,
 *  instead of opening and closing the descriptor itself.
 *
 *  \param i2cfd File descriptor from which to read the data
 *  \param slave Slave address for read (8-bit format)
 *  \param data Pointer to a buffer for incoming data
 *  \param count The number of u8 bytes to read
 *
 *  \retval "Number of bytes read" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
extern ssize_t
i2c_master_read_on_fd( int i2cfd, u8 slave, u8 *data, size_t count );


/** \brief Read in master mode from a specified I2C bus
 *
 *  As \ref i2c_master_read, except uses the specified I2C bus number.
 *  
 *
 *  \param bus I2C Bus number
 *  \param slave Slave address for read (8-bit format)
 *  \param data Pointer to a buffer for incoming data
 *  \param count The number of u8 bytes to read
 *
 *  \retval "Number of bytes read" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
extern ssize_t
i2c_master_read_on_bus( u8 bus, u8 slave, u8 *data, size_t count );


/** \brief Read from the I2C bus in slave receiver mode
 *
 *  Read from the I2C bus using slave receiver mode.  Read up to \a count
 *  bytes into the buffer \a data.  This command will wait for an incoming
 *  message, and handle whatever bus problems it can.  However, if the command
 *  times out (a few seconds) without receiving a message, it will be
 *  necessary to resend the original command (probably using
 *  \ref i2c_master_write) and call i2c_slave_read again to get the response.
 *  Suitable for raw I2C communication where another device will act as a
 *  master, or for use with IPMI devices.
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param data Pointer to a buffer for incoming data
 *  \param count The maximum number of u8 bytes to read
 *
 *  \retval "Number of bytes read" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@-exportlocal@*/
/*@external@*/extern ssize_t
i2c_slave_read( char *i2c_dev, /*@out@*/u8 *data, size_t count )
/*@modifies *data@*/;
/*@=exportlocal@*/


/** \brief Read in slave receiver mode from an I2C bus specified by a file descriptor
 *
 *  As \ref i2c_slave_read, but uses the passed file descriptor to access the bus
 *  instead of opening and closing the I2C bus itself.
 *
 *  \param i2cfd File descriptor from which to read the data
 *  \param data Pointer to a buffer for incoming data
 *  \param count The maximum number of u8 bytes to read
 *
 *  \retval "Number of bytes read" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
extern ssize_t
i2c_slave_read_on_fd( int i2cfd, u8 *data, size_t count );
    
/** \brief Read in slave receiver mode from a specified I2C bus
 *
 *  As \ref i2c_slave_read, but uses the specified I2C bus.
 *
 *  \param bus I2C Bus number
 *  \param data Pointer to a buffer for incoming data
 *  \param count The maximum number of u8 bytes to read
 *
 *  \retval "Number of bytes read" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
extern ssize_t
i2c_slave_read_on_bus( u8 bus, u8 *data, size_t count );

/** \brief Write a number of bytes, then read a number of bytes on I2C
 *
 *  Write the message contained in \a write_data to the i2c bus in master
 *  transmit mode.  Write \a write_count bytes.  Next, perform a repeated
 *  start, and then read from the bus in master receiver mode.  Read
 *  \a read_count bytes.  This command keeps ownership of the bus between
 *  write and read. It handles all the start/repeated start/stop functionality
 *  internally, and will try to handle any bus problems that occur.  It may
 *  be neccessary to resend this command if problems occur.
 *  Suitable for raw I2C communication with "dumb" devices such as LM7x
 *  sensors only, but not with IPMI devices.
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param slave Slave address for read (8-bit format)
 *  \param write_data Pointer to a buffer for outgoing data
 *  \param read_data Pointer to a buffer for incoming data
 *  \param write_count The number of bytes to write
 *  \param read_count The number of bytes to read
 *  
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */  
/*@external@*/extern int
i2c_writeread( char *i2c_dev, u8 slave, u8 *write_data, /*@out@*/u8 *read_data,
               size_t write_count, size_t read_count )
/*@modifies *read_data@*/;


/** \brief Send an arbitrary number of read and/or write message on I2C
 *
 *  Perform a number of i2c transactions (reads or writes) while maintaining
 *  ownership of the bus.  That is, it sends repeated start conditions between
 *  each read/write instead of stops.  See the \ref i2c_message structure for
 *  details of the options available for each send/receive command.
 *  Suitable only for raw i2c access with dumb sensors
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param messages A pointer to an array of \ref i2c_message structs
 *  \param msgcount The number of \ref i2c_message structs above
 *
 *  \retval "Number of messages transfered" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@-exportlocal@*/
/*@external@*/extern int
i2c_multimsg( char *i2c_dev, /*@out@*/i2c_message *messages, int msgcount )
/*@modifies *messages@*/;
/*@=exportlocal@*/

/** \brief Write a number of bytes, then read a number of bytes on I2C
 *
 *  Write the message contained in \a write_data to the i2c bus in master
 *  transmit mode.  Write \a write_count bytes.  Next, perform a repeated
 *  start, and then read from the bus in master receiver mode.  Read
 *  \a read_count bytes.  This command keeps ownership of the bus between
 *  write and read. It handles all the start/repeated start/stop functionality
 *  internally, and will try to handle any bus problems that occur.  It may
 *  be neccessary to resend this command if problems occur.
 *  Suitable for raw I2C communication with "dumb" devices such as LM7x
 *  sensors only, but not with IPMI devices.
 *
 *  \param i2cfd File descriptor for the bus
 *  \param slave Slave address for read (8-bit format)
 *  \param write_data Pointer to a buffer for outgoing data
 *  \param read_data Pointer to a buffer for incoming data
 *  \param write_count The number of bytes to write
 *  \param read_count The number of bytes to read
 *
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
i2c_writeread_on_fd( int i2cfd, u8 slave, u8* write_data, u8* read_data,
                   size_t write_count, size_t read_count );

/** \brief Write a number of bytes, then read a number of bytes on I2C
 *
 *  Write the message contained in \a write_data to the i2c bus in master
 *  transmit mode.  Write \a write_count bytes.  Next, perform a repeated
 *  start, and then read from the bus in master receiver mode.  Read
 *  \a read_count bytes.  This command keeps ownership of the bus between
 *  write and read. It handles all the start/repeated start/stop functionality
 *  internally, and will try to handle any bus problems that occur.  It may
 *  be neccessary to resend this command if problems occur.
 *  Suitable for raw I2C communication with "dumb" devices such as LM7x
 *  sensors only, but not with IPMI devices.
 *
 *  \param bus I2C Bus number
 *  \param slave Slave address for read (8-bit format)
 *  \param write_data Pointer to a buffer for outgoing data
 *  \param read_data Pointer to a buffer for incoming data
 *  \param write_count The number of bytes to write
 *  \param read_count The number of bytes to read
 *
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
i2c_writeread_on_bus( u8 bus, u8 slave, u8* write_data, u8* read_data,
                   size_t write_count, size_t read_count );

/** \brief Send an arbitrary number of read and/or write message on I2C
 *
 *  Perform a number of i2c transactions (reads or writes) while maintaining
 *  ownership of the bus.  That is, it sends repeated start conditions between
 *  each read/write instead of stops.  See the \ref i2c_message structure for
 *  details of the options available for each send/receive command.
 *  Suitable only for raw i2c access with dumb sensors
 *
 *  \param i2cfd File descriptor to use for data read/write
 *  \param messages A pointer to an array of \ref i2c_message structs
 *  \param msgcount The number of \ref i2c_message structs above
 *
 *  \retval "Number of messages transfered" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@-exportlocal@*/
/*@external@*/extern int
i2c_multimsg_on_fd( int i2cfd, /*@out@*/i2c_message *messages, int msgcount )
/*@modifies *messages@*/;
/*@=exportlocal@*/


/** \brief Determine our own slave address
 *
 *  Returns our slave address (8-bit format) on the I2C bus connected
 *  to the host.
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \retval "Our slave address" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
i2c_get_host_address( char *i2c_dev )
/*@modifies nothing*/;


/** \brief Set our own slave address
 *
 *  Set our slave address on the I2C bus connected to the host
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param slave_address Our new slave address (8-bit format)
 *  
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
i2c_set_host_address( char *i2c_dev, u8 slave_address )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;

/** \brief Set recovery info
 *
 *  Set i2c bus recovery info
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param info pointer to the recovery info structure
 *  
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */

extern int 
i2c_set_recovery_info( char *i2c_dev, bus_recovery_info_T* info);

/** \brief Get recovery info
 *
 *  Get i2c bus recovery info
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \param info pointer to the recovery info structure
 *  
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */

extern int 
i2c_get_recovery_info( char *i2c_dev, bus_recovery_info_T* info);


/** \brief Reset the I2C controller
 *
 *  Reset the I2C controller.  This function is useful in the event of some
 *  sort of catastrophic driver/controller weirdness
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
i2c_reset( char *i2c_dev )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;
    

/** \brief Stop all health monitoring and use of I2C bus
 *
 *  All use of the I2C bus will be halted until a call to
 *  \ref resume_i2cbus_activity is made.  This is useful to make sure that
 *  no bus activity is taking place (for example) while the host machine
 *  is booting, because some servers use I2C for various things during
 *  initialization, and many of them don't work well in a multi-master
 *  environment.
 *
 *  \param i2c_dev String containing i2c device file to access
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int stop_i2cbus_activity( char *i2c_dev )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;


/** \brief Resume health monitoring and use of I2C bus
 *
 *  Go back to using the I2C bus normally after a call to
 *  \ref stop_i2cbus_activity.
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int resume_i2cbus_activity( void )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;


/** \brief Determine the status of I2C use on the card
 *
 *  If you are using \ref stop_i2cbus_activity and \ref resume_i2cbus_activity
 *  to control access to the I2C bus, this function can determine which state
 *  the bus is currently in.
 *
 *  \retval HEALTH_MON_ACTIVE Health monitoring active, OK to use I2C bus
 *  \retval HEALTH_MON_INACTIVE Health monitoring off, do not use I2C bus
 *  \retval HHM_FAILURE Status of the bus cannot be determined
 */
/*@external@*/extern int i2cbus_status( void )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;


/** \} */

#ifdef __cplusplus
}
#endif

#endif
