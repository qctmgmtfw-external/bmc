/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 ******************************************************************
 *
 * HWConfig.h
 * Hardware Config Data
 *
 *  Author: Rama Rao Bisa <ramab@ami.com>
 *
 ******************************************************************/
#ifndef HWCONFIG_H
#define HWCONFIG_H

#define MAX_I2C_BUSES       1

#pragma pack( 1 )

 /**
 * @struct ThreshSenConfig_T
 * @brief Threshold sensor configuration information.
**/
typedef struct
{
    INT8U   Enable;
    INT8U   Interval;

} PACKED  ThreshSenConfig_T;


/**
 * @struct NonThreshSenConfig_T 
 * @brief Non-threshold sensor configuration information.
**/
typedef struct
{
    INT8U   Enable;
    INT8U   Interval;

} PACKED  NonThreshSenConfig_T;


/**
 * @struct I2CConfig_T
 * @brief I2C configuration information.
**/
typedef struct
{
    INT8U   Active;
    INT8U   OwnSlaveAddr;
    INT8U   IntrNo;
    INT16U  OpFreq;
    INT16U  BaseRegAddr;

} PACKED  I2CConfig_T;


/**
 * @struct HWConfig_T
 * @brief Hardware configuration information.
**/
typedef struct
{
    INT8U                   HWCSignature [4];
    I2CConfig_T             I2CConfig [MAX_I2C_BUSES];
    ThreshSenConfig_T       ThreshSenConfig [MAX_SENSORS_ALLOWED];
    NonThreshSenConfig_T    NonThreshSenConfig [MAX_SENSORS_ALLOWED];

} PACKED  HWConfig_T;

#pragma pack( )

#endif /* HWCONFIG_H */

