/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

/**
 * utils.c
**/
#include <stdio.h>
#include <ctype.h>
#include "utils.h"


#define LITTLEENDIAN 1
/**
 * @brief Converts LitteEndian to BigEndian
 * @return BigEndian Format
**/
u16
htonsEx(u16 InValue)
{

#ifndef LITTLEENDIAN
    u16		OutValue	= 0;
    u8*		pInValue	= (u8*) &InValue;
    u8*		pOutValue	= (u8*) &OutValue;

    *pOutValue			= *(pInValue + 1);
    *(pOutValue + 1)	= *pInValue;

    return OutValue;
#else
    return InValue;
#endif
}

/**
 *	@brief Converts LitteEndian to BigEndian
 *	@return BigEndian Format
 **/
u32
htonlEx (u32 InValue)
{

#ifndef LITTLEENDIAN
    u32		OutValue	= 0;
    u8*		pInValue	= (u8*) &InValue;
    u8*		pOutValue	= (u8*) &OutValue;

    *pOutValue			= *(pInValue + 3);
    *(pOutValue + 1)    = *(pInValue + 2);
    *(pOutValue + 2)    = *(pInValue + 1);
    *(pOutValue + 3)    = *pInValue;

    return OutValue;
#else
    return InValue;
#endif
}

/**
 * @brief Dumps the Buffer
 * @return void
**/
void
dump_buffer(char *Buffer, int size)
{
  int i;
  int j;

  for (i=0;i<size;i++){
    if ((!(i % 16))&& (i!=0)){
      printf("   ");
      for (j=i-16;j<i;j++){
	if (isprint((unsigned char)Buffer[j]))
	  printf("%c",(unsigned char)Buffer[j]);
	else
	  printf(".");
      }
      printf("\n");
    }
    printf("%02X ",(unsigned char)Buffer[i]);
  }
  printf("   ");

  if (size%16){
    for(j=0;j<(16-(size%16));j++)
      printf("   ");
  }

  for(j=(size-1)-((size-1)%16);j<size;j++){
    if (isprint((unsigned char)Buffer[j]))
      printf("%c",(unsigned char)Buffer[j]);
    else
      printf(".");
  }
  printf("\n");

}
