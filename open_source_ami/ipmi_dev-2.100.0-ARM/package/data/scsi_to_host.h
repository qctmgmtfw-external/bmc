/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 *****************************************************************
 * $Header: $
 *
 * $Revision: $
 *
 * $Date: $
 *
 ******************************************************************
 ******************************************************************
 * 
 * scsi_to_host.h
 *
 *  Author: Rama Rao Bisa <RamaB@ami.com>
 ******************************************************************/
#ifndef SCSI_TO_HOST_H
#define SCSI_TO_HOST_H

#include "Types.h"

/**
 * @def htoipmi_u32
 * @brief Converts unsigned 32 bit data from Host format to scsi format.
 * @param Val 32bit value to be converted.
 **/
#define htoscsi_u32(Val)    ((INT32U)(((Val & 0xff) << 24) +    \
                            ((Val & 0xff00) << 8) +             \
                            ((Val & 0xff0000) >> 8) +           \
                            ((Val & 0xff000000) >> 24)))

/**
 * @def ipmitoh_u32
 * @brief Converts unsigned 32 bit data from scsi format to Host format.
 * @param Val 32bit value to be converted.
 **/
#define scsitoh_u32(Val)    ((INT32U)(((Val & 0xff) << 24) +    \
                            ((Val & 0xff00) << 8) +             \
                            ((Val & 0xff0000) >> 8) +           \
                            ((Val & 0xff000000) >> 24)))

/**
 * @def htoipmi_u16
 * @brief Converts unsigned 16 bit data from Host format to scsi format.
 * @param Val 16bit value to be converted.
 **/
#define htoscsi_u16(Val)    ((INT16U)(((Val & 0xff) << 8) + (Val >> 8)))

/**
 * @def htoipmi_u16
 * @brief Converts unsigned 16 bit data from scsi format to Host format.
 * @param Val 16bit value to be converted.
 **/
#define scsitoh_u16(Val)    ((INT16U)(((Val & 0xff) << 8) + (Val >> 8)))


#endif  /* SCSI_TO_HOST_H */
