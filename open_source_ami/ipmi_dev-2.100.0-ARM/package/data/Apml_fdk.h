/*
 * Master include file for the APML FDK - V1.0
 *
 ******************************************************************************


============================================================
License Agreement

Copyright (c) 2008, 2009 Advanced Micro Devices, Inc.

All rights reserved.

Redistribution and use in any form of this material and any product thereof
including software in source or binary forms, along with any related
documentation, with or without modification ("this material"), is permitted
provided that the following conditions are met:

+ Redistributions of source code of any software must retain the above
copyright notice and all terms of this license as part of the code.

+ Redistributions in binary form of any software must reproduce the above
copyright notice and all terms of this license in any related documentation
and/or other materials.

+ Neither the names nor trademarks of Advanced Micro Devices, Inc. or any
copyright holders or contributors may be used to endorse or promote products
derived from this material without specific prior written permission.

+ Notice about U.S. Government restricted rights: This material is provided
with RESTRICTED RIGHTS. Use, duplication or disclosure by the U.S. Government
is subject to the full extent of restrictions set forth in FAR52.227 and
DFARS252.227 et seq., or any successor or applicable regulations. Use of this
material by the U.S. Government constitutes acknowledgment of the proprietary
rights of Advanced Micro Devices, Inc. and any copyright holders and
contributors.

+ In no event shall anyone redistributing or accessing or using this material
commence or participate in any arbitration or legal action relating to this
material against Advanced Micro Devices, Inc. or any copyright holders or
contributors. The foregoing shall survive any expiration or termination of this
license or any agreement or access or use related to this material.

+ ANY BREACH OF ANY TERM OF THIS LICENSE SHALL RESULT IN THE IMMEDIATE
REVOCATION OF ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE THIS MATERIAL.

THIS MATERIAL IS PROVIDED BY ADVANCED MICRO DEVICES, INC. AND ANY COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" IN ITS CURRENT CONDITION AND WITHOUT ANY
REPRESENTATIONS, GUARANTEE, OR WARRANTY OF ANY KIND OR IN ANY WAY RELATED TO
SUPPORT, INDEMNITY, ERROR FREE OR UNINTERRUPTED OPERATION, OR THAT IT IS FREE
FROM DEFECTS OR VIRUSES.  ALL OBLIGATIONS ARE HEREBY DISCLAIMED - WHETHER
EXPRESS, IMPLIED, OR STATUTORY - INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED
WARRANTIES OF TITLE, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
ACCURACY, COMPLETENESS, OPERABILITY, QUALITY OF SERVICE, OR NON-INFRINGEMENT.
IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, PUNITIVE,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, REVENUE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED OR BASED ON ANY THEORY OF LIABILITY
ARISING IN ANY WAY RELATED TO THIS MATERIAL, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE. THE ENTIRE AND AGGREGATE LIABILITY OF ADVANCED MICRO DEVICES,
INC. AND ANY COPYRIGHT HOLDERS AND CONTRIBUTORS SHALL NOT EXCEED TEN DOLLARS
(US $10.00). ANYONE REDISTRIBUTING OR ACCESSING OR USING THIS MATERIAL ACCEPTS
THIS ALLOCATION OF RISK AND AGREES TO RELEASE ADVANCED MICRO DEVICES, INC. AND
ANY COPYRIGHT HOLDERS AND CONTRIBUTORS FROM ANY AND ALL LIABILITIES,
OBLIGATIONS, CLAIMS, OR DEMANDS IN EXCESS OF TEN DOLLARS (US $10.00). THE
FOREGOING ARE ESSENTIAL TERMS OF THIS LICENSE AND, IF ANY OF THESE TERMS ARE
CONSTRUED AS UNENFORCEABLE, FAIL IN ESSENTIAL PURPOSE, OR BECOME VOID OR
DETRIMENTAL TO ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR
CONTRIBUTORS FOR ANY REASON, THEN ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE
THIS MATERIAL SHALL TERMINATE IMMEDIATELY. MOREOVER, THE FOREGOING SHALL
SURVIVE ANY EXPIRATION OR TERMINATION OF THIS LICENSE OR ANY AGREEMENT OR
ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE IS HEREBY PROVIDED, AND BY REDISTRIBUTING OR ACCESSING OR USING THIS
MATERIAL SUCH NOTICE IS ACKNOWLEDGED, THAT THIS MATERIAL MAY BE SUBJECT TO
RESTRICTIONS UNDER THE LAWS AND REGULATIONS OF THE UNITED STATES OR OTHER
COUNTRIES, WHICH INCLUDE BUT ARE NOT LIMITED TO, U.S. EXPORT CONTROL LAWS SUCH
AS THE EXPORT ADMINISTRATION REGULATIONS AND NATIONAL SECURITY CONTROLS AS
DEFINED THEREUNDER, AS WELL AS STATE DEPARTMENT CONTROLS UNDER THE U.S.
MUNITIONS LIST. THIS MATERIAL MAY NOT BE USED, RELEASED, TRANSFERRED, IMPORTED,
EXPORTED AND/OR RE-EXPORTED IN ANY MANNER PROHIBITED UNDER ANY APPLICABLE LAWS,
INCLUDING U.S. EXPORT CONTROL LAWS REGARDING SPECIFICALLY DESIGNATED PERSONS,
COUNTRIES AND NATIONALS OF COUNTRIES SUBJECT TO NATIONAL SECURITY CONTROLS.
MOREOVER, THE FOREGOING SHALL SURVIVE ANY EXPIRATION OR TERMINATION OF ANY
LICENSE OR AGREEMENT OR ACCESS OR USE RELATED TO THIS MATERIAL.

This license forms the entire agreement regarding the subject matter hereof and
supersedes all proposals and prior discussions and writings between the parties
with respect thereto. This license does not affect any ownership, rights,
title, or interest in, or relating to, this material. No terms of this license
can be modified or waived, and no breach of this license can be excused, unless
done so in a writing signed by all affected parties. Each term of this license
is separately enforceable. If any term of this license is determined to be or
becomes unenforceable or illegal, such term shall be reformed to the minimum
extent necessary in order for this license to remain in effect in accordance
with its terms as modified by such reformation. This license shall be governed
by and construed in accordance with the laws of the State of Texas without
regard to rules on conflicts of law of any state or jurisdiction or the United
Nations Convention on the International Sale of Goods. All disputes arising out
of this license shall be subject to the jurisdiction of the federal and state
courts in Austin, Texas, and all defenses are hereby waived concerning personal
jurisdiction and venue of these courts.
============================================================


 ****************************************************************************** 
 */


