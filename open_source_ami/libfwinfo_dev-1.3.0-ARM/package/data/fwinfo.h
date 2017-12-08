/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008-2011, American Megatrends Inc.    ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* fwinfo.h
*
******************************************************************/
#ifndef _FWINFO_H
#define _FWINFO_H

#define FWINFO_CFG_FILE		"/etc/versions"

/* Above file MUST have the information in the following way.
	FW_VERSION=<fw version string>
	FW_DATE=<fw date string>
	FW_BUILDTIME=<fw build time string>
	FW_DESC=<fw description string>
*/
 
typedef struct {
	char FWVersion[64];
	char FWDate[64];
	char FWBuildTime[64];
	char FWDesc[512];
} FWINFO_STRUCT;

typedef struct __fmhdetail
{
    char ModuleName[10];    //null terminated
    char ModuleVersion[5];
}
FMH_DETAIL;


int GetFWInfo(FWINFO_STRUCT *fwInfo);
int GetFMHDetail(FMH_DETAIL* fmhdetail,int* nNumMods);
int GetEZUSBFwInfo(char* ezusbver);

#endif 
	
	
