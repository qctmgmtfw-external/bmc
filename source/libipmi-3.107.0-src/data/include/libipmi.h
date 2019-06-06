/*****************************-*- andrew-c -*-********************************/
/* Filename:    libipmi.h                                                    */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Created:     10/06/2003                                                   */
/* Modified:    03/24/2004                                                   */
/* Description: The public interface to the IPMI library.  All IPMI related  */
/*              functions are accessible through this interface.  Inherited  */
/*              from libhhm, which has gone on to a better place.            */
/*****************************************************************************/

#ifndef LIBIPMI_H
#define LIBIPMI_H

/** \file libipmi.h
 *  \brief Public interface for all IPMI related functions
 *  
 *  This library contains all functionality needed to send IPMI commands and
 *  receive responses via the I2C bus.  Common useful IPMI commands like
 *  GetDeviceID, GetSensorReading, and others are built into this library.
 *  If a needed command is not present, it can still be sent using the
 *  \ref ipmi_generic_command function.
 *
 *  This library handles all the details of sequence numbers, waiting for
 *  responses, verifying checksums, retrying commands in the event of
 *  timeouts and failures, etc.

*/

/** \mainpage IPMI Command Library
 *
 *  \section Overview
 *  
 *  This library contains all functionality needed to send IPMI commands and
 *  receive responses via the I2C bus.  Common useful IPMI commands like
 *  GetDeviceID, GetSensorReading, and others are built into this library.
 *  If a needed command is not present, it can still be sent using the
 *  \ref ipmi_generic_command function.
 *
 *  This library handles all the details of sequence numbers, waiting for
 *  responses, verifying checksums, retrying commands in the event of
 *  timeouts and failures, etc.

*/


#include "coreTypes.h"
#include "ipmi_structs.h"
#define s8 int8
#define s32 int32

/*! Return value that indicates a specified sensor is disabled, and cannot be read */
#define IPMI_SENSOR_DISABLED        ( -3 )

/* IPMI threshold state definitions for monitoring */
#define THRESH_UNINITIALIZED        ( (u16)0x00 ) /*!< Threshold state on first run */
#define THRESH_NORMAL               ( (u16)0x01 ) /*!< Sensor is normal (unused in IPMI ) */
#define THRESH_UP_NONCRIT           ( (u16)0x02 ) /*!< IPMI Upper Non-Critical Threshold */
#define THRESH_UP_CRITICAL          ( (u16)0x04 ) /*!< IPMI Upper Critical Threshold */
#define THRESH_LOW_NONCRIT          ( (u16)0x08 ) /*!< IPMI Lower Non-Critical Threshold */
#define THRESH_LOW_CRITICAL         ( (u16)0x10 ) /*!< IPMI Lower Critical Threshold */
#define THRESH_ACCESS_FAILED        ( (u16)0x20 ) /*!< Access failed sensor state */
#define THRESH_UP_NON_RECOV         ( (u16)0x40 ) /*!< IPMI Upper Non-Recoverable Threshold */
#define THRESH_LOW_NON_RECOV        ( (u16)0x80 ) /*!< IPMI Lower Non-Recoverable Threshold */


/*****                NetFn Definitions and Macros                       *****/
/* Adapted/stolen from MegaNASFreeBSD/Development/ipmid/ipmb.h originally    */
/* written by Subash Kalbarga                                                */

#define NETFN_CHASSIS           ( (u8)0x00 )
#define NETFN_BRIDGE   	        ( (u8)0x02 )
#define NETFN_SENSOREVENT       ( (u8)0x04 )
#define NETFN_APP               ( (u8)0x06 )
#define NETFN_FIRMWARE          ( (u8)0x08 )
#define NETFN_STORAGE           ( (u8)0x0A )

/* Macros for converting netfn/lun combos */
#define NETFN( netfnlun )       ( ( netfnlun & (u8)0xFC ) >> 2 )
#define NETLUN( netfnlun )      ( ( netfnlun & (u8)0x03 ) )
#define NETFNLUN( netfn,lun )   ( ( netfn << 2 ) | ( lun ) )

