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
 * IPMI_IPMB.H
 * IPMB IPMI related structures
 *
 * Author: Rama Bisa <ramab@ami.com>
 *
 *
 *****************************************************************/
#ifndef IPMI_IPMB_H
#define IPMI_IPMB_H

#include "Types.h"

#pragma pack( 1 )

/**
 * @struct IPMIEvtMsgHdr_T
 * @brief IPMI Event Message Header. 
**/
typedef struct
{
    IPMIMsgHdr_T    IPMIMsgHdr;
    INT8U   EvMRev;
    INT8U   SensorType;
    INT8U   SensorNum;
    INT8U   Ev_Type_Dir;
    INT8U   EventData1;
    INT8U   EventData2;
    INT8U   EventData3;
    INT8U   ChkSum;
} PACKED  IPMIEvtMsgHdr_T;


/**
 * @struct EvtRecord_T
 * @brief Event Record structure.
**/
typedef struct 
{
    INT8U	GeneratorID;
    INT8U	EvMRev;			/**< Event Message format version. */
    INT8U	SensorType;		/**< Sensor Type Code. */
    INT8U	SensorNum;		/**< Number of sensor that generated the event.	*/
    INT8U	Ev_Type_Dir;	/**< [7]   - 0b = Assertion event	
    						 		   - 1b = Deassertion event.
    						     [6:0] - Event Type Code.   */
    INT8U	EventData1;		/**< Event Data Field Contents.	*/
    INT8U	EventData2;		/**< Event Data Field Contents.	*/
    INT8U	EventData3;		/**< Event Data Field Contents.	*/

} PACKED  EvtRecord_T;

#pragma pack( )

#endif /* IPMI_IPMB_H */
