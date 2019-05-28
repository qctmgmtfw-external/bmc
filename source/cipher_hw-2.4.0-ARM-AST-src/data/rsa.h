/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
#ifndef _RSA_H
#define _RSA_H
#include "cipher.h"

#if defined(SOC_AST2300) || defined(SOC_AST1050_1070)
#define GROUP_AST2300 1
#endif
#if defined(SOC_AST2400) || defined(SOC_AST1250) || defined(SOC_AST2500) || defined(SOC_AST2520)
#define GROUP_AST2300_PLUS 1
#endif

#if defined(GROUP_AST2300_PLUS)
#define MIN_RSA_MOD_LEN     4   //32-bits   for modulus
#define MIN_RSA_EXP_LEN     1   //2-bits    for exponent
#define MAX_RSA_KEY_LEN   508   //4064-bits for both modulus and exponent
#elif defined(GROUP_AST2300)
#define MIN_RSA_MOD_LEN     4   //32-bits   for modulus
#define MIN_RSA_EXP_LEN     1   //2-bits    for exponent
#define MAX_RSA_KEY_LEN   256   //2048-bits for both modulus and exponent
#endif

/* rsa */
typedef struct {
    unsigned int mp_len;
    unsigned char rsa_mp[MAX_RSA_KEY_LEN];
    unsigned int mod_exp_bitnum;
} rsa_mp_info_t;

int FillMp(rsa_mp_info_t *rsa_mp_info, cipher_data_t  *cipher_data);

#endif /* rsa.h */
