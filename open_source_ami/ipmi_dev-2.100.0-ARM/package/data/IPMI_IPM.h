/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * IPMI_IPM.h
 * IPMDevice Commands Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder <Reddy bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_IPMDEV_H
#define IPMI_IPMDEV_H
#include "Types.h"


#pragma pack( 1 )

/*** Macro Definitions ***/
/**
 * @def DEVICE_ID
 * @brief Device ID
 **/
#define DEVICE_ID                       0x20
/**
 * @def SDR_SUPPORT
 * @brief SDR support bit flag 
 **/
#define SDR_SUPPORT                     0x80    
/**
 * @def DEV_REVISION                    
 * @brief Device Revision 
 **/
#define DEV_REVISION                    0x01    
#define IPMI_DEV_REVISION               DEV_REVISION
/**
 * @def DEV_AVAILABLE                   
 * @brief Device availability bit flag
 **/
#define DEV_AVAILABLE                   0x80
/**
 * @def IPMI_VERSION
 * @brief IPMI version
 **/
#define IPMI_VERSION                    0x02
/**
 * @def CHASSIS_DEVICE_SUPPORT          
 * @brief Chassis Device support bit flag
 **/
#define CHASSIS_DEVICE_SUPPORT          ( CHASSIS_DEVICE << 7 )
/**
 * @def BRIDGE_COMMAND_SUPPORT          
 * @brief Bridge Device support bit flag
 **/
#define BRIDGE_COMMAND_SUPPORT          ( BRIDGE_DEVICE << 6 )
/**
 * @def IPMB_EVENT_GENERATOR_SUPPORT    
 * @brief IPMB Event generator support bit flag 
 **/
#define IPMB_EVENT_GENERATOR_SUPPORT    ( EVENT_GENERATOR << 5 )
/**
 * @def IPMB_EVENT_RECEIVER_SUPPORT     
 * @brief IPMB Event receiver support bit flag 
 **/
#define IPMB_EVENT_RECEIVER_SUPPORT     ( EVENT_RECEIVER << 4 )
/**
 * @def FRU_INVENTORY_DEVICE_SUPPORT    
 * @brief FRU Device support bit flag 
 **/
#define FRU_INVENTORY_DEVICE_SUPPORT    ( FRU_DEVICE << 3 )
/**
 * @def SEL_DEVICE_SUPPORT              
 * @brief SEL Device support bit flag 
 **/
#define SEL_DEVICE_SUPPORT              ( SEL_DEVICE << 2 )
/**
 * @def SDR_REPOSITORY_DEVICE_SUPPORT   
 * @brief SDR Device support bit flag
 **/
#define SDR_REPOSITORY_DEVICE_SUPPORT   ( SDR_DEVICE << 1 )
/**
 * @def SENSOR_DEVICE_SUPPORT           
 * @brief Sensor Device support bit flag 
 **/
#define SENSOR_DEVICE_SUPPORT           ( SENSOR_DEVICE )

#define DEV_SUPPORT                     CHASSIS_DEVICE_SUPPORT          | BRIDGE_COMMAND_SUPPORT        | \
                                        IPMB_EVENT_GENERATOR_SUPPORT    | IPMB_EVENT_RECEIVER_SUPPORT   | \
                                        FRU_INVENTORY_DEVICE_SUPPORT    | SEL_DEVICE_SUPPORT            | \
                                        SDR_REPOSITORY_DEVICE_SUPPORT   | SENSOR_DEVICE_SUPPORT 
/**
 * @def MFG_ID_AMI                      
 * @brief IANA number for AMI 
 **/
#define MFG_ID_AMI                      {0x00,0x00,0x00}    
/**
 * @def PROD_ID_AMI_PM                  
 * @brief AMI Product ID {'E','M'} 
 **/
#define PROD_ID_AMI_PM                  0x0000

/**
 * @def DEV_GUID_NODE                   
 * @brief node - Mac Address 
 **/
#define DEV_GUID_NODE                   {0x45,0xDF,0x44,0xAC,0x25,0xD4} 
/**
 * @def DEV_GUID_CLOCK_SEQ              
 * @brief GUID Clolck Sequenbce
 **/
#define DEV_GUID_CLOCK_SEQ              0xB24A  
/**
 * @def DEV_GUID_TIME_HIGH
 * @brief GUID Time High 
 **/
#define DEV_GUID_TIME_HIGH              0x48F3  
/**
 * @def DEV_GUID_TIME_MID               
 * @brief GUID Time Mid 
 **/
#define DEV_GUID_TIME_MID               0x4F46  
/**
 * @def DEV_GUID_TIME_LOW               
 * @brief GUID Time Low 
 **/
#define DEV_GUID_TIME_LOW               0xBD2C300A 

/**
 * @def GST_NO_ERROR
 * @brief Error Code for Get Self Test Results
 **/
#define GST_NO_ERROR                    0x55 
/**
 * @def GST_CORRUPTED_DEVICES           
 * @brief Error code for Get Self Test Results 
 **/