/* Macros for converting seq/lun combos */
#define SEQ( seqlun )           ( ( seqlun & (u8)0xFC ) >> 2 )
#define SLUN( seqlun )          ( ( seqlun & (u8)0x03 ) )
#define SEQLUN( seq, lun )      ( ( seq << 2 ) | ( lun ) )


/*****                 IPMI Constant Definitions                         *****/

/* Completion Codes */
#define IPMI_SUCCESS            ( (u8)0x00 )

/* Sensor Data Record types */
#define SDR_FULL                ( (u8)0x01 ) /**< SDR RecordType for Full Sensor Record */
#define SDR_COMPACT             ( (u8)0x02 ) /**< SDR RecordType for Compact Sensor Record */

/* Raw sensor reading numeric format codes */
#define SDR_READING_UNSIGNED    ( (u8)0 )
#define SDR_READING_1SCOMP      ( (u8)1 )
#define SDR_READING_2SCOMP      ( (u8)2 )
#define SDR_READING_NONANALOG   ( (u8)3 )

/* Max size of an SDR of type SDR_FULL.  SDR_COMPACT records are smaller. */
#define SDR_MAX_SIZE            ( 64 )

/* Chassis control codes */
#define CHASSIS_POWER_DOWN      ( (u8)0x00 )
#define CHASSIS_POWER_UP        ( (u8)0x01 )
#define CHASSIS_POWER_CYCLE     ( (u8)0x02 )
#define CHASSIS_HARD_RESET      ( (u8)0x03 )
#define CHASSIS_PULSE_DUMP      ( (u8)0x04 )
#define CHASSIS_ACPI_POWER_DOWN ( (u8)0x05 )

/* Sensor Type Codes */
#define IPMI_SENSOR_TEMPERATURE     ( (u8)0x01 )
#define IPMI_SENSOR_VOLTAGE         ( (u8)0x02 )
#define IPMI_SENSOR_CURRENT         ( (u8)0x03 )
#define IPMI_SENSOR_FAN             ( (u8)0x04 )
#define IPMI_SENSOR_POWER_SUPPLY    ( (u8)0x08 )
#define IPMI_SENSOR_POWER_UNIT      ( (u8)0x09 )
#define IPMI_SENSOR_COOLING_DEVICE  ( (u8)0x0A )
#define IPMI_SENSOR_MEMORY          ( (u8)0x0C )

