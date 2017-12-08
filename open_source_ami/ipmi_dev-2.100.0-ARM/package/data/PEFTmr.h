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
 * PEFTmr.h
 * 
 * 
 * Author: Govind Kothandapani <govindk@ami.com> 
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef PEFTMRMGR_H
#define PEFTMRMGR_H

#include "Types.h"
#include "SELRecord.h"
#include "Message.h"

/*** Macro Definitions ***/
/**
 * @def MAX_DEFERRED_ALERTS
 * @brief Maximum number of Deferred Alerts
 **/
#define MAX_DEFERRED_ALERTS     8

/**
 * @def MAX_PET_ACK     
 * @brief Maximum number of PET acknowledgements 
 **/
#define MAX_PET_ACK     (MAX_DEFERRED_ALERTS * MAX_EVT_FILTER_ENTRIES) 

#pragma pack( 1 )

/*** Type definitions ***/

/**
 * @struct  PEFTmrMgr_T
 * @brief   PEF Timer table
**/
typedef struct
{
    INT8U   TmrArmed;       /**< Flag to indicate Timer is armed or not */
    INT8U   StartTmr;       /**< Flag to indicate Timer started or not */
    INT8U   TmrInterval;    /**< Timeout interval */
    INT8U   InitCountDown;  /**< Initial Count down value */
    INT8U   TakePEFAction;  /**< Action to be taken on timeout */
    INT8U   StartDlyTmr;        /**< Count Startup delay*/
    INT8U   AlertStartDlyTmr;   /**< Count Alert Startup delay*/
    INT8U   StartDlyResetFlag;
    INT8U	PEFTimerResetFlag;

} PACKED  PEFTmrMgr_T;

/**
 * @struct  PETAckTimeOutMgr_T
 * @brief   PET Acknowledgement Timer table
**/
typedef struct
{
    INT8U               Present ;   /**< Flag to indicate Timer is armed or not */
    INT8U               AckTimeOut; /**< Acknowledgement time out value */
    INT8U              RetryInterval;  /* Initial Retry interval value ,we have to use for each retry PEF Traps*/
    INT8U               Retries;    /**< Number of retries */
    INT16U              SequenceNum;/**< Sequence No from PET */
    INT32U              Timestamp;  /**< Timestamp from PET */
    INT8U               DestSel;    /**< Destination Selector from PET */
     INT8U              Channel;   /* Since we are supporting the mulit LAN channel  we have to maintain the LAN channel */
    SELEventRecord_T    EvtRecord;  /**< Event record from PET */

} PACKED  PETAckTimeOutMgr_T;

/**
 * @struct  DeferredAlertEntries_T
 * @brief   Matched entries table for deferred alerts
**/
typedef struct
{
    INT8U   EvtFilterNum;
    INT8U   Total;
    INT8U   Entries[MAX_ALERT_POLICY_ENTRIES];
    INT8U   ChannelNum;
    INT8U   DestType;
    INT8U   AlertStatus;
    INT16U  SeqNum;
    INT8U   Present;
} PACKED  DeferredAlertEntries_T;

/**
 * @struct  DeferredAlert_T
 * @brief   Deferred Alerts table
**/
typedef struct
{
    INT16U  ID;
    INT8U   Total;
    DeferredAlertEntries_T  AlertEntries[MAX_EVT_FILTER_ENTRIES];

} PACKED  DeferredAlert_T;

#pragma pack( )




/**
 * @brief This function takes an action on PEF timeout depending on the actions specified.
 **/
extern void PEFTimerTask (int BMCInst);

/**
 * @brief This function re sends the alert on PET Acnowledgement timeout
 **/
extern void PETAckTimerTask (int BMCInst);

/**
 * @brief This function is used to count down the PEF alert and power startup delay
 **/
extern void PEFStartDlyTimerTask (int BMCInst);


#endif /* PEFTMRMGR_H */
