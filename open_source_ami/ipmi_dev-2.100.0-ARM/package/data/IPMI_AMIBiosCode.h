/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy, Building 200, Norcross,         **
 **                                                            **
 **        Georgia 30093, USA. Phone-(770)-246-8600.           **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * IPMI_AMIBiosCode.h
 * 
 *
 * Author: Gokulakannan. S <gokulakannans@amiindia.co.in>
 *****************************************************************/


#ifndef IPMI_AMIBIOSCODE_H_
#define IPMI_AMIBIOSCODE_H_

#define MAX_SIZE 256

#define CURRENT     0x00
#define PREVIOUS    0x01

#pragma pack (1)

typedef struct
{
    INT8U Command;
} PACKED AMIGetBiosCodeReq_T;

typedef struct
{
    INT8U CompletionCode;
    INT8U BiosCode[MAX_SIZE];
} PACKED AMIGetBiosCodeRes_T;

#pragma pack ()

#endif /* IPMI_AMIBIOSCODE_H_ */
