/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2011, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
 
/****************************************************************
 * cipher.h
 * Structure used  by Cipher drivers and libraries
 ****************************************************************/


#ifndef CIPHER_H
#define CIPHER_H

#define MAX_DATA_BUF (64*1024)
#define MAX_KEY_BUF 256
#define MAX_IV_BUF  32

#define HMAC_SHA1_IPAD 0x36
#define HMAC_SHA1_OPAD 0x5c


#define AES_BLOCK_SIZE        16
#define DES_BLOCK_SIZE        8
#define SHA1_BLOCK_SIZE       64
#define SHA1_DIGEST_SIZE      20
#define SHA1_DIGEST_INT_SIZE  5
#define MAX_SHA1_KEY_SIZE     20

#define SHA_BLOCK_SIZE       64

#define CIPHER_PROGRESS_INIT        0xFF
#define CIPHER_PROGRESS_START       0x00
#define CIPHER_KS_PROGRESS_START    0x01
#define CIPHER_PROGRESS_IN          0x02
#define CIPHER_PROGRESS_COMPLETE    0x03
#define CIPHER_KS_PROGRESS_COMPLETE 0x04

#define RET_FAILED     (-1)
#define RET_NONE        (0)
#define RET_HANDLED     (1)

/* Cipher Data  */
typedef struct
{
   unsigned char  algo;         /* Encryption algo */
   unsigned char  method;       /* Encryption method */
   unsigned char  mode;         /* Specific mode of the method */
     
   unsigned int   key_bitlen;
   unsigned int   key_len;
   unsigned char  *key;
   
   unsigned int   iv_len;
   unsigned char  *iv;          /* Initial Vector */
   
   unsigned int   data_len;
   unsigned char  *data_in;     /* Data to cipher coprocessor */
   unsigned char  *data_out;    /* Data from cipher coprocessor */
   unsigned char  progress;          /* Indicate the progress of data transfer */
   
} __attribute__((packed)) cipher_data_t;




/* Encryption Algorithms */
typedef enum
{
   CIPHER_GETINFO = 0x00, 
   CIPHER_AES,
   CIPHER_RC4,
   CIPHER_DES,
   CIPHER_3DES,
   CIPHER_RSA,
   CIPHER_RNG,
   CIPHER_SHA1,
   CIPHER_SHA224,
   CIPHER_SHA256,
   CIPHER_MD5,
   CIPHER_HMAC_SHA1,
   CIPHER_HMAC_SHA224,
   CIPHER_HMAC_SHA256,
   CIPHER_HMAC_MD5,
 	
   INVALID_ALGO = 0xFF 
} eCipherAlgo; 

typedef enum
{
    AES_SET_KEY,
    AES_SET_DEC_KEY,
    AES_GET_KEY,
    AES_SET_IV,
    AES_ENCRYPTO,
    AES_DECRYPTO

} eAESActions;

typedef enum
{
    DES_SET_KEY = 0x00,
    DES_SET_IV,
    DES_GET_IV,
    DES_ENCRYPTO,
    DES_DECRYPTO
} eDESActions;

typedef enum
{
    DES3_SET_KEY,
    DES3_SET_IV,
    DES3_ENCRYPTO,
    DES3_DECRYPTO
} eDES3Actions;

typedef enum
{
    CIPHERMODE_NONE = 0x00,
    CIPHERMODE_ECB,
    CIPHERMODE_CBC,
    CIPHERMODE_CFB,
    CIPHERMODE_OFB,
    CIPHERMODE_CTR
    
} eCipherMode;

typedef enum
{
    CIPHER_KEYBIT_NONE = 0x00,
    CIPHER_KEYBIT_128,
    CIPHER_KEYBIT_192,
    CIPHER_KEYBIT_256,
    
} eCipherKeyBitLen;

typedef enum
{
    RC4_SET_KEY,
    RC4_CRYPTO
 
} eRC4Actions;

typedef enum
{
    HMAC_SHA1_SET_KEY,
    HMAC_SHA1_HASH
    
} eHMACSHA1Actions;

typedef enum
{
    HMAC_SET_KEY,
    HMAC_HASH
    
} eHMACActions;

typedef struct
{    
    unsigned int (*cipher_get_max_datalen)(void);    
    int (*cipher_get_info)(cipher_data_t* cipher_data);  
    int (*cipher_process_data_padding)(cipher_data_t* cipher_data);
    int (*cipher_process_remove_data_padding)(cipher_data_t* cipher_data);          
    int (*cipher_process_aes)(cipher_data_t* cipher_data);
    int (*cipher_process_rc4)(cipher_data_t* cipher_data);
    int (*cipher_process_des)(cipher_data_t* cipher_data);
    int (*cipher_process_3des)(cipher_data_t* cipher_data);
    int (*cipher_process_rsa)(cipher_data_t* cipher_data);
    int (*cipher_process_rng)(cipher_data_t* cipher_data);
    int (*cipher_process_sha1)(cipher_data_t* cipher_data);
    int (*cipher_process_sha224)(cipher_data_t* cipher_data);
    int (*cipher_process_sha256)(cipher_data_t* cipher_data);
    int (*cipher_process_md5)(cipher_data_t* cipher_data);
    int (*cipher_process_hmac_sha1)(cipher_data_t* cipher_data);
    int (*cipher_process_hmac_sha224)(cipher_data_t* cipher_data);
    int (*cipher_process_hmac_sha256)(cipher_data_t* cipher_data);
    int (*cipher_process_hmac_md5)(cipher_data_t* cipher_data);

} cipher_hal_operations_t;

typedef struct
{
    /* int (*process_intr) (int dev_id); */
} cipher_core_funcs_t;

struct cipher_hal
{
    cipher_hal_operations_t *pcipher_hal_ops;
};

struct cipher_dev
{
    struct cipher_hal *pcipher_hal;
};

typedef cipher_data_t cipher_ioctl_data;



#endif  /* CIPHER_H */

