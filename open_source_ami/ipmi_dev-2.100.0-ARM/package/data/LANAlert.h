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
 ****************************************************************/
/*****************************************************************
 *
 * LANAlert.h
 * SNMP Trap generation
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef LANALERT_H
#define LANALERT_H

#include "Types.h"
#include "SELRecord.h"

#pragma pack( 1 )

#define MAX_EVENT_DATA                  0x8
#define MAX_OEM_TRAP_FIELDS             64

/*** Type Definitions ***/
/**
 * @struct VarBindings_T
 * @brief Structure for the SNMP Trap varible bindings
 **/
typedef struct
{
    INT8U   GUid[16];           /**< Global unique ID */
    INT16U  SeqCookie;          /**< Sequence Cookie */
    INT32U  TimeStamp;          /**< Platform Local Time Stamp */
    INT16U  UTCOffset;          /**< UTC offset in minutes */
    INT8U   TrapSourceType;     /**< Trap Source Type */
    INT8U   EventSourceType;    /**< Event Source Type */
    INT8U   EventSeverity;      /**< Event Severity */
    INT8U   SensorDevice;       /**< Identifies instance of a Device */
    INT8U   SensorNumber;       /**< Identifies instance of a Sensor */
    INT8U   Entity;             /**< Platform Entity ID */
    INT8U   EntityInstance;     /**< Entity instance */
    INT8U   EvtData[MAX_EVENT_DATA]; /**< Maxmimum event data */
    INT8U   LanguageCode;       /**<Language code */
    INT32U  ManufacturerID;     /**< Manufacturer ID per IANA.*/
    INT16U  SystemID;           /**< System ID */
    INT8U   Oem[MAX_OEM_TRAP_FIELDS]; /**< Oem specific trap fields */

}   PACKED VarBindings_T;


typedef struct
{
    INT8U Type;
    INT8U  Length;
    INT8U RecType;
    INT8U  RecData[61];
}PACKED OemPETField_T;
#pragma pack ()



/*** Extern Declarations ***/

/*** Function Prototypes ***/

/**
 * @brief   Generates SNMP Trap through LAN
 * @param   EvtRecord Pointer to Event record
 * @param   DestSel Destination selector
 * @param   AlertImmFlag Alert Immediate flag
 * @param   Event Severity 
 * @return  TRUE if success else FALSE
 **/
extern INT8U LANAlert (_NEAR_  SELEventRecord_T*   EvtRecord,
                               INT8U               DestSel,
                               INT8U               EventSeverity,
                               INT8U               AlertImmFlag,INT8U EthIndex,INT8U  *AlertStr, int BMCInst);
#endif /* LANALERT_H */
