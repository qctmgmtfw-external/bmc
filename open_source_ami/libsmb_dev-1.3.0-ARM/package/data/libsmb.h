/*****************************-*- andrew-c -*-********************************/
/* Filename:    libsmb.h                                                     */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Created:     10/03/2003                                                   */
/* Modified:    05/15/2006                                                   */
/* Description: Public prototypes for the I2C access library                 */
/*****************************************************************************/

#ifndef LIBSMB_H
#define LIBSMB_H

/** \file libsmb.h
 *  \brief Public interface for all smb functions
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

//#include "coreTypes.h"
#include "libi2c.h"






/** \name smb Functions */
/** \{ */

/** \brief Write data to the smb bus in master mode
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


/*@external@*/extern ssize_t 


smb_master_write( char *i2c_dev, u8 slave,u8 size,u8 comm, u8 *data_write, size_t count_write,u8 pec )
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
/*@external@*///extern ssize_t

//smb_master_write_host_notify(char *i2c_dev,u8 comm,u8 *data);

/*@external@*/extern ssize_t
smb_master_read( char *i2c_dev, u8 slave, u8 size,u8 comm, u8 *data, u8 pec)
/*@modifies *data@*/;


/*@external@*/extern int
smb_writeread( char *i2c_dev, u8 slave, u8 *write_data,u8 *read_data,
               size_t write_count, u8 size, u8 comm, u8 pec )
/*@modifies *read_data@*/;



/*@external@*/extern ssize_t
smb_host_notify_slave_read( char *i2c_dev, /*@out@*/u8 *data, size_t count)
/*@modifies *data@*/;



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



#ifdef __cplusplus
}
#endif

#endif
