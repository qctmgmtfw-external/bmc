/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2011, American Megatrends Inc.             **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * SSIDefs.h
 * SSI Common Definitions
 *
 * Author: Kevin Cheng <kevincheng@ami.com.tw>
 *
 ****************************************************************/

#ifndef _H_SSIDEFS_
#define _H_SSIDEFS_

#define SSI_ID              0x02
#define DEFAULT_FRU_DEV_ID  0x00

#define IPMB_PRIMARY        1
#define IPMB_SECONDARY      2

#define MAX_POWER_DRAW          CONFIG_SPX_FEATURE_SSICB_MAX_POWER_DRAW
#define POWER_MULTIPLIER        CONFIG_SPX_FEATURE_SSICB_POWER_MULTIPLIER
#define POWER_LEVEL             ((10 * MAX_POWER_DRAW) / POWER_MULTIPLIER)

/*
 * Operational State Defined Conditions
 * Conditions numbered 8 and above are available for FRU type class use.
 * A value of 1 for a condition indicates the condition is true/asserted.
 */
#define BIT32(x)                        (((INT32U)1) << (x))

#define COND_PRESENT                    0       /* FRU presence state (true = present)             */
#define COND_INSERTION_CRITERIA         1       /* Handle Switch State (true = closed)             */
#define COND_ACT_POLICY                 2       /* Activation Policy Bit State                     */
#define COND_ACTIVATED                  3       /* Activation State (true = activated)             */
#define COND_POWER_ON                   4       /* Asserted means that power is on                 */
#define COND_POWER_FAILURE              5       /* Asserted means that power failure               */
#define COND_POWER_BUDGET               6       /* Power Budget Allocated                          */
#define COND_EKEYED                     7       /* EKeying process complete                        */
#define COND_SDR_INTEGRATE              8       /* Message to managed FRUs to integrate their SDRs */
#define COND_STANDBY                    9       /* Standby State                                   */
#define COND_MAX_CONDITION              32      /* Includes OBSM and FRU type specific conditions  */
#define COND_RESERVED                   0xFF	/* Special condition value used internally         */

/* Bit map versions */
#define COND_PRESENT_BIT                BIT32(COND_PRESENT)
#define COND_INSERTION_CRITERIA_BIT     BIT32(COND_INSERTION_CRITERIA)
#define COND_ACT_POLICY_BIT             BIT32(COND_ACT_POLICY)
#define COND_ACTIVATED_BIT              BIT32(COND_ACTIVATED)
#define COND_POWER_ON_BIT               BIT32(COND_POWER_ON)
#define COND_POWER_FAILURE_BIT          BIT32(COND_POWER_FAILURE)
#define COND_POWER_BUDGET_BIT           BIT32(COND_POWER_BUDGET)
#define COND_EKEYED_BIT                 BIT32(COND_EKEYED)
#define COND_SDR_INTEGRATE_BIT          BIT32(COND_SDR_INTEGRATE)
#define COND_STANDBY_BIT                BIT32(COND_STANDBY)

#pragma pack (1)

/**
 * @enum  STATUS
 * @brief Error type status codes.
**/
typedef enum
{
    ST_OK,
    ST_INVALID_PARAM,
    ST_OUT_OF_RANGE,
    ST_ERROR,
    ST_MAX
} STATUS;

/**
 * @enum  OpState
 * @brief Operational States
**/
typedef enum 
{
    MSTATE_M0 = 0,      /* Not Present State              */
    MSTATE_M1,          /* Inactive State                 */
    MSTATE_M2,          /* Activation Requested State     */
    MSTATE_M3,          /* Activation In Progress State   */
    MSTATE_M4,          /* Active State (running)         */
    MSTATE_M5,          /* Deactivation Requested State   */
    MSTATE_M6,          /* Deactivation In Progress State */
    MSTATE_M7,          /* Communication Lost State       */
    MSTATE_M8,          /* Active State (standby)         */
    MSTATE_UNKNOWN,
    MSTATE_MAX_STATE,
    MSTATE_BAD_STATE = 0xFF
} OpState;

/**
 * @enum  OpStateChangeCause
 * @brief Cause of Operational State Change
**/
typedef enum 
{
    CAUSE_NORMAL           = 0,
    CAUSE_CMD_SHELF_MGR    = 1,
    CAUSE_OPERATOR_EJECTOR = 2,
    CAUSE_PROG_ACTION      = 3,
    CAUSE_COMM_LOST        = 4,
    CAUSE_LOCAL_FAILURE    = 5,
    CAUSE_SURPRISE_EXTRXN  = 6,
    CAUSE_PROVIDED         = 7,
    CAUSE_INVALID_HW_ADDR  = 8,
    CAUSE_UNEXPECTED_DEACT = 9,
    CAUSE_UNEXPECTED_ACT   = 10,
    CAUSE_UNKNOWN          = 15
} OpStateChangeCause;

/**
 * @struct OpStateFruObj_T
 * @brief  Operational State FRU Object
**/
typedef struct
{
    INT8U                FruId;             /* FRU Id for this FRU           */
    OpState              CurrentState;      /* Current M State               */
    OpState              PreviousState;     /* Last M State                  */
    INT32U               CurrentConditions; /* FRU type specific conditions  */
    INT32U               NewConditions;     /* Condition change notification */
    OpStateChangeCause   ChangeCause;       /* Last M state change cause     */
    INT8U                CurrentPowerLevel; /* Current granted power level   */
} PACKED OpStateFruObj_T;

#pragma pack ()

#endif //_H_SSIDEFS_