#ifndef APML_FDK_H
#define APML_FDK_H 1

#include "coreTypes.h"


/* **************************************************************************/
/*                                                                          */
/*                           Configurable Settings                          */
/*                                                                          */
/* **************************************************************************/

#define APML_FDK_MAJOR		1
#define APML_FDK_MINOR		0
#define APML_TRACE_LEVEL_DEF	APML_TRACE_OFF
#define APML_DEBUG_LEVEL_DEF	APML_DEBUG_OFF
#define USE_TSI_ADDR	0x10



/* **************************************************************************/
/*                                                                          */
/*             Static Defines Used By User Platform and APML FDK            */
/*                                                                          */
/* **************************************************************************/

/* Static maximum processors APML supports natively */
#define APML_MAX_PROCS	2 // Natively it can support upto 8 Proc

/* Option value to user_get_proc_addr() in user_platform */
#define PROC_USE_RMI	0x00	/* Return RMI adddress */
#define PROC_USE_TSI	0x01	/* Return TSI adddress rather than RMI */

/* **************************************************************************/
/*                                                                          */
/*                   Mandatory Platform Specific Definitions                */
/*                                                                          */
/* **************************************************************************/


/* Basic variable typedefs used by the FDK */
/*typedef char			int8;
typedef unsigned char		uint8;
typedef long int		int32;
typedef unsigned long int	uint32;
*/

/* user_platform_data is mandatory for the FDK but the contents are 
 * completely user defined. */
typedef struct user_platform_data {
	uint8	addr[APML_MAX_PROCS];
	uint8	last_addr_val;
	int32	apml_smbus_number;
	;
} USER_PLATFORM_DATA;

#define APML_SB_RMI_REVISION_1_0        0x02 
#define APML_SB_RMI_REVISION_1_1        0x03 


/* Platform specific error codes 
 * These errors should be from 300-399 for maximal compatiblity with other
 * projects. */
#define USER_IO_ERROR		300
#define USER_REMOTE_IO_ERROR	301
#define USER_GEN_ERROR		302
#define USER_UNK_ERROR		303
#define USER_BUS_OPEN_FAIL	304


/* Maps platform specific error codes to a description */
typedef struct user_err_desc {
	uint32		code;
	const CHAR	*desc;
} USER_ERR_DESC;



/* **************************************************************************/
/*                                                                          */
/*                             Static Defines                               */
/*                                                                          */
/* **************************************************************************/

/* Boolean values */
#define APML_TRUE	0x01
#define APML_FALSE	0x00


