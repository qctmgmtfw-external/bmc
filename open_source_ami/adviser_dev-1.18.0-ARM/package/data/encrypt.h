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
 * @file   encrypt.h
 * @author ramkumars@ami.com
 * 
 * @brief public interface for keyboard and mouse encryption functions
 *
 * @detail The encryption library wrapper provides a set of interface
 *        functions for the encryption library. It hides the
 *        internal algorithm used for encryption. The
 *        following set of interfaces will be provided:
 *        
 *         -# GetEncryptionHandle
 *         -# Encrypt Data
 *         -# DecryptData
 *         -# CloseEncryptionHandle
 *
 *        The default encryption algorithm to be used is blowfish
 *        but if the user wishes to make his own algorithm then
 *        provision has been be made for this in the client side
 *        so that user can select his own algorithm
 *
 *        The Encryption key to be used should satisfy the following criteria:
 *
 *        -# The Key should not be transferred over the network
 *        -# The Key should be unique for each session,
 *           that is two sessions should not have the same key,
 *           even if the user is same.
 */

#ifndef __ADVISER_ENCRYPT_H__
#define __ADVISER_ENCRYPT_H__

#include "blowfish.h"

/// BFHANDLE - blowfish handle is used here beacuse the encryption algorithm used is blowfish,
/// this should be changed when the encryption algorithm is changed
typedef  BFHANDLE  ENCRYPT_HANDLE;

/**
 * @brief Returns the keybd encryption handle. The default implementation of this
 *        function is blowfish algorithm. 
 * @param key for the encyption
 * @param length of the key
 * return NULL if failed , handle if passed
 **/
ENCRYPT_HANDLE
GetEncryptionHandleFromKey (unsigned char* key, int KeyLen);

/**
 * @brief Closes the Encryption session handle
 * @param Encryption session handle
 * returns none
 **/
void CloseEncryptionHandle (ENCRYPT_HANDLE);

/**
 * @brief Encrypts the data packet. This function calls the blowfish
 *        functions to encrypt the data.
 * @param inBuf - input buffer pointer
 * @param sizeInBuf - input buffer size
 * @param outBuf - output buffer size
 * @param sizeOutBuf - size of output buffer
 * @param ENCRYPT_HANDLE - handle to the encryption session
 * returns -1 on error 
 **/
int
EncryptPkt (char *inBuf,
            unsigned int  sizeInBuf,
            char *outBuf,
            unsigned int sizeOutBuf,
            ENCRYPT_HANDLE );


/**
 * @brief Decrypts the encrypted data. This is done using the blowfish algorithm
 * @param packet - pointer to the encrypted data buffer
 * @param packLen - length of the encrypted data buffer
 * @param ENCRYPT_HANDLE - handle to encryption session
 * returns -1 on error 
 **/
int
DecryptPkt (char *packet,
            int packLen,
            ENCRYPT_HANDLE );

#endif // __ADVISER_ENCRYPT_H__
