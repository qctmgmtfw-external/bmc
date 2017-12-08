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
#ifndef PDKCMDS_H
#define PDKCMDS_H
#include "Types.h"
#include "SELRecord.h"
#include "MsgHndlr.h"
#include "ExtendedConfig.h"


#define PDK_CMD_LENGTH_UNKNOWN 0xFFFFFFFF

/*------------------ Type Definitions ---------------------------*/
typedef struct
{
    INT8U	ChannelNum;
    INT8U	Privilege;

} PDK_ChannelInfo_T;
typedef INT32U (*pOEMCmdHndlr_T) (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes);

typedef struct
{

    INT8U  			NetFn;
    INT8U  			Cmd;
    INT8U  			Privilege;
    pOEMCmdHndlr_T	CmdHndlr;
    INT32U			ReqLen;		/* 0xFF - Any Length */
    INT16U			FFConfig;

} OEMCmdHndlrMap_T;

typedef struct
{
    INT8U    OwnerID;
    INT8U    ChannelNum;

} OwnerIDMap_T;



/*---------------------------------------------------------------------------
 * @fn PDK_PostProcessCmd
 *
 * @brief This function is invoked before processing an incoming IPMI command.
 * Use this function to implement a new IPMI command, override an existing IPMI
 * command or perform a certain action on an incoming IPMI command.
 *
 * @param  NetFnLUN		- Net Function & LUN for which the command is addressed.
 * @param  pChannelInfo - Pointer to the channel information.
 * @param  pRequest     - Request IPMI Command Packet.
 * @param  RequestLen   - Length of the Request.
 * @param  pResponse    - Pointer to buffer to hold the response.
 * @param  pResponseLen - Pointer to the variable to hold response length.
 * @param  BMCInst - BMC instances,
 *
 * @return 0		- If the firmware needs to handle this command.
 *         -1		- otherwise
 *----------------------------------------------------------------------*/
extern int PDK_PreProcessCmd    (	   INT8U 				NetFnLUN,
                                                                INT8U				Cmd,
                                                                _FAR_ PDK_ChannelInfo_T*	pChannelInfo,
                                                                _FAR_ INT8U*				pRequest,
                                                                INT8U				RequestLen,
                                                                _FAR_ INT8U*				pResponse,
                                                                _FAR_ INT8U*				pResponseLen,
                                                                int BMCInst);


/*---------------------------------------------------------------------------
 * @fn PDK_PostProcessCmd
 *
 * @brief This function is invoked after an IPMI command is handled by the
 * core firmware. The response is contained in pResponse and the length of
 * the response is in pResponseLen. Use this function if you would like to
 * modify certain fields of the response or modify the response based on
 * certain conditions.
 *
 * @param  NetFnLUN		- Net Function & LUN for which the command is addressed.
 * @param  pChannelInfo - Pointer to the channel information.
 * @param  pRequest     - Request IPMI Command Packet.
 * @param  RequestLen   - Length of the Request.
 * @param  pResponse    - Pointer to buffer to hold the response.
 * @param  pResponseLen - Pointer to the variable to hold response length.
 * @param  BMCInst - BMC instances
 *
 * @return 0		- If the firmware needs to handle this command.
 *         -1		- otherwise
 *----------------------------------------------------------------------*/
extern int PDK_PostProcessCmd (INT8U 				NetFnLUN,
                                                                INT8U				Cmd,
                                                                _FAR_ PDK_ChannelInfo_T*	pChannelInfo,
                                                                _FAR_ INT8U*				pRequest,
                                                                INT8U				RequestLen,
                                                                _FAR_ INT8U*				pResponse,
                                                                _FAR_ INT8U*				pResponseLen,
                                                                int BMCInst);

/**
*@fn PDKGetOEMMsgHndlrMap 
*@brief Helps in getting command handler for particular NetFn
*@parm NetFn -NetFunction
*@param pCmdHndlrMap - Command Handler for the given NetFn
*@param BMCInst - BMC instances
*@return Returns 0 on success and -1 for failure
*/
extern int	  PDKGetOEMMsgHndlrMap (INT8U NetFn, _FAR_ CmdHndlrMap_T ** pCmdHndlrMap,int BMCInst);


/**
 * PDK_GetCmdSupCfgMask
 * 
 * Get OEM command support mask
 **/
extern int  PDK_GetCmdSupCfgMask (INT8U NetFn, INT8U Cmd, _NEAR_ INT16U* pFFConfig,int BMCInst);

/**
*@fn PDKAMIGetChNum
*@brief This function helps in getting the corresponding channnel no
             from SensorOwnerID to be used by sendmsg command
*@param SensorOwnerID Sensor Owner ID
*@param BMCInst - BMC instances
*@return Returns ChannelNum on success
                Returns '1', if no match occurs
*/
extern int PDKAMIGetChNum(INT8U SensorOwnerID,int BMCInst);


#endif	/* PDKCMDS_H */

