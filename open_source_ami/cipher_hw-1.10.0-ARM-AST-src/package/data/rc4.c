/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2011, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
 
#include "rc4.h"

/*-- Crypto Algorithm--- ------------------------------------------*/

void rc4_set_key( struct rc4_key_st *st, unsigned char *key,  int len )
{
    int i, j = 0, k = 0, a;
    unsigned char *state;

    st->x = 0;
    st->y = 0;
    state = st->state;

    for( i = 0; i < 256; i++ )
    {
        state[i] = i;
    }

    j = k = 0;

    for( i = 0; i < 256; i++ )
    {
        a = state[i];
        j = (unsigned char) ( j + a + key[k] );
        state[i] = state[j]; 
        state[j] = a;
        if( ++k >= len ) 
            k = 0;
    }
}


