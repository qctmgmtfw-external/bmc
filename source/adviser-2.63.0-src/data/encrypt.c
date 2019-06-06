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
 * @file   kbdencrypt.c
 * @brief  implementation of keyboard and mouse encryption wrapper functions
 * 
 * 
 **/
#ifndef _ENCRYPT_C
#define _ENCRYPT_C
#include "encrypt.h"
#include "dbgout.h"

/** 
 * @brief returns keyboard and mouse encryption handle 
 * 
 * @param key encryption key
 * @param keyLen key length
 * 
 * @return handle if successfull, otherwise NULL
 */
ENCRYPT_HANDLE
GetEncryptionHandleFromKey (unsigned char* key, int keyLen)
{    
    return blowfishInit (key, keyLen);
}

/** 
 * @brief close the encryption session handle
 * 
 * @param eHandle encryption session handle
 *
 * @return none
 */
void
CloseEncryptionHandle (ENCRYPT_HANDLE eHandle)
{
    blowfishClose (eHandle);
}

/** 
 * @brief Encrypts the inBuf, outBuf contains the encrypted buffer
 * 
 * @param inBuf input buffer
 * @param sizeInBuf length of encrypted buffer
 * @param outBuf output buffer - encrypted buffer
 * @param sizeOutBuf length of encrypted buffer
 * @param eHandle encryption session handle
 * 
 * @return 0 on success, -1 on failure
 */
int
EncryptPkt (char* inBuf,
            unsigned int sizeInBuf,
            char* outBuf,
            unsigned int sizeOutBuf,
            ENCRYPT_HANDLE eHandle)
{
    
    return blowfishEncryptPacket (inBuf, sizeInBuf, outBuf, sizeOutBuf, eHandle);
    
}
/**
 * @brief Decrypts the encrypted data. This is done using the blowfish algorithm
 * @param packet - pointer to the encrypted data buffer
 * @param packLen - length of the encrypted data buffer
 * @param eHandle - handle to encryption session
 * returns -1 on error 
 **/
int DecryptPkt (char* packet,
                int packLen,
                ENCRYPT_HANDLE eHandle)
{
    
    return blowfishDecryptPacket (packet, packLen, eHandle);   
}
#endif // _ENCRYPT_C
