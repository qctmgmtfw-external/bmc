/*****************************************************************
 ******************************************************************
 ****                                                            **
 ****    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ****                                                            **
 ****            All Rights Reserved.                            **
 ****                                                            **
 ****        6145-F, Northbelt Parkway, Norcross,                **
 ****                                                            **
 ****        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ****                                                            **
 ******************************************************************
 ******************************************************************
 *******************************************************************
 **
 ** BMCInfo.h
 ** Multiple BMC functions.
 **
 **  Author: Suresh V <sureshv@amiindia.co.in>
 *******************************************************************/


extern int CheckIPMIConfigurations(int BMCInst);
extern void GetIPMIConfFile (int BMCInst, char* ConfFile);
extern int GetBMCConfInfo();
extern int GetIPMIConfInfo(int BMCInst);
extern void FreeAllocPortMem(int BMCInst);
