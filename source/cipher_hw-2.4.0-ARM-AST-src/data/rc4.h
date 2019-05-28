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
#ifndef _RC4_H
#define _RC4_H

struct rc4_key_st
{
    int x, y;
    unsigned char state[256];
};


void rc4_set_key( struct rc4_key_st *st, unsigned char *key,  int len );


#endif /* rc4.h */