/* Sensor Unit Type Codes from the IPMI spec */
#define IPMI_UNIT_UNSPECIFIED   0
#define IPMI_UNIT_DEGREES_C     1
#define IPMI_UNIT_DEGREES_F     2
#define IPMI_UNIT_DEGREES_K     3
#define IPMI_UNIT_VOLTS         4
#define IPMI_UNIT_AMPS          5
#define IPMI_UNIT_WATTS         6
#define IPMI_UNIT_JOULES        7
#define IPMI_UNIT_COULOMBS      8
#define IPMI_UNIT_VA            9
#define IPMI_UNIT_NITS          10
#define IPMI_UNIT_LUMEN         11
#define IPMI_UNIT_LUX           12
#define IPMI_UNIT_CANDELA       13
#define IPMI_UNIT_KPA           14
#define IPMI_UNIT_PSI           15
#define IPMI_UNIT_NEWTON        16
#define IPMI_UNIT_CFM           17
#define IPMI_UNIT_RPM           18
#define IPMI_UNIT_HZ            19
#define IPMI_UNIT_MICROSECOND   20
#define IPMI_UNIT_MILLISECOND   21
#define IPMI_UNIT_SECOND        22
#define IPMI_UNIT_MINUTE        23
#define IPMI_UNIT_HOUR          24
#define IPMI_UNIT_DAY           25
#define IPMI_UNIT_WEEK          26
#define IPMI_UNIT_MIL           27
#define IPMI_UNIT_INCHES        28
#define IPMI_UNIT_FEET          29
#define IPMI_UNIT_CUIN          30
#define IPMI_UNIT_CUFEET        31
#define IPMI_UNIT_MM            32
#define IPMI_UNIT_CM            33
#define IPMI_UNIT_M             34
#define IPMI_UNIT_CUCM          35
#define IPMI_UNIT_CUM           36
#define IPMI_UNIT_LITERS        37
#define IPMI_UNIT_FLUIDOUNCE    38
#define IPMI_UNIT_RADIANS       39
#define IPMI_UNIT_STERADIANS    40
#define IPMI_UNIT_REVOLUTIONS   41
#define IPMI_UNIT_CYCLES        42
#define IPMI_UNIT_GRAVITIES     43
#define IPMI_UNIT_OUNCE         44
#define IPMI_UNIT_POUND         45
#define IPMI_UNIT_FTLB          46
#define IPMI_UNIT_OZIN          47
#define IPMI_UNIT_GAUSS         48
#define IPMI_UNIT_GILBERTS      49
#define IPMI_UNIT_HENRY         50
#define IPMI_UNIT_MILLIHENRY    51
#define IPMI_UNIT_FARAD         52
#define IPMI_UNIT_MICROFARAD    53
#define IPMI_UNIT_OHMS          54
#define IPMI_UNIT_SIEMENS       55
#define IPMI_UNIT_MOLE          56
#define IPMI_UNIT_BECQUEREL     57
#define IPMI_UNIT_PPM           58
#define IPMI_UNIT_RESERVED      59
#define IPMI_UNIT_DECIBELS      60
#define IPMI_UNIT_DBA           61
#define IPMI_UNIT_DBC           62
#define IPMI_UNIT_GRAY          63
#define IPMI_UNIT_SIEVERT       64
#define IPMI_UNIT_COLORTEMPDK   65
#define IPMI_UNIT_BIT           66
#define IPMI_UNIT_KILOBIT       67
#define IPMI_UNIT_MEGABIT       68
#define IPMI_UNIT_GIGABIT       69
#define IPMI_UNIT_BYTE          70
#define IPMI_UNIT_KILOBYTE      71
#define IPMI_UNIT_MEGABYTE      72
#define IPMI_UNIT_GIGABYTE      73
#define IPMI_UNIT_WORD          74
#define IPMI_UNIT_DWORD         75
#define IPMI_UNIT_QWORD         76
#define IPMI_UNIT_LINE          77
#define IPMI_UNIT_HIT           78
#define IPMI_UNIT_MISS          79
#define IPMI_UNIT_RETRY         80
#define IPMI_UNIT_RESET         81
#define IPMI_UNIT_OVERRUNFLOW   82
#define IPMI_UNIT_UNDERRUN      83
#define IPMI_UNIT_COLLISION     84
#define IPMI_UNIT_PACKETS       85
#define IPMI_UNIT_MESSAGES      86
#define IPMI_UNIT_CHARACTERS    87
#define IPMI_UNIT_ERROR         88
#define IPMI_UNIT_CORRERROR     89
#define IPMI_UNIT_UNCORRERROR   90



/** \name IPMI Functions */
/** \{ */

/** \brief Send the IPMI Get Sensor Reading command and get the response
 *
 *  Send the GetSensorReading command to the specified slave with \a sensor as
 *  the sensor number, and get the response.
 *
 *  \param slave The slave address of the IPMI device (8-bit format)
 *  \param sensor The sensor number on the IPMI device
 *  \param reading The address of a u8 that will be filled with the sensor
 *  reading.  Note that this is a raw sensor value which must be converted
 *  by use of the \ref ipmi_convert_reading function.
 *  \param response The address of a \ref GetSensorReadingResponseStruct
 *  if you want the entire sensor response, not just the sensor reading.
 *  Pass \c NULL if you just want the sensor reading.
 *
 *  \retval "IPMI response code" Success
 *  \retval HHM_FAILURE The sensor reading could not be retrieved.
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_get_sensor_reading( u8 slave, u8 sensor, /*@out@*/u8 *reading,
                         /*@null@*//*@out@*/GetSensorReadingResponseStruct *response )
