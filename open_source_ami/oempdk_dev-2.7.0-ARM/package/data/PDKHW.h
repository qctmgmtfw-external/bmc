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
 *****************************************************************
 ******************************************************************
 *
 * PDKHW.h
 * Platform specfic hardware specfic configuration.
 *
 *  Author: Gowtham Shanmukham <gowthams@ami.com>
 ******************************************************************/
#ifndef _PDKHW_H_
#define _PDKHW_H_
#include "Types.h"
#include "IPMI_ChassisDevice.h"
/*---------------------------------------------------------------------------
 * @fn PDK_PlatformInit
 *
 * @brief This function should be used to perform any platform specific
 * initialization. This function is called before any other initialization
 * (SEL, SDR, and Platform Configuration etc.) is done. OS Queues are not
 * available and cannot be used.
 * @return	0  if success
 *			-1 if error
 *---------------------------------------------------------------------------*/
extern int PDK_PlatformInit (void);

/*-----------------------------------------------------------------
 * @fn PDK_PlatformInitDone
 * @brief Do any platform specific initialisation here. This function
 * is called after other initialization (SDR, SEL, etc.) is done.
 * OS Queues are available here.
 * @return  0  if success
 *          -1 if error
 * CHECK_POINT: 01
 *-----------------------------------------------------------------*/
extern int PDK_PlatformInitDone (void);

/*---------------------------------------------------------------------------
 * @fn PDK_PowerOnChassis
 *
 * @brief This function is invoked to power on the chassis. The default
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *
 *---------------------------------------------------------------------------*/
