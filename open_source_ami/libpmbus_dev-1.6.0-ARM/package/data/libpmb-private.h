/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
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
* libpmb-private.h
*
* Private prototypes and defines for libpmb.
*
* Author: Revanth A <revantha@amiindia.co.in>
*
******************************************************************/
#ifndef LIBPMB_PRIVATE_H
#define LIBPMB_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "coreTypes.h"
#include "libi2c.h"

/**************************** I2C Definitions ********************************/

#define I2C_READ_WRITE    2
#define PMB_WORD    2
#define PMB_BYTE    1
#define PMB_BLOCK    3
#define I2C_SEND_BYTE    3
#define PMB_BLOCK_PROC_CALL    7

typedef enum
{
    PAGE=0x0,
    OPERATION,
    ON_OFF_CONFIG,
    CLEAR_FAULTS,
    PHASE,
    WRITE_PROTECT=0x10,
    STORE_DEFAULT_ALL,
    RESTORE_DEFAULT_ALL,
    STORE_DEFAULT_CODE,
    RESTORE_DEFAULT_CODE,
    STORE_USER_ALL,
    RESTORE_USER_ALL,
    STORE_USER_CODE,
    RESTORE_USER_CODE,
    CAPABILITY,
    QUERY,
    VOUT_MODE=0x20,
    VOUT_COMMAND,
    VOUT_TRIM,
    VOUT_CAL_OFFSET,
    VOUT_MAX,
    VOUT_MARGIN_HIGH,
    VOUT_MARGIN_LOW,
    VOUT_TRANSITION_RATE,
    VOUT_DROOP,
    VOUT_SCALE_LOOP,
    VOUT_SCALE_MONITOR,
    COEFFICIENTS=0x30,
    POUT_MAX=0x31,
    MAX_DUTY,
    FREQUENCY_SWITCH,
    VIN_ON=0x35,
    VIN_OFF,
    INTERLEAVE,
    IOUT_CAL_GAIN,
    IOUT_CAL_OFFSET,
    FAN_CONFIG_1_2,
    FAN_COMMAND_1,
    FAN_COMMAND_2,
    FAN_CONFIG_3_4,
    FAN_COMMAND_3,
    FAN_COMMAND_4,
    VOUT_OV_FAULT_LIMIT,
    VOUT_OV_FAULT_RESPONSE,
    VOUT_OV_WARN_LIMIT,
    VOUT_UV_WARN_LIMT,
    VOUT_UV_FAULT_LIMIT,
    VOUT_UV_FAULT_RESPONSE,
    IOUT_OC_FAULT_LIMIT,
    IOUT_OC_FAULT_RESPONSE,
    IOUT_OC_LV_FAULT_LIMIT,
    IOUT_OC_LV_FAULT_RESPONSE,
    IOUT_OC_WARN_LIMIT,
    IOUT_UC_FAULT_LIMIT,
    IOUT_UC_FAULT_RESPONSE,
    OT_FAULT_LIMIT=0x4F,
    OT_FAULT_RESPONSE,
    OT_WARN_LIMIT,
    UT_WARN_LIMIT,
    UT_FAULT_LIMIT,
    UT_FAULT_RESPONSE,
    VIN_OV_FAULT_LIMIT,
    VIN_OV_FAULT_RESPONSE,
    VIN_OV_WARN_LIMIT,
    VIN_UV_WARN_LIMIT,
    VIN_UV_FAULT_LIMIT,
    VIN_UV_FAULT_RESPONSE,
    IIN_OC_FAULT_LIMIT,
    IIN_OC_FAULT_RESPONSE,
    IIN_OC_WARN_LIMIT,
    POWER_GOOD_ON,
    POWER_GOOD_OFF,
    TON_DELAY,
    TON_RISE,
    TON_MAX_FAULT_LIMIT,
    TON_MAX_FAULT_RESPONSE,
    TOFF_DELAY,
    TOFF_FALL,
    TOFF_MAX_WARN_LIMIT,
    POUT_OP_FAULT_LIMIT=0x68,
    POUT_OP_FAULT_RESPONSE,
    POUT_OF_WARN_LIMIT,
    PIN_OP_WARN_LIMIT,
    STATUS_BYTE=0x78,
    STATUS_WORD,
    STATUS_VOUT,
    STATUS_IOUT,
    STATUS_INPUT,
    STATUS_TEMPERATURE,
    STATUS_CML,
    STATUS_OTHER,
    STATUS_MFR_SPECIFIC,
    STATUS_FANS_1_2,
    STATUS_FANS_3_4,
    READ_VIN=0x88,
    READ_IIN,
    READ_VCAP,
    READ_VOUT,
    READ_IOUT,
    READ_TEMPERATURE_1,
    READ_TEMPERATURE_2,
    READ_TEMPERATURE_3,
    READ_FAN_SPEED_1,
    READ_FAN_SPEED_2,
    READ_FAN_SPEED_3,
    READ_FAN_SPEED_4,
    READ_DUTY_CYCLE,
    READ_FREQUENCY,
    READ_POUT,
    READ_PIN,
    PMBUS_REVISION,
    MFR_ID,
    MFR_MODEL,
    MFR_REVISION,
    MFR_LOCATION,
    MFR_DATE,
    MRF_SERIAL,
    MFR_VIN_MIN=0xA0,
    MFR_VIN_MAX,
    MFR_IIN_MAX,
    MFR_PIN_MAX,
    MFR_VOUT_MIN,
    MFR_VOUT_MAX,
    MFR_IOUT_MAX,
    MFR_POUT_MAX,
    MFR_TAMBIENT_MAX,
    MFR_TAMBIENT_MIN,
    MFR_EFFICIENCY_LL,
    MFR_EFFICIENCY_HL,
    USER_DATA_00,
    USER_DATA_01,
    USER_DATA_02,
    USER_DATA_03,
    USER_DATA_04,
    USER_DATA_05,
    USER_DATA_06,
    USER_DATA_07,
    USER_DATA_08,
    USER_DATA_09,
    USER_DATA_10,
    USER_DATA_11,
    USER_DATA_12,
    USER_DATA_13,
    USER_DATA_14,
    USER_DATA_15
}PMB_CMD;


