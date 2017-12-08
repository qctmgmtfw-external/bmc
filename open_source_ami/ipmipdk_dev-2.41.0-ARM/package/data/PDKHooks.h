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
 * pdkhooks.h
 * Hooks that are invoked at different points of the Firmware
 * execution.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 ******************************************************************/
#ifndef PDKHOOKS_H
#define PDKHOOKS_H
#include "Types.h"
#include "SELRecord.h"
#include "WDT.h"
#include "LANAlert.h"
#include "SensorMonitor.h"
#include "Message.h"
#include "Pnm.h"



/* hold locate button for this long to run Push-To-Test function */
#define LOCATE_PUSH_TO_TEST	5
#define PUSH_TO_TEST_TIME       15

#define MAX_PS_NUM              3
#define MAX_CPU_NUM             2
#define MAX_FAN_NUM             8
#define MAX_HDD_NUM             4
#define MAX_DIMM_NUM            2 /* DIMM pairs */

/* number of ECC errors before fault */
#define ECC_CE_THRESHOLD        24
#define ECC_CE_TIMEOUT          (24 * 60 *60) /* 24 hours */

/* File to store UUID   */
#define DEV_GUID_FILE     "/conf/Dev_GUID.dat"
#define SYS_GUID_FILE     "/conf/Sys_GUID.dat"


#define PLATFORM_CONF_ROOT      "/etc/sensors"
#define PLATFORM_CONF_OVERRIDE  "/conf/override_sensor_config"
#define PLATFORM_FAN_OVERRIDE   "/conf/override_fan_control"
#define PLATFORM_OTEMP_OVERRIDE "/conf/override_temp_shutdown"
/* Last Power On via IPMI commands */
#define PDK_LAST_POWER_ON_VIA_IPMI					0x10
#define PDK_LAST_POWER_DOWN_VIA_POWER_FAULT			0x08
#define PDK_LAST_POWER_DOWN_VIA_POWER_LOCK			0x04
#define PDK_LAST_POWER_DOWN_VIA_POWER_OVERLOAD		0x02
#define PDK_LAST_POWER_DOWN_VIA_AC_FAILURE			0x01

#define PENDING_BRIDGE_REQ    1
#define KCS_FWD_REQ           2
#define INVALID_FWD_CMD       3


/*------------------ Type Definitions ---------------------------*/

typedef struct
{
    INT8U    TempFault;
    INT8U    PSFault  [MAX_PS_NUM  + 1];
    INT8U    FanFault [MAX_FAN_NUM + 1];
    INT8U    CPUFault [MAX_CPU_NUM + 1];
    INT8U    HDDFault [MAX_HDD_NUM + 1];
    INT8U    ECCFault_UE [MAX_CPU_NUM][MAX_DIMM_NUM];
    INT32U   ECCFault_CE [MAX_CPU_NUM][MAX_DIMM_NUM][ECC_CE_THRESHOLD];
    INT8U    NumAlerts;
    INT8U    NumFatal;
} PDK_FaultInfo_T;

typedef struct
{
    INT8U    ChassisID;
    INT8U    BoardRev;
    INT8U    SPBoardRev;
    INT8U    IOBoard;
    INT8U    NMIBtnStatus;
    INT8U    FLctBtnStatus;
    INT8U    BLctBtnStatus;
    PDK_FaultInfo_T FaultInfo;
} PDK_SharedMem_T;

typedef struct {

    INT8U   PowerUpDelay;
    INT8U   PowerOnDelay;
    INT8U   PowerCycleDelay;
    INT8U   ForcePwrDownDelay;

} PowerButton_T;

typedef struct {

    INT16U  IntNo;
    INT8U   Rsvd1;
    INT8U   Rsvd2;	
    INT8U   Rsvd3;

} IntSensorProp_T;

/* PDK Interrupt Information
 * Used for PMCP Interrupt configuration
 *  */
typedef struct {

    int 	int_num;			/* Interrupt number */		
    INT8U	TiggerMethod;		/* Trigger Method */
    INT8U   TriggerType;		/* Trigger Type */

} PDK_IntInfo_T;


typedef struct {

    INT8U	   NMIDelay;		/* Delay to hold NMI low */

} NMI_T;


/*---------------- Serial MUX control ---------------------------*/
#define MUX_2_SYS                   0
#define MUX_2_BMC                   1

/*------------ SEL Limit Exceeded Actions -----------------------*/
#define		SET_MSG_FLAG_OEM_0						0x20
#define		SET_MSG_FLAG_OEM_1						0x40
#define		SET_MSG_FLAG_OEM_2						0x80