/*@globals fileSystem@*/
/*@modifies *reading,fileSystem,response@*/;


/** \brief Get a reservation to access the SDR repository
 *
 *  This is required for certain IPMI commands, such as GetSDR when doing
 *  partial reads and from non-zero offsets, as well as other commands.
 *  See the IPMI spec for details.
 *
 *  \param slave The slave address of the IPMI device (8-bit format)
 *  \param response A pointer to an \ref IPMBReserveSDRRepositoryStruct that
 *  this function fills in
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_reserve_sdr_repository( u8 slave, /*@out@*/IPMBReserveSDRRepositoryStruct *response )
/*@globals fileSystem@*/
/*@modifies *response,fileSystem@*/;


/** \brief Get a reservation to access the SEL
 *
 *  Get a reservation to access the SEL.  This is required for
 *  certain IPMI commands  See the IPMI spec for details.
 *
 *  \param slave The slave address of the IPMI device (8-bit format)
 *  \param response An \ref ReserveSELResponseStruct that this
 *  function fills in.
 *  
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_reserve_sel_repository( u8 slave, /*@out@*/ReserveSELResponseStruct *response )
/*@globals fileSystem@*/
/*@modifies *response,fileSystem@*/;


/** \brief Read an SDR entry or part of an SDR entry from an IPMI device
 *
 *  Read an SDR entry from the SDR repository.  Usually only BMCs have SDR
 *  repositories.  SDR entries have useful info such as sensor numbers,
 *  sensor types, and data conversion equations, and can be used to determine
 *  the type of sensors available, and how to monitor them.
 *  This is probably not the function you are looking for.  This function is
 *  a raw interface to the GetSDR function, suitable for performing partial
 *  reads and testing.  If you just want to read the entire SDR entry, use
 *  \ref ipmi_get_sdr_full which is much easier to use.
 *
 *  \param slave The slave address of the IPMI device (8-bit format)
 *  \param reservation_id An SDR reservation ID obtained from the 
 *  \ref ipmi_reserve_sdr_repository command, or zero if not doing a partial
 *  read and \a offset_into_record is zero.
 *  \param record_id The SDR record ID to read
 *  \param bytes_to_read The number of bytes to read from the record. Pass
 *  0xFF to read the entire record.   Be aware, however, that most BMCs do not
 *  support sending a response large enough to contain the entire record.
 *  \param offset_into_record The starting point for the SDR read operation in
 *  the record.  If this is not 0, you need to call
 *  \ref ipmi_reserve_sdr_repository before this function.
 *  \param record_data A buffer that will be filled in with the entire GetSDR
 *  response (cast this buffer to an \ref IPMBGetSDRResponseStruct after
 *  calling).  The length of the SDR entry is variable, so the data buffer
 *  should be ~255 bytes long to be safe.
 *
 *  \retval "Length of the SDR read" Success.  The IPMI completion code is 
 *  contained within the \ref IPMBGetSDRResponseStruct returned in
 *  \a record_data
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_get_sdr( u8 slave, u16 reservation_id, u16 record_id,
              u8 offset_into_record, u8 bytes_to_read, /*@out@*/u8 *record_data )
/*@globals fileSystem@*/
/*@modifies *record_data,fileSystem@*/;


