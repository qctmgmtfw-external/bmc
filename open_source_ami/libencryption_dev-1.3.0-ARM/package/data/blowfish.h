/****************************************************************

 **                                                            **

 **    (C)Copyright 2006-2009, American Megatrends Inc.        **

 **                                                            **

 **            All Rights Reserved.                            **

 **                                                            **

 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **

 **                                                            **

 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **

 **                                                            **

****************************************************************/

#ifndef __BLOWFISH_H__
#define __BLOWFISH_H__


//To get proper user defined sizes.
#define REQ_ENCRYPT_BUFF_SIZE( X )  (X % 8)  != 0 ? X + ( 8 - ( X % 8 )): X
#define REQ_DECRYPT_BUFF_SIZE( X )  X

typedef void * BFHANDLE;

BFHANDLE blowfishInit(  unsigned char *key, int keyLen);
int blowfishEncryptPacket(char *inBuf, unsigned int  sizeInBuf, char *outBuf, unsigned int sizeOutBuf, BFHANDLE );
int blowfishDecryptPacket( char *packet, int packLen,BFHANDLE );
void blowfishClose(BFHANDLE ctx );
unsigned long rotateLong(unsigned long dWord);

#endif // __BLOWFISH_H__



