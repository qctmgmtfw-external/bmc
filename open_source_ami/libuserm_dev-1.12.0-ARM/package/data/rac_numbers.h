/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2002-2003, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/****************************************************************
  $Header: $

  $Revision: $

  $Date: $
 *****************************************************************/
/*****************************************************************
 * File name    : rac_numbers.h
 * Author       : Anandh Mahalingam
 * Created      : Sep 16, 2003
 * G3 Number definitions file
 *****************************************************************/

#ifndef _RAC_NUMBERS_H_
#define _RAC_NUMBERS_H_

/**********************Service Type Enumerations******************/
/*There could be deviatiosn between service types and standard groups
Service types may be more detailed for e.g: within CURI (which has only one grp)
there may be curi local, curi nw etc*/
typedef enum _ServiceType
{
    RAC_SERVICE_TYPE_RESERVED = 0,
    RAC_SERVICE_TYPE_WEB,
    RAC_SERVICE_TYPE_VIDEO,
    RAC_SERVICE_TYPE_HID,
    RAC_SERVICE_TYPE_CURINW,
    RAC_SERVICE_TYPE_CURILOC,
    RAC_SERVICE_TYPE_CURIUSB,
    RAC_SERVICE_TYPE_SSH,
    RAC_SERVICE_TYPE_OSD
}
RAC_SERVICE_TYPE;
/**********************Service Type Enumerations******************/


/*************************Library Module Numbers*********************/
typedef enum _ModuleNumber
{
    MODULE_NUM_AUTH = 0,
    MODULE_NUM_MISC,
    MODULE_NUM_USERM,
    MODULE_NUM_NETCFG,
    MODULE_NUM_SEL,
    MODULE_NUM_ALERT,
    MODULE_NUM_FWALL,
    MODULE_NUM_SOLCFG,
    MODULE_NUM_HOSTCTL,
    MODULE_NUM_CARDHEALTH,
    MODULE_NUM_FTP,
    MODULE_NUM_NTP,
    MODULE_NUM_REDIRCFG,
    MODULE_NUM_FPGACFG,
	MODULE_NUM_HOSTHEALTH,
	MODULE_NUM_CALIB,
	MODULE_NUM_OSHB,
    MODULE_NUM_FLASHER,
    MODULE_NUM_UBENV,
	MODULE_NUM_PWRCTRL,
	MODULE_NUM_MACRO
} MODULE_NUMBER;
/*************************Library Module Numbers*********************/


#endif  /*  _RAC_NUMBERS_H_  */

/************************* End of File ****************************/