extern int PDK_PowerOnChassis (int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_PowerOffChassis
 *
 * @brief This function is invoked to power off the chassis. The default
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *---------------------------------------------------------------------------*/
extern int PDK_PowerOffChassis (int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_ResetChassis
 *
 * @brief This function is invoked to power reset the chassis. The default
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *---------------------------------------------------------------------------*/
extern int PDK_ResetChassis (int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_PowerCycleChassis
 *
 * @brief This function is invoked to power cycle the chassis. The default
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *---------------------------------------------------------------------------*/
extern int PDK_PowerCycleChassis (int BMCInst);

/*--------------------------------------------------------------------
 * @fn PDK_DiagInterruptChassis
 * @brief Raises the diag interrupt of the chassis.
 *--------------------------------------------------------------------*/
extern int PDK_DiagInterruptChassis (int BMCInst);

/*--------------------------------------------------------------------
 * @fn PDK_SMIInterruptChassis
 * @brief Raises the SMI interrupt of the chassis.
 *--------------------------------------------------------------------*/
int PDK_SMIInterruptChassis (int BMCInst);

/*----------------------------------------------------------------------------
 * @fn PDK_ChassisDiagInt
 *
 * @brief This function is invoked to create the chassis diag interrupt
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *----------------------------------------------------------------------------*/
extern void PDK_ChassisDiagInt (void);

/*-----------------------------------
* @fn PDK_ChassisSMI
 *
 * @brief This function is invoked to create the SMI
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *-----------------------------------*/
extern void PDK_ChassisSMI (void);


/*---------------------------------------------------------------------------
* @fn PDK_IsACPowerOn
*
* @brief This function returns the AC power on status. That is, it returns
*        true if the power-on is due to the AC-on.
*---------------------------------------------------------------------------*/
extern bool PDK_IsACPowerOn(int BMCInst);
/*-----------------------------------
* @fn PDK_SoftOffChassis
 *
 * @brief This function is invoked to create the Soft Off (Soft ShutDown)
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *-----------------------------------*/
extern void PDK_SoftOffChassis (int BMCInst);


/*---------------------------------------------------------------------
 * @fn PDK_GetSystemStatus
 *
 * @brief This control function is called to get the various system status
 *
 * @param   pStatus	- Pointer to get the System status
 *					PS_GOOD_CHANGED
 *					POWER_BUTTON_ACTIVE
 *					RESET_BUTTON_ACTIVE
 *					S5_SIGNAL_ACTIVE
 *					S1_SIGANL_ACTIVE
 *					PROC_HOT_ACTIVE
 *					THERMAL_TRIP_ACTIVE
 *					FRB3_TIMER_HALT
 *					CHIPSET_ERR2
 *					CHIPSET_ERR1
 *					CHIPSET_ERR0
 *					PCI_RAS_ERR
 *
 * @return  0x00	Sussess.
 *			0xff	Failure.	//CLEANUP return parameter
 *-----------------------------------------------------------------------*/
extern INT8U PDK_GetSystemStatus (INT32U* pStatus);

/*---------------------------------------------------------------------
 * @fn PDK_OnSystemEventDetected
 *
 * @brief	This control function is called by the core when it detecte's any
 *			of the below event.
 *
 * @param   State	PS_GOOD_CHANGED
 *					POWER_BUTTON_ACTIVE
 *					RESET_BUTTON_ACTIVE
 *					S5_SIGNAL_ACTIVE
 *					S1_SIGANL_ACTIVE
 *					PROC_HOT_ACTIVE
 *					THERMAL_TRIP_ACTIVE
 *					FRB3_TIMER_HALT
 *					CHIPSET_ERR2
 *					CHIPSET_ERR1
 *					CHIPSET_ERR0
 *					PCI_RAS_ERR
 *
 * @return  0x00	Success
 *			0xff	Failure	//CLEANUP return parameter
 *-----------------------------------------------------------------------*/
extern int PDK_OnSystemEventDetected (INT32U State, int BMCInst);

/*---------------------------------------------------------------------
 * @fn PDK_GetPSGood
 *
 * @brief This control function is called to get the power PS_GOOD status.
 *
 * @param	*pPSGood	- TRUE	PS_GOOD is Good.
 *						- FALSE PS_GOOD is Bad
 *
 * @return	0x00		- Success //CLEANUP return parameter
 *			0xff		- Failed.
 *----------------------------------------------------------------------*/
extern int PDK_GetPSGood (int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_ChassisIdentify
 *
 * @brief This function is invoked in response to chassis identify command.
 * The default implementation turns on / off the chassis identify LED.
 *
 * @param Timeout 	- Chassis Identify timeout
 *---------------------------------------------------------------------------*/
void PDK_ChassisIdentify (INT8U Force, INT8U Timeout, int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_SwitchEMPMUX
 *
 * @brief This function is invoked to switch the EMP MUX. The default
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *
 * @param Direction - MUX_HOST Switch the MUX to host
 *					- MUX_BMC  Switch the MUX to BMC
 *					- MUX_SOL Switch the MUX for Serial Over LAN (SOL)
 *---------------------------------------------------------------------------*/
extern void PDK_SwitchEMPMux (INT8U Direction, int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_AssertSOLCTS
 *
 * @brief This function is invoked to assert SOL serial port CTS signal.
 *
 *---------------------------------------------------------------------------*/
extern void PDK_AssertSOLCTS (int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_DeAssertSOLCTS
 *
 * @brief This function is invoked to deassert SOL serial port CTS signal.
 *
 *---------------------------------------------------------------------------*/
extern void PDK_DeAssertSOLCTS (int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_GlowLED
 *
 * @brief This function should turn on / off the LED. The default
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *
 * @param	LEDNum 	- The LED Number  to control
 *			IsOn  	- 1 to turn on.  0 to turn off.
 *---------------------------------------------------------------------------*/
extern void PDK_GlowLED (INT8U LEDNum, INT8U n, int BMCInst);

/*---------------------------------------------------------------------------
 * @fn PDK_TriggerLED
 *
 * @brief This function should trigger the timers such as 555 timer to blink an LED
 *
 * @param	LEDNum 	- 8 or 9
  *---------------------------------------------------------------------------*/
extern void PDK_TriggerLED (INT8U LEDNum);

/*---------------------------------------------------------------------------
 * @fn PDK_GenerateBeep
 *
 * @brief This function should turn on / off the beep. The default
 * implementation links to the soft processor code (PMCP). You can override
 * with your own C code implementation.
 *
 * @param	IsOn	- 1 to turn on the beep.
					- 0 to turn off the beep.
 *---------------------------------------------------------------------------*/
extern void PDK_GenerateBeep (INT8U IsOn, int BMCInst);



/*---------------------------------------------------------------------
 * @fn PDK_HookIntHandler
 *
 * @brief This control function is called to Hook specific Interrupt
 * handlers
 *
 * @param	IntType		- S5_HANDLER
 *						- S1_HANDLER
 *						- FRB3_TIMER_HALT_HANDLER
 *						- PS_FAILURE_HANDLER
 *			Handler		- The interrupt handler
 *
 * @return	0x00		- Success
 *			0xff		- Failed.
 *----------------------------------------------------------------------*/
//extern INT8U PDK_HookIntHandler (INT8U IntType, pISR_T Handler);


/*---------------------------------------------------------------------
 * @fn PDK_OnIntEnter
 *
 * @brief This control function is called before the interrupt is handled
 *
 * @param	IntType		- S5_HANDLER
 *						- S1_HANDLER
 *						- FRB3_TIMER_HALT_HANDLER
 *						- PS_FAILURE_HANDLER
 *
 * @return	0x00		- Success
 *			0xff		- Failed.
 *----------------------------------------------------------------------*/
extern INT8U PDK_OnIntEnter (INT8U IntType);


/*---------------------------------------------------------------------
 * @fn PDK_OnIntExit
 *
 * @brief This control function is called before the interrupt is handled
 *
 * @param	IntType		- S5_HANDLER
 *						- S1_HANDLER
 *						- FRB3_TIMER_HALT_HANDLER
 *						- PS_FAILURE_HANDLER
 *
 * @return	0x00		- Success
 *			0xff		- Failed.
 *----------------------------------------------------------------------*/
extern INT8U PDK_OnIntExit (INT8U IntType);

/*---------------------------------------------------------------------------
 * @fn PDK_EnableOEMInterrupt
 *
 * @brief This function should enable any OEM interrupt pins specific to the
 * platform. This function is responsible for registering the interrupt with
 * the core firmware and setting up the registers for the proper functioning
 * of the interrupt.
 *---------------------------------------------------------------------------*/
void PDK_EnableOEMInterrupt (void);

/*---------------------------------------------------------------------------
 * @fn PDK_DisableOEMInterrupt
 *
 * @brief This function is responsible for disabling the OEM interrupt.
 * This function should only mask the interrupts and need not un-register
 * the interrupt handler.
 *---------------------------------------------------------------------------*/
void PDK_DisableOEMInterrupt (void);

/*------------------------------------------------------------------
 * @fn PDK_GetSystemState
 * @brief This function should return the current status of the
 * system. This will be polled every 10 milliseconds.
 *------------------------------------------------------------------*/
extern INT32U PDK_GetSystemState (int BMCInst);

/**
 * DebugPortInit
 **/
extern void PDK_DebugPortInit (void);

/**
 * PDK_DbgOut
 **/
extern void PDK_DbgOut (INT8U Ch);

/**
 * PDK_DbgIn
 **/
extern void PDK_DbgIn (INT8U* Ch);

/**
 * PDK_GetADCChannelMask
 **/
extern int PDK_GetADCChannelMask(_FAR_ INT8U* Mask);

/**
 * PDK_ReadFANTachs
 **/
extern int PDK_ReadFANTachs(_FAR_ INT8U* TachReading);

/**
 * PDK_FPEnable
 **/
extern int PDK_FPEnable (INT8U FPEnable, int BMCInst);

/*--------------------------------------------------------------------------
 * @fn	PDK_GetPowerOnState
 * @brief Indicates if power is on or off.
 * @return		TRUE  -	if Power is on.
 *			FALSE - if Power is off..
 *--------------------------------------------------------------------------*/

extern BOOL PDK_GetPowerOnState (void);


#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
/*--------------------------------------------------------------------------
 * @fn    PDK_ReadSlotID
 * @brief Read the slot ID of blade.
 *						   
 * @return	0  	Success.
 *         -1   Failed.
 *--------------------------------------------------------------------------*/
extern INT8U PDK_ReadSlotID(void);

/*--------------------------------------------------------------------------
 * @fn    PDK_GetSlotID
 * @brief Return the slot ID of blade.
 *						   
 * @return	0  	Success.
 *         -1   Failed.
 *--------------------------------------------------------------------------*/
extern INT8U PDK_GetSlotID(int BMCInst);

/*--------------------------------------------------------------------------
 * @fn    PDK_GetPhysicalSlotNum
 * @brief Return the physical slot number.
 *						   
 * @return	0  	Success.
 *         -1   Failed.
 *--------------------------------------------------------------------------*/
extern INT8U PDK_GetPhysicalSlotNum(int BMCInst);

/*--------------------------------------------------------------------------
 * @fn    PDK_GenerateSlotBasedIPMBAddr
 * @brief Generate the IPMB address to blade based on slot ID.
 *						   
 * @return	0  	Success.
 *         -1   Failed.
 *--------------------------------------------------------------------------*/
extern INT8U PDK_GenerateSlotBasedIPMBAddr(int BMCInst);

/*--------------------------------------------------------------------------
 * @fn    PDK_GetSlotBasedIPMBAddr
 * @brief Return the IPMB address to blade based on slot ID.
 *						   
 * @return	0  	Success.
 *         -1   Failed.
 *--------------------------------------------------------------------------*/
extern INT8U PDK_GetSlotBasedIPMBAddr(int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_IsPowerOnReset
 * @brief This is provided to implement the platform specific
 *        Is Power On Reset detection for Operational State Machine.
 *
 * @return TRUE  - Success.
 *         FALSE - Failed.
 *-----------------------------------------------------------------*/
extern BOOL PDK_IsPowerOnReset(int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_SetPowerLevel
 * @brief This is provided to implement the platform specific
 *        Set Power Level procedure for Operational State Machine.
 *
 * @param pFruObj  - Pointer of Operational State FRU object. 
 * @param NewLevel - New Power Level value being granted.
 *
 * @return ST_OK            - Success.
 *         ST_INVALID_PARAM - Invalid parameter.
 *         ST_ERROR         - Error.
 *-----------------------------------------------------------------*/
extern STATUS PDK_SetPowerLevel(OpStateFruObj_T *pFruObj, INT8U NewLevel, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_RenegotiationPower
 * @brief This is provided to implement the platform specific
 *        Renegotiation Power procedure for Operational State Machine.
 *
 * @param pFruObj  - Pointer of Operational State FRU object. 
 *
 * @return ST_OK            - Success.
 *         ST_ERROR         - Error.
 *-----------------------------------------------------------------*/
extern STATUS PDK_RenegotiationPower(OpStateFruObj_T *pFruObj, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_PowerWakeUp
 * @brief This is provided to implement the platform specific
 *        Power Wake Up procedure for Operational State Machine.
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern void PDK_PowerWakeUp(int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_UpdatePowerLEDStatus
 * @brief This is provided to implement the platform specific
 *        update Power LED status procedure according to Operational
 *        State Machine.
 *
 * @param pFruObj  - Pointer of Operational State FRU object. 
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern void PDK_UpdatePowerLEDStatus (OpStateFruObj_T *pFruObj, int BMCInst);

/*-----------------------------------------------------------------
 * @fn PDK_UpdateFaultLEDStatus
 * @brief This is provided to implement the platform specific
 *        update Fault LED status procedure according to Aggregated
 *        Fault Sensor.
 *
 * @param FaultState - State according to Aggregated Fault Sensor.
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern void PDK_UpdateFaultLEDStatus (INT8U FaultState, int BMCInst);
#endif


// These functions are form Linux G3 code check if needed.
extern int  PDK_ColdResetBMC            (int BMCInst);
extern int  PDK_WarmResetBMC            (int BMCInst);
extern int  PDK_PowerGoodChassis        (void);
extern int  PDK_AssertSystemReset       (void);
extern int  PDK_DeAssertSystemReset     (void);
extern int  PDK_GetBSP                  (void);
extern int  PDK_EnableProcessor         (int ProcNum);
extern int  PDK_DisableProcessor        (int ProcNum);
extern int  PDK_EnableVRM               (void);
extern int  PDK_DisableVRM              (void);
extern void PDK_GlowFaultLED            (int Fault, int State, INT8U Data, int BMCInst);
extern int  PDK_SetFanSpeed				(INT8U FanSpeed);
extern void PDK_FanControl              (int BMCInst);
extern int  PDK_TransferFRUData			(INT8U Type, INT8U Number, INT8U* Data, INT8U DataLen);
extern void PDK_ClearCMOS               (int BMCInst);
extern int  PDK_EnableHWIntPin          (INT8U Pin);
extern int  PDK_DisableHWIntPin         (INT8U Pin);
extern int  PDK_PlatformSetupLED        (int BMCInst);

extern int PDK_HandleChassisInterrupts(int fdesc,int BMCInst);
extern int PDK_RegisterChassisInterrupts();
extern int PDK_HandleInterruptSensors(int fdesc, int BMCInst);
extern int PDK_SetPowerLimit (INT8U ExceptionAction, INT16U PwrLimitInWatts, INT32U CorrectionTimeLimitInMsec, int BMCInst);
extern int PDK_ActivatePowerLimit (INT8U Activate, int BMCInst);
extern int PDK_ReadServerPowerUsage  (INT16U* pCurPwrInWatts, int BMCInst);

#endif	/* _PDKHW_H_*/


