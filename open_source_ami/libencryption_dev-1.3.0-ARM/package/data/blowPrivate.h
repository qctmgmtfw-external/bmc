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
* Filename: blowPrivate.c
*
******************************************************************/
#ifndef __BLOWPRIVATE_H__
#define __BLOWPRIVATE_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coreTypes.h"
#include "dbgout.h"


#define MAXKEYBYTES	56          /* 448 bits */
#define N               16          /* max iterations for blowfish algorithm */



typedef struct {

    unsigned long P[16 + 2];

    unsigned long S[4][256];

    unsigned int init ;
    

} BLOWFISH_CTX;


static char * padForEightMul(char *buff, unsigned int size ,char *padBuf);

static void blowfishEncrypt(BLOWFISH_CTX *ctx, unsigned long *xl, unsigned long *xr);

static void blowfishDecrypt(BLOWFISH_CTX *ctx, unsigned long *xl, unsigned long *xr);

static bool isBlowfishInit(BLOWFISH_CTX *ctx );

static unsigned long F(BLOWFISH_CTX *ctx, unsigned long x);

#endif //__BLOWPRIVATE_H__
