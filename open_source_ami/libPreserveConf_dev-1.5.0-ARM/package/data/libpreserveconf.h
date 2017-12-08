 /*************************************************************** 
 **                                                            ** 
 **       (C)Copyright 2011, American Megatrends Inc.          ** 
 **                                                            ** 
 **                  All Rights Reserved.                      ** 
 **                                                            ** 
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             ** 
 **                                                            ** 
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         ** 
 **                                                            ** 
 ****************************************************************
 ****************************************************************
 *
 * libpreserveconf.h
 * 
 * Author: Prakash S <prakashs@amiindia.co.in>
 *
 ***************************************************************/
#ifndef LIBPRESERVECONF_H
#define LIBPRESERVECONF_H
#include "Types.h"

#define MAX_FILE_LEN 64
#define PRESERVE_AMI_FILE "/conf/Preserve-AMI.ini"
#define PRESERVE_OEM_FILE "/conf/Preserve-OEM.ini"

#define PRESERVE_AMI_FILE_OLDCONF "/mnt/oldconf/Preserve-AMI.ini"
#define PRESERVE_OEM_FILE_OLDCONF "/mnt/oldconf/Preserve-OEM.ini"

typedef struct 
{
	INT8U Selector;
	char Config_File[MAX_FILE_LEN];
	INT8U Status;
} PACKED PreserveConf_T;

extern int SetPreserveStatus(int, int, char*);
extern int GetPreserveStatus(int, char*);

#endif 