/** \brief Read an entire SDR entry from an IPMI device
 *
 *  Like \ref ipmi_get_sdr, but this command always reads the entire SDR entry,
 *  even if it needs to break the operation down into a series of partial
 *  reads.  Like \ref ipmi_get_sdr, \a record_data should be able to accomodate
 *  255 bytes.
 *
 *  \param slave The slave address of the IPMI device (8-bit format)
 *  \param record_id The SDR record to read
 *  \param record_data A buffer the function fills in that will contain the
 *  \ref IPMBGetSDRResponseStruct and the entire SDR entry specified in 
 *  \a record_id
 *
 *  \retval "Length of the SDR read" Success.  The completion code is contained
 *  within the \ref IPMBGetSDRResponseStruct returned in \a record_data
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_get_sdr_full( u8 slave, u16 record_id, /*@out@*/u8 *record_data )
/*@globals fileSystem@*/
/*@modifies *record_data,fileSystem@*/;


/** \brief Get an SEL entry from an IPMI device
 *
 *  Read an SEL entry from the SEL.  Usually only BMCs have an SEL.  This is
 *  probably not the function you are looking for.  This function is a raw
 *  interface to the GetSEL function, suitable for performing partial reads
 *  and testing.  If you just want to read the entire SEL entry, use
 *  \ref ipmi_get_sel_full, which is much easier to use.
 *
 *  \param slave The slave address of the IPMI device (8-bit format)
 *  \param reservation_id An SEL reservation ID obtained from the
 *  \ref ipmi_reserve_sel_repository command, or zero if not doing a partial
 *  read and \a offset_into_record is zero.
 *  \param record_id The SEL record ID to read.
 *  \param offset_into_record The offset into the specified record to read
 *  from.  Zero for the beginning of the record.
 *  \param bytes_to_read The number of bytes to read from the record. Pass
 *  0xFF to read the entire record.   Be aware, however, that most BMCs do
 *  not support sending a response large enough to contain the entire record
 *  \param response A \ref GetSELEntryResponseStruct that will be filled in
 *  with the entire GetSEL response.
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_get_sel_entry( u8 slave, u16 reservation_id, u16 record_id,
                    u8 offset_into_record, u8 bytes_to_read,
                    /*@out@*/GetSELEntryResponseStruct *response )
/*@globals fileSystem@*/
/*@modifies *response,fileSystem@*/;


/** \brief Get an entire SEL entry from an IPMI device
 *
 *  Like \ref ipmi_get_sel_entry, but this command always reads the entire SEL
 *  entry, even if it needs to break the operation down into a series of
 *  partial reads
 *
 *  \param slave The slave address of the IPMI device (8-bit format)
 *  \param record_id The SEL entry to read
 *  \param response A \ref GetSELEntryResponseStruct pointer which will
 *  be filled in by this function.
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
ipmi_get_sel_full( u8 slave, u16 record_id, /*@out@*/GetSELEntryResponseStruct *response )
/*@globals fileSystem@*/
/*@modifies *response,fileSystem@*/;


/** \brief Send the IPMI Get Device ID command and get the response
 *
 *  Send the get device ID IPMI command to a slave address on the I2C bus, and
 *  receive the response.
 *
 *  \param slave Slave address of the IPMI device (8-bit format)
 *  \param response The above slave's complete response to the get device
 *  ID command.
 *
 *  \retval "IPMI Completion Code" Succes
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_get_device_id( u8 slave, /*@out@*/IPMBGetDeviceIDResponseStruct *response )
/*@globals fileSystem@*/
/*@modifies *response,fileSystem@*/;


/** \brief Send the IPMI Chassis Power Down command
 *
 *  \param slave Slave address of the IPMI device (8-bit format)
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_chassis_power_down( u8 slave )
/*@globals fileSystem@*//*@modifies fileSystem@*/;


/** \brief Send the IPMI Chassis Power Up command
 *
 *  \param slave Slave address of the IPMI device (8-bit format)
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_chassis_power_up( u8 slave )
/*@globals fileSystem@*//*@modifies fileSystem@*/;


/** \brief Send the IPMI Chassis Power Cycle command
 *
 *  \param slave Slave address of the IPMI device (8-bit format)
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_chassis_power_cycle( u8 slave )
/*@globals fileSystem@*//*@modifies fileSystem@*/;