/*------------------ System Events ------------------------------*/
#define		PS_PWRGD                        BIT0
#define		FP_PWR_BTN_ACTIVE               BIT1
#define		FP_RST_BTN_ACTIVE               BIT2
#define		FP_NMI_BTN_ACTIVE               BIT3
#define		FP_SLEEP_BTN_ACTIVE             BIT4
#define		S5_SIGNAL_ACTIVE                BIT7
#define		S1_SIGNAL_ACTIVE                BIT8
#define		PROC_HOT_ACTIVE                 BIT9
#define		THERMAL_TRIP_ACTIVE             BIT10
#define		FRB3_TIMER_HALT                 BIT11
#define		CHIPSET_ERR2                    BIT12
#define		CHIPSET_ERR1                    BIT13
#define		CHIPSET_ERR0                    BIT14
#define		PCI_RAS_ERR                     BIT15
#define		PCI_SYS_SERR                    BIT16
#define		WAKE_ON_LAN                     BIT17
#define		SECURE_MODE_KB                  BIT18
#define		CPU0_SKTOCC                     BIT19
#define		CPU1_SKTOCC                     BIT20


/*-----------------------------------------------------------------
 * @fn PDK_BeforeCreatingTasks
 *
 * @brief This function is called is before the tasks are created.
 * You can perform any initialization required for the proper
 * functioning of any of the standard or OEM tasks. You can even
 * create an OEM task here.
 *
 * @return  0
 *-----------------------------------------------------------------*/
