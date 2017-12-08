/*******************************************************************
*********************************************************************
*****                                                              **
*****    (C)Copyright 2008-2009, American Megatrends Inc.          **
*****                                                              **
*****    All Rights Reserved.                                      **
*****                                                              **
*****    5555 , Oakbrook Pkwy, Norcross,                           **
*****                                                              **
*****    Georgia - 30093, USA. Phone-(770)-246-8600.               **
*****                                                              **
*********************************************************************
*********************************************************************
*********************************************************************
***
*** Apml.h
*** Apml Command Handler.
***
*** Author:
********************************************************************/
#ifndef _APML_H_
#define _APML_H_

#include "Types.h"
#include "MsgHndlr.h"

/*------------------ APML Command Numbers -------------------------*/
//SB-RMI Commands
#define CMD_APML_GET_INTERFACE_VERSION	0x01
#define CMD_APML_READ_RMI_REG			0x02
#define CMD_APML_WRITE_RMI_REG			0x03
#define CMD_APML_READ_CPUID				0x04
#define CMD_APML_READ_HTC_REG			0x05
#define CMD_APML_WRITE_HTC_REG			0x06
#define CMD_APML_READ_PSTATE			0x07
#define CMD_APML_READ_MAX_PSTATE		0x08
#define CMD_APML_READ_PSTATE_LIMIT		0x09
#define CMD_APML_WRITE_PSTATE_LIMIT		0x0A
#define CMD_APML_READ_MCR				0x0B
#define CMD_APML_WRITE_MCR				0x0C
// SB-TSI Commands
#define CMD_APML_READ_TSI_REG			0x0D
#define CMD_APML_WRITE_TSI_REG			0x0E

#define CMD_APML_READ_TDP_LIMIT_REG             0x0F 
#define CMD_APML_WRITE_TDP_LIMIT_REG            0x10 
#define CMD_APML_READ_PROCESSOR_POWER_REG       0x11 
#define CMD_APML_READ_POWER_AVERAGING_REG       0x12 
#define CMD_APML_READ_DRAM_THROTTLE_REG         0x13 
#define CMD_APML_WRITE_DRAM_THROTTLE_REG        0x14 
 
/*APML Command Handler table */
extern const CmdHndlrMap_T g_APML_CmdHndlr[];


/* ApmlGetInterfaceVersionReq_T */
typedef struct
{
    INT8U   		Proc;
} PACKED ApmlGetInterfaceVersionReq_T;


/* ApmlGetInterfaceVersionResp_T */
typedef struct
{
    INT8U   		CompletionCode;
    INT8U 			Version;
} PACKED ApmlGetInterfaceVersionRes_T;

/* ApmlReadRMIReq_T */
typedef struct
{
    INT8U			Proc;	
    INT8U			Register;
} PACKED ApmlReadRMIReq_T;


/* ApmlReadRMIRes_T */
typedef struct
{
    INT8U			CompletionCode;
    INT8U			Value;	
} PACKED ApmlReadRMIRes_T;

	
/* ApmlWriteRMIReq_T */
typedef struct
{
    INT8U			Proc;	
    INT8U			Register;
    INT8U			Value;
} PACKED ApmlWriteRMIReq_T;


/* ApmlWriteRMIRes_T */
typedef struct
{
    INT8U			CompletionCode;
} PACKED ApmlWriteRMIRes_T;

 
/* ApmlReadCPUIdReq_T */
typedef struct
{
    INT8U   		Proc;
    INT8U   		Core;
    INT32U			Cpuid_func;
} PACKED ApmlReadCPUIdReq_T;


/* ApmlReadCPUIdRes_T */
typedef struct
{
    INT8U   		CompletionCode;
    INT32U 			eax;
    INT32U			ebx;
    INT32U			ecx;
    INT32U			edx;
} PACKED ApmlReadCPUIdRes_T;

/* ApmlReadHTCReq_T */
typedef struct
{
    INT8U   		Proc;
} PACKED ApmlReadHTCReq_T;


/* ApmlReadHTCRes_T */
typedef struct
{
    INT8U   		CompletionCode;
    INT32U 			High;
    INT32U			Low;
} PACKED ApmlReadHTCRes_T;

/* ApmlWriteHTCReq_T */
typedef struct
{
    INT8U   		Proc;
    INT8U   		RegisterValueSize;
    INT32U 			High;
    INT32U			Low;
} PACKED ApmlWriteHTCReq_T;


/* ApmlWriteHTCRes_T */
typedef struct
{
    INT8U   		CompletionCode;
} PACKED ApmlWriteHTCRes_T;


/* ApmlReadPStateReq_T */
typedef struct
{
    INT8U   		Proc;
    INT8U   		Core;
} PACKED ApmlReadPStateReq_T;


/* ApmlReadPStateRes_T */
typedef struct
{
    INT8U   		CompletionCode;
    INT8U 			PState;
} PACKED ApmlReadPStateRes_T;


/* ApmlReadMaxPStateReq_T */
typedef struct
{
    INT8U   		Proc;
} PACKED ApmlReadMaxPStateReq_T;


/* ApmlReadMaxPStateReq_T */
typedef struct
{
    INT8U   		CompletionCode;
    INT8U 			MaxPState;
} PACKED ApmlReadMaxPStateRes_T;


/* ApmlReadPStateLimitReq_T */
typedef struct
{
    INT8U			Proc;
} PACKED ApmlReadPStateLimitReq_T;


/* ApmlReadPStateLimitRes_T */
typedef struct
{
    INT8U			CompletionCode;
    INT8U			LimitActive;
    INT8U			PStateLimit;
} PACKED ApmlReadPStateLimitRes_T;

