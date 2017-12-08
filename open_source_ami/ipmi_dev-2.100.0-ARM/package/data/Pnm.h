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
*** Pnm.h
*** Pnm Command Handler.
***
*** Author: JCChiu@ami.com.tw
********************************************************************/
#ifndef _PNM_H_
#define _PNM_H_

#include "Types.h"
#include "MsgHndlr.h"
#include "hal_defs.h"

/*------------------ PNM Command Numbers -------------------------*/
#define CMD_PNM_OEM_GET_READING              0xE2
#define CMD_PNM_OEM_ME_POWER_STATE_CHANGE    0xE3

/*------------------ PNM Reading Type -------------------------*/
#define PNM_PLATFORM_POWER_CONSUMPTION      0
#define PNM_INLET_AIR_TEMPERATURE           1
#define PNM_ICC_TDC                         2

#define NM_IPMB_MSG_Q        MSG_PIPES_PATH "NMIPMBQ"
#define INVALID_RECORD    0xff
#define MAX_PDK_NM_Sensor 80

/*PNM Command Handler table */
extern const CmdHndlrMap_T g_PNM_CmdHndlr[];


/* PnmOemGetReadingReq_T */
typedef struct
{
    INT8U   ReadingType;
    INT8U   reserved1;
    INT8U   reserved2;
} PnmOemGetReadingReq_T;


/* PnmOemGetReadingrRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ReadingType;
    INT8U   ReadingData1;
    INT8U   ReadingData2;
} PnmOemGetReadingrRes_T;


/* PnmMePowerStateChangeRes_T */
typedef struct
{
    INT8U   CompletionCode;
} PnmMePowerStateChangeRes_T;

typedef struct
{
    INT8U		TYPE_ENUM;
    INT8U		ID;
    INT8U       valid;
} SensorInfoSYS_T;

typedef int (*Fill_handler) (MsgPkt_T *pMsgPkt ,INT16U *byte_counter );
typedef int (*Parse_handler)(MsgPkt_T *pMsgPkt, INT8U NM_SensorID, INT8U res_par, int BMCInst);

typedef struct
{
    INT8U		TYPE_ENUM;
    INT8U		BMC_SensorID;
    INT8U		NetFnLUN;
    INT8U		cmd;
    INT8U       NM_SensorID;
    INT8U       res_par;
    Fill_handler fill_package_hndlr;
    Parse_handler parse_package_hndlr;
} SensorInfoCUST_T;

extern int PnmOemGetReading   (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,int BMCInst);
extern int PnmOemMePowerStateChange (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,int BMCInst);

// generic function for NodeManager
#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
extern int NM_RegisterOneSensor(hal_t *phal);
#endif
#endif //_PNM_H_