extern int PDK_BeforeCreatingTasks (int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_AfterCreatingTasks
 *
 * @brief This function is called just after all the tasks are created.
 *
 * @return  0
 *-----------------------------------------------------------------*/
extern int  PDK_AfterCreatingTasks (int BMCInst);


/*-----------------------------------------------------------------
 * @fn PDK_OnTaskStartup
 *
 * @brief This function is called on the startup of a particular task.
 * Only core tasks will invoke this hook. The id passed as input to
 * this function is the ID of the task that is starting up. This hook
 * is not invoked for any of the OEM tasks. For OEM tasks you need to
 * perform the startup functions in the task itself.
 *
 * @param	Id - Task ID
 *				SMBIFC_TASK_PRIO
 *				LAN_TASK_PRIO
 *				DHCP_TASK_PRIO
 *				SHMEM_MUTEX_PRIO
 *				USB_TASK_PRIO
 *				RVDP_TASK_PRIO
 *				UKVM_TASK_PRIO
 *				SM_TASK_PRIO
 *				SERIAL_R_TASK_PRIO
 *				SOL_TASK_PRIO
 *				MSG_HNDLR_PRIO
 *				TIMER_TASK_PRIO
 *				CHASSIS_TASK_PRIO
 *				IPMB_TASK_PRIO
 *				SMBUS_TASK_PRIO
 *				BT_TASK_PRIO
 *				KCS1_TASK_PRIO
 *				SERIAL_TASK_PRIO
 *				ICMB_TASK_PRIO
 *				NVR_TASK_PRIO
 *				PEF_TASK_PRIO
 *				SENSOR_MONITOR_PRIO
 *				MDL_TASK_PRIO
 *-----------------------------------------------------------------*/
extern void PDK_OnTaskStartup (INT8U Id,int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_PreMonitorSensor
 *
 * @brief This function is invoked before a sensor is monitored. This
 * function can be used in place of the default sensor monitoring code.
 * This function should return 0 if the default sensor monitoring needs
 * to be run, -1 otherwise.
 *
 * @param	SensorNum		- The sensor number that is being monitored.
 * @param	pSensorReading  - Pointer to the buffer to hold the sensor reading.
 * @param	pReadFlags  	- Pointer to the flag holding the read status.
 *
 * @return  0	- if default sensor monitoring needs to run.
 *			-1	- otherwise
 *---------------------------------------------------------------------------*/
extern int PDK_PreMonitorSensor (INT8U SensorNum,	_FAR_ INT16U* pSensorReading,
											_FAR_ INT8U* pReadFlags,int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_PostMonitorSensor
 *
 * @brief This function is invoked immediately after the sensor monitoring
 * code is run. This function can be used to override the values returned,
 * or perform certain actions based on the sensor reading etc. This function
 * should return 0 if this sensor reading needs to be considered for this
 * cycle, -1 otherwise.
 *
 * @param	SensorNum		- The sensor number that is being monitored.
 * @param	pSensorReading  - Pointer to the buffer to hold the sensor reading.
 * @param	pReadFlags  	- Pointer to the flag holding the read status.
 *
 * @return  0	- if sensor reading is to be considered for this cycle.
 *			-1	- otherwise
 *---------------------------------------------------------------------------*/
extern int PDK_PostMonitorSensor ( INT8U SensorNum,	_FAR_ INT16U*  pSensorReading,
													_FAR_ INT8U* pReadFlags,int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_PreMonitorAllSensors
 *
 * @brief This function is invoked before all the sensors are read and before 
 * any event action is taken. This function should return 0 if it needs to 
 * proceed to sensor monitoring cycle otherwise return 0.
 *
 * @return  0	- if need to proceed to sensor monitor cycle.
 *			-1	- otherwise
 *---------------------------------------------------------------------------*/
extern int PDK_PreMonitorAllSensors (bool bInitAgentRearmed,int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_PostMonitorAllSensors
 *
 * @brief This function is invoked immediately after all the sensors are read
 * and before any event action is taken. This function should return 0 if this
 * sensor monitoring cycle needs to generate events, -1 otherwise.
 *
 * @param	NumSensor  	 - Number of sensors in this monitoring cycle.
 * @param	pSensorTable - Table holding the sensor readings.
 *
 * @return  0	- if sensor sensor monitoring cycle needs to generate event.
 *			-1	- otherwise
 *---------------------------------------------------------------------------*/
extern int PDK_PostMonitorAllSensors ( INT8U NumSensor, _FAR_ INT16U* pSensorTable,int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_PreMonitorIndividualSensor
 *
 * @brief This function is invoked immediately after all the sensors are read
 * and before any event action is taken. This function should return 0 if this
 * sensor monitoring cycle needs to generate events, -1 otherwise.
 *
 * @param       pSensorInfo  - Sensor Information.
 *
 * @return  0   - if sensor sensor monitoring cycle needs to generate event.
 *         -1   - otherwise
 *---------------------------------------------------------------------------*/
extern int PDK_PreMonitorIndividualSensor (SensorInfo_T *pSensorInfo, int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_PostSetLanParam
 *
 * @brief This control function is called if there is a change in the Lan 
 * configuration
 *
 * @param[in]   pReq   - Pointer to the request Structure.
 * @param[in]   ReqLen - Length of the request Structure.
 * @param[in]   BMCInst- BMC instance number.
 *
 * @return  0    on Success,
 *          -1   on failure.
 *-----------------------------------------------------------------------*/
extern int PDK_PostSetLanParam (INT8U* pReq, INT8U ReqLen, int BMCInst);
/*---------------------------------------------------------------------
 * @fn PDK_PreAddSEL
 *
 * @brief This control function is called just before the MegaRAC-PM core
 * adds a SEL entry.
 *
 * @param   pSELEntry	- The SEL Record that is being added.
 *
 * @return  0x00	The MegaRAC PM core should not add this record.
 *			0xff	The MegaRAC PM core should add this record.
 *-----------------------------------------------------------------------*/
extern INT8U PDK_PreAddSEL (_FAR_ INT8U*	pSELEntry,int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_PostAddSEL
 *
 * @brief This control function is called just after the MegaRAC-PM core
 * adds a SEL entry.
 *
 * @param   pSELEntry	- The SEL Record that is being added.
 *
 * @return  0x00	The MegaRAC PM core should not add this record.
 *			0xff	The MegaRAC PM core should add this record.
 *-----------------------------------------------------------------------*/
extern INT8U PDK_PostAddSEL (_FAR_ INT8U*	pSELEntry,int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_PreClearSEL
 *
 * @brief This control function is called just before the MegaRAC-PM core
 * clears the SEL.
 *
 *
 * @return  0x00	The MegaRAC PM core should not clear the SEL.
 *			0xff	The MegaRAC PM core should clear the SEL.
 *-----------------------------------------------------------------------*/
extern INT8U PDK_PreClearSEL ( int BMCInst );

/*---------------------------------------------------------------------
 * @fn PDK_GetSELLimit
 *
 * @brief This control function is called to get the SEL Storage percentage.
 *
 * @return  0x00		The MegaRAC PM core should not handle SEL Storage limit.
 *			SELLimit	The SEL storage Limit in Perecentage.
 *-----------------------------------------------------------------------*/
extern INT8U PDK_GetSELLimit ( int BMCInst );

/*---------------------------------------------------------------------
 * @fn PDK_SELLimitExceeded
 *
 * @brief This control function is called when SEL count exceded the SEL Limit
 *		  	specifed by PDK_GetSELLimit control function.
 *
 * @return  0x00	Success.
 *			0xff	Failed.
 *-----------------------------------------------------------------------*/
extern INT8U PDK_SELLimitExceeded ( int BMCInst );

/*-------------------------------------------------------------------------
 * @fn PDK_PEFOEMAction
 * @brief This function is called by the Platform Event Filter to perform
 * OEM Filter actions. Perform any additional filters that are OEM specific
 * and any actions for these filters.
 * @param pEvtRecord - Event record that triggered the PEF.
 * @return 	0	- Success
 *			1 	- Failed.
 *------------------------------------------------------------------------*/
extern int PDK_PEFOEMAction (SELEventRecord_T*   pEvtRecord, int BMCInst);

/*-----------------------------------------------------------------------
 * @fn PDK_TimerTask
 *
 * @brief   This function is invoked by message handler every one second.
 *
 *-----------------------------------------------------------------------*/
extern void PDK_TimerTask (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforeBMCColdReset
 * @brief This function is called by the core cold reset function
 * @return  0x00	The MegaRAC PM core will cold reset BMC
 *			-1   	The MegaRAC PM core will not cold reset BMC
 *------------------------------------------------------------------------*/
extern int PDK_BeforeBMCColdReset (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforeBMCWarmReset
 * @brief This function is called by the core warm reset function
 * @return  0x00	The MegaRAC PM core will warm reset BMC
 *			-1   	The MegaRAC PM core will not warm reset BMC
 *------------------------------------------------------------------------*/
extern int PDK_BeforeBMCWarmReset (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforeBMCWarmReset
 * @brief This function is called by the core cold reset function
 * @return  0x00	The MegaRAC PM core will run init agent
 *			-1   	The MegaRAC PM core will not run init agent
 *------------------------------------------------------------------------*/
extern int PDK_BeforeInitAgent (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_AfterInitAgent
 * @brief This function is called by the core
 * @param   Completion code of the last SDR operation
 * @return  0x00	Init agent is run successfully
 *			-1   	Init agent did not run successfully
 *------------------------------------------------------------------------*/
extern int PDK_AfterInitAgent (INT8U StatusCode,int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforePowerOnChassis
 * @brief This function is called by the core before performing power on chassis
 * @return  0x00	The MegaRAC PM core will perform power on chassis
 *			-1   	The MegaRAC PM core will not perform power on chassis
 *------------------------------------------------------------------------*/
extern int PDK_BeforePowerOnChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforePowerOnChassis
 * @brief This function is called by the core before performing power on chassis
 *------------------------------------------------------------------------*/
extern void PDK_AfterPowerOnChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforePowerOffChassis
 * @brief This function is called by the core before performing power on chassis
 * @return  0x00	The MegaRAC PM core will perform power off chassis
 *			-1   	The MegaRAC PM core will not perform power on chassis
 *------------------------------------------------------------------------*/
extern int PDK_BeforePowerOffChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_AfterPowerOffChassis
 * @brief This function is called by the core after performing power off chassis
 *------------------------------------------------------------------------*/
extern void PDK_AfterPowerOffChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforeResetChassis
 * @brief This function is called by the core before performing power on chassis
 * @return  0x00	The MegaRAC PM core will perform reset the chassis
 *			-1   	The MegaRAC PM core will not perform reset on the chassis
 *------------------------------------------------------------------------*/
extern int PDK_BeforeResetChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_AfterResetChassis
 * @brief This function is called by the core after performing reset on chassis
 *------------------------------------------------------------------------*/
extern void PDK_AfterResetChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforePowerCycleChassis
 * @brief This function is called by the core before performing power on chassis
 * @return  0x00	The MegaRAC PM core will perform power cycle on chassis
 *			-1   	The MegaRAC PM core will not perform power cycle on chassis
 *------------------------------------------------------------------------*/
extern int PDK_BeforePowerCycleChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_AfterPowercycleChassis
 * @brief This function is called by the core after performing power cycle on chassis
 *------------------------------------------------------------------------*/
extern void PDK_AfterPowerCycleChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforeDiagInterrupt
 * @brief This function is called by the core before performing power on chassis
 * @return  0x00	The MegaRAC PM core will rise the diag interrupt
 *			-1   	The MegaRAC PM core will not rise the diag interrupt
 *------------------------------------------------------------------------*/
extern int PDK_BeforeDiagInterrupt (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_AfterDiagInterrupt
 * @brief This function is called by the core after performing rising
 * the diag interrupt
 *------------------------------------------------------------------------*/
extern void PDK_AfterDiagInterrupt (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforeSMIInterrupt
 * @brief This function is called by the core before performing power on chassis
 * @return  0x00	The MegaRAC PM core will rise the SMI
 *			-1   	The MegaRAC PM core will not rise SMI
 *------------------------------------------------------------------------*/
extern int PDK_BeforeSMIInterrupt (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_AfterSMIInterrupt
 * @brief This function is called by the core after performing rising
 * the diag interrupt
 *------------------------------------------------------------------------*/
extern void PDK_AfterSMIInterrupt (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BeforeSoftOffChassis
 * @brief This function is called by the core before performing soft off chassis
 * @return  0x00	The MegaRAC PM core will perform soft shutdown
 *			-1   	The MegaRAC PM core will not perform soft shutdown
 *------------------------------------------------------------------------*/
extern int PDK_BeforeSoftOffChassis (int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_AfterSoftOffChassis
 * @brief This function is called by the core after performing soft shutdown
 *------------------------------------------------------------------------*/
extern void PDK_AfterSoftOffChassis (int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_OnACPIStateChange
 *
 * @brief This control function is called when the ACPI State Changes
 * @param   PreviousState	Previous state of the chassis
 * @param   CurrentState	Current  state of the chassis
 *-----------------------------------------------------------------------*/
extern void PDK_OnACPIStateChange (int CurrentState, int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_OnSMI
 *
 * @brief This control function is called when SMI signal is generated
 * from the system
 *-----------------------------------------------------------------------*/
extern void PDK_OnSMI (void);

/*---------------------------------------------------------------------
 * @fn PDK_OnNMI
 *
 * @brief This control function is called when NMI signal is generated
 * from the system
 *-----------------------------------------------------------------------*/
extern void PDK_OnNMI (void);

/*---------------------------------------------------------------------
 * @fn PDK_OnSerialRxInterrupt
 *
 * @param	Port	- Serial port number (0=SCI0, 1=SCI1)
 * @param	ch		- 8-bit character received
 *
 * @return  0	Process character normally
 *			1	No further processing of this character is required
 *-----------------------------------------------------------------------*/
int PDK_OnSerialRxInterrupt(INT8U Port, INT8U ch);

/*---------------------------------------------------------------------
 * @fn PDK_PreInit
 *
 * @brief This control function is called before any intialization
 * 
 *-----------------------------------------------------------------------*/
void
PDK_PreInit (int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_PostInit
 *
 * @brief This control function is called before any intialization
 * 
 *-----------------------------------------------------------------------*/
void
PDK_PostInit (int BMCInst);

/*-----------------------------------------------------------------------
 * @fn PDK_250MSTimerCB
 *
 * @brief This function is called from Chassis Timer Task.
 * 		  The frequency of this callback is equal to the CHASSIS_TIMER_INTERVAL
 * 		  macro defined for the chassis task . Default is 250 millisec.
 *
 *-----------------------------------------------------------------------*/
void PDK_ChassisTimerCB (int BMCInst);

extern void PDK_GetSelfTestResults(_FAR_ INT8U* SelfTestResult, int BMCInst);
/*---------------------------------------------------------------------
 * @fn PDK_InitSOLPort
 *
 * @brief This fucntion is called for any OEM specific SOL initialization
 *
 * @param ptty_struct - Pointer to terminal attributes structure
 *-----------------------------------------------------------------------*/
void
PDK_InitSOLPort (struct termios *ptty_struct, int BMCInst);



/*----------------------------------------------------------------------------
 * @fn PDK_GetSpecificSensorReading
 *
 * @brief This Hook is invoked in the get Sensor Reading cmd
 * This function can be used when internal Sensor values are to be returned or 
 * sensor values needs to be faked.
 *
 * @param	pReq   - Request Data
 * 		ReqLen - Request length
 * 		pRes- Response Data	
 *
 * @return  0	- if not handled.
 *	   -1	- if handled
 *---------------------------------------------------------------------------*/
int PDK_GetSpecificSensorReading ( INT8U* pReq, INT8U ReqLen, INT8U* pRes, int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_LPCReset
 *
 * @brief This fucntion is called during LPC Reset
 *
 * *-----------------------------------------------------------------------*/
void PDK_LPCReset (int BMCInst);

/*----------------------------------------------------------------------
 * @fn PDK_SetCurCmdChannelInfo
 * @This function updates the PDK library with the current command
 *       channel and privelege
 *----------------------------------------------------------------------*/
extern void PDK_SetCurCmdChannelInfo (INT8U Channel, INT8U Privilege, int BMCInst);


/*-----------------------------------------------------------------
 * @fn PDK_SetSMSAttn
 *
 * @brief This function is used to Set SMS attention bit
 *        .
 *
 * @return None.
 *-----------------------------------------------------------------*/
void PDK_SetSMSAttn (INT8U KCSIfcNum,int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_ClearSMSAttn
 *
 * @brief This function is used to Clear SMS attention bit
 *        .
 *
 *
 * @return None.
 *-----------------------------------------------------------------*/
void PDK_ClearSMSAttn (INT8U KCSIfcNum, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_SetOBF
 *
 * @brief This function is used to Set OBF bit
 *        .
 *
 * @return None.
 *-----------------------------------------------------------------*/
void PDK_SetOBF (INT8U KCSIfcNum,int BMCInst);
/*----------------------------------------------------------------------
 * @fn PDK_GetMfgProdID
 * @brief This function is used to get Manufacture ID and Product ID
 * @param[in] BMCInst - BMC instanve number.
 * @param[out] MfgID - Pointer for Manufacture ID.
 * @param[out] ProdID - Pointer for Product ID.
 * @returns 0.
 *----------------------------------------------------------------------*/
int PDK_GetMfgProdID (INT8U* MfgID, INT16U* ProdID, int BMCInst);

/*----------------------------------------------------------------------
 * @fn PDK_GetPETOEMCustomField
 * @brief This function is used to add OEM custom field in PETs
 * @param CustomField
 * @returns sizeof CustomField
 *----------------------------------------------------------------------*/
INT8U PDK_GetPETOEMCustomField (INT8U *pOEMCustomField);

/*----------------------------------------------------------------------
 * @fn PDK_BeforeWatchdogAction
 * @brief This function is used to do custom actions on watchdog expiry 
 *          before watchdog actions
 * @param TmrAction - as per IPMI spec field
 * @param pWDTTmrMgr - pointer to internal watchdog timer mgmt struct
 * @param BMCInst - Instance number.
 * @returns 0 on success
 *          -1 on failure which makes cancel watchdog action
 *----------------------------------------------------------------------*/
int 
PDK_BeforeWatchdogAction (INT8U TmrAction, WDTTmrMgr_T* pWDTTmrMgr, int BMCInst);

/*----------------------------------------------------------------------
 * @fn PDK_WatchdogAction
 * @brief This function is used to do custom actions on watchdog expiry
 * @param TmrAction - as per IPMI spec field
 * @param pWDTTmrMgr - pointer to internal watchdog timer mgmt struct
 * @returns nothing
 *----------------------------------------------------------------------*/
void PDK_WatchdogAction(INT8U TmrAction,WDTTmrMgr_T* pWDTTmrMgr, int BMCInst);

/*-----------------------------------------------------------------------
 * @fn PDK_InitSystemGUID
 * @brief This function is used to override the OEM specified GUID
 * @param SystemGUID - System GUID to be override
 * @return 0 on overriding the GUID value otherwise 1
 *----------------------------------------------------------------------*/
int PDK_InitSystemGUID(INT8U* SystemGUID);

/*-----------------------------------------------------------------------
 * @fn PDK_InitDeviceGUID
 * @brief This function is used to override the OEM specified GUID
 * @param DeviceGUID - Device GUID to be override
 * @return 0 on overriding the GUID value otherwise 1
 *----------------------------------------------------------------------*/
int PDK_InitDeviceGUID(INT8U * DeviceGUID);

/*-----------------------------------------------------------------
 * @fn PDK_LoadOEMSensorDefault
 *
 * @brief This function is used to Load any OEM Specific
 *        Sensor initialization
 *        .
 *
 * @param  Sensor Information 
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern void PDK_LoadOEMSensorDefault(SensorInfo_T* pSensorInfo, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_SetLastPowerEvent
 *
 * @brief This callback function is set the Last Power Event 
 *        for chassis status command.
 *
 * @param  None
 *
 * @return   None.
 *-----------------------------------------------------------------*/
void PDK_SetLastPowerEvent (INT8U Event, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_GetLastPowerEvent
 *
 * @brief This callback function is get the Last Power Event 
 *        for chassis status command.
 *
 * @param  None
 *
 * @return   None.
 *-----------------------------------------------------------------*/
INT8U PDK_GetLastPowerEvent (int BMCInst);


/*-----------------------------------------------------------------
 * @fn PDK_AppendOEMPETData
 *
 * @brief This hook is used to append any OEM specific 
 *        PET Data.
 *
 * @param  None
 *
 * @return   None.
 *-----------------------------------------------------------------*/
int PDK_AppendOEMPETData (VarBindings_T* pVarBindings, int Length, INT8U *AlertStr, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_GetLanOEMParam
 *
 * @brief This hook is used to append any OEM specific
 *        LAN Configuration parameters.
 *
 * @param  pReq   - Request Data
 *      ReqLen - Request length
 *      pRes- Response Data
 *      BMCInst - BMC Instance number.
 * 
 * @return   Length of data
 *-----------------------------------------------------------------*/
int PDK_GetLanOEMParam ( INT8U* pReq, INT8U ReqLen, INT8U* pRes, int BMCInst );

/*-----------------------------------------------------------------
 *  * @fn PDK_SetLanOEMParam
 *
 * @brief This hook is used to append any OEM specific
 *        LAN Configuration parameters.
 *
 * @param  pReq   - Request Data
 *      ReqLen - Request length
 *      pRes- Response Data
 *      BMCInst - BMC Instance number.
 * 
 * @return   Length of data, error if legth less then one
 *-----------------------------------------------------------------*/
int PDK_SetLanOEMParam ( INT8U* pReq, INT8U ReqLen, INT8U* pRes, int BMCInst );

/*-----------------------------------------------------------------
 * @fn PDK_GetSysInfoOEMParam
 *
 * @brief This hook is used to append any OEM specific
 *        System Information parameters.
 *
 * @param  Num : Parameter selector
 *         pDataAddr : Data address
 * @return   Length of data
 *-----------------------------------------------------------------*/
int PDK_GetSysInfoOEMParam (INT8U Num, unsigned int *pDataAddr, int BMCInst);

/*-----------------------------------------------------------------
 *  * @fn PDK_SetSysInfoOEMParam
 *
 * @brief This hook is used to append any OEM specific
 *        System Information parameters.
 *
 * @param  Num : Parameter selector
 *         pDataAddr : Data address
 *
 * @return   Length of data, error if legth less then one
 *-----------------------------------------------------------------*/
int PDK_SetSysInfoOEMParam (INT8U Num, unsigned int *pDataAddr, int BMCInst);
 void PDK_ProcessOEMRecord(INT8U *OEMRec,int BMCInst);


int PDK_SensorAverage(INT8U SensorNum, INT16U* Reading, INT8U* pReadFlags, int BMCInst);


extern int  PDK_SMCDelay(INT8U SlaveAddr, int BMCInst);


extern bool g_bInitAgentRearmed;
/*-----------------------------------------------------------------
 * @fn PDK_GetEthIndex
 *
 * @brief This function is used to get eth index.
 *
 * @param  Channel
 *
 * @return EthIndex
 *-----------------------------------------------------------------*/
extern INT8U PDK_GetEthIndex (INT8U Channel, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_GetLANChannel
 *
 * @brief This function is used to get lan channel.
 *
 * @param  EthIndex
 *
 * @return Channel
 *-----------------------------------------------------------------*/
extern INT8U PDK_GetLANChannel (INT8U EthIndex, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_PnmOemGetReading
 *
 * @brief This hook is used to get the sensor value of PNM power
 * consumption, inlet air temperature.
 *
 * @param Readingtype - Tartget Sensor type.
 * @param SensorReading - Point to Sensor reading value.
 * @return   None.
 *-----------------------------------------------------------------*/
extern INT8U PDK_PnmOemGetReading (INT8U Readingtype, INT8U DomainID, INT16U *SensorReading, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_PnmOemMePowerStateChange
 *
 * @brief This hook is used to get the ME power state when power
 *        node manager ME power state change event comming.
 *
 * @param PowerState - ME power state.
 *
 * @return   None.
 *-----------------------------------------------------------------*/
extern void PDK_PnmOemMePowerStateChange (INT8U PowerState, int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_POHCounter
 * @brief This function is called everytime the POH counter timer 
 *               is incremented for each minute
 * @param[in/out] MinuteCount - Pointer which holds the minutes count. 
 * @return  0x00	
 *			
 *------------------------------------------------------------------------*/
extern int PDK_POHCounter(INT32U* MinuteCount);

/*-------------------------------------------------------------------------
 * @fn PDK_GetBMCSlaveAddr
 * @brief This function is called to get  the IPMB Slave address  
 * @return  0x00	
 *			
 *------------------------------------------------------------------------*/
extern int PDK_GetBMCSlaveAddr();

/*-------------------------------------------------------------------------
 * @fn PDK_SwitchMux
 * @brief This function is called to Switch Mux settings  
 * @param AccessMode - Access Mode for IPMI messaging
 *------------------------------------------------------------------------*/

extern void PDK_SwitchMux(INT8U AccessMode,int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_GetPrimaryIPMBAddr
 * @brief This function is called to get  the Primary IPMB address  
 * @return  0x00	
 *			
 *------------------------------------------------------------------------*/
extern int PDK_GetPrimaryIPMBAddr(INT8U* Primary);

/*-------------------------------------------------------------------------
 * @fn PDK_GetSecondaryIPMBAddr
 * @brief This function is called to get  the Secondary IPMB address  
 * @return  0x00	
 *			
 *------------------------------------------------------------------------*/
extern int PDK_GetSecondaryIPMBAddr(INT8U* Secondary);

/*-------------------------------------------------------------------------
 * @fn PDK_CheckAcpiPowerState
 * @brief This function is called to validate the system power state  
 * *			
 *------------------------------------------------------------------------*/
extern int PDK_CheckAcpiPowerState(INT8U SysPowerState,INT8U DevPowerState);

/*-------------------------------------------------------------------------
 * @fn PDK_RMCPLoginAudit
 * @brief This function is called by RMCP to log login events
 * @return  0x00	OEM SEL Record Added successfully.
 *			-1   	OEM SEL Record can't been added.
 *------------------------------------------------------------------------*/
extern int PDK_RMCPLoginAudit (INT8U EvtType, INT8U UserId, INT8U *ipaddr, int BMCInst);

/*-------------------------------------------------------------------------
 * @fn PDK_BiosIPSource
 * @brief This function is to override the BIOS ip source actions
 * *			
 *------------------------------------------------------------------------*/
extern int PDK_BiosIPSource(INT8U ChannelNumber );

/*-------------------------------------------------------------------------
 * @fn PDK_BMCOtherSourceIP
 * @brief This function is to override the BMC other source ip actions
 * *			
 *------------------------------------------------------------------------*/
extern int PDK_BMCOtherSourceIP(INT8U ChannelNumber );

extern char *PDK_GetPlatformPath();

/*
 * @fn PDK_GetIfaceCount
 * @brief This hook is used to override the number of interfaces with Kernel.
 *        This hook will be invoked when the CMD_AMI_GET_IFACE_STATE(0x72) 
 *        IPMI command with parameter AMI_GET_IFACE_COUNT(0x03).
 * @param[out] EthIndexList - List of Ethernet Index available.
 * @param[out] IndexCount - Number of interface supported.
 * @param[in] BMCInst - BMC Instance number.
 * @retval 0xFF - Not Supported (Core should continue with Netmon driver)
 *         Otherwise - Success. 
 */
extern INT8U PDK_GetIfaceCount(INT8U *EthIndexList, INT8U *IndexCount, int BMCInst);

/*
 * @fn PDK_OnLanStateChange
 * @brief This hook is used to Perform any OEM specific changes 
 *        when there is change in LAN state.
 * @param[in] PrevLanConfig - Pointer which holds the previous LAN information.
 * @param[in] CurrLanConfig - Pointer which holds the current LAN information.
 * @param[in] BMCInst - BMC Instance number.
 */
extern void PDK_OnLanStateChange(INT8U *PrevLanConfig, INT8U *CurrLanConfig, int BMCInst);

/*
 * @fn PDK_AfterInitIPMISockets
 * @brief This hook will be invoked after initializing the IPMI sockets.
 * @param[in] BMCInst - BMC Instance number.
 */
extern void PDK_AfterInitIPMISockets(int BMCInst);

/*
 * @fn PDK_SetPreserveStatus
 * @brief This hook will be invoked to set preserve status
 * @param[in] Selector - to set status for the particular selector
 * @param[in] Status - status to set  
 * @param[in] BMCInst - BMC Instance number.
 */
extern int PDK_SetPreserveStatus (int Selector, int Status, int BMCInst);

/*
 * @fn PDK_GetPreserveStatus
 * @brief This hook will be invoked to get preserve status
 * @param[in] Selector - to get status for the particular selector   
 * @param[in] BMCInst - BMC Instance number.
 */
extern int PDK_GetPreserveStatus (int Selector, int BMCInst);

/*
 * @fn PDK_IPMBBridgeRequest
 * @brief This hook is used to post ipmi request directly to the IPMB bridge request 
 * @param pMsgPkt       - Pointer to the message packet
 */
extern int PDK_IPMBBridgeRequest ( MsgPkt_T* pMsgPkt, int BMCInst );

/*-----------------------------------------------------------------
 * @fn IsCmdFwdReqd
 *
 * @brief This function is used to check whether the IPMI Cmd is
 *        to be forwarded to ipmb bridge.
 *
 *-----------------------------------------------------------------*/
extern BOOL IsCmdFwdReqd (INT8U NetFn, INT8U Cmd);

#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
/*-----------------------------------------------------------------
 * @fn PDK_PreOpState
 *
 * @brief This control is called by Operation State Machine before
 *        the initialization.
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern void PDK_PreOpState(int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_PostOpState
 *
 * @brief This control is called by Operation State Machine after
 *        all initialization.
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern void PDK_PostOpState(int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_IsInsertionCriteriaMet
 *
 * @brief This is provided to implement the platform specific
 *        insertion criteria for Operational State Machine.
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern BOOL PDK_IsInsertionCriteriaMet(int BMCInst);
#endif

/* @fn PDK_PostClearSEL
 *
 * @brief This hook is used to make SEL.dat file empty.
 * @param[in]   BMCInst - BMC Instance number.
 * @param[out]     : status.
*/
extern INT8U PDK_PostClearSEL(int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_PowerONOFFAudit
 *
 * @brief This is provided to add power on/off audit to SEL
 * 
 *d
 * @return None.
 *-----------------------------------------------------------------*/
extern void PDK_PowerONOFFAudit(INT8U EvtType, int BMCInst);

#endif	/* PDKHOOKS_H */