/** \brief Send the IPMI Chassis Hard Reset command
 *
 *  \param slave Slave address of the IPMI device (8-bit format)
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_chassis_hard_reset( u8 slave )
/*@globals fileSystem@*//*@modifies fileSystem@*/;


/** \brief Send the IPMI Chassis ACPI Power Down command
 *
 *  \param slave Slave address of the IPMI device (8-bit format)
 *
 *  \retval "IPMI Completion Code" Success
 *  \retval HHM_FAILURE Failure, errno set
 *
 *  \note For almost all sensors, the IPMI response code should be 0x00.
 *  Some devices might return other completion codes to convey OEM specific
 *  information, or just to be contrary.  If the behaviour of a particular
 *  device is not well known with respect to completion codes, assume that if
 *  a completion code other than 0x00 is returned, the value in \a reading
 *  is invalid.
 */
/*@external@*/extern int
ipmi_chassis_acpi_power_down( u8 slave )
/*@globals fileSystem@*//*@modifies fileSystem@*/;


/** \brief Send an arbitrary IPMI command, and get the response
 *
 *  Send any IPMI command your heart desires to any I2C/LUN combo and receive
 *  the response.  See the IPMI spec for further info.
 *
 *  \param slave Slave address of the IPMI device (8-bit format)
 *  \param netFn The net function of the command to send
 *  \param targetLUN The LUN of the IPMI device
 *  \param command A buffer containing the command number, and any extra data
 *  which is needed for the command.
 *  \param cmd_len The length of the command buffer
 *  \param response_buffer The buffer which will be filled with the response
 *  to your ipmi command.
 *  \param response_len The size of the buffer to which
 *  \a response_buffer points
 *
 *  \retval "Length of the IPMI response" Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
ipmi_generic_command( u8 slave, u8 netFn, u8 targetLUN, u8 *command,
                      size_t cmd_len, /*@out@*/u8 *response_buffer,
                      size_t response_len )
/*@globals fileSystem@*/
/*@modifies *response_buffer,fileSystem@*/;


/** \brief Convert a raw sensor reading to its final reading using an SDR
 *
 *  Use the standard IPMI method and SDR data to convert a raw sensor reading
 *  into its converted value.  See the IPMI spec for further info on this
 *  conversion.
 *
 *  \param sdr_buffer A pointer to a complete SDR entry which contains reading
 *  conversion data.  IE, it is a Full SDR entry.
 *  \param raw_reading The raw sensor reading you want to convert
 *  \param converted_reading Pointer to an s32 that will be assigned the
 *  converted reading on success.
 *
 *  \retval 0 Success, or sensor out of IPMI sensor min/max reading range, with
 *  0 in \a converted_reading
 *  \retval HHM_FAILURE Not enough information to perform the conversion
 */
/*@external@*/extern int
ipmi_convert_reading( u8 *sdr_buffer, u8 raw_reading, /*@out@*/double *converted_reading )
/*@modifies *converted_reading@*/;


/** \brief Use an SDR entry to read the corresponding sensor
 *
 *  Use a (usually saved) SDR entry to read the current value of
 *  a sensor.  The reading is performed as specified by the SDR
 *  entry passed in \a sdr_buffer.  The reading obtained is the
 *  final, converted sensor reading
 *
 *  \param sdr_buffer Pointer to a complete SDR buffer that contains
 *  information on accessing a sensor.
 *  \param raw_reading The raw sensor reading fresh from the BMC.
 *  \param reading The converted sensor reading read and converted
 *  as specified by the SDR entry.
 *  \param discrete Pointer to a u8 used as a boolean, indicates if
 *  the sensor read is a discrete sensor instead of an analog sensor
 *
 *  \retval HHM_FAILURE Failure
 *  \retval IPMI_SENSOR_DISABLED Reading is invalid, this sensor is disabled
 *  \retval 0 Success
 */
