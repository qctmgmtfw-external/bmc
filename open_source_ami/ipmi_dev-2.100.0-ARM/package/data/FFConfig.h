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
 * FFConfig.h
 * Firmware firewall configuration.
 *
 *  Author: Basavaraj Astekar<basavarja@ami.com>
 *          Ravinder Reddy<bakkar@ami.com>
 ******************************************************************/
#ifndef FF_CONFIG_H
#define FF_CONFIG_H
#include "Types.h"
#include "PMConfig.h"
#include "MsgHndlr.h"
#include "IPMIConf.h"


/*** External Definitions ***/
#define FF_SUCCESS              0x00
#define FF_NETFN_ERR            0x10
#define FF_CMD_ERR              0x20
#define FF_CHANNEL_ERR          0x30

#pragma pack( 1 )
/**
 * @struct FFSubFnTbl_T
 * @brief Firmware Firewall sub-function table structure.
**/
typedef struct
{
    INT8U   NetFn;
    INT8U   Cmd;
    INT32U  SubFn [MAX_NUM_CHANNELS];

} PACKED  FFSubFnTbl_T;

#pragma pack( )

/**
 * @brief Checks for Sub-function configuration.
 * @param ChannelNum Channel Number
 * @param NetFn Net Function
 * @param Cmd Command
 * @param SubFn Sub Function
 * @return 1 if Sub Fn enabled, 0 if disabled.
**/
extern int CheckSubFn (INT8U ChannelNum, INT8U NetFn, INT8U Cmd, INT8U SubFn);

/**
 * @brief Checks for command configurability
 * @param pCmdHndlrMap Command handler map
 * @param ChannelNum Channel Number
 * @param NetFn Net Function
 * @param Cmd Command
 * @return 1 if command enabled, 0 if disabled.
**/
extern int CheckCmdCfg (_FAR_ CmdHndlrMap_T* pCmdHndlrMap, 
                        INT8U ChannelNum, INT8U NetFn, INT8U Cmd,int BMCInst);

/**
 * @brief Get command configuration information address.
 * @param NetFn - Net function of the command.
 * @param Cmd - Command.
 * @return The command configuration.
**/
extern _FAR_ INT8U* GetCmdCfgAddr (INT8U NetFn, INT8U Cmd, int BMCInst);

/**
 * @brief Get sub-function mask.
 * @param ChannelNum - Channel number.
 * @param NetFn - Command net function.
 * @param Cmd - Command.
 * @param pSubFnMask - Sub-function mask.
**/
extern int GetSubFnMask (INT8U ChannelNum, INT8U NetFn, 
                         INT8U Cmd, _NEAR_ INT32U* pSubFnMask,int BMCInst);

/**
 * @brief Get command support configuration mask.
 * @param NetFn - Command net function.
 * @param Cmd - Command.
 * @param pFFConfig - Command support configuration information.
 * @return 0 if success, -1 if error.
**/
extern int GetCmdSupCfgMask (INT8U NetFn, INT8U Cmd, _NEAR_ INT16U* pFFConfig,int BMCInst);

#endif /* FF_CONFIG_H */