#define GST_CORRUPTED_DEVICES           0x57 
/**
 * @def GST_CANNOT_ACCESS_SEL           
 * @brief Error code for Get Self Test Results 
 **/
#define GST_CANNOT_ACCESS_SEL           0x80 
/**
 * @def GST_CANNOT_ACCESS_SDR           
 * @brief Error code for Get Self Test Results
 **/
#define GST_CANNOT_ACCESS_SDR           0x40
/**
 * @def GST_CANNOT_ACCESS_FRU           
 * @brief Error code for Get Self Test Results 
 **/
#define GST_CANNOT_ACCESS_FRU           0x20 
/**
 * @def GST_IPMB_DO_NOT_RESPOND         
 * @brief Error code for Get Self Test Results 
 **/
#define GST_IPMB_DO_NOT_RESPOND         0x10 
/**
 * @def GST_SDR_EMPTY
 * @brief Error code for Get Self Test Results
 **/
#define GST_SDR_EMPTY                   0x08 
/**
 * @def GST_FRU_CORRUPTED               
 * @brief Error code for Get Self Test Results 
 **/
#define GST_FRU_CORRUPTED               0x04 
/**
 * @def GST_BBLK_CORRUPTED              
 * @brief Error code for Get Self Test Results 
 **/
#define GST_BBLK_CORRUPTED              0x02 
/**
 * @def GST_FIRMWARE_CORRUPTED          
 * @brief Error code for Get Self Test Results 
 **/
#define GST_FIRMWARE_CORRUPTED          0x01 

/**
 * @def IPMI_ACPI_S0                    
 * @brief ACPI State
 **/
#define IPMI_ACPI_S0                    0x00    
/**
 * @def IPMI_ACPI_S1                    
 * @brief ACPI State 
 **/
#define IPMI_ACPI_S1                    0x01    
/**
 * @def IPMI_ACPI_S2
 * @brief ACPI State 
 **/
#define IPMI_ACPI_S2                    0x02    
/**
 * @def IPMI_ACPI_S3                    
 * @brief ACPI State
 **/
#define IPMI_ACPI_S3                    0x03    
/**
 * @def IPMI_ACPI_S4                    
 * @brief ACPI State 
 **/
#define IPMI_ACPI_S4                    0x04
/**
 * @def IPMI_ACPI_S5                    
 * @brief ACPI State 
 **/
#define IPMI_ACPI_S5                    0x05    
/**
 * @def IPMI_ACPI_S4_S5                 
 * @brief ACPI State 
 **/
#define IPMI_ACPI_S4_S5                 0x06    
/**
 * @def IPMI_ACPI_G3                    
 * @brief ACPI State 
 **/
#define IPMI_ACPI_G3                    0x07
/**
 * @def IPMI_ACPI_LEGACY_ON
 * @brief ACPI State
 **/
#define IPMI_ACPI_LEGACY_ON             0x20

/**
 * @def IPMI_ACPI_LEGACY_OFF
 * @brief ACPI State
 **/
#define IPMI_ACPI_LEGACY_OFF            0x21

/**
 * @def IPMI_ACPI_SLEEPING_S1_S3                    
 * @brief ACPI State 
 **/
#define IPMI_ACPI_SLEEPING_S1_S3        0x08
/**
 * @def IPMI_ACPI_G1_SLEPPING_S1_S4                    
 * @brief ACPI State 
 **/
#define IPMI_ACPI_G1_SLEPPING_S1_S4     0x09


/* GetDevIDRes_T */
typedef struct {
    INT8U   CompletionCode;
    INT8U   DeviceID;
    INT8U   DevRevision;
    INT8U   FirmwareRevision1;
    INT8U   FirmwareRevision2;
    INT8U   IPMIVersion;
    INT8U   DevSupport;
    INT8U   MfgID[3];
    INT16U  ProdID;
    INT32U  AuxFirmwareRevision;
} PACKED  GetDevIDRes_T;

/* GetSelfTestRes_T */
typedef struct {
    INT8U   CompletionCode;
    INT8U   TestResultByte1;
    INT8U   TestResultByte2;
} PACKED  GetSelfTestRes_T;

/* SetACPIPwrStateReq_T */
typedef struct {
    INT8U   ACPISysPwrState;
    INT8U   ACPIDevPwrState;
} PACKED  SetACPIPwrStateReq_T;

/* getACPIPwrStateRes_T */
typedef struct {
    INT8U   CompletionCode;
    INT8U   ACPISysPwrState;
    INT8U   ACPIDevPwrState;
} PACKED  GetACPIPwrStateRes_T;

/* GetDevGUIDRes_T */
typedef struct {
    INT8U   CompletionCode;
    INT8U   Node[6];
    INT16U  ClockSeq;
    INT16U  TimeHigh;
    INT16U  TimeMid;
    INT32U  TimeLow;
} PACKED  GetDevGUIDRes_T;

#pragma pack( )

#endif /* IPMI_IPMDEV_H */