typedef struct
{
    u8 cmdName;
    u8 transType;
    u8 dataBytes;
}PMBus_t;

const PMBus_t g_PMBus[] =
{
    {PAGE,I2C_READ_WRITE,PMB_BYTE},
    {OPERATION,I2C_READ_WRITE,PMB_BYTE},
    {ON_OFF_CONFIG,I2C_READ_WRITE,PMB_BYTE},
    {CLEAR_FAULTS,I2C_SEND_BYTE,0},
    {PHASE,I2C_READ_WRITE,PMB_BYTE},
    {WRITE_PROTECT,I2C_READ_WRITE,PMB_BYTE},
    {STORE_DEFAULT_ALL,I2C_SEND_BYTE,0},
    {RESTORE_DEFAULT_ALL,I2C_SEND_BYTE,0},
    {STORE_DEFAULT_CODE,I2C_WRITE,PMB_BYTE},
    {RESTORE_DEFAULT_CODE,I2C_WRITE,PMB_BYTE},
    {STORE_USER_ALL,I2C_SEND_BYTE,0},
    {RESTORE_USER_ALL,I2C_SEND_BYTE,0},
    {STORE_USER_CODE,I2C_WRITE,PMB_BYTE},
    {RESTORE_USER_CODE,I2C_WRITE,PMB_BYTE},
    {CAPABILITY,I2C_READ,PMB_BYTE},
    {QUERY,I2C_READ_WRITE,PMB_BLOCK_PROC_CALL},
    {VOUT_MODE,I2C_READ_WRITE,PMB_WORD},
    {VOUT_COMMAND,I2C_READ_WRITE,PMB_WORD},
    {VOUT_TRIM,I2C_READ_WRITE,PMB_WORD},
    {VOUT_CAL_OFFSET,I2C_READ_WRITE,PMB_WORD},
    {VOUT_MAX,I2C_READ_WRITE,PMB_WORD},
    {VOUT_MARGIN_HIGH,I2C_READ_WRITE,PMB_WORD},
    {VOUT_MARGIN_LOW,I2C_READ_WRITE,PMB_WORD},
    {VOUT_TRANSITION_RATE,I2C_READ_WRITE,PMB_WORD},
    {VOUT_DROOP,I2C_READ_WRITE,PMB_WORD},
    {VOUT_SCALE_LOOP,I2C_READ_WRITE,PMB_WORD},
    {VOUT_SCALE_MONITOR,I2C_READ_WRITE,PMB_WORD},
    {COEFFICIENTS,I2C_READ_WRITE,PMB_BLOCK_PROC_CALL},
    {POUT_MAX,I2C_READ_WRITE,PMB_WORD},
    {MAX_DUTY,I2C_READ_WRITE,PMB_WORD},
    {FREQUENCY_SWITCH,I2C_READ_WRITE,PMB_WORD},
    {VIN_ON,I2C_READ_WRITE,PMB_WORD},
    {VIN_OFF,I2C_READ_WRITE,PMB_WORD},
    {INTERLEAVE,I2C_READ_WRITE,PMB_WORD},
    {IOUT_CAL_GAIN,I2C_READ_WRITE,PMB_WORD},
    {IOUT_CAL_OFFSET,I2C_READ_WRITE,PMB_WORD},
    {FAN_CONFIG_1_2,I2C_READ_WRITE,PMB_BYTE},
    {FAN_COMMAND_1,I2C_READ_WRITE,PMB_WORD},
    {FAN_COMMAND_2,I2C_READ_WRITE,PMB_WORD},
    {FAN_CONFIG_3_4,I2C_READ_WRITE,PMB_BYTE},
    {FAN_COMMAND_3,I2C_READ_WRITE,PMB_WORD},
    {FAN_COMMAND_4,I2C_READ_WRITE,PMB_WORD},
    {VOUT_OV_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {VOUT_OV_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {VOUT_OV_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {VOUT_UV_WARN_LIMT,I2C_READ_WRITE,PMB_WORD},
    {VOUT_UV_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {VOUT_UV_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {IOUT_OC_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {IOUT_OC_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {IOUT_OC_LV_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {IOUT_OC_LV_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {IOUT_OC_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {IOUT_UC_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {IOUT_UC_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {OT_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {OT_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {OT_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {UT_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {UT_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {UT_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {VIN_OV_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {VIN_OV_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {VIN_OV_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {VIN_UV_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {VIN_UV_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {VIN_UV_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {IIN_OC_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {IIN_OC_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {IIN_OC_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {POWER_GOOD_ON,I2C_READ_WRITE,PMB_WORD},
    {POWER_GOOD_OFF,I2C_READ_WRITE,PMB_WORD},
    {TON_DELAY,I2C_READ_WRITE,PMB_WORD},
    {TON_RISE,I2C_READ_WRITE,PMB_WORD},
    {TON_MAX_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {TON_MAX_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {TOFF_DELAY,I2C_READ_WRITE,PMB_WORD},
    {TOFF_FALL,I2C_READ_WRITE,PMB_WORD},
    {TOFF_MAX_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {POUT_OP_FAULT_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {POUT_OP_FAULT_RESPONSE,I2C_READ_WRITE,PMB_BYTE},
    {POUT_OF_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {PIN_OP_WARN_LIMIT,I2C_READ_WRITE,PMB_WORD},
    {STATUS_BYTE,I2C_READ,PMB_BYTE},
    {STATUS_WORD,I2C_READ,PMB_WORD},
    {STATUS_VOUT,I2C_READ,PMB_BYTE},
    {STATUS_IOUT,I2C_READ,PMB_BYTE},
    {STATUS_INPUT,I2C_READ,PMB_BYTE},
    {STATUS_TEMPERATURE,I2C_READ,PMB_BYTE},
    {STATUS_CML,I2C_READ,PMB_BYTE},
    {STATUS_OTHER,I2C_READ,PMB_BYTE},
    {STATUS_MFR_SPECIFIC,I2C_READ,PMB_BYTE},
    {STATUS_FANS_1_2,I2C_READ,PMB_BYTE},
    {STATUS_FANS_3_4,I2C_READ,PMB_BYTE},
    {READ_VIN,I2C_READ,PMB_WORD},
    {READ_IIN,I2C_READ,PMB_WORD},
    {READ_VCAP,I2C_READ,PMB_WORD},
    {READ_VOUT,I2C_READ,PMB_WORD},
    {READ_IOUT,I2C_READ,PMB_WORD},
    {READ_TEMPERATURE_1,I2C_READ,PMB_WORD},
    {READ_TEMPERATURE_2,I2C_READ,PMB_WORD},
    {READ_TEMPERATURE_3,I2C_READ,PMB_WORD},
    {READ_FAN_SPEED_1,I2C_READ,PMB_WORD},
    {READ_FAN_SPEED_2,I2C_READ,PMB_WORD},
    {READ_FAN_SPEED_3,I2C_READ,PMB_WORD},
    {READ_FAN_SPEED_4,I2C_READ,PMB_WORD},
    {READ_DUTY_CYCLE,I2C_READ,PMB_WORD},
    {READ_FREQUENCY,I2C_READ,PMB_WORD},
    {READ_POUT,I2C_READ,PMB_WORD},
    {READ_PIN,I2C_READ,PMB_WORD},
    {PMBUS_REVISION,I2C_READ,PMB_BYTE},
    {MFR_ID,I2C_READ_WRITE,PMB_BLOCK},
    {MFR_MODEL,I2C_READ_WRITE,PMB_BLOCK},
    {MFR_REVISION,I2C_READ_WRITE,PMB_BLOCK},
    {MFR_LOCATION,I2C_READ_WRITE,PMB_BLOCK},
    {MFR_DATE,I2C_READ_WRITE,PMB_BLOCK},
    {MRF_SERIAL,I2C_READ_WRITE,PMB_BLOCK},
    {MFR_VIN_MIN,I2C_READ,PMB_WORD},
    {MFR_VIN_MAX,I2C_READ,PMB_WORD},
    {MFR_IIN_MAX,I2C_READ,PMB_WORD},
    {MFR_PIN_MAX,I2C_READ,PMB_WORD},
    {MFR_VOUT_MIN,I2C_READ,PMB_WORD},
    {MFR_VOUT_MAX,I2C_READ,PMB_WORD},
    {MFR_IOUT_MAX,I2C_READ,PMB_WORD},
    {MFR_POUT_MAX,I2C_READ,PMB_WORD},
    {MFR_TAMBIENT_MAX,I2C_READ,PMB_WORD},
    {MFR_TAMBIENT_MIN,I2C_READ,PMB_WORD},
    {MFR_EFFICIENCY_LL,I2C_READ_WRITE,PMB_BLOCK},
    {MFR_EFFICIENCY_HL,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_00,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_01,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_02,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_03,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_04,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_05,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_06,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_07,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_08,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_09,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_10,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_11,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_12,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_13,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_14,I2C_READ_WRITE,PMB_BLOCK},
    {USER_DATA_15,I2C_READ_WRITE,PMB_BLOCK}
};

/************************* Function Prototypes *******************************/
extern inline int PMBus_ReadVIN(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadIIN(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadPIN(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadRevision(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusWord(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusCML(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusOther(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusIOUT(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusINPUT(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusTemp(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusFan1_2(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadStatusFan3_4(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_SendClearFaults(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadVOUTMODE(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadVOUT(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadMFR_IOUT_MAX(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadIOUT(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadPOUT(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadCapability(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_ReadPIN_OP_WARN_LIMIT(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_WritePIN_OP_WARN_LIMIT(char *i2c_dev, u8 slave,u8 *write_buf);

extern inline int PMBus_ReadON_OFF_CONFIG(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_WriteON_OFF_CONFIG(char *i2c_dev, u8 slave,u8 *write_buf);

extern inline int PMBus_ReadOperation(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_WriteOperation(char *i2c_dev, u8 slave,u8 *write_buf);

extern inline int PMBus_ReadPOUT_MAX(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_WritePOUT_MAX(char *i2c_dev, u8 slave,u8 *write_buf);

extern inline int PMBus_ReadPOUT_OP_FAULT_LIMIT(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_WritePOUT_OP_FAULT_LIMIT(char *i2c_dev, u8 slave,u8 
*write_buf);

extern inline int PMBus_ReadPOUT_OP_FAULT_RESPONSE(char *i2c_dev, u8 slave,u8 
*read_buf);

extern inline int PMBus_WritePOUT_OP_FAULT_RESPONSE(char *i2c_dev, u8 slave,u8 
*write_buf);

extern inline int PMBus_ReadPAGE(char *i2c_dev, u8 slave,u8 *read_buf);

extern inline int PMBus_WritePAGE(char *i2c_dev, u8 slave,u8 *write_buf);

extern inline int PMBus_ReadQUERY(char *i2c_dev, u8 slave,u8 *buf);

extern inline int PMBus_ReadCOEFFICIENTS(char *i2c_dev, u8 slave,u8 *buf);

extern inline int PMBus_ReadTemp(char *i2c_dev, u8 slave,int Temp_num,u8 *read_buf);

extern inline int PMBus_ReadFanSpeed(char *i2c_dev, u8 slave,int Fan_num,u8 *read_buf);
#ifdef __cplusplus
}
#endif

#endif    //LIBPMB_PRIVATE_H
