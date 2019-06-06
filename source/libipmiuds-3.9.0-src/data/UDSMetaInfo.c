/*******************************************************************
********************************************************************
****                                                              **
****    (C)Copyright 2008-2009, American Megatrends Inc.          **
****                                                              **
****    All Rights Reserved.                                      **
****                                                              **
****    5555 , Oakbrook Pkwy, Norcross,                           **
****                                                              **
****    Georgia - 30093, USA. Phone-(770)-246-8600.               **
****                                                              **
********************************************************************
********************************************************************
********************************************************************
**
** UDSMetaInfo.c
** UDS Meta Info file
**
** Author:
*******************************************************************/
#include "Types.h"
#include "MsgHndlr.h"
#include "Support.h"
#include "IPMIConf.h"

IPMILibMetaInfo_T g_UDSMetaInfo[] =
{
    {
    "SUPPORT_UDS_IFC",
    "UDSIfcTask",
    "UDS",
    1,						// Ifc Type
    1, 					   // Argument
    0,
    NULL
    },
};


INT8U GetLibMetaInfo (IPMILibMetaInfo_T **pMetaInfo)
{
    *pMetaInfo = &g_UDSMetaInfo[0];

    // return the argument count
    return (sizeof(g_UDSMetaInfo) / sizeof(g_UDSMetaInfo[0]));
}
