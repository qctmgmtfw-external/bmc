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
 * PDKDefs.h
 * PDK definitions used in core and in PDK modules.
 *
 *  Author: Gowtham Shanmukham <gowthams@ami.com>
 ******************************************************************/
#ifndef _PDKDEFS_H_
#define _PDKDEFS_H_
#include "Types.h"


/*---------------------------------------------------------------------------
 * CHECK point codes
 *---------------------------------------------------------------------------*/
#define		CKP_BBLK_CHIP_INITILIZED				0x02
#define		CKP_BBLK_SRAM_ENABLED					0x03
#define		CKP_BBLK_SRAM_TEST_COMPLETED			0x04
#define		CKP_BBLK_EXITED							0x05
#define		CKP_BBLK_ENTERED_FW_UPGRADE				0x06


/*----------------------------------------------------------------
 * Chassis Control Actions
 *---------------------------------------------------------------*/
#define 	CHASSIS_POWER_OFF						0x00
#define 	CHASSIS_POWER_ON						0x01
#define 	CHASSIS_POWER_CYCLE						0x02
#define 	CHASSIS_POWER_RESET						0x03
#define 	CHASSIS_DIAG_INT						0x04
#define 	CHASSIS_SOFT_OFF						0x05
#define 	CHASSIS_SMI								0x06
#define 	CHASSIS_IDENTIFY						0x07

/*----------------------------------------------------------------
 * System Status
 *---------------------------------------------------------------*/

/*----------------------------------------------------------------
 * BMC Status
 *---------------------------------------------------------------*/
#define		BMC_POWER_ON							0x01
#define		BMC_COLD_RESET							0x02
#define		BMC_WARM_RESET							0x03
#define		BMC_SELF_TEST							0x04
#define		BMC_INIT_AGENT							0x05
#define		BMC_START_UP							0x06

/*----------------------------------------------------------------
 * Interrupt Hook Handler Type
 *---------------------------------------------------------------*/
#define		AC_LOST_HANDLER							0x00
#define		PS_HANDLER								0x01
#define		PWR_BTN_HANDLER							0x02
#define		RESET_BTN_HANDLER						0x03
#define		S5_HANDLER								0x10
#define		S1_HANDLER								0x15
#define		PROC_HOT_HANDLER						0x20
#define		THERMAL_TRIP_HANDLER					0x21
#define		FRB3_TIMER_HANDLER						0x22
#define		RES_REQ_HANDLER							0x23

/*----------------------------------------------------------------
 * ACPI Power State
 *---------------------------------------------------------------*/
#define		ACPI_S0									0x00
#define		ACPI_S1									0x01
#define		ACPI_S2									0x02
#define		ACPI_S3									0x03
#define		ACPI_S4									0x04
#define		ACPI_S5									0x05

/*---------------------------------------------------------------
 * OEM Configuration Area Tuple
 *--------------------------------------------------------------*/
#define		CHANNEL_INFO_TUPLE						0
#define		USER_INFO_TUPLE							1
#define		PEF_CONFIG_TUPLE						2
#define		CHASSIS_CONFIG_TUPLE					3
#define		LAN_CONFIG_TUPLE						4
#define		SM_CONFIG_TUPLE							5
#define		OEM_CONFIG_DATA_TUPLE					6

/*----------------------------------------------------------------
 * OEM Soft Processor Commands
 *---------------------------------------------------------------*/
#define		OEMSP_MONITOR_ON_STANDBY				2
#define		OEMSP_IMMEDIATE_POWER_SHUTDOWN			3
#define		OEMSP_GRACEFUL_SHUTDOWN                 4
#define		OEMSP_RWI2C								5
/*----------------------------------------------------------------
 * Serial Mux State
 *---------------------------------------------------------------*/
#define		MUX_2_SYS								0
#define		MUX_2_BMC								1
#define     MUX_2_SOL                               2
#define		MUX_2_RAC								3

/*----------------------------------------------------------------
 * SEL Limit Exceeded Actions
 *---------------------------------------------------------------*/
#define		SET_MSG_FLAG_OEM_0						0x20
#define		SET_MSG_FLAG_OEM_1						0x40
#define		SET_MSG_FLAG_OEM_2						0x80

/*---------------------------------------------------------------------*
 * ISR Handler type.
 *---------------------------------------------------------------------*/

#endif	/* _PDKDEFS_H_*/