/*@external@*/extern int
hhm_access_sdr_sensors( u8 *sdr_buffer, /*@out@*/u8 *raw_reading,
                        /*@out@*/float *reading, /*@out@*/u8 *discrete )
/*@globals fileSystem@*/
/*@modifies *raw_reading,*reading,fileSystem,discrete@*/;

/** \} */

/** \brief Extract a descriptive string from an SDR entry
 *
 *  Read the text description field from the provided SDR, convert it
 *  to a nice standard ascii string, and copy it into \a description.
 *
 *  \param sdr_buffer Pointer to a buffer containing an SDR
 *  \param description Pointer to a buffer that will be filled with the
 *  description of this SDR.  \a Description should have 17 bytes of storage,
 *  to account for the 16 possible bytes of description defined by IPMI, and
 *  the \c NULL terminator.
 *
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
extract_sdr_id( u8 *sdr_buffer, /*@out@*/char *description )
/*@modifies *description@*/;


/** \brief Convert an IPMI (or AMI OEM) unit type code to a descriptive string
 *
 *  Convert an IPMI or AMI OEM unit type code into a string suitable for
 *  display.  This function could (for example), convert the unit type 0x01 to
 *  the string "degrees C".
 *
 *  \param unit_type_code An IPMI (or AMI OEM) unit type code
 *  \param unit_type_string A char buffer that will contain the unit string
 *  on completion.
 *  \param unit_string_len The length of the unit string.  Expect around 20
 *  chars at most, but there is no internal limit to any particular size.
 *
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set, "Unrecognized" is set in
 *  \a unit_type_string.
 */
/*@external@*/extern int
sdk_unit_to_string( u8 unit_type_code, /*@out@*/char *unit_type_string,
                    size_t unit_string_len )
/*@modifies unit_type_string@*/;


/** \brief Generate a string corresponding to a discrete reading code
 *
 *  Convert a discrete state (supplied in \a reading) into a string using the
 *  \a reading_type_code and the IPMI table of discrete states.
 *
 *  \param reading The sensor reading (discrete state)
 *  \param reading_type_code The reading type code from the SDR
 *  \param sensor_type The sensor type code from the SDR
 *  \param discrete_string Pointer to a buffer we will fill with text
 *  describing the discrete state
 *  \param string_size The amount of storage at \a discrete_string
 *
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
ipmi_discrete_reading_string( s32 reading, u8 reading_type_code,
                              u8 sensor_type,
                              /*@out@*/char *discrete_string,
                              size_t string_size )
/*@modifies discrete_string@*/;


/** \brief Get the threshold state of a sensor based on its SDR
 *
 *  Determine the state of a given sensor based on threshold information
 *  from its SDR.  This state is a bitfield with individual bits indicating
 *  which thresholds have been passed.  A bit is set to 1 when the threshold
 *  it represents has been passed.  This function also handles threshold
 *  hysteresis values correctly.
 *
 *  \param sensor_reading A raw, unconverted sensor reading
 *  \param sdr_buffer A pointer to a buffer containing an SDR
 *  \param last_state The previous state of the sensor, obtained from a prior
 *  call to this function.  Pass \ref THRESH_UNINITIALIZED if there is no
 *  \a last_state because this is the first call to \ref get_sdr_sensor_state
 *  for this sensor.
 *  \param current_state Variable that will be filled with the current state
 *  of the sensor, based on information passed in \a sensor_reading and 
 *  \a sdr_buffer.  See \ref THRESH_UP_NONCRIT for an example of a state
 *  bitfield.
 *
 *  \retval 0 Success
 *  \retval HHM_FAILURE Failure, errno set
 */
/*@external@*/extern int
get_sdr_sensor_state( u8 sensor_reading, u8 *sdr_buffer, u16 last_state,
                      /*@out@*/u16 *current_state )
/*@modifies *current_state@*/;

#endif