/* ApmlWritePStateLimitReq_T */
typedef struct
{
    INT8U			Proc;
    INT8U			LimitActive;
    INT8U			PStateLimit;
} PACKED ApmlWritePStateLimitReq_T;


/* ApmlWritePStateLimitRes_T */
typedef struct
{
    INT8U			CompletionCode;
} PACKED ApmlWritePStateLimitRes_T;

/* ApmlReadMCRReq_T */
typedef struct
{
    INT8U   		Proc;
    INT8U   		Core;
    INT32U			MCR_Addr;
} PACKED ApmlReadMCRReq_T;


/* ApmlReadMCRRes_T */
typedef struct
{
    INT8U   		CompletionCode;
    INT32U 			High;
    INT32U			Low;
} PACKED ApmlReadMCRRes_T;


/* ApmlWriteMCRReq_T */
typedef struct
{
    INT8U   		Proc;
    INT8U   		Core;
    INT32U			MCR_Addr;
    INT8U			RegisterValueSize;
    INT32U 			High;
    INT32U			Low;
} PACKED ApmlWriteMCRReq_T;

/* ApmlWriteMCRReq_T */
typedef struct
{
    INT8U   		CompletionCode;
} PACKED ApmlWriteMCRRes_T;

/* ApmlReadTSIReq_T */
typedef struct
{
    INT8U			Proc;	
    INT8U			Register;
} PACKED ApmlReadTSIReq_T;


/* ApmlReadTSIRes_T */
typedef struct
{
    INT8U			CompletionCode;
    INT8U			Value;	
} PACKED ApmlReadTSIRes_T;

	
/* ApmlWriteTSIReq_T */
typedef struct
{
    INT8U			Proc;	
    INT8U			Register;
    INT8U			Value;
} PACKED ApmlWriteTSIReq_T;


/* ApmlWriteTSIRes_T */
typedef struct
{
    INT8U			CompletionCode;
} PACKED ApmlWriteTSIRes_T;

typedef struct 
{ 
        INT8U                   Proc; 
} PACKED ApmlReadTDPLimitReq_T; 
 
 
/* ApmlReadTDPLimitRes_T */ 
typedef struct 
{ 
        INT8U                   CompletionCode; 
        INT8U                   ApmlTdpLimiPercent; 
        INT16U                  ApmlTdpLimit; 
} PACKED ApmlReadTDPLimitRes_T; 
 
 
/* ApmlWriteTDPLimitReq_T */ 
typedef struct 
{ 
        INT8U                   Proc; 
        INT8U                   ApmlTdpLimiPercent; 
        INT16U                  ApmlTdpLimit; 
} PACKED ApmlWriteTDPLimitReq_T; 
 
/* ApmlWriteTDPLimitRes_T */ 
typedef struct 
{ 
        INT8U                   CompletionCode; 
} PACKED ApmlWriteTDPLimitRes_T; 
 
/* ApmlReadProcessorPowerReq_T */ 
typedef struct 
{ 
        INT8U                   Proc; 
} PACKED ApmlReadProcessorPowerReq_T; 
 
 
/* ApmlReadProcessorPowerRes_T */ 
typedef struct 
{ 
        INT8U                   CompletionCode; 
        INT16U                  Tdp2Watt; 
        INT16U                  BaseTdp; 
        INT16U                  ProcessorTdp; 
} PACKED ApmlReadProcessorPowerRes_T; 
 
/* ApmlReadPowerAveragingPeriodReq_T */ 
typedef struct 
{ 
        INT8U                   Proc; 
} PACKED ApmlReadPowerAveragingPeriodReq_T; 
 
 
/* ApmlReadPowerAveragingPeriodRes_T */ 
typedef struct 
{ 
        INT8U                   CompletionCode; 
        uint8                   RunAvgRange; 
        uint16                  ApmTdpLimit; 
        uint32                  TdpRunAvgAccCap; 
} PACKED ApmlReadPowerAveragingPeriodRes_T; 
 
/* ApmlReadDramThrottleReq_T */ 
typedef struct 
{ 
        INT8U                   Proc; 
} PACKED ApmlReadDramThrottleReq_T; 
 
 
/* ApmlReadDramThrottleRes_T */ 
typedef struct 
{ 
        INT8U                   CompletionCode; 
        INT8U                   BwCapCmdThrottleMode; 
        INT8U                   BwCapEn; 
} PACKED ApmlReadDramThrottleRes_T; 
 
 
/* ApmlWriteDramThrottleReq_T */ 
typedef struct 
{ 
        INT8U                   Proc; 
        INT8U                   BwCapCmdThrottleMode; 
        INT8U                   BwCapEn; 
} PACKED ApmlWriteDramThrottleReq_T; 
 
/* ApmlWriteDramThrottleRes_T */ 
typedef struct 
{ 
        INT8U                   CompletionCode; 
} PACKED ApmlWriteDramThrottleRes_T; 

extern int ApmlInit(int BMCInst);
extern int ApmlClose(int BMCInst);

extern int ApmlGetInterfaceVersion (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadRMIReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlWriteRMIReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadCPUId (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadHTCReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlWriteHTCReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadPState (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadMaxPState (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadPStateLimit (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlWritePStateLimit (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadMCR (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlWriteMCR (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlReadTSIReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlWriteTSIReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);

extern int ApmlReadTDPLimitReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst); 
extern int ApmlWriteTDPLimitReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst); 
extern int ApmlReadProcessorPowerReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst); 
extern int ApmlReadPowerAveragingReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst); 
extern int ApmlReadDramThrottleReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ApmlWriteDramThrottleReg (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
#endif //_APML_H_