/* Initialization and configuration */
#define APML_MAX_CORES		32 
#define APML_TRACE_OFF		APML_FALSE
#define APML_TRACE_ON		APML_TRUE
#define APML_DEBUG_OFF		APML_FALSE
#define APML_DEBUG_ON		APML_TRUE
#define SAR_H_DEFAULT		0xFFFFFFFF
#define SAR_L_DEFAULT		0xFFFFFFFF
#define APML_CMD_RETRIES	20
#define MIN_ADDR_BYTES_DEFAULT	8
#define LOAD_ADDRESS_COMMAND	0x81
#define DEFAULT_RMI_CORE	0x00
#define CORE_BRCST		0xFF


/* Status codes */
#define APML_SUCCESS		0
#define APML_FAILURE		-1
#define APML_BAD_PROC_NDX	101
#define APML_UNK_PROC_NUM	102
#define APML_PEC_FAIL		103
#define APML_RCV_LEN_MATCH	104
#define APML_CMD_TMO		106
#define APML_RSP_TMO		107
#define APML_SBI_BUSY		108
#define APML_HDT_BUSY		109
#define APML_CORE_NA		110
#define APML_UNK_CMD		111
#define APML_INV_RD_LEN		112
#define APML_XS_DATA_LEN	113
#define APML_UNK_CORE		114
#define APML_CMD_NA		115
#define APML_UNK_RESPONSE	116
#define APML_NO_RESPONSE	117
#define APML_BAD_RMI_VERSION	118
#define APML_ILL_BCAST		119
#define APML_ABORT_TMO		120
#define APML_NOT_SUPPORTED	199


/* **************************************************************************/
/*                                                                          */
/*                              Static Structures                           */
/*                                                                          */
/* **************************************************************************/


/* Read/write sizes */
typedef enum {
	APML_BYTE	= 1,
	APML_WORD	= 2,
	APML_DWORD	= 4,
	APML_QWORD	= 8
} Apml_Msize;


/* Holds SMBus statistics */
typedef struct apml_proc_stat {
	uint32	bytes_xmtd;
	uint32	bytes_rcvd;
	uint32	rcv_pec_errors;
	uint32	cmd_cnt;
} APML_PROC_STATS;


/* Main APML state and configuration structure */
typedef struct apml_ctl_struct {
	uint8			maj_rev;
	uint8			min_rev;
	uint8			rmi_rev[APML_MAX_PROCS];
	uint8			rmi_core_target;
	uint8			cores[APML_MAX_PROCS];
	uint32			last_sar_h[APML_MAX_PROCS][APML_MAX_CORES];
	uint32			last_sar_l[APML_MAX_PROCS][APML_MAX_CORES];
	uint8			min_addr_bytes;
	uint8			trace_level;
	uint8			debug_level;
	uint8 			initialized;
	APML_PROC_STATS		stat;
	USER_PLATFORM_DATA	platform;
} APML_DEV_CTL;


/* SMBus representation of APML commands stored here */
typedef struct apml_smbus_cmd {
	uint32		xlen;
	uint32		rlen;
	uint8		xd[32];
	uint8		rd[32];
	uint8		rpec;
	const CHAR	*id;
} APML_SMBUS_CMD;


/* Maps APML status to FDK status codes */
typedef struct apml_errs {
	uint8	icode;
	uint32	acode;
} APML_MAP;


/* Maps FDK status codes to text descriptions */
typedef struct apml_err_desc {
	uint32		code;
	const CHAR	*desc;
} APML_ERR_DESC;




/* **************************************************************************/
/*                                                                          */
/*                      Public APML Function Definitions                    */
/*                                                                          */
/* **************************************************************************/


extern uint32 apml_read_cpuid(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		core,
	uint32		cpuid_func,
	uint32		*eax_val,
	uint32		*ebx_val,
	uint32		*ecx_val,
	uint32		*edx_val,
	int			BMCInst);

extern uint32 apml_interface_version(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		*version,
    int         BMCInst);

extern uint32 apml_read_htc_reg(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint32		*reg_val_h,
	uint32		*reg_val_l,
	int			BMCInst);

extern uint32 apml_write_htc_reg(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		reg_val_size,
	uint32		reg_val_h,
	uint32		reg_val_l,
	int			BMCInst);

extern uint32 apml_read_pstate(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		core,
	uint8		*pstate,
	int			BMCInst);

extern uint32 apml_read_pstate_limit(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		*limit_active,
	uint8		*pstate_limit,
	int			BMCInst);

extern uint32 apml_write_pstate_limit(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		limit_active,
	uint8		pstate_limit,
	int			BMCInst);

extern uint32 apml_read_max_pstate(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		*val,
	int			BMCInst);

extern uint32 apml_read_mcr(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		core,
	uint32		mcr_addr,
	uint32		*reg_val_h,
	uint32		*reg_val_l,
	int			BMCInst);

extern uint32 apml_write_mcr(
	APML_DEV_CTL	*ctl,
	uint8		proc,
	uint8		core,
	uint32		mcr_addr,
	uint32		reg_val_size,
	uint32		reg_val_h,
	uint32		reg_val_l,
	int			BMCInst);

