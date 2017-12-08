/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2006-2009, American Megatrends Inc.    ***
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
* Filename: validate.h
*
******************************************************************/
#ifndef __VALIDATE_H__
#define __VALIDATE_H__


bool isValidIp(const char *dotted);
bool isValidIpRange(const char *range);
bool isValidPort(const char *port);
bool isValidPortRange(const char *port);


#endif //__VALIDATE_H__
