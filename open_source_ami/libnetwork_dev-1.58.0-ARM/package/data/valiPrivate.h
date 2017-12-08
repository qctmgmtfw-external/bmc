/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
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
* Filename: valiPrivate.c
*
******************************************************************/
#ifndef __VALIPRIVATE_H__
#define __VALIPRIVATE_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "coreTypes.h"

#define MAX_PORT_ALLOWED 11016807 //max port number to consider.

#define MAX_RANGE_BUFF_SIZE 256 //for range of ip's
#define MIN_RANGE_BUFF_SIZE 20 //for single ip

#define MAX_IP_NUMBER 255 //max dotted string lieteral for ip like "255.255.255.255"
#define MAX_MASK_BITS 32 //maximum ip range bits 
int  stringToNumber(const char *s, unsigned int min, unsigned int max, unsigned int *ret);
bool isValidMask(char *mask);
bool IsValidLib(char *libname);
void Escaper(char *orgstr/*IN*/, char *escstr/*OUT*/);

#endif // __VALIPRIVATE_H__
