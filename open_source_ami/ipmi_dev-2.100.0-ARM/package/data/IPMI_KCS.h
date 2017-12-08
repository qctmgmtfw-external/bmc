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
 *
 * ipmi_kcs.h
 * KCS specific IPMI structures.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 ******************************************************************/
#ifndef IPMI_KCS_H
#define IPMI_KCS_H
#include "Types.h"
#include "PDKAccess.h"

#pragma pack( 1 )

/* KCS Request Structure */
typedef struct
{
    INT8U   NetFnLUN;
    INT8U   Cmd;

} PACKED  KCSReq_T;
#pragma pack( )


/**
 * @def SET_SMS_ATN
 * @brief Macro to set the SMS Attention bit.
**/
#define SET_SMS_ATN(KCSIFC_NUM, BMCInst)       \
    do{                                                            \
        if(g_PDKHandle[PDK_SETSMSATTN] != NULL)                   \
        {                                                                                           \
            ((void(*)(INT8U, int))g_PDKHandle[PDK_SETSMSATTN]) (KCSIFC_NUM, BMCInst);   \
        }                                                                                                                     \
    }while(0);

/**
 * @def CLEAR_SMS_ATN
 * @brief Macro to reset the SMS attention bit.
**/
#define CLEAR_SMS_ATN(KCSIFC_NUM, BMCInst)       \
do{                                                            \
    if(g_PDKHandle[PDK_CLEARSMSATTN] != NULL)                   \
    {                                                                                           \
        ((void(*)(INT8U, int))g_PDKHandle[PDK_CLEARSMSATTN]) (KCSIFC_NUM, BMCInst);   \
    }                                                                                                                     \
}while(0);

/**
 * @def SET_OBF
 * @brief Macro to set the OBF bit.
**/
#define SET_OBF(KCSIFC_NUM, BMCInst)       \
    do{                                                            \
        if(g_PDKHandle[PDK_SETOBF] != NULL)                   \
        {                                                                                           \
            ((void(*)(INT8U, int))g_PDKHandle[PDK_SETOBF]) (KCSIFC_NUM, BMCInst);   \
        }                                                                                                                     \
    }while(0);

#endif	/* IPMI_KCS_H */