extern uint32 apml_read_rmi_reg(
	APML_DEV_CTL	*dev,
	uint8		proc,
	uint8		option,
	uint8		reg,
	uint8		*data,
	int 			BMCInst);

extern uint32 apml_write_rmi_reg(
	APML_DEV_CTL	*dev,
	uint8		proc,
	uint8		option,
	uint8		reg,
	uint8		data,
	int			BMCInst);


extern uint32 apml_read_tsi_reg(
	APML_DEV_CTL	*dev,
	uint8		proc,
	uint8		reg,
	uint8		*data,
	int			BMCInst);

extern uint32 apml_write_tsi_reg(
	APML_DEV_CTL	*dev,
	uint8		proc,
	uint8		reg,
	uint8		data,
	int			BMCInst);

extern uint32 apml_read_tdp_limit(
        APML_DEV_CTL    *ctl,
        uint8           proc,
        uint8           *apml_tdp_limi_percent,
        uint16          *apml_tdp_limit,
        int			    BMCInst);
extern uint32 apml_write_tdp_limit(
        APML_DEV_CTL    *ctl,
        uint8           proc,
        uint8           apml_tdp_limi_percent,
        uint16          apml_tdp_limit,
        int			    BMCInst);
extern uint32 apml_read_processor_power(
        APML_DEV_CTL    *ctl,
        uint8           proc,
        uint16          *tdp2_watt,
        uint16          *base_tdp,
        uint16          *processor_tdp,
        int			    BMCInst);
extern uint32 apml_read_power_averaging_period(
        APML_DEV_CTL    *ctl,
        uint8           proc,
        uint16          *apml_tdp_limit,
        uint32          *tdp_run_avf_acc_cap,
        uint8           *run_avf_range,
        int			    BMCInst);
extern uint32 apml_read_dram_throttle(
        APML_DEV_CTL    *ctl,
        uint8           proc,
        uint8           *bw_cap_cmd_throttle_mode,
        uint8           *bw_cap_en,
        int			    BMCInst);
extern uint32 apml_write_dram_throttle(
        APML_DEV_CTL    *ctl,
        uint8           proc,
        uint8           bw_cap_cmd_throttle_mode,
        uint8           bw_cap_en,
        int			    BMCInst);

extern uint32 apml_init_fdk(APML_DEV_CTL *ctl,int BMCInst);

extern uint32 apml_close_fdk(APML_DEV_CTL *ctl,int BMCInst);



/* **************************************************************************/
/*                                                                          */
/*                       Private APML Function Definitions                  */
/*                                                                          */
/* **************************************************************************/

void apml_hal_err_log(
	APML_DEV_CTL	*ctl,
	APML_SMBUS_CMD	cmd,
	uint8		proc,
	uint8		core,
	uint32		res);

uint32 apml_check_resp(uint8 resp);

uint32 apml_load_sar(
	APML_DEV_CTL	*ctl,
	uint8		command,
	uint8		proc_addr,
	uint8		proc_ndx,
	uint8		core_ndx,
	uint32		addr_h,
	uint32		addr_l,
	int			BMCInst);

uint32 apml_update_sar(
	APML_DEV_CTL	*ctl,
	uint8		proc_ndx,
	uint8		core_ndx,
	uint32		inc);

uint32 apml_exec_block_process_call(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		wr_len,
	uint8		*wr_data,
	uint32		rd_len,
	uint8		*rd_data,
	int			BMCInst);

uint8 apml_pec(
	uint8	init,
	uint32	count,
	uint8	*data);



/* **************************************************************************/
/*                                                                          */
/*            Mandatory Private User Platform Function Definitions          */
/*                                                                          */
/* **************************************************************************/

void apml_print_user_error(uint32 res);

uint32 user_platform_init(APML_DEV_CTL *dev,int BMCInst);

uint32 user_platform_close(APML_DEV_CTL *ctl);

uint32 user_get_proc_addr(
	APML_DEV_CTL	*dev,
	uint8		proc,
	uint8		options,
	uint8		*addr);

uint32 user_smbus_write_byte(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		reg,
	uint32		data,
	int 			BMCInst);

uint32 user_smbus_write_word(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		reg,
	uint32		data,
	int			BMCInst);

uint32 user_smbus_read_byte(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		reg,
	uint32		*data,
	int			BMCInst);

uint32 user_smbus_read_word(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		reg,
	uint32		*data,
	int			BMCInst);

uint32 user_smbus_bwr_brd_process(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint8		*xmt_data,
	uint32		xmt_len,
	uint8		*rcv_data,
	uint32		rcv_len,
	int			BMCInst);


#endif /* !APML_FDK_H */

